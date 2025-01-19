using System.Collections.Immutable;

namespace SemanticMachine.Grammar.Interpretation;

public record Int32Type() : TypeDefinition("int", ImmutableDictionary<string, TypeDefinition>.Empty, ImmutableList<string>.Empty);
public record BooleanType() : TypeDefinition("bool", ImmutableDictionary<string, TypeDefinition>.Empty, ImmutableList<string>.Empty);

public static partial class PrimitiveTypes
{
    public static readonly TypeDefinition Int32 = new Int32Type();
    public static readonly TypeDefinition Boolean = new BooleanType();
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
        prefix.Add(PrimitiveTypes.Int32.Name, PrimitiveTypes.Int32).Add(PrimitiveTypes.Boolean.Name, PrimitiveTypes.Boolean);

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
        _ => throw new NotImplementedException()
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

    protected static TypeDefinition OperatorReturnType(ArithmeticOperation operation) => operation switch
    {
        ArithmeticOperation.Add or ArithmeticOperation.Minus or ArithmeticOperation.Mult or ArithmeticOperation.Divide or ArithmeticOperation.BitwiseOr or ArithmeticOperation.BitwiseAnd => PrimitiveTypes.Int32,
        ArithmeticOperation.LogicAnd or ArithmeticOperation.LogicOr or ArithmeticOperation.Equals => PrimitiveTypes.Boolean,
        _ => throw new Exception("why the fuck is there an unexpected binary operation type?")
    };

    bool SanityCheckType();
}

public class LeafArithmetic(IEvaluatable value) : IArithmetic
{
    public TypeDefinition Type => value.Type;
    public IEvaluatable Value => value;

    public IEnumerable<LazyEvaluatable> ArgumentCaptures => value.ArgumentCaptures;

    public bool SanityCheckType() => true;
}

public record NodeArithmetic(IArithmetic LeftChild, IArithmetic RightChild, ArithmeticOperation Operation) : IArithmetic
{
    public TypeDefinition Type => IArithmetic.OperatorReturnType(Operation);

    public IEnumerable<LazyEvaluatable> ArgumentCaptures => [.. LeftChild.ArgumentCaptures, .. RightChild.ArgumentCaptures];

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
    public TypeDefinition Type => PrimitiveTypes.Int32;

    public IEnumerable<LazyEvaluatable> ArgumentCaptures => [];
}

public record BooleanValue(bool Value) : IEvaluatable
{
    public TypeDefinition Type => PrimitiveTypes.Boolean;

    public IEnumerable<LazyEvaluatable> ArgumentCaptures => [];
}
