#include <vector>
#include <cstdint>
#include <unordered_map>
#include <iostream>
#include <limits>
#include <functional>
#include <algorithm>
#include <unordered_map>

using node_id = uint16_t;
using u32 = uint32_t;

using node_set = std::vector<bool>;


struct control_flow_node {
    std::vector<node_id> m_predecessors;
    std::vector<node_id> m_successors;
    node_id m_directSuccessor = 0;
    node_id m_targetSuccessor = 0;
    node_id m_postorder;
    node_id m_startLine = 0;
    node_id m_endLine = 0;
    node_id m_index = 0;
    node_id m_ipdom = 0;
};


[[nodiscard]] const control_flow_node& intersect(const node_id node_b1, const node_id node_b2, 
    const std::vector<control_flow_node>& nodes, const std::unordered_map<node_id, u32>& dfn) {
    const control_flow_node* b1 = &nodes[node_b1];
    const control_flow_node* b2 = &nodes[node_b2];
    std::cout << "doing intersection for u = " << node_b1 << " v = " << node_b2 << '\n';
    while (b1->m_index != b2->m_index) {
        while (b1->m_postorder < b2->m_postorder) {
            b1 = &nodes[b1->m_ipdom];
        }
        while (b2->m_postorder < b1->m_postorder) {
            b2 = &nodes[b2->m_ipdom];
        }
    }
    return *b1;
}

[[nodiscard]] std::vector<node_id> postorder(const std::vector<control_flow_node>& nodes) {
    std::vector<node_id> result;
    const u32 size = nodes.size();
    result.reserve(size);
    node_set visited(size, false);

    std::vector<std::pair<node_id, size_t>> stack;
    stack.emplace_back(nodes.back().m_index, 0);
    u32 i = 0;
    while (!stack.empty()) {
        auto [n, i] = stack.back();
        if (!visited[n]) {
            visited[n] = true;
        }

        const auto& preds = nodes[n].m_predecessors;
        if (i < preds.size()) {
            stack.back().second++;
            const auto p = preds[i];
            if (!visited[p]) {
                stack.emplace_back(p, 0);
            }
        }
        else {
            result.push_back(n);
            stack.pop_back();
        }
    }
    return result;
}

std::unordered_map<node_id, node_id> compute_postdominators(std::vector<control_flow_node>& m_nodes) {
    std::vector<node_id> rev_postord = postorder(m_nodes);
    static constexpr node_id UNDEF = std::numeric_limits<node_id>::max();
	std::unordered_map<node_id, u32> dfn;
    for (u32 i = 0; i < m_nodes.size(); ++i) {
        m_nodes[rev_postord[i]].m_postorder = i;
    }
    rev_postord.pop_back();
    std::reverse(rev_postord.begin(), rev_postord.end());
    const u32 N = rev_postord.size();
    std::unordered_map<node_id, node_id> ipdom;
    for (auto n : rev_postord) {
        ipdom[n] = UNDEF;
    }
    ipdom[m_nodes.back().m_index] = m_nodes.back().m_index;
    bool changed = true;
    while (changed) {
        changed = false;
        for (u32 i = 0; i < N; ++i) {
            node_id n = rev_postord[i];
            std::cout << "proc " << n << '\n';
            node_id new_ipdom = UNDEF;
            const control_flow_node& node = m_nodes.at(n);
            for (const auto& p : node.m_successors) {
                std::cout << " pred: " << p;
                if (ipdom[p] != UNDEF) {
                    new_ipdom = p;
                    break;
                }
            }
            std::cout << '\n';
            if (new_ipdom == UNDEF) {
                continue;
            }
            for (const auto& p : node.m_successors) {
                if (p != new_ipdom && ipdom[p] != UNDEF) {
                    new_ipdom = intersect(p, new_ipdom, m_nodes, dfn).m_index;
                }
            }

            if (ipdom.at(n) != new_ipdom) {
                ipdom[n] = new_ipdom;
                m_nodes.at(n).m_ipdom = new_ipdom;
                changed = true;
            }
        }
    }
    return ipdom;
}

int main() {
    std::vector<control_flow_node> nodes(15);

    for (node_id i = 0; i < nodes.size(); ++i)
        nodes[i].m_index = i;

    nodes[0].m_successors  = {14};
    nodes[1].m_successors  = {0, 12};
    nodes[2].m_successors  = {1};
    nodes[3].m_successors  = {1};
    nodes[4].m_successors  = {3};
    nodes[5].m_successors  = {3};
    nodes[6].m_successors  = {3};
    nodes[7].m_successors  = {3};
    nodes[8].m_successors  = {4, 13};
    nodes[9].m_successors  = {5, 10};
    nodes[10].m_successors = {6, 11};
    nodes[11].m_successors = {6, 7};
    nodes[12].m_successors = {8, 13};
    nodes[13].m_successors = {9, 10};

    nodes[0].m_predecessors  = {1};
    nodes[1].m_predecessors  = {2, 3};
    nodes[2].m_predecessors  = {};
    nodes[3].m_predecessors  = {4, 5, 6, 7};
    nodes[4].m_predecessors  = {8};
    nodes[5].m_predecessors  = {9};
    nodes[6].m_predecessors  = {10, 11};
    nodes[7].m_predecessors  = {11};
    nodes[8].m_predecessors  = {12};
    nodes[9].m_predecessors  = {13};
    nodes[10].m_predecessors = {9, 13};
    nodes[11].m_predecessors = {10};
    nodes[12].m_predecessors = {1};
    nodes[13].m_predecessors = {8, 12};
    nodes[14].m_predecessors = {0};

    auto res = compute_postdominators(nodes);
    for (const auto& [a, b] : res) {
        std::cout << "node " << a << " ipdom " << b << "\n";
    }
}