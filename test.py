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


graph = {0x2F: [4], 0x4: [3, 0x2E], 0x2E: [0x2D], 0x2D: [0x2C], 0x2C: [4], 0x3: [0x1], 1: [0, 2], 2: [1]}
print(reverse_postorder(graph, 0x2F))
