using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Interpretation;

public static partial class PrimitiveTypes
{
    public const string Int32 = "int32";
    public const string Boolean = "bool";
}

public enum ArithmeticOperation
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

public interface IArithmetic : IEvaluatable
{
    public static ImmutableDictionary<string, ISemanticUnit> LoadArithmeticPrimitives(ImmutableDictionary<string, ISemanticUnit> prefix) =>
        prefix.Add(PrimitiveTypes.Int32, new PrimitiveType(PrimitiveTypes.Int32)).Add(PrimitiveTypes.Boolean, new PrimitiveType(PrimitiveTypes.Boolean));

    public static ArithmeticOperation DeserializeOperation(string operation) => operation switch
    {
        "+" => ArithmeticOperation.Add,
        "-" => ArithmeticOperation.Minus,
        "*" => ArithmeticOperation.Mult,
        "/" => ArithmeticOperation.Divide,
        "&" => ArithmeticOperation.BitwiseAnd,
        "|" => ArithmeticOperation.BitwiseOr,
        "&&" => ArithmeticOperation.LogicAnd,
        "||" => ArithmeticOperation.LogicOr,
        "=" => ArithmeticOperation.Equals,
        _ => throw new Exception($"unexpected binary operation token: {operation}")
    };

    public static int ArithmeticOpAltitude(ArithmeticOperation op) => op switch
    {
        ArithmeticOperation.Add => 1,
        ArithmeticOperation.Minus => 1,
        ArithmeticOperation.Mult => 2,
        ArithmeticOperation.Divide => 2,
        ArithmeticOperation.BitwiseAnd => 3,
        ArithmeticOperation.BitwiseOr => 3,
        ArithmeticOperation.LogicAnd => -1,
        ArithmeticOperation.LogicOr => -1,
        ArithmeticOperation.Equals => 0,
        _ => throw new Exception("unexpected arithmetic operation, IArithmetic.ArithmeticOpAltitude"),
    };

    public static int ArithmeticOpAltitude(string op) => ArithmeticOpAltitude(DeserializeOperation(op));

    public static bool ValidityCheck(string ltype, string rtype, ArithmeticOperation operation) =>
    operation switch
    {
        ArithmeticOperation.Add or ArithmeticOperation.Minus or ArithmeticOperation.Mult or ArithmeticOperation.Divide or ArithmeticOperation.BitwiseAnd or ArithmeticOperation.BitwiseOr => ltype == rtype && ltype == PrimitiveTypes.Int32,
        ArithmeticOperation.LogicAnd or ArithmeticOperation.LogicOr => ltype == rtype && ltype == PrimitiveTypes.Boolean,
        ArithmeticOperation.Equals => ltype == rtype && (ltype == PrimitiveTypes.Boolean || ltype == PrimitiveTypes.Int32),
        _ => throw new Exception("why the fuck is there an unexpected binary operation type?")
    };

    protected static string OperatorReturnType(ArithmeticOperation operation) => operation switch
    {
        ArithmeticOperation.Add or ArithmeticOperation.Minus or ArithmeticOperation.Mult or ArithmeticOperation.Divide or ArithmeticOperation.BitwiseOr or ArithmeticOperation.BitwiseAnd => PrimitiveTypes.Int32,
        ArithmeticOperation.LogicAnd or ArithmeticOperation.LogicOr or ArithmeticOperation.Equals => PrimitiveTypes.Boolean,
        _ => throw new Exception("why the fuck is there an unexpected binary operation type?")
    };

    bool SanityCheckType();
}

public class LeafArithmetic(IEvaluatable value) : IArithmetic
{
    public string Type => value.Type;
    public IEvaluatable Value => value;

    public string PrettyPrint() => Value.PrettyPrint();

    public bool SanityCheckType() => true;
}

public record NodeArithmetic(IArithmetic LeftChild, IArithmetic RightChild, ArithmeticOperation Operation) : IArithmetic
{
    public string Type => IArithmetic.OperatorReturnType(Operation);

    public string PrettyPrint() => $"({Operation} {LeftChild.PrettyPrint()} {RightChild.PrettyPrint()})";

    public bool SanityCheckType() => Operation switch
    {
        ArithmeticOperation.Add or ArithmeticOperation.Minus or ArithmeticOperation.Mult or ArithmeticOperation.Divide or ArithmeticOperation.BitwiseAnd or ArithmeticOperation.BitwiseOr => LeftChild.Type == PrimitiveTypes.Int32 && RightChild.Type == PrimitiveTypes.Int32,
        ArithmeticOperation.LogicAnd or ArithmeticOperation.LogicOr => LeftChild.Type == PrimitiveTypes.Boolean && RightChild.Type == PrimitiveTypes.Boolean,
        ArithmeticOperation.Equals => LeftChild.Type == RightChild.Type,
        _ => false
    };
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
