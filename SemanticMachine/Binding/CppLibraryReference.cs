using SemanticMachine.Grammar.Interpretation;
using System.Text;
using Nito.Disposables;
using SemanticMachine.Grammar.Symbols.Expression;

namespace SemanticMachine.Binding;

/// <summary>
/// Manages the entry points of library functions in our runtime.
/// </summary>
public class CppLibraryReference
{
    public string OutputNamespace { get; set; } = "SigourneyEngine::Generated::Functional";
    public string OutputClass { get; set; } = "UpdateLoop";
    public string OutputHeader { get; set; } = "update_loop.h";
    public string OutputCpp { get; set; } = "update_loop.cpp";

    public string MainNamespace { get; set; } = "SigourneyEngine::FunctionalLayer";
    public string EnumerationSubNamespace { get; set; } = "Enumeration";
    public string MemorySubNamespace { get; set; } = "Memory";
    public string BaseClassName { get; set; } = "RuntimeBase";
    public string AllocatorGetter { get; set; } = "GetAllocator()";
    public string MakeArrayFunction { get; set; } = "MakeArray";

    public string CreateArrayEnumerable(string type, string[] values, VarNameDistributor nameDispo, TextWriter writer)
    {
        StringBuilder prepBuilder = new();

        string varName = nameDispo.GetName("arr");

        prepBuilder
            .Append($"{EnumerationSubNamespace}::ArrayEnumerable<{type}, {values.Length}>*")
            .Append(' ')
            .Append(varName)
            .Append(" = ")
            .Append(BaseClassName)
            .Append($"::{MakeArrayFunction}<")
            .Append(type)
            .Append(',')
            .Append(values.Length)
            .AppendLine($">({BaseClassName}::{AllocatorGetter});");

        writer.WriteLine(prepBuilder.ToString());

        for (int i = 0; i < values.Length; i++)
        {
            writer.WriteLine($"{varName}.SetItem(i, {values[i]});");
        }

        return varName;
    }

    public string PrintType(TypeDefinition type) => type switch
    {
        ArrayType arrType => $"{EnumerationSubNamespace}::IEnumerable<{PrintType(arrType.ElementType)}>*",
        CallableType callType => $"{MainNamespace}::ILambda<{string.Join(", ", [PrintType(callType.ReturnType), .. callType.Params.Select(param => PrintType(param.Type))])}>*",
        _ => type.Name
    };

    public IDisposable BeginHeaderFile(TextWriter writer)
    {
        writer.WriteLine($"#pragma once");
        writer.WriteLine($"#include\"runtime.h\"");

        foreach (string nspace in OutputNamespace.Split("::"))
        {
            writer.WriteLine($"namespace {nspace} {{");
        }
        writer.WriteLine();

        writer.WriteLine($"class {OutputClass} : public {MainNamespace}::{BaseClassName}");
        writer.WriteLine($"{{");
        writer.WriteLine($"public:");

        return Disposable.Create(() => 
        {
            writer.WriteLine("};");
            writer.WriteLine();

            foreach (string _ in OutputNamespace.Split("::"))
            {
                writer.WriteLine("}");
            }
        });
    }

    public IDisposable BeginCppFile(TextWriter writer)
    {
        writer.WriteLine($"#include \"{OutputHeader}\"");
        writer.WriteLine($"using namespace {MainNamespace};");
        writer.WriteLine($"using namespace {MainNamespace}::{EnumerationSubNamespace};");
        writer.WriteLine($"using namespace {MainNamespace}::{MemorySubNamespace};");
        writer.WriteLine($"using namespace {OutputNamespace};");

        return Disposable.Create(null);
    }

    public void CreateLambdaInstance(string lambdaType, string lambdaName, TextWriter writer)
    {
        writer.WriteLine($"{lambdaType}* {lambdaName} = {BaseClassName}::CreateLambda<{lambdaType}>();");
    } 
}
