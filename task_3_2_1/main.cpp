#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>


int main() {
  auto requests_count = size_t{0};
  std::cin >> requests_count;
  auto phonebook = std::unordered_map<size_t, std::string>{};
  for (auto request_id = size_t{0}; request_id < requests_count; ++request_id) {
    auto command = std::string{};
    std::cin >> command;
    if (command == "add") {
      auto number = size_t{0};
      std::cin >> number;
      auto name = std::string{};
      std::cin >> name;
      phonebook[number] = name;
    } else if (command == "del") {
      auto number = size_t{0};
      std::cin >> number;
      phonebook.erase(number);
    } else if (command == "find") {
      auto number = size_t{0};
      std::cin >> number;
      auto it = phonebook.find(number);
      if (it != phonebook.end()) {
        std::cout << it->second << '\n';
      } else {
        std::cout << "not found\n";
      }
    } else {
      assert(false);
    }
  }
  return 0;
}