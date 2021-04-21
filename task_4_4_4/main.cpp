#include <cassert>
#include <iostream>
#include <vector>

// Node of the splay tree
// Invariants:
// 1. any key in the left subtree is strictly less than value
// 2. any key in the right subtree is strictly greater than value
// 3. no duplicate keys


template <typename Value>
struct splay_tree_node {

  splay_tree_node(const Value& value)
    : value{value}
    , total{value}
    , parent{nullptr}
    , left{nullptr}
    , right{nullptr}
  {}

  ~splay_tree_node() {
    auto* const node = this;
    node->parent = nullptr;
    node->left = nullptr;
    node->right = nullptr;
    node->total = Value{};
  }

  bool is_root() const {
    const auto* node = this;
    return node->parent == nullptr;
  }

  bool is_left_child() const {
    const auto* node = this;
    return !node->is_root() && node->parent->left == node;
  }

  bool is_right_child() const {
    const auto* node = this;
    return !node->is_root() && node->parent->right == node;
  }

  const splay_tree_node<Value>* find_root() const {
    const auto* node = this;
    if (node == nullptr) {
      return nullptr;
    }
    while (node->parent != nullptr) {
      node = node->parent;
    }
    return node;
  }

  const splay_tree_node<Value>* rightmost_node() const {
    const auto* node = this;
    while (node->right != nullptr) {
      node = node->right;
    }
    return node;
  }

  splay_tree_node<Value>* rightmost_node() {
    const auto* const node = this;
    return const_cast<splay_tree_node<Value>*>(node->rightmost_node());
  }

  const splay_tree_node<Value>* leftmost_node() const {
    const auto* node = this;
    while (node->left != nullptr) {
      node = node->left;
    }
    return node;
  }

  splay_tree_node<Value>* leftmost_node() {
    const auto* const node = this;
    return const_cast<splay_tree_node<Value>*>(node->leftmost_node());
  }

  const splay_tree_node<Value>* next_node() const {
    const auto* node = this;
    auto next = static_cast<const splay_tree_node<Value>*>(nullptr);
    if (node->right != nullptr) {
      next = node->right;
      while (next->left != nullptr) {
        next = next->left;
      }
    } else {
      while (node->parent != nullptr && !node->is_left_child()) {
        node = node->parent;
      }
      next = node->parent;
    }
    return next;
  }

  splay_tree_node<Value>* next_node() {
    const auto* const node = this;
    return const_cast<splay_tree_node<Value>*>(node->next_node());
  }

  const splay_tree_node<Value>* prev_node() const {
    const auto* node = this;
    auto prev = static_cast<const splay_tree_node<Value>*>(nullptr);
    if (node->left != nullptr) {
      prev = node->left;
      while (prev->right != nullptr) {
        prev = prev->right;
      }
    } else {
      while (node->parent != nullptr && !node->is_right_child()) {
        node = node->parent;
      }
      prev = node->parent;
    }
    return prev;
  }

  splay_tree_node<Value>* prev_node() {
    const auto* const node = this;
    return const_cast<splay_tree_node<Value>*>(node->prev_node());
  }

  Value value;
  Value total;
  splay_tree_node<Value>* parent;
  splay_tree_node<Value>* left;
  splay_tree_node<Value>* right;
};

template <typename Value>
std::ostream& operator << (std::ostream& out, const splay_tree_node<Value>& node) {
  out << "[" << node.value << ", " << node.total << "]";
  return out;
}

template <typename Value>
void destroy_node(splay_tree_node<Value>* node) {
  assert(node != nullptr);
  delete node;
}

template <typename Value>
splay_tree_node<Value>* create_node(const Value& value) {
  return new splay_tree_node<Value>(value);
}

template <typename Value>
void print_subtree(std::ostream& out, const splay_tree_node<Value>* root) {
  out << "(";
  if (root != nullptr) {
    print_subtree(out, root->left);
    out << *root;
    print_subtree(out, root->right);
  }
  out << ")";
}

// destroy subtree under the node `root`
template <typename Value>
void destroy_substree(splay_tree_node<Value>* root) {
  if (root == nullptr) {
    return;
  }
  destroy_substree(root->left);
  root->left = nullptr;
  destroy_substree(root->right);
  root->right = nullptr;
  root->parent = nullptr;
  destroy_node(root);
}

