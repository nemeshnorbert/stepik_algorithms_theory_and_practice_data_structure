#include <cassert>
#include <iostream>
#include <vector>
#include <stack>

struct parse_result {
  bool ok;
  size_t position;
};

struct symbol_info {
  char symbol;
  size_t position;
};

char get_closing_bracket(char symbol) {
  if (symbol == '{') {
    return '}';
  } else if (symbol == '[') {
    return ']';
  } else if (symbol == '(') {
    return ')';
  } else {
    assert(false);
  }
  return 0;
}

parse_result parse(const std::string& text) {
  auto position = size_t{0};
  auto stck = std::stack<symbol_info>{};
  const auto size = text.size();
  for (auto position = size_t{0}; position < size; ++position) {
    const auto& symbol = text[position];
    if (symbol == '(' || symbol == '{' || symbol == '[') {
      stck.push(symbol_info{symbol, position});
    } else if (symbol == ')' || symbol == '}' || symbol == ']') {
      if (stck.empty()) {
        return parse_result{false, position};
      } else if (get_closing_bracket(stck.top().symbol) != symbol) {
        return parse_result{false, position};
      } else {
        stck.pop();
      }
    } else {
      continue;
    }
  }
  if (!stck.empty()) {
    return parse_result{false, stck.top().position};
  }
  return parse_result{true, position};
}

int main() {
  auto text = std::string{};
  std::cin >> text;
  auto result = parse(text);
  if (result.ok) {
    std::cout << "Success";
  } else {
    std::cout << result.position + 1;
  }
  return 0;
}
