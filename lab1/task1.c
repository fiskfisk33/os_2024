/* 
Process pairs: Create multiple processes and communicate via FIFOs
Implement a program based on the following specifications:
        Use fork() to create 1 child process
        Let pairs of processes communicate via one or more FIFOs
        use wait() on the master to gracefully terminate the set of processes
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>



int main(int argc, char const *argv[])
{
        const char *fifo_name = "/tmp/myfifo";
        const char *word_buffer = "hello UwU";
        int file_descriptor;
        mkfifo(fifo_name, 0666);


        pid_t pid;
        pid = fork();
        if(pid == -1){
                perror("the fork has failed"); // if the fork process fails
        }
        if(pid == 0){ //This is the child process (Consumer)
                //buffer size:
                const int buf_size = 256;
                // read buffer array of the buffer size
                char read_buffer[buf_size];
               
                //opens the file descriptor in read-only mode 
                file_descriptor = open(fifo_name, O_RDONLY);
                //reads the content with buffer amounts of chars at a time
                read(file_descriptor, read_buffer, buf_size);
                //closes the file descriptor
                close(file_descriptor);
                printf("Message: \"%s\"\n", read_buffer);

                //when it has "gjort sin grej" return 0        
                return 0;
        }else if(pid > 0){ //this is the parent process (Producer)
        
                file_descriptor = open(fifo_name, O_WRONLY);
                write(file_descriptor, word_buffer, strlen(word_buffer) + 1);
                close(file_descriptor);
                wait(NULL);
                unlink(fifo_name);
        }
        return 0;
        
}
