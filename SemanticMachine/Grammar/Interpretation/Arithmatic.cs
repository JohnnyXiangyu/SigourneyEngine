namespace SemanticMachine.Grammar.Interpretation;

public static partial class PrimitiveTypes
{
    public const string Int32 = "int32";
    public const string Boolean = "bool";
}

public enum ArithmaticOperation
{
    Add,
    Minus,
    Mult,
    Divide,
    BitwiseAnd,
    BitwiseOr,
    LogicAnd,
    LogicOr,
    Equals
}

public interface IArithmatic : IEvaluatable
{
    public static ArithmaticOperation DeserializeOperation(string operation) =>
    operation switch
    {
        "+" => ArithmaticOperation.Add,
        "-" => ArithmaticOperation.Minus,
        "*" => ArithmaticOperation.Mult,
        "/" => ArithmaticOperation.Divide,
        "&" => ArithmaticOperation.BitwiseAnd,
        "|" => ArithmaticOperation.BitwiseOr,
        "&&" => ArithmaticOperation.LogicAnd,
        "||" => ArithmaticOperation.LogicOr,
        "=" => ArithmaticOperation.Equals,
        _ => throw new Exception($"unexpected binary operation token: {operation}")
    };

    public static int ArithmaticOpAltitude(ArithmaticOperation op) =>
        op switch
        {
            ArithmaticOperation.Add => 1,
            ArithmaticOperation.Minus => 1,
            ArithmaticOperation.Mult => 2,
            ArithmaticOperation.Divide => 2,
            ArithmaticOperation.BitwiseAnd => 3,
            ArithmaticOperation.BitwiseOr => 3,
            ArithmaticOperation.LogicAnd => -1,
            ArithmaticOperation.LogicOr => -1,
            ArithmaticOperation.Equals => 0,
            _ => throw new Exception("unexpected arithmatic operation, IArithmatic.ArithmaticOpAltitude"),
        };

    public static int ArithmaticOpAltitude(string op) => ArithmaticOpAltitude(DeserializeOperation(op));

    public static bool ValidityCheck(string ltype, string rtype, ArithmaticOperation operation) =>
    operation switch
    {
        ArithmaticOperation.Add or ArithmaticOperation.Minus or ArithmaticOperation.Mult or ArithmaticOperation.Divide or ArithmaticOperation.BitwiseAnd or ArithmaticOperation.BitwiseOr => ltype == rtype && ltype == PrimitiveTypes.Int32,
        ArithmaticOperation.LogicAnd or ArithmaticOperation.LogicOr => ltype == rtype && ltype == PrimitiveTypes.Boolean,
        ArithmaticOperation.Equals => ltype == rtype && (ltype == PrimitiveTypes.Boolean || ltype == PrimitiveTypes.Int32),
        _ => throw new Exception("why the fuck is there an unexpected binary operation type?")
    };

    protected static string OperatorReturnType(ArithmaticOperation operation) => operation switch
    {
        ArithmaticOperation.Add or ArithmaticOperation.Minus or ArithmaticOperation.Mult or ArithmaticOperation.Divide or ArithmaticOperation.BitwiseOr or ArithmaticOperation.BitwiseAnd => PrimitiveTypes.Int32,
        ArithmaticOperation.LogicAnd or ArithmaticOperation.LogicOr or ArithmaticOperation.Equals => PrimitiveTypes.Boolean,
        _ => throw new Exception("why the fuck is there an unexpected binary operation type?")
    };
}

public class LeafArithmatic(IEvaluatable value) : IArithmatic
{
    public string Type => value.Type;
    public IEvaluatable Value => value;

    public string PrettyPrint() => Value.PrettyPrint();
}

public record NodeArithmatic(IArithmatic LeftChild, IArithmatic RightChild, ArithmaticOperation Operation) : IArithmatic
{
    public string Type => IArithmatic.OperatorReturnType(Operation);

    public string PrettyPrint() => $"({Operation} {LeftChild.PrettyPrint()} {RightChild.PrettyPrint()})";
}

public record Int32Value(int Value) : IEvaluatable
{
    public string Type => PrimitiveTypes.Int32;

    public string PrettyPrint() => Value.ToString();
}

public record BooleanValue(bool Value) : IEvaluatable
{
    public string Type => PrimitiveTypes.Boolean;

    public string PrettyPrint() => Value.ToString();
}
