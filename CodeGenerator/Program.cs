using SystemGraphResolver;

Console.WriteLine("Main routine start.");

const string UserDefinedPath = "C:\\Repos\\SigourneyEngine\\SigourneyEngine";
await UserDefinedContentParser.LoadDirectory(UserDefinedPath).ConfigureAwait(false);

Console.WriteLine("Main routine end.");