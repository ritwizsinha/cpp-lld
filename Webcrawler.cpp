#include <unordered_map>
#include <vector>
#include <string>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <future>
#include <unordered_set>

using namespace std;

unordered_map<string, vector<string>> sites = {
    {"google.com", {"abc.com", "skibidi.com", "chatgpt.com"}},
    {"abc.com", {"claude.com", "math.com"}},
    {"skibidi.com", {"instagram.com", "random.com"}},
    {"chatgpt.com", {"samaltman", "somethingelse", "elon musk"}},
    {"claude.com", {"aravinda"}},
    {"instagram.com", {"reels", "somethingelse"}},
    {"random.com", {}},
    {"samaltman", {"money", "elon musk"}},
    {"elon musk", {"money"}},
    {"math.com", {}},
    {"somethingelse", {}},
    {"aravinda", {}},
    {"reels", {}}
};

vector<string> makeWebRequest(const string &node) {
    // this_thread::sleep_for(chrono::milliseconds(rand() % 300));
    return sites.at(node);
};

class Threadpool {
    public:
    using Callback = function<void()>;
    explicit Threadpool(int threads): stop(0) {
        for(int i = 0; i < threads; i++) {
            pool.emplace_back(thread([this]() -> void {
                while(1) {
                    unique_lock<mutex> lock(mtx);
                    cv.wait(lock, [&]() -> bool {
                        return stop or !tasks.empty();
                    });
                    if (stop) return;

                    auto task = std::move(tasks.front());
                    tasks.pop();
                    lock.unlock();
                    task();
                }
           }));
        }
    }

    template<typename F, typename... Args>
    auto execute(F&& f, Args&&... args) -> future<decltype(f(args...))> {
        using return_type = decltype(f(args...));

        auto task = make_shared<packaged_task<return_type()>>(bind(std::forward<F>(f), std::forward<Args>(args)...));
        auto result = task->get_future();
        lock_guard lock(mtx);
        tasks.emplace([task]() -> void {
            (*task)();
        });
        cv.notify_one();
        return result;
    }

    ~Threadpool() {
        {
            lock_guard lock(mtx);
            stop = 1;
        }

        for(auto &t: pool) t.join();
    }

    private:
    vector<thread> pool;
    queue<Callback> tasks;
    mutex mtx;
    condition_variable cv;
    int stop; 
    
};


struct Crawler {
    public:
    Threadpool pool;
    unordered_set<string> visited;
    
    Crawler(int threads):pool(8) {
        
    }


    void Crawl(const string &key) {
        cout << key << endl;
        // cout << "Called crawler" << endl;
        std::function<void(const string&)> fn = [&](const string &node) {
            auto children = makeWebRequest(node);
            cout << "Querying for node " << node << endl; 
            for (auto &child : children) {
                // Schedule the recursive call as a new task.
                // cout << child << endl;
                pool.execute(fn, child);
            }
        };
        cout << "Pushing to threadpool" << endl; 
        // while(1) {
            pool.execute(fn, key);
            pool.execute(fn, key);
        // }
    }
};


int main() {

    Crawler c(8);

    c.Crawl("google.com");


}