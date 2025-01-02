using ParserFramework;
using SemanticMachine.Grammar.Symbols.Expression;
using System.Reflection;

Console.WriteLine("Hello, World!");

var assembly = Assembly.GetCallingAssembly();
var targetTypes = typeof(Expr).Assembly.GetTypes().Where(type => type.GetInterfaces().Contains(typeof(INonTerminal)));

foreach (var targetType in targetTypes)
{
    Console.WriteLine(targetType.Name);
}