template <typename Value>
void update_total(splay_tree_node<Value>* node) {
  if (node != nullptr) {
    node->total = node->value;
    node->total += (node->left != nullptr ? node->left->total : Value{});
    node->total += (node->right != nullptr ? node->right->total : Value{});
  }
}

// insert new node `new_node` to subtree at root `root`, without rebalancing
template <typename Key, typename Value, typename KeyExtractor, typename KeyComparator>
splay_tree_node<Value>* insert_node(
    splay_tree_node<Value>* root,
    const Value& value,
    const KeyExtractor& extractor,
    const KeyComparator& comparator) {
  assert(root != nullptr);
  auto node = static_cast<splay_tree_node<Value>*>(nullptr);
  while (root != nullptr) {
    node = root;
    if (comparator(extractor(value), extractor(root->value))) {
      if (root->left == nullptr) {
        auto new_node = create_node(value);
        root->left = new_node;
        new_node->parent = root;
        node = new_node;
        break;
      } else {
        root = root->left;
      }
    } else if (comparator(extractor(root->value), extractor(value))) {
      if (root->right == nullptr) {
        auto new_node = create_node(value);
        root->right = new_node;
        new_node->parent = root;
        node = new_node;
        break;
      } else {
        root = root->right;
      }
    } else {
      node = nullptr;
      break;
    }
  }
  if (node != nullptr) {
    auto parent = node->parent;
    while (parent != nullptr) {
      parent->total += node->value;
      parent = parent->parent;
    }
  }
  return node;
}

template <typename Value>
void left_rotate_node(splay_tree_node<Value>* node) {
  /* u is node, a is parent, B is branch, p is granny
  *
  *      p             p
  *      |             |
  *      a             u
  *     / \           / \
  *    u   C    =>   A   a
  *   / \               / \
  *  A  B              B  C
  */

  assert(node != nullptr);
  assert(node->parent != nullptr);
  auto parent = node->parent;
  auto branch = node->right;
  auto granny = parent->parent;
  if (granny != nullptr) {
    if (parent->is_left_child()) {
      granny->left = node;
    } else if (parent->is_right_child()) {
      granny->right = node;
    } else {
      assert(false);
    }
  }
  node->parent = granny;
  node->right = parent;
  parent->parent = node;
  parent->left = branch;
  if (branch != nullptr) {
    branch->parent = parent;
  }
  update_total(parent);
  update_total(node);
}

template <typename Value>
void right_rotate_node(splay_tree_node<Value>* node) {
  /* u is node, a is parent, B is branch, p is granny
  *
  *      p             p
  *      |             |
  *      a             u
  *     / \           / \
  *    C   u    =>   a  A
  *       / \       / \
  *      B  A      C  B
  */

  assert(node != nullptr);
  assert(node->parent != nullptr);
  auto parent = node->parent;
  auto branch = node->left;
  auto granny = parent->parent;
  if (granny != nullptr) {
    if (parent->is_left_child()) {
      granny->left = node;
    } else if (parent->is_right_child()) {
      granny->right = node;
    } else {
      assert(false);
    }
  }
  node->parent = granny;
  node->left = parent;
  parent->parent = node;
  parent->right = branch;
  if (branch != nullptr) {
    branch->parent = parent;
  }
  update_total(parent);
  update_total(node);
}

template <typename Value>
void rotate_node(splay_tree_node<Value>* node) {
  if (node->is_left_child()) {
    left_rotate_node(node);
  } else if (node->is_right_child()) {
    right_rotate_node(node);
  } else {
    assert(false);
  }
}

