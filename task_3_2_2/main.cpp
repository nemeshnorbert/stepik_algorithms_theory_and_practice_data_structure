#include <cassert>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

namespace {

size_t compute_hash(const std::string& text) {
  const auto n = text.size();
  constexpr auto x = size_t{263};
  constexpr auto p = size_t{1000000007};
  auto x_i = size_t{1};
  auto hash = size_t{0};
  for (auto i = size_t{0}; i < n; ++i) {
    auto addendum = (static_cast<size_t>(text[i]) * x_i) % p;
    hash = (hash + addendum) % p;
    x_i = (x_i * x) % p;
  }
  return hash;
}

}  // namespace

class phonebook {
 public:
  using page = std::list<std::string>;

  phonebook(size_t page_count)
    : pages_{page_count}
  {}

  void insert(const std::string& text) {
    auto& page = pages_[page_number(text)];
    auto here = std::find(std::begin(page), std::end(page), text);
    if (here == std::end(page)) {
      page.push_front(text);
    }
  }

  void erase(const std::string& text) {
    auto& page = pages_[page_number(text)];
    auto here = std::find(std::begin(page), std::end(page), text);
    if (here != std::end(page)) {
      page.erase(here);
    }
  }

  bool contains(const std::string& text) {
    auto& page = pages_[page_number(text)];
    return std::find(std::begin(page), std::end(page), text) != std::end(page);
  }

  page& get_page(const size_t number) {
    assert(number < pages_.size());
    return pages_[number];
  }

  const page& get_page(const size_t number) const {
    assert(number < page_count());
    return pages_[number];
  }

  size_t page_count() const {
    return pages_.size();
  }

 private:
  size_t page_number(const std::string& text) const {
    const auto page_count = pages_.size();
    return compute_hash(text) % page_count;
  }

 private:
  std::vector<std::list<std::string>> pages_;
};


int main() {
  auto pages_count = size_t{0};
  std::cin >> pages_count;
  auto requests_count = size_t{0};
  std::cin >> requests_count;
  auto notebook = phonebook{pages_count};
  for (auto request_id = size_t{0}; request_id < requests_count; ++request_id) {
    auto command = std::string{};
    std::cin >> command;
    if (command == "add") {
      auto record = std::string{};
      std::cin >> record;
      notebook.insert(record);
    } else if (command == "del") {
      auto record = std::string{};
      std::cin >> record;
      notebook.erase(record);
    } else if (command == "find") {
      auto record = std::string{};
      std::cin >> record;
      if (notebook.contains(record)) {
        std::cout << "yes\n";
      } else {
        std::cout << "no\n";
      }
    } else if (command == "check") {
      auto number = size_t{0};
      std::cin >> number;
      assert(number < notebook.page_count());
      for (const auto& record : notebook.get_page(number)) {
        std::cout << record << ' ';
      }
      std::cout << '\n';
    } else {
      assert(false);
    }
  }
};