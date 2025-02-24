#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

using namespace std;
int main() {
    mutex m, m2;
    int total_threads = 0;
    condition_variable cv;

    auto critical_fn = [&]() -> void {
        {
            unique_lock lock(m);
            cv.wait(lock, [&]() -> bool { return total_threads < 5; });
            total_threads++;
        }
        
        this_thread::sleep_for(chrono::milliseconds(rand() % 200));
        cout << "Running critical section code " << this_thread::get_id() << " total threads here " << total_threads <<  endl;        
        
        {
            lock_guard lock(m);
            total_threads--;
        }
        cv.notify_one();
    };


    vector<thread> threads;
    for(int i = 0; i < 100; i++) {
        threads.push_back(thread(critical_fn));
    }


    for(auto &t: threads) t.join();
}