// splay node `node`
template <typename Value>
void splay_node(splay_tree_node<Value>* node) {
  /* ------------------------------------------------------------------------------------
  * zig_zig
  *        p                                                                p
  *        |                                 p                              |
  *        b                                 |                              u
  *       / \                                a                             / \
  *      a   D    left_rotate_node(a)      /   \    left_rotate_node(u)   A   a
  *     / \                              u       b                           / \
  *    u   C                            / \     / \                         B   b
  *   / \                              A   B   C   D                           / \
  *  A   B                                                                    C   D
  * ------------------------------------------------------------------------------------
  * zig_zag
  *    p                             p
  *    |                             |                                p
  *    b                             b                                |
  *   / \                           / \                               u
  *  D   a    left_rotate_node(u)  D   u   right_rotate_node(u)     /   \
  *     / \                           / \                         b       a
  *    u   C                         A   a                       / \     / \
  *   / \                               / \                     D   A   B   C
  *  A   B                             B   C
  * ------------------------------------------------------------------------------------
  * zag_zig
  *       p                                  p
  *       |                                  |                                p
  *       b                                  b                                |
  *      / \                                / \                               u
  *     a   D    right_rotate_node(u)      u   D    left_rotate_node(u)     /   \
  *    / \                                / \                             a       b
  *   C   u                              a   B                           / \     / \
  *      / \                            / \                             C   A   B   D
  *     A   B                          C   A
  * ------------------------------------------------------------------------------------
  * zag_zag
  *    p                                                                  p
  *    |                                p                                 |
  *    b                                |                                 u
  *   / \                               a                                / \
  *  D   a   right_rotate_node(a)     /   \   right_rotate_node(u)      a   B
  *     / \                         b       u                          / \
  *    C   u                       / \     / \                        b   A
  *       / \                     D   C   A   B                      / \
  *      A   B                                                      D   C
  * ------------------------------------------------------------------------------------
  */
  assert(node != nullptr);
  while (node->parent != nullptr) {
    if (node->parent->is_root()) {
      rotate_node(node);
    } else {
      auto zig_zag = node->is_left_child() && node->parent->is_right_child();
      auto zag_zig = node->is_right_child() && node->parent->is_left_child();
      if (zig_zag || zag_zig) {
        rotate_node(node);
        rotate_node(node);
      } else {
        rotate_node(node->parent);
        rotate_node(node);
      }
    }
  }
}

// find node with key `key` in the subtree under node `root`. If such node doesn't exist
// return the last node during this search
template <typename Key, typename Value, typename KeyExtractor, typename KeyComparator>
splay_tree_node<Value>* find_candidate(
    splay_tree_node<Value>* root,
    const Key& key,
    const KeyExtractor& extractor,
    const KeyComparator& comparator) {
  auto node = static_cast<splay_tree_node<Value>*>(nullptr);
  if (root != nullptr) {
    node = root->parent;
    while (root != nullptr) {
      node = root;
      if (comparator(key, extractor(root->value))) {
        root = root->left;
      } else if (comparator(extractor(root->value), key)) {
        root = root->right;
      } else {
        node = root;
        break;
      }
    }
  }
  return node;
}

template <typename Key, typename Value, typename KeyExtractor, typename KeyComparator>
splay_tree_node<Value>* lower_bound(
    splay_tree_node<Value>* root,
    const Key& key,
    const KeyExtractor& extractor,
    const KeyComparator& comparator) {
  auto node = static_cast<splay_tree_node<Value>*>(nullptr);
  while (root != nullptr) {
    if (!comparator(extractor(root->value), key)) {
      node = root;
      root = root->left;
    } else {
      root = root->right;
    }
  }
  return node;
}

template <typename Key, typename Value, typename KeyExtractor, typename KeyComparator>
splay_tree_node<Value>* upper_bound(
    splay_tree_node<Value>* root,
    const Key& key,
    const KeyExtractor& extractor,
    const KeyComparator& comparator) {
  auto node = static_cast<splay_tree_node<Value>*>(nullptr);
  while (root != nullptr) {
    if (comparator(key, extractor(root->value))) {
      node = root;
      root = root->left;
    } else {
      root = root->right;
    }
  }
  return node;
}

template <typename Value>
splay_tree_node<Value>* copy_subtree(const splay_tree_node<Value>* root) {
  if (root == nullptr) {
    return nullptr;
  }
  auto node = create_node(root->value);
  assert(node != nullptr);
  node->total = root->total;
  node->left = copy_subtree(root->left);
  if (node->left != nullptr) {
    node->left->parent = node;
  }
  node->right = copy_subtree(root->right);
  if (node->right != nullptr) {
    node->right->parent = node;
  }
  return node;
}

