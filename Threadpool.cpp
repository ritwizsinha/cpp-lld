/*


*/
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include<memory>

using namespace std;

class ThreadPool {

    private:
    using Callback = function<void()>;
    vector<thread> pool;
    queue<Callback> tasks;
    mutex mtx;
    condition_variable cv;
    bool stop;
    public:
    explicit ThreadPool(int threads):stop(false) {
        for(int i = 0; i < threads; i++) {
            pool.emplace_back(thread([this]() -> void {
                while(1) {
                    unique_lock<mutex> lock(mtx);
                    cv.wait(lock, [&]() -> bool {return !tasks.empty() or stop;});
                    cout << "Running a thread for some work" << endl;
                    if (stop) {
                        return;
                    }

                    auto task = move(tasks.front());
                    tasks.pop();
                    cout << "Size of the queue " << tasks.size() << endl; 
                    lock.unlock();
                    task();
                }
            }));
        }
    }

    template<typename F, typename... Args>
    auto executeTask(F&& f, Args&&... args) -> future<decltype(f(args...))> {

        using return_type = decltype(f(args...));

        auto shared_task = make_shared<packaged_task<return_type()>>(bind(std::forward<F>(f), std::forward<Args>(args)...));
        future<return_type> res = shared_task->get_future();
        lock_guard lock(mtx);
        tasks.emplace([shared_task]() -> void {
           (*shared_task)(); 
        });
        cv.notify_one();
        return res;
    }


    ~ThreadPool() {
        {
            lock_guard  lock(mtx);
            stop = 1;
        }

        for(auto &t: pool) t.join();
    }

};


int main() {

    ThreadPool pool(28);
    auto func = [](int a, int b, string name) -> int {
        cout << "Outputting result " << a + b << " for " << name <<endl;
        return a + b;
    };
    while(1) {
        future<int> res = pool.executeTask(func, 1, 2, "Task" + to_string(rand() % 1000));
        cout << res.get() << endl;
    }


}

/*
Things Learnt:
1. Futures
2. Packaged Task
3. Bind
4. Forward

*/