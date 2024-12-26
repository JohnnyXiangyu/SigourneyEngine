using System.Text;

namespace CodeGenerator;
internal record CodeWriter(IEnumerable<string> Lines, IEnumerable<string> IncompleteLine, int Depth, string Indent = "    ")
{
    public static CodeWriter Empty() => new([], [], 0);

    public CodeWriter ApplyMany(IEnumerable<Func<CodeWriter, CodeWriter>> predicates) => 
        predicates.Aggregate(this, (prev, pred) => pred(prev));

    public override string ToString() => 
        Lines.Aggregate(new StringBuilder(), (prev, next) => prev.AppendLine(next)).ToString();

    public CodeWriter WriteLine() => WriteLine(string.Empty);

    public CodeWriter WriteLine(string line) => new(Lines.Append(IncompleteLine.Append(line).ConstructLine(Depth, Indent)), 
        [], 
        Depth, 
        Indent);

    public CodeWriter Write(string text) => new(Lines, IncompleteLine.Append(text), Depth, Indent);

    public CodeWriter MacroDefine(string name, string value) => WriteLine($"#define {name} {value}");

    public CodeWriter AddIndent(int newDepth) => new(Lines, IncompleteLine, Depth + newDepth, Indent);

    public CodeWriter TypedefStruct(string name, IEnumerable<Func<CodeWriter, CodeWriter>> contentGen) =>
        WriteLine($"typedef _{name}")
        .WriteLine("{")
        .AddIndent(1)
        .ApplyMany(contentGen)
        .AddIndent(-1)
        .WriteLine($"}} {name};");

    public CodeWriter Comment(string comment) => WriteLine($"// {comment}");

    public CodeWriter Function(string returnType, string name, string paramLine, IEnumerable<Func<CodeWriter, CodeWriter>> contentGen) =>
        WriteLine($"{returnType} {name}({paramLine})")
        .WriteLine("{")
        .AddIndent(1)
        .ApplyMany(contentGen)
        .AddIndent(-1)
        .WriteLine("}");

    public CodeWriter Case(string value, IEnumerable<Func<CodeWriter, CodeWriter>> contentGen) =>
        WriteLine($"case {value}:")
        .AddIndent(1)
        .ApplyMany(contentGen)
        .AddIndent(-1);

    public CodeWriter Switch(string variable, IEnumerable<Func<CodeWriter, CodeWriter>> contentGen) =>
        WriteLine($"switch ({variable})")
        .WriteLine("{")
        .ApplyMany(contentGen)
        .WriteLine("}");

    public CodeWriter DefineArray(string type, string name, IEnumerable<string> values) =>
        WriteLine($"{type} {name}[] =")
        .WriteLine("{")
        .AddIndent(1)
        .ApplyMany(values.Select<string, Func<CodeWriter, CodeWriter>>(value => (CodeWriter writer) => writer.Write(value).WriteLine(",")))
        .AddIndent(-1)
        .WriteLine("};");

    public CodeWriter If(string condition, IEnumerable<Func<CodeWriter, CodeWriter>> contentGen) =>
        WriteLine($"if ({condition})")
        .WriteLine("{")
        .AddIndent(1)
        .ApplyMany(contentGen)
        .AddIndent(-1)
        .WriteLine("}");
}
