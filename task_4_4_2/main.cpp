#include <cassert>
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

struct tree_node {
  int32_t key;
  int left;
  int right;
};

struct search_tree_check_result {
  bool is_search_tree;
  int64_t min_key;
  int64_t max_key;
};

search_tree_check_result is_search_tree_impl(int node_id, const std::vector<tree_node>& nodes) {
  constexpr auto kMin = std::numeric_limits<int64_t>::min();
  constexpr auto kMax = std::numeric_limits<int64_t>::max();
  if (node_id == int{-1}) {
    return search_tree_check_result{true, kMax, kMin};
  }
  const auto node_key = static_cast<int64_t>(nodes[node_id].key);
  const auto left_node_id = nodes[node_id].left;
  auto left_tree_check_result = is_search_tree_impl(left_node_id, nodes);
  const auto right_node_id = nodes[node_id].right;
  auto right_tree_check_result = is_search_tree_impl(right_node_id, nodes);
  const auto min_key = std::min({
    node_key, left_tree_check_result.min_key, right_tree_check_result.min_key});
  const auto max_key = std::max({
    node_key, left_tree_check_result.max_key, right_tree_check_result.max_key});
  const auto is_search_tree = bool{
    (left_tree_check_result.max_key < node_key) &&
    (node_key <= right_tree_check_result.min_key) &&
    left_tree_check_result.is_search_tree &&
    right_tree_check_result.is_search_tree};
  return search_tree_check_result{is_search_tree, min_key, max_key};
}

bool is_search_tree(const std::vector<tree_node>& nodes) {
  if (nodes.empty()) {
    return true;
  }
  const auto check_result = is_search_tree_impl(0, nodes);
  return check_result.is_search_tree;
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
  if (is_search_tree(nodes)) {
    std::cout << "CORRECT";
  } else {
    std::cout << "INCORRECT";
  }
  return 0;
};