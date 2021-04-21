#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <vector>


class heap_builder {
 public:
  void build(std::vector<size_t>& values) {
    swaps_.clear();
    auto count = values.size();
    for (auto idx = 0; idx <= count / 2; ++idx) {
      sift_down(count / 2 - idx, values);
    }
  }

  const std::vector<std::pair<size_t, size_t>> get_swaps() const {
    return swaps_;
  }

 private:
  void sift_down(int idx, std::vector<size_t>& values) {
    const auto size = values.size();
    auto candidate = idx;
    auto left = left_child(idx);
    if (left < size && values[left] < values[candidate]) {
      candidate = left;
    }
    auto right = right_child(idx);
    if (right < size && values[right] < values[candidate]) {
      candidate = right;
    }
    if (candidate != idx) {
      std::swap(values[idx], values[candidate]);
      swaps_.emplace_back(std::make_pair(idx, candidate));
      sift_down(candidate, values);
    }
  }

    static size_t parent(size_t idx) {
      return idx != 0 ? (idx - 1) / 2 : 0;
    }

    static size_t left_child(size_t idx) {
      return 2 * idx + 1;
    }

    static size_t right_child(size_t idx) {
      return 2 * idx + 2;
    }

 private:
  std::vector<std::pair<size_t, size_t>> swaps_;
};

int main() {
  auto count = size_t{0};
  std::cin >> count;
  assert(count >= 0);
  auto values = std::vector<size_t>{};
  values.reserve(count);
  for (auto idx = size_t{0}; idx < count; ++idx) {
    auto value = int{0};
    std::cin >> value;
    values.emplace_back(value);
  }
  auto builder = heap_builder{};
  builder.build(values);
  const auto& swaps = builder.get_swaps();
  std::cout << swaps.size() << '\n';
  for (const auto& item : swaps) {
    std::cout << item.first << ' ' << item.second << '\n';
  }
  return 0;
}