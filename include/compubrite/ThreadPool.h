#ifndef COMPUBRITE_THREADPOOL_H
#define COMPUBRITE_THREADPOOL_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <functional>
#include <future>

namespace CompuBrite {

/**
 * Manage a thread pool.
 * The ThreadPool class manages a group of threads and a queue of work
 * to perform.  As threads are free in the pool, they will pick up the
 * next work item, (Task), and perform it.
 */
class ThreadPool
{
public:
    /**
     * Construct a ThreadPool object and initialize it.
     */
    ThreadPool() = default;

    /**
     * Shutdown the ThreadPool, wait for all tasks to finish, then destroy
     * the ThreadPool object.
     */
    ~ThreadPool();

    /// ThreadPool objects should not be copied.
    ThreadPool(const ThreadPool&) = delete;

    /// ThreadPool objects should not be copied.
    ThreadPool& operator=(const ThreadPool&) = delete;

    /// ThreadPool objects can be moved.
    ThreadPool(ThreadPool &&) = default;

    /// ThreadPool objects can be moved.
    ThreadPool& operator=(ThreadPool&&) = default;

    /// @name Internal type aliases
    /// @{

    /**
     * A Task is a discrete unit of work to perform, a single function
     * taking no arguments, and returning no value.
     * Arguments should be supplied via lambda captures.
     * @see addTask()
     */
    using Task = std::function<void()>;

    /**
     * Helps to make the code more orthogonal.
     */
    using Mutex = std::mutex;

    /**
     * Parameterize the locking mechanism over the task queue.
     */
    using Lock = std::unique_lock<Mutex>;
    /// @}

    /**
     * Activate the pool by creating nThreads new threads to run in it.
     * active() can be called multiple times to increase the number of
     * threads running in the pool.
     * @param nThreads The number of new threads to start and add to the
     * pool.
     */
    ThreadPool& activate(size_t nThreads);

    /**
     * Add a task to the work queue for this pool.
     * The next free thread will pick up the task and run it.
     * @param task The task to add to the queue.
     */
    ThreadPool& addTask(Task &&task);

    /**
     * Add a task to the work queue which might return a value, (or throw
     * an exception).
     * @overload
     * @tparam Ret The return type for the task.
     * @param work The function to run, which will be wrapped in a Task.
     * @return A std::promise<Ret> which can be used to retrieve the
     * return value or exception if any.
     */
    template <typename Ret>
    std::promise<Ret> addTask(std::function<Ret ()> work)
    {
        std::promise<Ret> promise;
        auto task = [work, &promise] ()
        {
            try {
                auto r = work();
                promise.set_value(r);
            } catch (...) {
                promise.set_exception(std::current_exception());
            }
        };
        addTask(std::move(task));
        return promise;
    }

    /**
     * Shutdown this thread pool. Signal all threads to quit.
     * @return a reference to this ThreadPool to promote chaining.
     */
    ThreadPool& shutdown();

    /**
     * Wait for all threads to finish, (by calling thread::join())
     * @return a reference to this ThreadPool to promote chaining.
     */
    ThreadPool& wait();

    /**
     * @return false if the ThreadPool is shutdown, (or hasn't started yet).
     * @return true if the ThreadPool is active and threads are running.
     */
    bool active() const                { return _active; }

    /// @return The number of threads assigned to this ThreadPool
    size_t threads() const             { auto l = lock(); return _threads.size(); }

    /// @return the number of tasks in the ThreadPool
    size_t tasks() const               { auto l = lock(); return _tasks.size(); }
private:

    /**
     * All of the threads in the ThreadPool run this function.
     * Wait for the queue to be non-empty, (or in shutdown mode).  If
     * the ThreadPool is shutdown, then exit, letting the thread end.
     * If the queue is non-empty, then take the task at the front of the queue,
     * pop it off and execute it.
     */
    void svc();

    /**
     * Lock the mutex protecting this object.
     * @return a std::unique_lock<Mutex> object to control the Mutex.
     */
    Lock lock() const                   { return Lock(_mutex); }

private:
    using Threads = std::list<std::thread>;
    using Tasks = std::list<Task>;
    using CondVar = std::condition_variable;

    Threads             _threads;
    Tasks               _tasks;
    mutable Mutex       _mutex;
    mutable CondVar     _cv;
    std::atomic<bool>   _active{false};
};

/**
 * Convenience operator for adding tasks to a ThreadPool
 * Add a task to the pool similar to the way formatted output works:
 * @code
 *    ThreadPool pool;
 *    pool.activate(4);
 *    pool << []() { doSomething(); };
 * @endcode
 * @param pool The ThreadPool to receive the task.
 * @param task The ThreadPool::Task to be added to the queue.
 * @return A reference to the ThreadPool to support chaining.
 * @relates ThreadPool
 */
ThreadPool& operator<<(ThreadPool &pool, ThreadPool::Task &&task);

} // namespace CompuBrite

#endif // COMPUBRITE_THREADPOOL_H
