import networkx as nx
import matplotlib.pyplot as plt
from networkx.drawing.nx_agraph import graphviz_layout
from graphviz import Digraph
from IPython.display import Image
import networkx as nx

graph = {
    0: {14},
    1: {0, 12},
    2: {1},
    3: {1},
    4: {3},
    5: {3},
    6: {3},
    7: {3},
    8: {4, 13},
    9: {5, 10},
    10: {6, 11},
    11: {6, 7},
    12: {8, 13},
    13: {9, 10},
    14: {},
}

edge_list = [(src, dst) for src, dsts in graph.items() for dst in dsts]


G = nx.DiGraph()
G.add_edges_from(edge_list)
G = G.reverse()
print(nx.immediate_dominators(G, start=14))

dot = Digraph()
for src, dsts in graph.items():
    dot.node(str(src))
    for dst in dsts:
        dot.edge(str(src), str(dst))

dot.format = "png"
dot.render("cfg", cleanup=True)
Image(filename="cfg.png")
