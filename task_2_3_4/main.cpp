#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

class disjoint_set_union {
 public:
  disjoint_set_union()
    : parents_{}
    , ranks_{}
  {}

  void init(const size_t size) {
    parents_.resize(size);
    ranks_.resize(size);
    for (auto element = size_t{0}; element < size; ++element) {
      make_set(element);
    }
  }

  size_t find_set(const size_t element) {
    assert(element < parents_.size());
    auto representative = element;
    while (parents_[representative] != representative) {
      representative = parents_[representative];
    }
    auto current = element;
    while (parents_[current] != current) {
      auto next = parents_[current];
      parents_[current] = representative;
      current = next;
    }
    return representative;
  }

  void union_sets(const size_t element_a, const size_t element_b) {
    const auto representative_a = find_set(element_a);
    const auto representative_b = find_set(element_b);
    if (representative_a != representative_b) {
      if (ranks_[representative_a] > ranks_[representative_b]) {
        parents_[representative_b] = representative_a;
      } else {
        if (ranks_[representative_a] == ranks_[representative_b]) {
          ranks_[representative_b] += 1;
        }
        parents_[representative_a] = representative_b;
       }
    }
  }

  size_t size() const {
    return parents_.size();
  }

 private:
  void make_set(const size_t element) {
    parents_[element] = element;
    ranks_[element] = 0;
  }

 private:
  std::vector<size_t> parents_;
  std::vector<size_t> ranks_;
};

int main() {
  auto variables_count = size_t{0};
  std::cin >> variables_count;
  auto equalities_count = size_t{0};
  std::cin >> equalities_count;
  auto disequalities_count = size_t{0};
  std::cin >> disequalities_count;
  auto dsu = disjoint_set_union{};
  dsu.init(variables_count);
  for (auto equality_id = size_t{0}; equality_id < equalities_count; ++equality_id) {
    auto element_a_no = size_t{0};
    std::cin >> element_a_no;
    assert(1 <= element_a_no && element_a_no <= dsu.size());
    auto element_b_no = size_t{0};
    std::cin >> element_b_no;
    assert(1 <= element_b_no && element_b_no <= dsu.size());
    dsu.union_sets(element_a_no - 1, element_b_no - 1);
  }
  auto consistent = size_t{1};
  for (auto disequality_id = size_t{0}; disequality_id < disequalities_count; ++disequality_id) {
    auto element_a_no = size_t{0};
    std::cin >> element_a_no;
    assert(1 <= element_a_no && element_a_no <= dsu.size());
    auto element_b_no = size_t{0};
    std::cin >> element_b_no;
    assert(1 <= element_b_no && element_b_no <= dsu.size());
    if (dsu.find_set(element_a_no - 1) == dsu.find_set(element_b_no -1)) {
      consistent = 0;
    }
  }
  std::cout << consistent;
  return 0;
}