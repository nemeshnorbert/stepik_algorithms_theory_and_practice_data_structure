#include <cassert>
#include <iostream>
#include <vector>

struct tree_node {
  int key;
  int left;
  int right;
};

void in_order_printer(int node_id, const std::vector<tree_node>& nodes, std::ostream& out) {
  if (node_id == -1) {
    return;
  }
  in_order_printer(nodes[node_id].left, nodes, out);
  out << nodes[node_id].key << ' ';
  in_order_printer(nodes[node_id].right, nodes, out);
}

void pre_order_printer(int node_id, const std::vector<tree_node>& nodes, std::ostream& out) {
  if (node_id == -1) {
    return;
  }
  out << nodes[node_id].key << ' ';
  pre_order_printer(nodes[node_id].left, nodes, out);
  pre_order_printer(nodes[node_id].right, nodes, out);
}

void post_order_printer(int node_id, const std::vector<tree_node>& nodes, std::ostream& out) {
  if (node_id == -1) {
    return;
  }
  post_order_printer(nodes[node_id].left, nodes, out);
  post_order_printer(nodes[node_id].right, nodes, out);
  out << nodes[node_id].key << ' ';
}

int main() {
  auto node_count = size_t{0};
  std::cin >> node_count;
  auto nodes = std::vector<tree_node>{};
  nodes.reserve(node_count);
  for (auto node_id = size_t{0}; node_id < node_count; ++node_id) {
    auto node = tree_node{};
    std::cin >> node.key >> node.left >> node.right;
    nodes.emplace_back(node);
  }
  in_order_printer(0, nodes, std::cout);
  std::cout << '\n';
  pre_order_printer(0, nodes, std::cout);
  std::cout << '\n';
  post_order_printer(0, nodes, std::cout);
  return 0;
};