// merge two subtrees under node `lhs` and `rhs`
// all keys in subtree of `lhs` must be strictly less then any key in subtree of `rhs`
template <typename Value>
splay_tree_node<Value>* merge_subtrees(splay_tree_node<Value>* lhs, splay_tree_node<Value>* rhs) {
  assert(lhs == nullptr || lhs->parent == nullptr);
  assert(rhs == nullptr || rhs->parent == nullptr);
  if (lhs == nullptr) {
    return rhs;
  }
  if (rhs == nullptr) {
    return lhs;
  }
  auto max_lhs = lhs->rightmost_node();
  splay_node(max_lhs);
  assert(max_lhs->right == nullptr);
  max_lhs->right = rhs;
  rhs->parent = max_lhs;
  max_lhs->total += rhs->total;
  return max_lhs;
}

// split root node onto two trees `left` and `right` such that
// any key in `left` is less than `node->value`
// any key in `right` is greater or equal to `node->value`
template <typename Value, typename KeyExtractor, typename KeyComparator>
std::pair<splay_tree_node<Value>*, splay_tree_node<Value>*> split_root(
    splay_tree_node<Value>* root,
    const KeyExtractor& extractor,
    const KeyComparator& comparator) {
  assert(root != nullptr);
  assert(root->parent == nullptr);
  auto left = static_cast<splay_tree_node<Value>*>(nullptr);
  auto right = static_cast<splay_tree_node<Value>*>(nullptr);
  left = root->left;
  right = root;
  // forget relatives
  right->left = nullptr;
  right->parent = nullptr;
  if (left != nullptr) {
    left->parent = nullptr;
    right->total -= left->total;
  }
  return std::make_pair(left, right);
}

// split tree under node `root` onto two trees `left` and `right` such that
// any key in `left` is less than `key`
// any key in `right` is greater or equal to `bound->value`
template <typename Key, typename Value, typename KeyExtractor, typename KeyComparator>
std::pair<splay_tree_node<Value>*, splay_tree_node<Value>*> split_lower_impl(
    splay_tree_node<Value>* root,
    const Key& key,
    const KeyExtractor& extractor,
    const KeyComparator& comparator) {
  auto split = std::pair<splay_tree_node<Value>*, splay_tree_node<Value>*>{nullptr, nullptr};
  if (root != nullptr) {
    auto bound = lower_bound(root, key, extractor, comparator);
    if (bound != nullptr) {
      splay_node(bound);
      split = split_root(bound, extractor, comparator);
    } else {
      split = std::make_pair(root, nullptr);
    }
  }
  return split;
}

// split tree under node `root` onto two trees `left` and `right` such that
// any key in `left` is less than or equal to `key`
// any key in `right` is greater than`bound->value`
template <typename Key, typename Value, typename KeyExtractor, typename KeyComparator>
std::pair<splay_tree_node<Value>*, splay_tree_node<Value>*> split_upper_impl(
    splay_tree_node<Value>* root,
    const Key& key,
    const KeyExtractor& extractor,
    const KeyComparator& comparator) {
  auto split = std::pair<splay_tree_node<Value>*, splay_tree_node<Value>*>{};
  if (root != nullptr) {
    auto bound = upper_bound(root, key, extractor, comparator);
    if (bound != nullptr) {
      splay_node(bound);
      split = split_root(bound, extractor, comparator);
    } else {
      split = std::make_pair(root, nullptr);
    }
  }
  return split;
}

template <typename Key, typename Value, typename KeyExtractor, typename KeyComparator>
struct splay_tree {
  explicit splay_tree(const KeyComparator& comparator = KeyComparator{})
    : root{nullptr}
    , extractor{}
    , comparator{comparator}
  {}

  splay_tree(std::initializer_list<Value> init, const KeyComparator& comparator = KeyComparator{})
    : splay_tree{std::begin(init), std::end(init), comparator}
  {}

  template <typename Iter>
  splay_tree(Iter first, Iter last, const KeyComparator& comparator = KeyComparator{})
    : splay_tree(comparator) {
    for (auto it = first; it != last; ++it) {
      this->insert(*it);
    }
  }

  splay_tree(const splay_tree& other)
    : splay_tree{} {
    this->root = copy_subtree(other.root);
  }

