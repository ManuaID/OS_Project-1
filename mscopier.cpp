#define _OPEN_SYS
#define _OPEN_THREADS
#include <pthread.h>
#include <iostream>
#include <queue>
#include <stdlib.h>
#include <fstream>
#include <filesystem>

struct file_info {

    std::queue<std::string> q;

    std::string file_name;
    std::string source_dir;
    std::string destination_dir;

    int n;
    bool done = false;

    pthread_mutex_t queue_mutex;
    pthread_cond_t startRead;
    pthread_cond_t startWrite;
};


void* read_from_file(void* args) {
    struct file_info *f = static_cast<file_info*>(args);
    std::string file_path = f->source_dir + "/" + f->file_name;

    std::ifstream in_file(file_path);

    std::string temp_line;
    while(std::getline(in_file,temp_line)) {
        pthread_mutex_lock(&f->queue_mutex);

        while(f->q.size() >= f->n) {
            pthread_cond_wait(&f->startRead, &f->queue_mutex);
        }

        f->q.push(temp_line);
        pthread_cond_signal(&f->startWrite);
        pthread_mutex_unlock(&f->queue_mutex);
    }

    pthread_mutex_lock(&f->queue_mutex);
    f->done = true;
    pthread_cond_broadcast(&f->startWrite);
    pthread_mutex_unlock(&f->queue_mutex);


    return nullptr;
};

void* write_from_queue(void* args) {
    struct file_info *f = static_cast<file_info*>(args);

    std::ofstream out_file(f->destination_dir + "/" + f->file_name);

    while(true) {        
        pthread_mutex_lock(&f->queue_mutex);
        while(f->q.empty() && !f->done) {
            pthread_cond_wait(&f->startWrite,&f->queue_mutex);
        }

        if(f->q.empty() && f->done) {
            pthread_mutex_unlock(&f->queue_mutex);
            break;
        }
        std::string content = f->q.front();
        std::cout << content << std::endl;
        f->q.pop();

        pthread_cond_signal(&f->startRead);
        
        out_file << content << '\n';
        pthread_mutex_unlock(&f->queue_mutex);
    }

    return nullptr;
};


int main(int args, char* argv[]) {
    struct file_info f;

    //Getting the value from a comand
    f.n = 20;
    f.source_dir = argv[2];
    f.destination_dir = argv[3];

    f.file_name = "source.txt";

    pthread_t p_read, p_write;

    //Initializing Mutex locks and condition
    pthread_mutex_init(&f.queue_mutex, NULL);
    pthread_cond_init(&f.startRead, NULL);
    pthread_cond_init(&f.startWrite, NULL);

    //Creating a thread for Producer and Consumer
    pthread_create(&p_read, NULL, read_from_file, &f);
    pthread_create(&p_write, NULL, write_from_queue, &f);

    //Waits all the therad to finish it's process before the main runs
    pthread_join(p_read, NULL);
    pthread_join(p_write, NULL);


    pthread_mutex_destroy(&f.queue_mutex);
    pthread_cond_destroy(&f.startRead);
    pthread_cond_destroy(&f.startWrite);

    return 0;
}