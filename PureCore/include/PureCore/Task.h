/*
 * Copyright (c) 2023-present ChenDong, email <baisaichen@live.com>. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include "PureCore/PureCoreLib.h"
#include "PureCore/Thread.h"
#include "PureCore/NodeList.h"

#include <functional>
#include <mutex>
#include <condition_variable>

namespace PureCore {
class Task;
class TaskThread : public Thread {
private:
    friend class Task;
    TaskThread(Task& task);
    virtual ~TaskThread() = default;

protected:
    virtual void work();

private:
    Task& mTask;

    PURE_DISABLE_COPY(TaskThread)
};

class TaskNode : public Node {
private:
    friend class Task;
    TaskNode(std::function<void()> func);
    virtual ~TaskNode() = default;

private:
    std::function<void()> mFunc;

    PURE_DISABLE_COPY(TaskNode)
};

class PURECORE_API Task {
public:
    Task(uint32_t size);
    ~Task();

    int run();
    void stop(int64_t timeout);
    int add_task(std::function<void()> func);

private:
    friend void TaskThread::work();
    void work();

private:
    std::vector<TaskThread*> mThreades;
    std::mutex mMutex;
    std::condition_variable mCondition;
    NodeList mNodes;
    std::atomic<bool> mRunning{};
    std::atomic<uint32_t> mRunningTask{};
};

}  // namespace PureCore
