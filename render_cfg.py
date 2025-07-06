import networkx as nx

G: nx.MultiDiGraph = nx.MultiDiGraph()

return_node_idx = ""

with open("build/graph.txt") as f:
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
            text = parts[1].replace(";", "<BR/>")
            G.add_node(node, text=text)
            if "Return" in text:
                return_node_idx = node
        elif mode == "edges":
            u, v = line.split()
            G.add_edge(u, v)

labels = nx.get_node_attributes(G, "text")
pos = nx.nx_agraph.graphviz_layout(G, prog="dot")
A = nx.nx_agraph.to_agraph(G)
A.add_subgraph([return_node_idx], rank="max")

A.graph_attr.update(bgcolor="#0F0F0F", rankdir="TB", splines="ortho")
A.node_attr.update(fontcolor="#8ADCFE", style="filled")
A.edge_attr.update(color="white")
for n in A.nodes():
    text = G.nodes[n]["text"]
    n.attr[
        "label"
    ] = f"""<
<TABLE BORDER="1" CELLBORDER="1" CELLSPACING="0" CELLPADDING="10">
  <TR><TD ALIGN="LEFT" BALIGN="LEFT"><FONT FACE="monospace">{text}</FONT></TD></TR>
</TABLE>
>"""
    n.attr["fillcolor"] = "#0F0F0F"
    n.attr["fontsize"] = "12"
    n.attr["shape"] = "plaintext"
    n.attr["color"] = "#8ADCFE"  # magenta border color

A.layout(prog="dot")
A.draw("output.svg")
