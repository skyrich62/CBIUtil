/**
 * The MIT License (MIT)
 *
 * @copyright
 * Copyright (c) 2020 Rich Newman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * @file
 * @brief Interface for the TaskQueue
 */


#ifndef COMPUBRITE_TASKQUEUE_H
#define COMPUBRITE_TASKQUEUE_H

#include <list>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

namespace CompuBrite
{

/**
 * Manage a queue of tasks.
 */
class TaskQueue
{
public:
    /**
     * A Task is a discrete unit of work to perform, a single function
     * taking no arguments, and returning no value.
     * Arguments should be supplied via lambda captures.
     * @see addTask()
     */
    using Task = std::function<void()>;

    TaskQueue() = default;
    virtual ~TaskQueue() = default;

    TaskQueue(const TaskQueue &) = delete;
    TaskQueue(TaskQueue &&) = default;

    TaskQueue& operator=(const TaskQueue&&) = delete;
    TaskQueue& operator=(TaskQueue&&) = default;

   /**
     * Add a task to the work queue.
     * @param task The task to add to the queue.
     */
    TaskQueue& addTask(Task &&task);

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
     * If the TaskQueue is non-empty, then pop the first task off and return it.
     * Otherwise, just return a null Task.
     */
    Task pop();

    /// @return true if the queue is empty.
    bool empty() const            { return _tasks.empty(); }

    /// @return the number of tasks on the queue.
    auto size() const             { return _tasks.size(); }

private:
    using Tasks = std::list<Task>;
    Tasks                           _tasks;
};

/// A convenience operator to add tasks to the queue.
TaskQueue&
operator<<(TaskQueue &queue, TaskQueue::Task &&task);

} // namespace CompuBrite

#endif // COMPUBRITE_TASKQUEUE_H
