#ifndef BAMBOO_BLOCKING_DEQUE_H_
#define BAMBOO_BLOCKING_DEQUE_H_

#include <condition_variable>
#include <deque>
#include <mutex>

namespace bamboo {

template <typename T>
class BlockingDeque {
 public:
  void PushFront(const T& value);
  void PushFront(T&& value);
  void PushBack(const T& value);
  void PushBack(T&& value);
  T PopFront();
  T PopBack();
  // TODO: emblace

  std::size_t Size();

 private:
  std::mutex mutex_;
  std::condition_variable cond_;
  std::deque<T> queue_;
};

template <typename T>
void BlockingDeque<T>::PushFront(const T& value) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_front(value);
  }
  cond_.notify_one();
}

template <typename T>
void BlockingDeque<T>::PushFront(T&& value) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_front(std::move(value));
  }
  cond_.notify_one();
}

template <typename T>
void BlockingDeque<T>::PushBack(const T& value) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_back(value);
  }
  cond_.notify_one();
}

template <typename T>
void BlockingDeque<T>::PushBack(T&& value) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_back(std::move(value));
  }
  cond_.notify_one();
}

template <typename T>
T BlockingDeque<T>::PopFront() {
  std::unique_lock<std::mutex> lock(mutex_);
  cond_.wait(lock, [this]() { return !queue_.empty(); });
  T value(std::move(queue_.front()));
  queue_.pop_front();
  return value;
}

template <typename T>
T BlockingDeque<T>::PopBack() {
  std::unique_lock<std::mutex> lock(mutex_);
  cond_.wait(lock, [this]() { return !queue_.empty(); });
  T value(std::move(queue_.back()));
  queue_.pop_back();
  return value;
}

template <typename T>
std::size_t BlockingDeque<T>::Size() {
  std::lock_guard<std::mutex> lock(mutex_);
  return queue_.size();
}

}  // namespace bamboo

#endif  // BAMBOO_BLOCKING_DEQUE_H_
