namespace SemanticMachine;
internal class LispSynthesizer
{
    public static HashSet<string> s_binops = ["+", "-", "*", "/"];

    private abstract record ConstructorState
    {
        public virtual IEnumerable<object> Initialize(IEnumerable<object> result, object instigator) => result;
        public abstract (ConstructorState, IEnumerable<object>) OnNext(IEnumerable<object> result, object token);
        protected static (ConstructorState, IEnumerable<object>) JumpState<T>(T newState, IEnumerable<object> result, object instigator) where T : ConstructorState =>
            (newState, newState.Initialize(result, instigator));
    }

    private record Appender(ConstructorState? Owner = null) : ConstructorState
    {
        private ConstructorState SelectOwenr() => Owner ?? this;

        public override (ConstructorState, IEnumerable<object>) OnNext(IEnumerable<object> result, object token) => 
            token switch
            {
                "." => JumpState(new FunctionPrimer(SelectOwenr()), result, token),
                "(" => JumpState(new Invocation(SelectOwenr(), result.SkipLast(1)), result, token),
                string possibleBinop => s_binops.Contains(token) ? 
                    JumpState(new BinopAppender(SelectOwenr(), possibleBinop), result, possibleBinop) 
                        : (SelectOwenr(), result.Append(token)),
                _ => (SelectOwenr(), result.Append(token))
            };
    }

    private record FunctionPrimer(ConstructorState Parent) : ConstructorState
    {
        public override (ConstructorState, IEnumerable<object>) OnNext(IEnumerable<object> result, object token) => 
            (Parent, [..result.SkipLast(1), (object[])[token, result.Last()]]);
    }

    private record BinopAppender(ConstructorState Parent, string Operator) : ConstructorState
    {
        public override (ConstructorState, IEnumerable<object>) OnNext(IEnumerable<object> input, object token) => 
            (Parent, [.. input.SkipLast(1), (IEnumerable<object>)[Operator, input.Last(), token]]);
    }

    private record Invocation(ConstructorState Parent, IEnumerable<object> ParentStack) : ConstructorState
    {
        private Appender GetInner() => new(this);

        private (ConstructorState, IEnumerable<object>) ExecuteInner(IEnumerable<object> result, object token) =>
            GetInner().OnNext(result, token);

        private static IEnumerable<object> ExpandCaller(object head, IEnumerable<object> stack) =>
            head switch
            {
                IEnumerable<object> list => [.. list, .. stack],
                object obj => [obj, .. stack]
            };

        public override IEnumerable<object> Initialize(IEnumerable<object> result, object instigator) =>
            !result.Any() ? throw new Exception("Missing subject before open parenthesis")
            : instigator switch
            {
                "(" => ExpandCaller(result.Last(), []),
                _ => throw new Exception($"Unexpected Invocation instigator: {instigator}")
            };

        public override (ConstructorState, IEnumerable<object>) OnNext(IEnumerable<object> result, object token) =>
            token switch
            {
                "(" => JumpState(new Invocation(this, result.SkipLast(1)), result, token),
                ")" => (Parent, ParentStack.Append(result)),
                string _ => ExecuteInner(result, token),
                _ => ExecuteInner(result, token)
            };
    }

    /// <summary>
    /// Turn a string of tokens into lisp format
    /// </summary>
    /// <param name="tokens"></param>
    public static IEnumerable<object> Synthesize(List<string> tokens)
    {
        ConstructorState state = new Appender();
        IEnumerable<object> progress = [];

        foreach (string token in tokens)
        {
            (state, progress) = state.OnNext(progress, token);
        }

        return progress;
    }

    public static IEnumerable<string> PrettyPrint(object lisp) => 
        lisp switch
        {
            IEnumerable<object> listObj => listObj.SelectMany<object, string>((token, i) => i == 0 ? [.. PrettyPrint(token)] : ["; ", .. PrettyPrint(token)]).Prepend("(").Append(")"),
            _ => [(string)lisp]
        };
}
