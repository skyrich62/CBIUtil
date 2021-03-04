#include "CompuBrite/ThreadPool.h"

namespace CompuBrite {

ThreadPool::~ThreadPool()
{
    shutdown();
    wait();
}

ThreadPool&
ThreadPool::shutdown()
{
    _active = false;
    _cv.notify_all();
    return *this;
}

ThreadPool&
ThreadPool::wait()
{
    for (auto &thread : _threads) {
        _cv.notify_all();
        if (thread.joinable()) {
            thread.join();
        }
    }
    return *this;
}

ThreadPool&
ThreadPool::activate(size_t nThreads)
{
    auto lock = this->lock();
    for (size_t n = 0u; n < nThreads; ++n) {
        _threads.emplace_back(std::thread([this] {svc(); }));
    }
    _active = true;
    return *this;
}

void
ThreadPool::svc()
{
    while (true) {
        auto lock = this->lock();
        _cv.wait(lock, [this] () { return !(_active && _tasks.empty()); });
        if (!_active) {
            return;
        }
        if (_tasks.empty()) {
            continue;
        }
        auto task = _tasks.front();
        _tasks.pop_front();
        lock.unlock();
        if (task) {
            task();
        }
    }
}

ThreadPool&
ThreadPool::addTask(Task &&task)
{
    auto lock = this->lock();
    _tasks.emplace_back(std::move(task));
    _cv.notify_all();
    return *this;
}

ThreadPool&
operator<< (ThreadPool &pool, ThreadPool::Task &&task)
{
    return pool.addTask(std::move(task));
}

} // namespace CompuBrite

// EOF
