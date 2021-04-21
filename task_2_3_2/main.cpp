#include <cassert>
#include <iostream>
#include <vector>
#include <queue>

struct processor_info {
  size_t finish_time;
  size_t processor_id;
};

struct task_info {
  size_t processor_id;
  size_t start_time;
};

bool operator < (const processor_info& lhs, const processor_info& rhs) {
  return (rhs.finish_time < lhs.finish_time) ||
    (rhs.finish_time == lhs.finish_time && rhs.processor_id < lhs.processor_id);
}

int main() {
  auto processor_count = size_t{0};
  std::cin >> processor_count;
  auto tasks_count = size_t{0};
  std::cin >> tasks_count;
  auto durations = std::vector<size_t>{};
  durations.reserve(tasks_count);
  for (auto task_id = size_t{0}; task_id < tasks_count; ++task_id) {
    auto task_duration = size_t{0};
    std::cin >> task_duration;
    durations.emplace_back(task_duration);
  }
  auto processors_queue = std::priority_queue<processor_info>{};
  for (auto processor_id = size_t{0}; processor_id < processor_count; ++processor_id) {
    processors_queue.emplace(processor_info{0, processor_id});
  }
  assert(!processors_queue.empty());
  auto reports = std::vector<task_info>{};
  reports.reserve(tasks_count);
  for (auto task_id = size_t{0}; task_id < tasks_count; ++task_id) {
    const auto free_processor = processors_queue.top();
    reports.emplace_back(task_info{free_processor.processor_id, free_processor.finish_time});
    processors_queue.pop();
    processors_queue.push(
      processor_info{free_processor.finish_time + durations[task_id], free_processor.processor_id});
  }
  for (const auto& report : reports) {
    std::cout << report.processor_id << ' ' << report.start_time << '\n';
  }
  return 0;
}