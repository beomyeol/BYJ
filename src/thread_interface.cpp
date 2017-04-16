#include "bamboo/thread_interface.h"

using namespace bamboo;

ThreadInterface::~ThreadInterface() {
}

void ThreadInterface::start() {
  thread_ = std::thread(&ThreadInterface::run, this);
}

void ThreadInterface::join() {
  thread_.join();
}

void ThreadInterface::detach() {
  thread_.detach();
}

bool ThreadInterface::joinable() const {
  return thread_.joinable();
}