  splay_tree(splay_tree<Key, Value, KeyExtractor, KeyComparator>&& other)
    : splay_tree{} {
    this->swap(other);
  }

  splay_tree<Key, Value, KeyExtractor, KeyComparator>& operator = (
      const splay_tree<Key, Value, KeyExtractor, KeyComparator>& other) {
    if (this != std::addressof(other)) {
      auto temp = splay_tree<Key, Value, KeyExtractor, KeyComparator>{other};
      this->swap(temp);
    }
    return *this;
  }

  splay_tree<Key, Value, KeyExtractor, KeyComparator>& operator = (
     splay_tree<Key, Value, KeyExtractor, KeyComparator>&& other) {
    this->swap(other);
    return *this;
  }

  ~splay_tree() {
    clear();
  }

  splay_tree_node<Value>* find(const Key& key) {
    auto* const tree = this;
    auto node = find_candidate(tree->root, key, tree->extractor, tree->comparator);
    if (node != nullptr) {
      splay_node(node);
      tree->root = node;
      const auto strictly_less = tree->comparator(tree->extractor(node->value), key);
      const auto strictly_greater = tree->comparator(key, tree->extractor(node->value));
      if (strictly_less || strictly_greater) {
        return nullptr;
      }
    }
    return node;
  }

  // insert value `value` into the tree `tree` and rebalance the tree
  splay_tree_node<Value>* insert(const Value& value) {
    auto* const tree = this;
    auto node = static_cast<splay_tree_node<Value>*>(nullptr);
    if (tree->root == nullptr) {
      auto new_node = create_node(value);
      tree->root = new_node;
      new_node->parent = nullptr;
      node = new_node;
    } else {
      node = insert_node<Key, Value, KeyExtractor, KeyComparator>(
        tree->root, value, tree->extractor, tree->comparator);
      if (node != nullptr) {
        splay_node(node);
        tree->root = node;
      }
    }
    return node;
  }

  // erase node `node` from tree `tree`
  splay_tree_node<Value>* erase(splay_tree_node<Value>* node) {
    auto* const tree = this;
    splay_node(node);
    const auto left = node->left;
    if (node->left != nullptr) {
      node->left->parent = nullptr;
    }
    const auto right = node->right;
    if (node->right != nullptr) {
      node->right->parent = nullptr;
    }
    destroy_node(node);
    tree->root = merge_subtrees(left, right);
    return right;
  }

  void split_lower(
      const Value& value,
      splay_tree<Key, Value, KeyExtractor, KeyComparator>& left_tree,
      splay_tree<Key, Value, KeyExtractor, KeyComparator>& right_tree) {
    auto* const tree = this;
    auto trees = split_lower_impl(
      tree->root, tree->extractor(value), tree->extractor, tree->comparator);
    tree->root = nullptr;
    left_tree.clear();
    left_tree.root = trees.first;
    right_tree.clear();
    right_tree.root = trees.second;
  }

  void split_upper(
      const Value& value,
      splay_tree<Key, Value, KeyExtractor, KeyComparator>& left_tree,
      splay_tree<Key, Value, KeyExtractor, KeyComparator>& right_tree) {
    auto* const tree = this;
    auto trees = split_upper_impl(
      tree->root, tree->extractor(value), tree->extractor, tree->comparator);
    tree->root = nullptr;
    left_tree.clear();
    left_tree.root = trees.first;
    right_tree.clear();
    right_tree.root = trees.second;
  }

  void merge(splay_tree<Key, Value, KeyExtractor, KeyComparator>& rhs) {
    auto* const lhs = this;
    lhs->root = merge_subtrees(lhs->root, rhs.root);
    rhs.root = nullptr;
  }

  void swap(splay_tree<Key, Value, KeyExtractor, KeyComparator>& other) {
    auto* const tree = this;
    std::swap(tree->root, other.root);
  }

  void clear() {
    auto* const tree = this;
    destroy_substree(tree->root);
  }

  splay_tree_node<Value>* root;
  KeyExtractor extractor;
  KeyComparator comparator;
};

