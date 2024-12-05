/* 
Implement a program based on the following specifications:
        Use fork() to create >= 1 child processes
        Let the parent and all its child processes communicate via 1-2 message queues
        Define a protocol that is used by the parent to send commands to the workers
        Bonus:
                Implement a queue used by the children to send messages to the parent
        Use wait() on the master to gracefully terminate the set of processes
*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <fcntl.h>
#include <string.h>
#include <wait.h>

const int msg_num_max = 10;
const int msg_size_max = 128;
const char *name = "/mymq";

int main(int argc, char const *argv[]) {
        /* code */
        ssize_t n;
        mqd_t mqueue; //message queue descriptor type
        struct mq_attr attr;
        attr.mq_maxmsg = msg_num_max;
        attr.mq_msgsize = msg_size_max;
        mqueue = mq_open(name, O_CREAT, 0666, &attr);
        mq_close(mqueue);
        const char *write_msg = "Hello UwU";

        pid_t pid;
        pid = fork();
        if (pid == -1) {
                perror("the fork has failed"); // if the fork process fails
        }
        if (pid == 0) { // This is the child process (Consumer)
                char buf[msg_size_max];
                mqueue = mq_open(name, O_RDONLY);
                if(mqueue == -1){
                        perror("mq open failed, child");
                }
                n = mq_receive(mqueue, buf, msg_size_max, NULL);
                buf[n] = '\0';
                printf("Received msg: %s\n", buf);
                mq_close(mqueue);

        }else if(pid > 0){ //this is the parent process (Producer)
                mqueue = mq_open(name, O_WRONLY);
                if(mqueue == -1){
                        perror("mq open failed, parent");
                        _exit(1);
                }
                mq_send(mqueue, write_msg, strlen(write_msg), 0);
                mq_close(mqueue);
                wait(NULL);
        }

                mq_unlink(name);
        return 0;
}
