#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include<condition_variable>


struct semaphore  {
    std::mutex mtx ;
    std::atomic_bool unlock;
    std::condition_variable condition;
    semaphore();
};
semaphore::semaphore(){
    unlock.store(true, std::memory_order_release);
}

void lock(semaphore * s){

    std::unique_lock<std::mutex> lock(s->mtx);
    s->condition.wait(lock, [&s]{return s->unlock.load(std::memory_order_acquire);});
    s->unlock.store(false, std::memory_order_release);
}
void unlock(semaphore *s){
    std::unique_lock<std::mutex>lock(s->mtx);
    s->unlock.store(true, std::memory_order_release);
    s->condition.notify_one();
}

void write_file(semaphore *s, std::ofstream *file, std::uint8_t num) {
    lock(s);
    if (file->is_open()) {
        *file << "Hello from ";
        *file << "thread " << (int)num << std::endl;
        unlock(s);
    } else {
        std::cout << "Something was wrong" << std::endl;
    }
}
int main() {
    std::string path = "prueba.txt";
    std::ofstream file(path);
    std::vector<std::thread> handlers;
    semaphore S1;

    for (std::uint8_t i = 0; i < 255; i++) {
       handlers.emplace_back(write_file, &S1, &file, i);
    }

    for (auto & handle: handlers)
    handle.join();
    file.close();
    return 0;
    
}


