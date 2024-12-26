using CodeGenerator;
using System.Data;
using System.Text.RegularExpressions;

namespace SystemGraphResolver;
internal partial class UserDefinedContentParser
{
	[GeneratedRegex(@"typedef struct[\s]*([\d\p{L}_]*)[\s]*\{([^\{\}]*)\}(?<name>[^;]*);")]
	private static partial Regex ResStructPattern();

	[GeneratedRegex(@"void (?<name>[^\s\(\);\{\}]+)\((?<parameters>[^\(\);\{\}]*)\);")]
	private static partial Regex SystemStructPattern();

	[GeneratedRegex(@"(?<const>const[\s]+)?(?<type>[^\s\*]+)\*[\s]+(?<name>[^,\s]+)")]
	private static partial Regex ParameterPattern();

	private static string ResNameToMacroId(string resName) => $"{resName.ToUpper()}_RESID";
	private static string SysNameToMacroId(string sysName) => $"{sysName.ToUpper()}_SYSID";

	public static async Task LoadDirectory(string topDirectory)
	{
		// get all resource types
		Dictionary<string, ResourceMetadata> allResTypes = await GetAllResourceTypes(topDirectory).ConfigureAwait(false);
		Dictionary<string, SystemMetadata> allSystemTypes = await GetAllSystemTypes(topDirectory, allResTypes).ConfigureAwait(false);
        SystemMetadata[] systemsById = [.. allSystemTypes.Values.OrderBy(x => x.Id)];
        SystemGraph systemGraph = SystemGraph.Create(systemsById);

		var empty = CodeWriterPredicateBuilder.Empty();

		CodeWriter writer = CodeWriter.Empty().ApplyMany(empty
			.Comment("resources")
			.MacroDefine("SE_GEN_RES_COUNT", allResTypes.Count.ToString())
			.Concat(allResTypes.Values.Aggregate(empty,
				(pred, nextValue) => pred.MacroDefine(ResNameToMacroId(nextValue.Name), nextValue.Id.ToString())))
			.WriteLine()
			.Comment("systems")
			.MacroDefine("SE_GEN_SYS_COUNT", allSystemTypes.Count.ToString())
			.Concat(allSystemTypes.Values.Aggregate(empty,
				(pred, systemType) => pred.MacroDefine(SysNameToMacroId(systemType.Name), systemType.Id.ToString())))
			.WriteLine()
			.Comment("system graph initial state (number of children) (make a copy to use)")
			.DefineArray("int", "SE_GEN_INITIAL_SCHEDULING_STATE", systemGraph.Nodes.Select(x => $"{x.CountChildren} /*{x.Name}*/"))
			.WriteLine()
			.Comment("system graph update function")
			.Function("void inline", "SE_GEN_SystemGraphUpdate", "int* currentState, int finishedNode, void (*callback)(int nextAvailable)", empty
				.Switch("finishedNode", systemGraph.Nodes.Aggregate(empty,
					(pred, systemNode) => pred
					.Case($"{SysNameToMacroId(systemNode.Name)}", systemNode.Parents.Aggregate(empty,
						(pred, parent) => pred
						.WriteLine($"currentState[{parent}]--;")
						.If($"currentState[{parent}] == 0", empty.WriteLine($"callback({parent});")))
						.WriteLine("break;")))))
			.WriteLine());

        // TODO: bake implementation for dependency injection (provider functions)
		// TODO: this needs design from the query/storage system. we'll have to move forward with functional implementation first
		Console.WriteLine(writer.ToString());
    }

	private static async Task<Dictionary<string, ResourceMetadata>> GetAllResourceTypes(string topDirectory) =>
		await FilterAllContents(topDirectory, "*.res.h")
		.Select(x => x.Replace('\n', ' '))
		.Select(content => ResStructPattern().Match(content))
		.SelectMany(match => DepleteMatches(match).ToAsyncEnumerable())
		.Where(x => x.Success)
		.Select(match => match.Groups["name"].Value.Trim())
		.Select((name, index) => new ResourceMetadata(name, index))
		.ToDictionaryAsync(resMetadata => resMetadata.Name)
		.ConfigureAwait(false);

	private static IEnumerable<Match> DepleteMatches(Match original)
	{
		yield return original;
		while (original.NextMatch() != original)
		{
			original = original.NextMatch();
			yield return original;
		}
	}

	private static async Task<Dictionary<string, SystemMetadata>> GetAllSystemTypes(string topDirectory, Dictionary<string, ResourceMetadata> resourceTypes) =>
		await FilterAllContents(topDirectory, "*.sys.h")
		.Select(content => SystemStructPattern().Match(content))
		.SelectMany(match => DepleteMatches(match).ToAsyncEnumerable())
		.Select(match => (
			name: match.Groups["name"].Value,
			parameters: match.Groups["parameters"].Value
			.Split(',')
			.Select(x => x.Trim())
			.Select(x => ParameterPattern().Match(x))
			.Select(match => !match.Success ?
				ParameterInfo.Invalid
				: new ParameterInfo(match.Groups["name"].Value, match.Groups["type"].Value, match.Groups["const"].Value.Length > 0 ? ParamType.Read : ParamType.Write))
		))
		.Where(pair => pair.parameters.All(x => x.Type != ParamType.Invalid && resourceTypes.ContainsKey(x.Resource)))
		.Select((pair, index) => new SystemMetadata(
			pair.name,
			index,
			pair.parameters
				.Where(x => x.Type == ParamType.Read)
				.Select(x => resourceTypes[x.Resource])
				.Select(resType => resType.Id),
			pair.parameters
				.Where(x => x.Type == ParamType.Write)
				.Select(x => resourceTypes[x.Resource])
				.Select(resType => resType.Id)))
		.ToDictionaryAsync(value => value.Name);

	public static async IAsyncEnumerable<string> FilterAllContents(string topDir, string pattern)
	{
		foreach (string filePath in Directory.EnumerateFiles(topDir, pattern, SearchOption.AllDirectories))
		{
			yield return await File.ReadAllTextAsync(filePath).ConfigureAwait(false);
		}
	}
}
