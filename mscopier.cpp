#define _OPEN_SYS
#define _OPEN_THREADS
#include <pthread.h>
#include <iostream>
#include <queue>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <unordered_set>

struct file_info {

    std::queue<std::string> q;

    std::string file_name;
    std::string source_dir;
    std::string destination_dir;

    int word_limit = 20;
    int num_active_reader = 0;
    bool done = false;

    pthread_mutex_t queue_mutex;
    pthread_mutex_t writer_mutex;
    pthread_cond_t startRead;
    pthread_cond_t startWrite;

    std::unordered_set<std::string> seen;

    std::ofstream out_file;
};


void* read_from_file(void* args) {
    struct file_info *f = static_cast<file_info*>(args);
    std::string file_path = f->source_dir + "/" + f->file_name;

    std::ifstream in_file(file_path);
    std::string temp_line;

    pthread_mutex_lock(&f->queue_mutex);
    f->num_active_reader++;
    pthread_mutex_unlock(&f->queue_mutex);

    while(std::getline(in_file,temp_line)) {
        pthread_mutex_lock(&f->queue_mutex);

        while(f->q.size() >= f->word_limit) {
            pthread_cond_wait(&f->startRead, &f->queue_mutex);
        }

        if(f->seen.find(temp_line) == f->seen.end()) {
            f->q.push(temp_line);
            f->seen.insert(temp_line);
            pthread_cond_signal(&f->startWrite);
        }
        pthread_mutex_unlock(&f->queue_mutex);
    }

    pthread_mutex_lock(&f->queue_mutex);
    f->num_active_reader--;
    if(f->num_active_reader <= 0) {
        f->done = true;
    }
    pthread_cond_broadcast(&f->startWrite);
    pthread_mutex_unlock(&f->queue_mutex);

    return 0;
};

void* write_from_queue(void* args) {
    struct file_info *f = static_cast<file_info*>(args);

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
        f->q.pop();
        
        
        
        pthread_cond_signal(&f->startRead);
        pthread_mutex_unlock(&f->queue_mutex);
        
        pthread_mutex_lock(&f->writer_mutex);
        
        f->out_file << content << '\n';

        pthread_mutex_unlock(&f->writer_mutex);
    }

    return nullptr;
};


int main(int args, char* argv[]) {
    struct file_info f;

    std::vector<std::pair<pthread_t,pthread_t>> threads;
    
    //Getting the value from a comand
    int n = std::stoi(argv[1]);
    f.source_dir = argv[2];
    f.destination_dir = argv[3];
    
    f.file_name = "source.txt";
    
    threads.resize(n);

    f.out_file.open(f.destination_dir + "/" + f.file_name,std::ios::binary);

    //Initializing Mutex locks and condition
    pthread_mutex_init(&f.queue_mutex, NULL);
    pthread_mutex_init(&f.writer_mutex, NULL);
    pthread_cond_init(&f.startRead, NULL);
    pthread_cond_init(&f.startWrite, NULL);

    //Creating a thread for Producer and Consumer
    for(int i = 0; i < n; ++i)  {
        pthread_create(&threads[i].first, NULL, read_from_file, &f);
        pthread_create(&threads[i].second, NULL, write_from_queue, &f);
    }

    //Waits all the therad to finish it's process before the main runs
    for(int i = 0;i < n; ++i) {
        pthread_join(threads[i].first, NULL);
        pthread_join(threads[i].second, NULL);
    }

    //Delete all mutex and condition
    pthread_mutex_destroy(&f.queue_mutex);
    pthread_mutex_destroy(&f.writer_mutex);
    pthread_cond_destroy(&f.startRead);
    pthread_cond_destroy(&f.startWrite);

    return 0;
}