#include "bamboo/thread_pool.h"

#include "bamboo/log.h"

namespace bamboo {

class DummyTask : public Task {
 public:
  void Do() override {}
};

ThreadPool::ThreadPool(std::size_t pool_size)
    : stopped_(false), threads_(pool_size) {}

ThreadPool::~ThreadPool() { Stop(); }

void ThreadPool::Start() {
  stopped_ = false;
  for (std::size_t i = 0; i < threads_.size(); ++i) {
    threads_[i] = std::thread(&ThreadPool::DoTask, this);
  }
}

void ThreadPool::Stop() {
  stopped_ = true;
  // add dummy tasks
  for (std::size_t i = 0; i < threads_.size(); ++i) {
    tasks_.PushBack(std::unique_ptr<Task>(new DummyTask()));
  }
  for (std::size_t i = 0; i < threads_.size(); ++i) {
    threads_[i].join();
  }
}

void ThreadPool::Submit(std::unique_ptr<Task> task) {
  tasks_.PushBack(std::move(task));
}

void ThreadPool::Resize(std::size_t pool_size) {
  CHECK(stopped_) << "Thread pool should be stopped";
  threads_.resize(pool_size);
}

void ThreadPool::DoTask() {
  while (!stopped_) {
    std::unique_ptr<Task> task = tasks_.PopFront();
    task->Do();
  }
}

}  // namespace bamboo
