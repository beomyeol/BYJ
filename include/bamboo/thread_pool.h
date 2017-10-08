#ifndef BAMBOO_THREAD_POOL_H_
#define BAMBOO_THREAD_POOL_H_

#include <memory>
#include <thread>
#include <vector>

#include "bamboo/blocking_deque.h"
#include "bamboo/task.h"

namespace bamboo {

class ThreadPool {
 public:
  ThreadPool(std::size_t pool_size);
  ~ThreadPool();

  void Start();
  void Stop();

  void Submit(std::unique_ptr<Task> task);

  void Resize(std::size_t pool_size);

 private:
  void DoTask();

  volatile bool stopped_;

  BlockingDeque<std::unique_ptr<Task>> tasks_;
  std::vector<std::thread> threads_;
};

}  // namespace bamboo

#endif  // BAMBOO_THREAD_POOL_H_
