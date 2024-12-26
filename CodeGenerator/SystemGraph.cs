namespace SystemGraphResolver;

internal record CompactSystemNode(string Name, int Id, int CountChildren, IEnumerable<int> Parents);

internal record SystemGraph(CompactSystemNode[] Nodes)
{
    public static SystemGraph Create(SystemMetadata[] nodes)
	{
		(var outgoingEdges, var incomingEdges) = LabelEdges(PopulateConflicts(nodes).ToList());
		IEnumerable<CompactSystemNode> DrawGraph()
		{
            for (int i = 0; i < nodes.Length; i++)
            {
				yield return new CompactSystemNode(nodes[i].Name, nodes[i].Id, outgoingEdges[i].Count, incomingEdges[i]);
            }
        }
		return new SystemGraph(DrawGraph().ToArray());
	}

	private static IEnumerable<HashSet<int>> PopulateConflicts(SystemMetadata[] nodes) =>
		nodes.Select((nodeLeft, indexLeft) =>
			nodes.Select((nodeRight, indexRight) => (nodeRight, indexRight))
			.Where(pairRight => pairRight.indexRight > indexLeft)
			.Where(pairRight =>
				nodeLeft.Writes.Intersect(pairRight.nodeRight.Reads).Any()
				|| nodeLeft.Writes.Intersect(pairRight.nodeRight.Writes).Any()
				|| pairRight.nodeRight.Writes.Intersect(nodeLeft.Reads).Any())
			.Select(pairRight => pairRight.indexRight)
			.ToHashSet());

	private static (List<HashSet<int>>, List<HashSet<int>>) LabelEdges(List<HashSet<int>> conflicts)
	{
		List<HashSet<int>> outgoingEdges = [];
		List<HashSet<int>> incomingEdges = [];
		List<HashSet<int>> consolidatedLinks = [];

		foreach (int _ in Enumerable.Range(0, conflicts.Count))
		{
			outgoingEdges.Add([]);
			incomingEdges.Add([]);
			consolidatedLinks.Add([]);
		}

		HashSet<int> visited = [];
		Stack<int> toVisit = [];

		foreach (int i in Enumerable.Range(0, conflicts.Count))
		{
			toVisit.Push(i);
		}

		while (toVisit.Count > 0)
		{
			int nextNode = toVisit.Pop();
			if (visited.Contains(nextNode))
				continue;

			visited.Add(nextNode);

			if (outgoingEdges[nextNode].Count >= incomingEdges[nextNode].Count)
			{
				foreach (int neighbour in conflicts[nextNode].Where(x => !consolidatedLinks[nextNode].Contains(x)))
				{
					outgoingEdges[nextNode].Add(neighbour);
					incomingEdges[neighbour].Add(nextNode);

					consolidatedLinks[nextNode].Add(neighbour);
					consolidatedLinks[neighbour].Add(nextNode);

					toVisit.Push(neighbour);
				}
			}
			else
			{
				foreach (int neighbour in conflicts[nextNode].Where(x => !consolidatedLinks[nextNode].Contains(x)))
				{
					incomingEdges[nextNode].Add(neighbour);
					outgoingEdges[neighbour].Add(nextNode);

					consolidatedLinks[nextNode].Add(neighbour);
					consolidatedLinks[neighbour].Add(nextNode);

					toVisit.Push(neighbour);
				}
			}
		}

		return (outgoingEdges, incomingEdges);
	}
}
