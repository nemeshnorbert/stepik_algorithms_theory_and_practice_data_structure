#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>

void read_input(
  std::istream& in,
  size_t& buffer_size,
  std::vector<int>& arrivals,
  std::vector<int>& durations) {
  in >> buffer_size;
  auto packets_count = size_t{0};
  in >> packets_count;
  arrivals.reserve(packets_count);
  durations.reserve(packets_count);
  for (auto packet_id = size_t{0}; packet_id < packets_count; ++packet_id) {
    auto arrival_time = int{0};
    in >> arrival_time;
    arrivals.emplace_back(arrival_time);
    auto duration_time = int{0};
    in >> duration_time;
    assert(duration_time >= 0);
    durations.emplace_back(duration_time);
  }
}

std::vector<int> compute_delivery_times(
  const size_t buffer_size,
  const std::vector<int>& arrivals,
  const std::vector<int>& durations) {
  std::is_sorted(std::begin(arrivals), std::end(arrivals));
  const auto packets_count = arrivals.size();
  assert(packets_count == durations.size());
  auto packets_queue = std::list<int>{};
  auto deliveries = std::vector<int>{};
  deliveries.reserve(packets_count);
  // main loop
  for (auto packet_id = size_t{0}; packet_id < packets_count; ++packet_id) {
    const auto arrival_time = arrivals[packet_id];
    const auto duration_time = durations[packet_id];
    while (!packets_queue.empty() && packets_queue.front() <= arrival_time) {
      packets_queue.pop_front();
    }
    if (packets_queue.size() < buffer_size) {
      auto begin = arrival_time;
      if (!packets_queue.empty()) {
        begin = std::max(arrival_time, packets_queue.back());
      }
      deliveries.emplace_back(begin);
      packets_queue.push_back(begin + duration_time);
    } else {
      deliveries.emplace_back(-1);
    }
  }
  return deliveries;
}

int main() {
  auto buffer_size = size_t{0};
  auto arrivals = std::vector<int>{};
  auto durations = std::vector<int>{};
  read_input(std::cin, buffer_size, arrivals, durations);
  auto deliveries = compute_delivery_times(buffer_size, arrivals, durations);
  for (const auto& delivery_time : deliveries) {
    std::cout << delivery_time << '\n';
  }
  return 0;
}