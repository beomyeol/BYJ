#ifndef BAMBOO_TASK_H_
#define BAMBOO_TASK_H_

namespace bamboo {

// Task interface that will be submitted to bamboo::ThreadPool.
class Task {
 public:
  virtual ~Task() {}

  virtual void Do() = 0;
};

}  // namespace bamboo

#endif  // BAMBOO_TASK_H_
