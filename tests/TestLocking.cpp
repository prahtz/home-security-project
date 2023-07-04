#include "Locking.hpp"
#include <cassert>
#include <list>
#include <thread>
#include <unistd.h>
#include <chrono>

using namespace std;

class A {
    public:
        int id;
        A(int id) {this->id = id;}
};

class B {
    public:
        int id;
        B(int id) {
            sleep(1);
            this->id = id;
        }
};

class Test {
    private:
        Locking<list<A>> alist = Locking<list<A>>(list<A>{});
        Locking<list<B>> blist = Locking<list<B>>(list<B>{});
        const int push_size = 100000;
        thread t0, t1, t2;
            
    public:
        Test() {
            
        }

        void testWithLock() {
            t0 = thread([this](){this->serviceWithLock(0);});
            t1 = thread([this](){this->serviceWithLock(1);});
            t2 = thread([this](){this->serviceWithLock(2);});

            t0.join();
            t1.join();
            t2.join();
            
            int ids[3] = {0, 0, 0};
            int current = -1;
            //same ids should be found only sequentially
            for (auto it = alist->begin(); it != alist->end(); ++it) {
                int a_id = (*it).id;
                if (current == -1)
                    current = a_id;
                if (a_id != current) {
                    assert(ids[a_id] == 0);
                    current = a_id;
                }
                ids[current] += 1;
            } 
        }

        void testSingleOperation() {
            // emplace_back class B will wait for 1 second and then assign the id, data race should not occur.
            t0 = thread([this](){this->serviceSingleOperation(0);});
            t1 = thread([this](){this->serviceSingleOperation(1);});
            t2 = thread([this](){this->serviceSingleOperation(2);});
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            
            t0.join();
            t1.join();
            t2.join();
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); 
            auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
            assert(elapsedSeconds >= 3);
        }

        void serviceWithLock(int id) {
            alist.with_lock<void>(
                [this, id](list<A>& l) {
                    for (int i = 0; i < push_size; i++)
                        l.emplace_back(id);
                }
            );
        }

        void serviceSingleOperation(int id) {
            blist->emplace_back(id);
        }
        
};



int main () {
    Test t;
    t.testWithLock();
    t.testSingleOperation();
}