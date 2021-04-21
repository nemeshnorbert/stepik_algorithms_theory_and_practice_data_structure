#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

uint32_t sum_mod(uint32_t lhs, uint32_t rhs, uint32_t mod) {
  return static_cast<uint32_t>(
    (static_cast<uint64_t>(lhs) + static_cast<uint64_t>(rhs)) %
    static_cast<uint64_t>(mod));
}

uint32_t neg_mod(uint32_t val, uint32_t mod) {
  return mod - val % mod;
}

uint32_t sub_mod(uint32_t lhs, uint32_t rhs, uint32_t mod) {
  return sum_mod(lhs, neg_mod(rhs, mod), mod);
}

uint32_t mul_mod(uint32_t lhs, uint32_t rhs, uint32_t mod) {
  return static_cast<uint32_t>(
    (static_cast<uint64_t>(lhs) * static_cast<uint64_t>(rhs)) %
    static_cast<uint64_t>(mod));
}

uint32_t power_mod(uint32_t base, uint32_t degree, uint32_t mod) {
  // std::cout << "base = " << base << '\n';
  // std::cout << "degree = " << degree << '\n';
  // std::cout << "mod = " << mod << '\n';
  auto power = uint32_t{1};
  while (degree > 0) {
    // if degree is odd
    if (degree & 1) {
      power = mul_mod(power, base, mod);
      degree -= 1;
    }
    degree >>= 1;
    base = mul_mod(base, base, mod);
  }
  // std::cout << "power = " << power << '\n';
  return power;
}

struct extended_gcd_result {
  int64_t x;
  int64_t y;
  uint32_t gcd;
};

extended_gcd_result extended_gcd(uint32_t a, uint32_t b) {
  auto x = int64_t{1};
  auto y = int64_t{0};
  auto x1 = int64_t{0};
  auto y1 = int64_t{1};
  auto a1 = int64_t{a};
  auto b1 = int64_t{b};
  while (b1 != 0) {
    auto temp = int64_t{0};
    auto q = a1 / b1;
    //
    temp = x1;
    x1 = x - q * x1;
    x = temp;
    //
    temp = y1;
    y1 = y - q * y1;
    y = temp;
    //
    temp = b1;
    b1 = a1 - q * b1;
    a1 = temp;
  }
  assert(a1 >= 0);
  return extended_gcd_result{x, y, static_cast<uint32_t>(a1)};
}

uint32_t inv_mod(uint32_t val, uint32_t mod) {
  auto result = extended_gcd(val, mod);
  assert(result.gcd == uint32_t{1});
  // std::cout << "result = {x = " << result.x << ", y = " << result.y << ", gcd = " << result.gcd << "}\n";
  // std::cout << "val * x + mod * y = " << val * result.x + mod * result.y << "\n";
  auto imod = int64_t{mod};
  // std::cout << "result.x = " << result.x << '\n';
  // std::cout << "imod = " << imod << '\n';
  // std::cout << "result.x % imod = " << result.x % imod << '\n';
  // std::cout << "(imod + result.x % imod) = " << (imod + result.x % imod) << '\n';
  // std::cout << "(imod + result.x % imod) % imod = " << (imod + result.x % imod) % imod << '\n';
  // std::cout << "static_cast<uint32_t>((imod + result.x % imod) % imod) = " << static_cast<uint32_t>((imod + result.x % imod) % imod);
  // std::cout << "inv_mod(" << val << ", " << mod << ") = " << static_cast<uint32_t>((imod + result.x % imod) % imod) << "\n";
  return static_cast<uint32_t>((imod + result.x % imod) % imod);
}

uint32_t div_mod(uint32_t lhs, uint32_t rhs, uint32_t mod) {
  return mul_mod(lhs, inv_mod(rhs, mod), mod);
}

template <typename Iter>
uint32_t compute_hash(Iter first, Iter last, uint32_t base, uint32_t mod) {
  auto base_power = uint32_t{1};
  auto hash = uint32_t{0};
  for (auto it = first; it != last; ++it) {
    const auto code = static_cast<uint32_t>(*it);
    auto monom = mul_mod(code, base_power, mod);
    hash = sum_mod(hash, monom, mod);
    base_power = mul_mod(base_power, base, mod);
  }
  return hash;
}

uint32_t update_hash(
  uint32_t hash, char old_char, char new_char, uint32_t base, uint32_t mod, uint32_t base_power, uint32_t inv_base) {
  // std::cout << "old_char = " << old_char << '\n';
  // std::cout << "new_char = " << new_char << '\n';
  const auto old_code = static_cast<uint32_t>(old_char);
  const auto new_code = static_cast<uint32_t>(new_char);
  // std::cout << "old_code " << old_code << '\n';
  // std::cout << "new_code " << new_code << '\n';
  // std::cout << "hash = " << hash << '\n';
  hash = sub_mod(hash, old_code, mod);
  // std::cout << "after sub hash = " << hash << '\n';
  hash = mul_mod(hash, inv_base, mod);
  // std::cout << "after div hash = " << hash << '\n';
  hash = sum_mod(hash, mul_mod(new_code, base_power, mod), mod);
  // std::cout << "after sum hash = " << hash << '\n';
  return hash;
}

std::vector<size_t> find_substring(const std::string& pattern, const std::string& text) {
  if (pattern.size() > text.size()) {
    return std::vector<size_t>{};
  }
  if (pattern.empty()) {
    auto positions = std::vector<size_t>(text.size(), size_t{0});
    std::iota(std::begin(positions), std::end(positions), size_t{0});
    return positions;
  }
  const auto window_size = pattern.size();

  const auto pattern_begin = std::begin(pattern);
  const auto pattern_end = std::end(pattern);
  const auto text_begin = std::begin(text);
  const auto text_end = std::end(text);
  auto window_begin = text_begin;
  auto window_end = text_begin + window_size;

  constexpr auto base = uint32_t{263};
  constexpr auto mod = uint32_t{1000000007};
  const auto pattern_hash = compute_hash(pattern_begin, pattern_end, base, mod);
  // std::cout << "pattern_hash = " << pattern_hash << '\n';
  auto subtext_hash = compute_hash(window_begin, window_end, base, mod);
  const auto base_power = power_mod(base, window_size - 1, mod);
  const auto inv_base = inv_mod(base, mod);
  // std::cout << "base_power = " << base_power << '\n';
  auto positions = std::vector<size_t>{};
  while (window_end <= text_end) {
    if (subtext_hash == pattern_hash) {
      const auto is_equal = std::equal(pattern_begin, pattern_end, window_begin, window_end);
      if (is_equal) {
        // std::cout << "add position\n";
        positions.emplace_back(static_cast<size_t>(window_begin - text_begin));
      }
    }
    // std::cout << "subtext_hash = " << subtext_hash << '\n';
    if (window_end != text_end) {
      subtext_hash = update_hash(subtext_hash, *window_begin, *window_end, base, mod, base_power, inv_base);
      ++window_begin;
      ++window_end;
    } else {
      break;
    }
  }
  return positions;
}

int main() {
  auto pattern = std::string{};
  std::cin >> pattern;
  auto text = std::string{};
  std::cin >> text;
  auto positions = find_substring(pattern, text);
  for (const auto& position : positions) {
    std::cout << position << ' ';
  }
  return 0;
};