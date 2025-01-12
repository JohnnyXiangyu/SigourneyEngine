namespace SemanticMachine.Binding;
public class VarNameDistributor
{
    private int _counter;

    public string GetName(string prefix = "") => $"{prefix}_var_{_counter++}";
}
