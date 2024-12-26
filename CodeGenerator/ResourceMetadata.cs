namespace SystemGraphResolver;
internal record ResourceMetadata(string Name, int Id)
{
	public override string ToString() => $"{Name}={Id}";
}