template <typename Key, typename Value, typename KeyExtractor, typename KeyComparator>
std::ostream& operator << (
    std::ostream& out, const splay_tree<Key, Value, KeyExtractor, KeyComparator>& tree) {
  print_subtree(out, tree.root);
  return out;
}

template <typename T>
class fast_range_summator {
 public:
  fast_range_summator()
    : tree{}
  {}

  void add(const T& number) {
    tree.insert(number);
  }

  void remove(const T& number) {
    auto node = tree.find(number);
    if (node != nullptr) {
      tree.erase(node);
    }
  }

  bool contains(const T& number) {
    auto node = tree.find(number);
    return (node != nullptr);
  }

  T sum(const T& low, const T& high) {
    assert(low <= high);
    auto left_tree = tree_type{};
    auto middle_tree = tree_type{};
    auto right_tree = tree_type{};
    auto middle_right_tree = tree_type{};
    tree.split_lower(low, left_tree, middle_right_tree);
    middle_right_tree.split_upper(high, middle_tree, right_tree);
    const auto total = (middle_tree.root != nullptr ? middle_tree.root->total : T{});
    middle_tree.merge(right_tree);
    left_tree.merge(middle_tree);
    tree.swap(left_tree);
    return total;
  }

 private:

  template <typename Key>
  struct default_key_extractor {
    const Key& operator () (const Key& t) const {
      return t;
    }
  };

  template <typename Key>
  struct default_key_comparator {
    bool operator ()(const Key& lhs, const Key& rhs) const {
      return lhs < rhs;
    }
  };

  using tree_type = splay_tree<T, T, default_key_extractor<T>, default_key_comparator<T>>;

  tree_type tree;
  const bool debug = true;
};

class request_value_generator {
 public:
  request_value_generator(int64_t seed = 0)
    : seed{seed}
  {}

  int64_t get(int64_t value) {
    return (value + this->seed) % 1000000001;
  }

  void set_seed(int64_t seed) {
    this->seed = seed;
  }

 private:
    int64_t seed;
};

enum class summator_request_type : uint8_t {
  add,
  remove,
  find,
  sum
};

struct summator_request {
  summator_request_type type;
  std::vector<int64_t> params;
};

std::vector<summator_request> read_requests(std::istream& in) {
  auto requests_count = size_t{0};
  in >> requests_count;
  auto requests = std::vector<summator_request>{};
  requests.reserve(requests_count);
  for (auto request_id = size_t{0}; request_id < requests_count; ++request_id) {
    auto command = std::string{};
    in >> command;
    auto request = summator_request{summator_request_type::add, std::vector<int64_t>{}};
    if (command == "+") {
      auto value = int64_t{0};
      in >> value;
      request.type = summator_request_type::add;
      request.params.push_back(value);
    } else if (command == "-") {
      auto value = int64_t{0};
      in >> value;
      request.type = summator_request_type::remove;
      request.params.push_back(value);
    } else if (command == "?") {
      auto value = int64_t{0};
      in >> value;
      request.type = summator_request_type::find;
      request.params.push_back(value);
    } else if (command == "s") {
      auto left = int64_t{0};
      in >> left;
      auto right = int64_t{0};
      in >> right;
      request.type = summator_request_type::sum;
      request.params.push_back(left);
      request.params.push_back(right);
    } else {
      assert(false);
    }
    requests.emplace_back(request);
  }
  return requests;
}

int main() {
  auto generator = request_value_generator{};
  auto summator = fast_range_summator<int64_t>{};
  auto requests = read_requests(std::cin);
  for (const auto& request : requests) {
    switch (request.type) {
      case summator_request_type::add: {
        summator.add(generator.get(request.params[0]));
        break;
      }
      case summator_request_type::remove: {
        summator.remove(generator.get(request.params[0]));
        break;
      }
      case summator_request_type::find: {
        if (summator.contains(generator.get(request.params[0]))) {
          std::cout << "Found\n";
        } else {
          std::cout << "Not found\n";
        }
        break;
      }
      case summator_request_type::sum: {
        const auto total = summator.sum(
          generator.get(request.params[0]), generator.get(request.params[1]));
        std::cout << total << '\n';
        generator.set_seed(total);
        break;
      }
      default : {
        assert(false);
        break;
      }
    }
  }
  return 0;
}