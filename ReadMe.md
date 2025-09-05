# OS Assignment 2 Project 1
### Task 1)
Task: Create a multithread program that copies a file from a specified directory  and create a file with the content within the copied file into the specified destination directory

Explanation:
This program works by first taking in the number of files, source directory and target directory to copy the file to from the command like.

Example:
`./mmcopier 10 source_dir destination_dir`  
- 10 files to copy,
- Source directory: source_dir
- Destination directory: destination_dir

Then using **n** it generates the necessary filename, then it formulate filename, Source directory and Destination directory into a tuple which then pushed into a vector to be iterated when creating the threads.

After preparing the files it generates **n** thread using pthread_create() function. For the parameter it takes in thread id, void* function, an argument in which the copy of the tuple is created within the memory space of the thread.

When a thread is created, it calls the function `read_write_file()`. Inside this function, the thread unpacks the tuple to obtain the file’s source and destination paths. Using this information, it opens the source file, reads its contents, and then creates a new file in the destination directory with the same name. Finally, it writes the contents of the source file into the new file.

Then to achieve thread synchronisation the main program waits until the **n** number of thread's execution to finishes. in which each thread is managed using `pthread_join()` 

### Task 2)
Task:



