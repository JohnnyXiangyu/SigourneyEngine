using System.Text;

namespace SystemGraphResolver;

internal enum ParamType
{
    Write,
    Read,
    Invalid
}

internal record ParameterInfo(string Name, string Resource, ParamType Type)
{
    public static ParameterInfo Invalid { get; } = new(string.Empty, string.Empty, ParamType.Invalid);
}

internal record SystemMetadata(string Name, int Id, IEnumerable<int> Reads, IEnumerable<int> Writes)
{
    public override string ToString()
    {
        StringBuilder sb = new();
        sb.Append(Name);
        sb.Append(" r=");
        foreach (int res in Reads)
        {
            sb.Append(res);
            sb.Append(',');
        }

        sb.Append(" w=");
        foreach (int res in Writes)
        {
            sb.Append(res);
            sb.Append(',');
        }

        return sb.ToString();
    }
}
