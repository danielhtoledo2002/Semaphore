#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include<condition_variable>
struct semaphore  {
    std::mutex mtx ;
    std::atomic_bool desbloqueado;
    std::condition_variable condition;
    semaphore();
};
semaphore::semaphore(){
    desbloqueado.store(true, std::memory_order_release);
}

void bloquear(semaphore * s){

    std::unique_lock<std::mutex> lock(s->mtx);
    s->condition.wait(lock, [&s]{return s->desbloqueado.load(std::memory_order_acquire);});
    s->desbloqueado.store(false, std::memory_order_release);
}
void desbloquear(semaphore *s){
    std::unique_lock<std::mutex>lock(s->mtx);
    s->desbloqueado.store(true, std::memory_order_release);
    s->condition.notify_one();
}

void write_file(semaphore *s, std::ofstream *file, std::uint8_t num) {
    bloquear(s);
    if (file->is_open()) {
        *file << "Hola desde ";
        *file << "el proceso " << (int)num << std::endl;
        desbloquear(s);
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


