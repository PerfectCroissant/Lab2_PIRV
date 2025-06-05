#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <algorithm>
#include <random>
#include <numeric>
#include <vector>

using namespace std;

constexpr int exits = 5;
const int totalEmployees = 100;
std::mutex mtx;
std::condition_variable cv;
std::atomic<int> current_stage(0);

void worker(int id, bool isSpecial) {
    if (isSpecial) {
        std::cout << "Спец. сотрудник с ID " << id << " прошел через доп. турникет. \n";
    } else {
        std::cout << "Сотрудник с ID " << id << " прошел через турникет. \n";
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500 + rand() % 1000));
}

void ExitValidation(int id, vector<int>& employees, atomic<int>& turnstile_load) {
    vector<thread> threads;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> batchSizeDist(5, 100);
    std::unique_lock<std::mutex> lock(mtx);

    int i = 0;
    while (i < totalEmployees) {
        int batchSize = batchSizeDist(gen);
        batchSize = min(batchSize, totalEmployees - i);
        for (int j = 0; j < batchSize; ++j) {
            int employeeId = employees[i + j];
            
            bool isSpecial = (employeeId < 100);
            if (turnstile_load >= 10) {
                std::cout << "Турникет " << id << " перегружен. Открывается доп. турникет.\n";
                turnstile_load = 0;
            }

            threads.emplace_back(worker, employeeId, isSpecial);
            turnstile_load++;
        }

        for (auto& th : threads) {
            if (th.joinable()) {
                th.join();
            }
        }

        threads.clear();
        i += batchSize;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 + rand() % 1000));
    }
}

int main() {
    vector<thread> threads;
    vector<int> employees(1000);

    iota(employees.begin(), employees.end(), 1);
    random_device rd;
    mt19937 gen(rd());
    shuffle(employees.begin(), employees.end(), gen);
    employees.resize(totalEmployees);

    int threadsCount = 5;
    atomic<int> turnstile_load(0);

    for (int i = 0; i < threadsCount; ++i) {
        threads.emplace_back(ExitValidation, i, std::ref(employees), std::ref(turnstile_load));
    }
    for (auto& t : threads) {
        t.join();
    }
    cout << "Все работники в здании." << endl;
    return 0;
}
