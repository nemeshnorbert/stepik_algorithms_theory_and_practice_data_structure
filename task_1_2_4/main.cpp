#include <cassert>
#include <iostream>
#include <vector>

// Max stack
template <typename T, typename Comparator = std::less<T>>
class priority_stack {
 public:
  void push(const T& value) {
    assert(values_.size() == priorities_.size());
    values_.emplace_back(value);
    if (!priorities_.empty()) {
      const T& priority_now = priorities_.back();
      if (comp_(priority_now, value)) {
        priorities_.emplace_back(value);
      } else {
        priorities_.emplace_back(priority_now);
      }
    } else {
      priorities_.emplace_back(value);
    }
  }

  const T& top_priority() const {
    assert(values_.size() == priorities_.size());
    if (empty()) {
      assert(false);
    }
    return priorities_.back();
  }

  const T& top() const {
    assert(values_.size() == priorities_.size());
    if (empty()) {
      assert(false);
    }
    return values_.back();
  }

  void pop() {
    assert(values_.size() == priorities_.size());
    if (empty()) {
      assert(false);
    }
    values_.pop_back();
    priorities_.pop_back();
  }

  bool empty() const {
    assert(values_.size() == priorities_.size());
    return values_.empty();
  }

  size_t size() const {
    assert(values_.size() == priorities_.size());
    return values_.size();
  }

 private:
  std::vector<T> values_;
  std::vector<T> priorities_;
  Comparator comp_;
};

int main() {
  std::ios_base::sync_with_stdio(false);
  auto stck = priority_stack<int, std::less<int>>{};
  auto queries_count = size_t{0};
  std::cin >> queries_count;
  for (auto query_id = size_t{0}; query_id < queries_count; ++query_id) {
    auto command = std::string{};
    std::cin >> command;
    if (command == "push") {
      auto value = int{0};
      std::cin >> value;
      stck.push(value);
    } else if (command == "pop") {
      stck.pop();
    } else if (command == "max") {
      std::cout << stck.top_priority() << '\n';
    } else {
      assert(false);
    }
  }
  return 0;
}