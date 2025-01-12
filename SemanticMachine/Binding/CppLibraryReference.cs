using SemanticMachine.Grammar.Interpretation;
using System.Text;

namespace SemanticMachine.Binding;

/// <summary>
/// Manages the entry points of library functions in our runtime.
/// </summary>
public class CppLibraryReference
{
    public string MainNamespace { get; set; } = "SigourneyEngine::FunctionalLayer";
    public string EnumerationSubNamespace { get; set; } = "Enumeration";
    public string MemorySubNamespace { get; set; } = "Memory";
    public string PrefixOfMethods { get; set; } = "RuntimeBase";

    public (string creation, string finalVar) CreateArrayEnumerable(string type, string[] values, VarNameDistributor nameDispo)
    {
        StringBuilder prepBuilder = new();

        string varName = nameDispo.GetName("arr");

        prepBuilder
            .Append($"{EnumerationSubNamespace}::ArrayEnumerable<{type}, {values.Length}>*")
            .Append(' ')
            .Append(varName)
            .Append(" = ")
            .Append(PrefixOfMethods)
            .Append("::MakeArray<")
            .Append(type)
            .Append(',')
            .Append(values.Length)
            .AppendLine(">();");

        for (int i = 0; i < values.Length; i++)
        {
            prepBuilder.Append(varName)
                .Append(".SetItem(")
                .Append(i)
                .Append(", ")
                .Append(values[i])
                .AppendLine(");");
        }

        return (prepBuilder.ToString(), varName);
    }

    public string PrintType(TypeDefinition type) => type switch
    {
        ArrayType arrType => $"{EnumerationSubNamespace}::IEnumerable<{PrintType(arrType.ElementType)}>*",
        _ => type.Name
    };
}
