import networkx as nx
import matplotlib.pyplot as plt
from networkx.drawing.nx_agraph import graphviz_layout

G: nx.MultiDiGraph = nx.MultiDiGraph()

with open("manual_graph.txt") as f:
    mode = None
    for line in f:
        line = line.strip()
        if not line or line.startswith("#"):
            if "nodes" in line:
                mode = "nodes"
            elif "edges" in line:
                mode = "edges"
            continue
        if mode == "nodes":
            parts = line.split(maxsplit=1)
            node = parts[0]
            text = parts[0]
            G.add_node(node, text=text)
        elif mode == "edges":
            u, v = line.split()
            G.add_edge(u, v)


pos = graphviz_layout(G, prog="dot")
labels = nx.get_node_attributes(G, "text")

plt.figure(figsize=(7, 7))
nx.draw(G, pos, labels=labels, with_labels=True, node_color="lightblue", node_size=5000, font_size=8, font_color="black")
plt.show()
