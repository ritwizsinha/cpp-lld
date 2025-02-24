#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>


using namespace std;
int main() {
    int total_threads = 0;
    mutex m;
    condition_variable cv;
    auto critical_section = [&](int i) -> void {
        cout << "All threads wait here with id " << i << endl;
        {
            unique_lock lock(m);
            total_threads++;
            cv.wait(lock, [&]() -> bool { return total_threads == 10;});
            cv.notify_all();
        }

        cout << "All threads rush here with id " <<i << endl;

    };


    vector<thread> threads;
    for(int i = 0; i < 10; i++) {
        this_thread::sleep_for(chrono::milliseconds(rand() % 500));
        threads.push_back(thread(critical_section, i));
    }

    for(auto &t: threads) t.join();
}