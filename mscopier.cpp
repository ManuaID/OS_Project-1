#define _OPEN_SYS
#define _OPEN_THREADS
#include <pthread.h>
#include <iostream>
#include <queue>
#include <stdlib.h>
#include <fstream>
#include <filesystem>

struct file_info {
    std::string file_name;
    std::queue<std::string> q;
    std::string source_dir;
    std::string destination_dir;
    pthread_mutex_t queue_mutex;
    pthread_cond_t isEmpty;
    pthread_cond_t isNotEmpty;
};

void *read_file(void* args) {
    //As soon as file_info that is incoming is dereferenced then it creates a new file_info f within the stack with all the variable with the new
    struct file_info* f = static_cast<file_info*>(args);

    
    std::string file_path = f->source_dir + "/" + f->file_name;
    
    std::ifstream inputFiles(file_path);
    std::string content;
    
    while(getline(inputFiles, content)) {
        pthread_mutex_lock(&f->queue_mutex);
        f->q.push(content);
        pthread_cond_signal(&f->isNotEmpty);
        pthread_mutex_unlock(&f->queue_mutex);
    }

    inputFiles.close();

    return 0;
}

void *write_to_queue(void* args) {
    struct file_info* f = static_cast<file_info*>(args);

    pthread_mutex_lock(&f->queue_mutex);

    std::ofstream outputFiles;
    outputFiles.open(f->destination_dir + "/" + f->file_name, std::ios::binary);

    std::string content;
    while(f->q.empty()) {
        pthread_cond_wait(&f->isNotEmpty, &f->queue_mutex);
        content = f->q.front();
        f->q.pop();
        outputFiles << content << '\n';
        
        if(f->q.empty()) {
            pthread_mutex_unlock(&f->queue_mutex);
            break;
        }
        
    }

    pthread_cond_signal(&f->isEmpty);
    pthread_mutex_unlock(&f->queue_mutex);

    outputFiles.close();

    return 0;
}

int main(int args, char* argv[]) {
    struct file_info f;

    int n = std::stoi(argv[1]);
    std::string source_dir = argv[2];
    std::string destination_dir = argv[3];

    pthread_t p_read, p_write;

    std::queue<std::string> q;
    const int word_limit = 20;

    // pthread_mutex_init(&f.queue_mutex,NULL);
    // pthread_cond_init(&f.isEmpty, NULL);
    // pthread_cond_init(&f.isNotEmpty, NULL);

    f.file_name = "source.txt";
    f.source_dir = source_dir;
    f.destination_dir = destination_dir;

    pthread_create(&p_read,NULL,read_file,&f);
    pthread_create(&p_write,NULL, write_to_queue, &f);

    pthread_join(p_read,NULL);
    pthread_join(p_write,NULL);

    // pthread_mutex_destroy(&f.queue_mutex);
    // pthread_cond_destroy(&f.isEmpty);
    // pthread_cond_destroy(&f.isNotEmpty);

    free(p_read);
    free(p_write);

    return 0;
}