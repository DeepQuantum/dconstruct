import networkx as nx
import matplotlib.pyplot as plt
from networkx.drawing.nx_agraph import graphviz_layout


def reverse_postorder(graph, start):
    visited = set()
    postorder = []

    def dfs(node):
        if node in visited:
            return
        visited.add(node)
        for succ in graph.get(node, []):
            dfs(succ)
        postorder.append(node)

    dfs(start)
    postorder.reverse()
    return postorder


graph = {
    "A": ["B"],
    "B": ["C", "H"],
    "C": ["D", "E"],
    "D": ["F"],
    "E": ["F"],
    "F": ["G"],
    "G": ["B"],
}

G = nx.DiGraph()
for src, dsts in graph.items():
    for dst in dsts:
        G.add_edge(src, dst)

G.nodes["H"]["rank"] = "sink"

pos = graphviz_layout(G, prog="dot")

plt.figure(figsize=(8, 7))
nx.draw(G, pos, with_labels=True, arrows=True, node_size=1600, font_size=12)
plt.show()
