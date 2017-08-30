#ifndef __BAMBOO_BLOCKING_DEQUE__
#define __BAMBOO_BLOCKING_DEQUE__

#include <condition_variable>
#include <deque>
#include <mutex>

namespace bamboo {

template <typename T>
class BlockingDeque {
 public:
  void push_front(const T& value);
  void push_back(const T& value);
  T pop_front();
  T pop_back();
  // TODO: rvalue reference
  // TODO: emblace

  std::size_t size();

 private:
  std::mutex mutex_;
  std::condition_variable cond_;
  std::deque<T> queue_;
};

template <typename T>
void BlockingDeque<T>::push_front(const T& value) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_front(value);
  }
  cond_.notify_one();
}

template <typename T>
void BlockingDeque<T>::push_back(const T& value) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_back(value);
  }
  cond_.notify_one();
}

template <typename T>
T BlockingDeque<T>::pop_front() {
  std::unique_lock<std::mutex> lock(mutex_);
  cond_.wait(lock, [this]() { return !queue_.empty(); });
  T value(std::move(queue_.front()));
  queue_.pop_front();
  return value;
}

template <typename T>
T BlockingDeque<T>::pop_back() {
  std::unique_lock<std::mutex> lock(mutex_);
  cond_.wait(lock, [this]() { return !queue_.empty(); });
  T value(std::move(queue_.back()));
  queue_.pop_back();
  return value;
}

template <typename T>
std::size_t BlockingDeque<T>::size() {
  std::lock_guard<std::mutex> lock(mutex_);
  return queue_.size();
}

}  // namespace bamboo

#endif  // BAMBOO_BLOCKING_DEQUE_H
