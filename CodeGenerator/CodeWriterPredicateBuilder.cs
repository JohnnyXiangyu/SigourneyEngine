using System.Text;

namespace CodeGenerator;
internal static class CodeWriterPredicateBuilder
{
    public static string ConstructLine(this IEnumerable<string> line, int depth, string indent) => Enumerable.Range(0, depth).Select(_ => indent).Concat(line).Aggregate(new StringBuilder(), (prev, next) => prev.Append(next)).ToString();

    public static IEnumerable<Func<CodeWriter, CodeWriter>> Empty() => [];

    public static IEnumerable<Func<CodeWriter, CodeWriter>> WriteLine(this IEnumerable<Func<CodeWriter, CodeWriter>> predicates) =>
        predicates.Append(x => x.WriteLine());

    public static IEnumerable<Func<CodeWriter, CodeWriter>> WriteLine(this IEnumerable<Func<CodeWriter, CodeWriter>> predicates, string line) =>
        predicates.Append(x => x.WriteLine(line));

    public static IEnumerable<Func<CodeWriter, CodeWriter>> Write(this IEnumerable<Func<CodeWriter, CodeWriter>> predicates, string text) =>
        predicates.Append(x => x.WriteLine(text));

    public static IEnumerable<Func<CodeWriter, CodeWriter>> MacroDefine(this IEnumerable<Func<CodeWriter, CodeWriter>> predicates, string name, string value) =>
        predicates.Append(x => x.MacroDefine(name, value));

    public static IEnumerable<Func<CodeWriter, CodeWriter>> TypedefStruct(this IEnumerable<Func<CodeWriter, CodeWriter>> predicates, string name, IEnumerable<Func<CodeWriter, CodeWriter>>contentGen) =>
        predicates.Append(x => x.TypedefStruct(name, contentGen));

    public static IEnumerable<Func<CodeWriter, CodeWriter>> Comment(this IEnumerable<Func<CodeWriter, CodeWriter>> predicates, string comment) =>
        predicates.Append(x => x.Comment(comment));

    public static IEnumerable<Func<CodeWriter, CodeWriter>> Function(this IEnumerable<Func<CodeWriter, CodeWriter>> predicates, string returnType, string name, string paramLine, IEnumerable<Func<CodeWriter, CodeWriter>>contentGen) =>
        predicates.Append(x => x.Function(returnType, name, paramLine, contentGen));

    public static IEnumerable<Func<CodeWriter, CodeWriter>> Case(this IEnumerable<Func<CodeWriter, CodeWriter>> predicates, string value, IEnumerable<Func<CodeWriter, CodeWriter>>contentGen) =>
        predicates.Append(x => x.Case(value, contentGen));

    public static IEnumerable<Func<CodeWriter, CodeWriter>> Switch(this IEnumerable<Func<CodeWriter, CodeWriter>> predicates, string variable, IEnumerable<Func<CodeWriter, CodeWriter>>contentGen) =>
        predicates.Append(x => x.Switch(variable, contentGen));

    public static IEnumerable<Func<CodeWriter, CodeWriter>> DefineArray(this IEnumerable<Func<CodeWriter, CodeWriter>> predicates, string type, string name, IEnumerable<string> values) =>
        predicates.Append(x => x.DefineArray(type, name, values));

    public static IEnumerable<Func<CodeWriter, CodeWriter>> If(this IEnumerable<Func<CodeWriter, CodeWriter>> predicates, string condition, IEnumerable<Func<CodeWriter, CodeWriter>> contentGen) =>
        predicates.Append(x => x.If(condition, contentGen));
}
