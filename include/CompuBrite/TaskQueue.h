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
 *
 * @file
 * @brief Interface for TaskQueue
*/

#ifndef COMPUBRITE_TASKQUEUE_H_INCLUDED
#define COMPUBRITE_TASKQUEUE_H_INCLUDED

#include <functional>
#include <list>

namespace CompuBrite
{

/**
 * TaskQueue holds a list of std::function, (Task), objects and allows clients
 * to add new tasks to the back of the queue, and take tasks from the front.
 */
class TaskQueue
{
public:
    using Task = std::function<void(void)>;
    using Queue = std::list<Task>;

    TaskQueue() = default;
    ~TaskQueue() = default;

    TaskQueue(const TaskQueue&) = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;

    /// Add a new task to the queue.
    void add(Task task);

    /// Take a task from the queue, if possible.
    /// @return an empty std::function if no task is available, otherwise
    /// the task at the back of the queue.
    Task take();

    /// @return true if the TaskQueue is empty.
    bool empty()                  { return _queue.empty(); }

private:
    Queue                   _queue;
};

TaskQueue&
operator<<(TaskQueue &queue, TaskQueue::Task task);

} // namespace CompuBrite
#endif // COMPUBRITE_TASKQUEUE_H_INCLUDED
