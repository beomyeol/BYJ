#include "byj/thread.h"

using namespace byj;

thread::~thread() {
}

void thread::start() {
    thread_ = std::thread(&thread::run, this);
}

void thread::join() {
    thread_.join();
}

void thread::detach() {
    thread_.detach();
}

bool thread::joinable() const {
    return thread_.joinable();
}