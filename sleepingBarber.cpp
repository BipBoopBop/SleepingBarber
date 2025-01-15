#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <semaphore>
#include <random>
#include <synchapi.h>

using namespace std;

#define NWAITINGSEATS 5

mutex outputMtx;
void barber(int *waitingSeats);
void customer(int *waitingSeats, int id);
size_t my_rand(size_t min, size_t max);
binary_semaphore waitingRoom{1};
binary_semaphore barberAvailable{0};
counting_semaphore<NWAITINGSEATS> customersWaiting{0};

int main(){
    int waitingSeats = NWAITINGSEATS;
    
    thread threadb(barber, &waitingSeats);

    thread threadc1(customer, &waitingSeats, 1);
    thread threadc2(customer, &waitingSeats, 2);
    thread threadc3(customer, &waitingSeats, 3);
    thread threadc4(customer, &waitingSeats, 4);
    thread threadc5(customer, &waitingSeats, 5);
    thread threadc6(customer, &waitingSeats, 6);
    thread threadc7(customer, &waitingSeats, 7);
    thread threadc8(customer, &waitingSeats, 8);

    threadb.join();
    threadc1.join();
    threadc2.join();
    threadc3.join();
    threadc4.join();
    threadc5.join();
    threadc6.join();
    threadc7.join();
    threadc8.join();
}

void barber(int *waitingSeats){
    while(true){
        customersWaiting.acquire();
        waitingRoom.acquire();
        size_t customerid = NWAITINGSEATS - *waitingSeats;
        *waitingSeats += 1;
        barberAvailable.release();
        waitingRoom.release();

        {
            lock_guard lg(outputMtx);
            cout << "\tbarber is cutting hair of customer" << endl;
        }
        Sleep(10000);
        {
            lock_guard lg(outputMtx);
            cout << "\tbarber is cutting hair of customer end" << endl;
        }
        waitingRoom.acquire();
        if(*waitingSeats == NWAITINGSEATS){
            {
                lock_guard lg(outputMtx);
                cout << "\tbarber is going to sleep" << endl;
            }
        }
        waitingRoom.release();
    }
}

void customer(int *waitingSeats, int id){
    bool haircutted = false;
    while (true)
    {
        {
            lock_guard lg(outputMtx);
            cout << "customer #"<< id << " entering barber shop" << endl;
        }
        waitingRoom.acquire();
        if(*waitingSeats > 0){
            *waitingSeats -= 1;
            customersWaiting.release();
            waitingRoom.release();
            barberAvailable.acquire();
            {
                lock_guard lg(outputMtx);
                cout << "\tcustomer #"<< id << " is getting hair cut" << endl;
            }
            Sleep(10000);
            haircutted = true;
        }
        else{
            {
                lock_guard lg(outputMtx);
                cout << "no room for customer #"<< id << endl;
            }
            waitingRoom.release();
            haircutted = false;
        }
        {
            lock_guard lg(outputMtx);
            cout << "customer #" << id << " leaving barber shop";
            if(haircutted){
                cout << " hair cutted";
            }
            cout << endl;
        }
        Sleep(my_rand(10000,15000));
    }
    
}

size_t my_rand(size_t min, size_t max)
{
    static mt19937 rnd(std::time(nullptr));
    return uniform_int_distribution<>(min, max)(rnd);
}
