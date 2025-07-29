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

#include "PureCore/CoreErrorDesc.h"
#include "PureCore/OsHelper.h"
#include "PureCore/PureLog.h"
#include "PureCore/Task.h"

namespace PureCore {
///////////////////////////////////////////////////////////////////////////
// TaskThread
//////////////////////////////////////////////////////////////////////////
TaskThread::TaskThread(Task& task) : mTask(task) {}

void TaskThread::work() { mTask.work(); }

///////////////////////////////////////////////////////////////////////////
// TaskNode
//////////////////////////////////////////////////////////////////////////
TaskNode::TaskNode(std::function<void()> func) : mFunc(func) {}

//////////////////////////////////////////////////////////////////////////
// Task
//////////////////////////////////////////////////////////////////////////
Task::Task(uint32_t size) {
    if (size == 0) {
        size = 1;
    }
    std::unique_lock<std::mutex> lock(mMutex);
    for (uint32_t i = 0; i < size; ++i) {
        auto t = new TaskThread(*this);
        if (t == nullptr) {
            PureError("task ctor failed, memory is not enough!!!");
            continue;
        }
        mThreades.push_back(t);
        int err = t->run();
        if (err != Success) {
            PureError("task ctor failed, `{}`", get_error_desc(err));
        }
    }
}

Task::~Task() {
    std::unique_lock<std::mutex> lock(mMutex);
    for (auto t : mThreades) {
        delete t;
    }
    mThreades.clear();
}

int Task::run() {
    mRunning = true;
    return Success;
}

void Task::stop(int64_t timeout) {
    mRunning = false;
    {
        std::unique_lock<std::mutex> lock(mMutex);
        while (!mNodes.empty()) {
            delete mNodes.pop_front();
        }
        mCondition.notify_all();
    }
    int64_t left = timeout > 0 ? timeout : 1;
    while (mRunningTask.load(std::memory_order_relaxed) != 0 && left > 0) {
        PureCore::sleep(10);
        if (timeout > 0) {
            left -= 10;
        }
    }
}

int Task::add_task(std::function<void()> func) {
    if (!mRunning.load(std::memory_order_relaxed)) {
        return ErrorTaskIsStoped;
    }
    TaskNode* node = new TaskNode(func);
    std::unique_lock<std::mutex> lock(mMutex);
    mNodes.push_back(node);
    mCondition.notify_one();
    return Success;
}

void Task::work() {
    TaskNode* node = nullptr;
    while (mRunning.load(std::memory_order_relaxed)) {
        {
            std::unique_lock<std::mutex> lock(mMutex);
            node = mNodes.pop_front_t<TaskNode>();
            if (node == nullptr) {
                mCondition.wait(lock);
            }
        }
        if (node != nullptr) {
            ++mRunningTask;
            node->mFunc();
            --mRunningTask;
            delete node;
        }
    }
}

}  // namespace PureCore