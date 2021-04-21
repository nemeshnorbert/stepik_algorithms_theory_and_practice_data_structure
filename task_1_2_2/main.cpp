#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

struct tree_node {
  int parent_id = -1;
  std::vector<int> children;
};

int get_tree_root(const std::vector<tree_node>& tree) {
  auto root_id = int{0};
  auto parent_id = tree[root_id].parent_id;
  while (parent_id != -1) {
    root_id = parent_id;
    parent_id = tree[root_id].parent_id;
  }
  return root_id;
}


int get_tree_height_impl(int root_id, const std::vector<tree_node>& tree) {
  assert(0 <= root_id && root_id < tree.size());
  auto height = int{0};
  if (tree[root_id].children.empty()) {
    height = 1;
  } else {
    auto max_subtree_height = std::numeric_limits<int>::min();
    for (const auto& child_id : tree[root_id].children) {
      max_subtree_height = std::max(
        max_subtree_height, get_tree_height_impl(child_id, tree));
    }
    height = 1 + max_subtree_height;
  }
  return height;
}

int get_tree_height(const std::vector<tree_node>& tree) {
  auto root_id = get_tree_root(tree);
  return get_tree_height_impl(root_id, tree);
}


int main() {
  auto nodes_count = int{0};
  std::cin >> nodes_count;
  auto tree = std::vector<tree_node>(nodes_count);
  for (auto node_id = int{0}; node_id < nodes_count; ++node_id) {
    auto parent_id = int{0};
    std::cin >> parent_id;
    tree[node_id].parent_id = parent_id;
    if (parent_id != -1) {
      tree[parent_id].children.push_back(node_id);
    }
  }
  std::cout << get_tree_height(tree);
  return 0;
}
