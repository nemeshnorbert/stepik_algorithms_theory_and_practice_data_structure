#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using table_id = int;

class table {
 public:
  table(size_t rows_count)
    : rows_count_{rows_count}
    , link_table_id_{-1}
  {}

  size_t get_size() const {
    return rows_count_;
  }

  void set_size(size_t rows_count) {
    rows_count_ = rows_count;
  }

  void increase_size(size_t rows_count) {
    rows_count_ += rows_count;
  }

  void decrease_size(size_t rows_count) {
    assert(rows_count <= rows_count_);
    rows_count_ -= rows_count;
  }

  void set_link(table_id link_table_id) {
    link_table_id_ = link_table_id;
  }

  table_id get_link() const {
    return link_table_id_;
  }

 private:
  size_t rows_count_;
  table_id link_table_id_;
};

struct link_request_result {
  table_id source_idx;
  table_id destination_idx;
};

class database {
 public:
  database(const std::vector<size_t>& table_sizes)
    : tables_{} {
    for (const auto& size : table_sizes) {
      tables_.emplace_back(table{size});
    }
  }

  link_request_result set_link(table_id source_idx, table_id destination_idx) {
    source_idx = get_ancestor(source_idx);
    destination_idx = get_ancestor(destination_idx);
    if (source_idx != destination_idx) {
      tables_[destination_idx].increase_size(tables_[source_idx].get_size());
      tables_[source_idx].set_link(destination_idx);
      tables_[source_idx].set_size(0);
    }
    return link_request_result{source_idx, destination_idx};
  }

  size_t get_size(table_id idx) const {
    return tables_[idx].get_size();
  }

 private:
  table_id get_ancestor(table_id id) {
    auto forefather = id;
    auto ancestor = tables_[id].get_link();
    while (ancestor != table_id{-1}) {
      forefather = ancestor;
      ancestor = tables_[forefather].get_link();
    }
    // path compression
    ancestor = tables_[id].get_link();
    while (ancestor != table_id{-1}) {
      tables_[id].set_link(forefather);
      id = ancestor;
      ancestor = tables_[id].get_link();
    }
    return forefather;
  }

 private:
  std::vector<table> tables_;
};

int main() {
  auto tables_count = size_t{0};
  std::cin >> tables_count;
  auto requests_count = size_t{0};
  std::cin >> requests_count;
  auto table_sizes = std::vector<size_t>{};
  table_sizes.reserve(tables_count);
  for (auto table_idx = size_t{0}; table_idx < tables_count; ++table_idx) {
    auto table_size = size_t{0};
    std::cin >> table_size;
    table_sizes.emplace_back(table_size);
  }
  assert(table_sizes.size() == tables_count);
  assert(!table_sizes.empty());
  auto db = database(table_sizes);
  auto max_table_size = *std::max_element(std::begin(table_sizes), std::end(table_sizes));
  for (auto request_idx = size_t{0}; request_idx < requests_count; ++request_idx) {
    auto source_idx = size_t{0};
    std::cin >> source_idx;
    assert(1<= source_idx && source_idx <= tables_count);
    auto destination_idx = size_t{0};
    std::cin >> destination_idx;
    assert(1 <= destination_idx && destination_idx <= tables_count);
    auto response = db.set_link(source_idx - 1, destination_idx - 1);
    max_table_size = std::max(max_table_size, db.get_size(response.destination_idx));
    std::cout << max_table_size << '\n';
  }
  return 0;
}