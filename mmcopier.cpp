#include <iostream>
#include <filesystem>
#include <string>
#include <pthread.h>
#include <vector>
#include <fstream>

void *read_write_file(void* arg) {
    std::tuple info_tuple = *static_cast<std::tuple<std::string,std::string, std::string>*>(arg);

    std::string file_name = std::get<0>(info_tuple);
    std::filesystem::path source_dir = std::get<1>(info_tuple);
    std::filesystem::path target_dir = std::get<2>(info_tuple);

    std::string line;

    std::ifstream inputFile(source_dir/file_name, std::ios::binary);
    std::ofstream outfile(target_dir/file_name, std::ios::binary);

    if(!inputFile.is_open() || !outfile.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return nullptr;
    }

    while (std::getline(inputFile, line)) {
        outfile << line << '\n';
    }

    inputFile.close();
    outfile.close();

    return 0;
}

int main(int args, char* argv[]) {

    int n = std::stoi(argv[1]);   // convert string to int
    std::string source = argv[2]; // stays a string
    std::string destination = argv[3];

    std::filesystem::path folder_path= source;
    std::vector<std::tuple<std::string, std::string, std::string>> file_names;
    std::vector<pthread_t> threads;

    file_names.resize(n);
    threads.resize(n);

    if(std::filesystem::exists(folder_path) && std::filesystem::is_directory(folder_path)) {
        for(int i = 0; i < n; ++i) {
            std::string file_name = "source" + std::to_string(i + 1) + ".txt";
            file_names.at(i) = std::make_tuple(file_name, source, destination);
        }

        for(int i = 0; i < n; ++i) {
            pthread_create(&threads[i], NULL, read_write_file, (void*) &file_names.at(i));
        }

        for(int i = 0; i < n; ++i) {
            pthread_join(threads[i],NULL);
        }
    }

    fscanf(stdin, "c");
    return 0;
}
