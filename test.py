import networkx as nx
import matplotlib.pyplot as plt
from networkx.drawing.nx_agraph import graphviz_layout
from graphviz import Digraph
from IPython.display import Image
import networkx as nx

graph = {
    0: {1},
    1: {2, 3},
    2: set(),
    3: {4, 5, 6, 7},
    4: {8},
    5: {9},
    6: {10, 11},
    7: {11},
    8: {12},
    9: {13},
    10: {9, 13},
    11: {10},
    12: {1},
    13: {8, 12},
}

edge_list = [(dst, src) for src, dsts in graph.items() for dst in dsts]


G = nx.DiGraph()
G.add_edges_from(edge_list)
print(list(reversed(list(nx.dfs_postorder_nodes(G, source=0)))))
print(nx.immediate_dominators(G, start=12))

dot = Digraph()
for src, dsts in graph.items():
    dot.node(str(src))
    for dst in dsts:
        dot.edge(str(src), str(dst))

dot.format = "png"
dot.render("cfg", cleanup=True)
Image(filename="cfg.png")
