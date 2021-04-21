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

template <typename T, typename Comp = std::less<T>>
class priority_queue {
 public:
  bool empty() const {
    return left_.empty() && right_.empty();
  }

  size_t size() const {
    return left_.size() + right_.size();
  }

  void push(const T& value) {
    left_.push(value);
  }

  void pop() {
    if (empty()) {
      assert(false);
    }
    if (right_.empty()) {
      while (!left_.empty()) {
        const auto& top = left_.top();
        left_.pop();
        right_.push(top);
      }
    }
    right_.pop();
  }

  const T& top_priority() const {
    if (left_.empty() && right_.empty()) {
      assert(false);
    } else if (left_.empty()) {
      return right_.top_priority();
    } else if (right_.empty()) {
      return left_.top_priority();
    }
    return std::max(left_.top_priority(), right_.top_priority());
  }

 private:
  priority_stack<T, Comp> left_;
  priority_stack<T, Comp> right_;
};

int main() {
  std::ios_base::sync_with_stdio(false);
  auto count = std::size_t{0};
  std::cin >> count;
  auto values = std::vector<int>{};
  values.reserve(count);
  for (auto idx = size_t{0}; idx < count; ++idx) {
    auto value = int{0};
    std::cin >> value;
    values.emplace_back(value);
  }
  auto window_size = size_t{0};
  std::cin >> window_size;
  assert(window_size <= count);
  auto window = priority_queue<int>{};
  auto maxs = std::vector<int>{};
  maxs.reserve(count + 1 - window_size);
  for (auto idx = size_t{0}; idx < count; ++idx) {
     const auto& value = values[idx];
     if (window.size() < window_size) {
       window.push(value);
       if (window.size() == window_size) {
         maxs.emplace_back(window.top_priority());
       }
     } else {
       window.pop();
       window.push(value);
       maxs.emplace_back(window.top_priority());
     }
  }
  for (const auto& max : maxs) {
    std::cout << max << ' ';
  }
  return 0;
}