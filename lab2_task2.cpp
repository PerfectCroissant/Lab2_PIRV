#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

using namespace std;

struct Order {
    int id;
    int priority;
    bool urgent;
    bool operator<(const Order& other) const {
        return priority > other.priority;
    }
};

class ProductionLine {
private:
    priority_queue<Order> orderQueue;
    mutex queueMutex;
    condition_variable cv;
    vector<thread> machines;
    atomic<bool> machineActive[4];
    atomic<bool> stop{false};

public:
    ProductionLine() {
        for (int i = 0; i < 4; ++i) {
            machineActive[i] = true;
            machines.emplace_back(&ProductionLine::machineWorker, this, i);
        }
    }

    ~ProductionLine() {
        stop = true;
        cv.notify_all();
        for (auto& t : machines) {
            if (t.joinable())
                t.join();
        }
    }

    void addOrder(Order order) {
        lock_guard<mutex> lock(queueMutex);
        orderQueue.push(order);
        cv.notify_one();
    }

    void breakdownMachine(int id) {
        machineActive[id] = false;
        cout << "Станок " << id << " вышел из строя.\n";
        cv.notify_all();
    }

    void repairMachine(int id) {
        machineActive[id] = true;
        cout << "Станок " << id << " снова работает.\n";
        cv.notify_all();
    }

private:
    void machineWorker(int id) {
        while (!stop) {
            unique_lock<mutex> lock(queueMutex);
            cv.wait(lock, [&] { return !orderQueue.empty() || stop; });

            if (stop) break;

            if (!machineActive[id]) {
                lock.unlock();
                this_thread::sleep_for(chrono::milliseconds(100));
                continue;
            }

            if (!orderQueue.empty()) {
                Order order = orderQueue.top();
                orderQueue.pop();
                lock.unlock();

                cout << "Станок " << id << " выполняет заказ №" << order.id
                     << (order.urgent ? " (срочный)" : "") << ", приоритет: " << order.priority << endl;

                this_thread::sleep_for(chrono::milliseconds(order.urgent ? 500 : 1000));
            }
        }
    }
};

int main() {
    ProductionLine line;

    for (int i = 1; i <= 10; ++i) {
        Order order;
        order.id = i;
        order.priority = i % 5 + 1;
        order.urgent = (i % 3 == 0);

        line.addOrder(order);
        this_thread::sleep_for(chrono::milliseconds(200));
    }

    this_thread::sleep_for(chrono::seconds(2));
    line.breakdownMachine(2);

    this_thread::sleep_for(chrono::seconds(3));
    line.repairMachine(2);

    this_thread::sleep_for(chrono::seconds(5));
    return 0;
}
