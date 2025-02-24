/*

Mutual Rendezvous

*/

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>


using namespace std;



int main() {
mutex m1, m2;
auto fn1 = [&]() {
    m1.lock();
    cout << "a1" << endl;
    m1.unlock();
    m2.lock();
    cout << "a2" << endl;
    m2.unlock();
};

auto fn2 = [&]() {
    m2.lock();
    cout << "b1" << endl;
    m2.unlock();
    m1.lock();
    cout << "b2" << endl;
    m1.unlock();
};

// a2 -> b1 and b2 -> a1
for(int i = 0; i < 10; i++) {

    auto t1 = thread(fn1);
    auto t2 = thread(fn2);
    t1.join();
    t2.join();

    cout << endl;
}

}