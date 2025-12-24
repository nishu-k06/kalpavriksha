#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>

//port number for server
#define PORT 8080

pthread_mutex_t lock;//mutex variable for critical section

void* handleClient(void* arg) {
    int sock =*(int*)arg;//initialize socket
    free(arg);  // prevent memory leak

    int choice,amount,balance;

    while (1) {
        if (read(sock, &choice, sizeof(choice)) <= 0)//read choice from client
            break;

        if (choice ==4)
            break;

        pthread_mutex_lock(&lock);

        FILE *fp=fopen("accountDB.txt", "r+");//open file to read and write 
        if (fp==NULL) {//if file doesn't exist
            perror("File open failed");
            balance = 0;
        } else{
            if(fscanf(fp,"%d", &balance) != 1)
                balance =0;
        }

        if(choice== 1){ // Withdraw
            read(sock,&amount,sizeof(amount));
            if (balance>= amount) {
                balance-= amount;
                fseek(fp,0, SEEK_SET);//move file pointer to beginning
                fprintf(fp,"%d\n",balance);
                write(sock,&balance,sizeof(balance));//send updated balance to client
            } else {
                int fail =-1;
                write(sock,&fail,sizeof(fail));
            }
        }
        else if(choice==2){ // Deposit
            read(sock,&amount,sizeof(amount));
            balance+=amount;
            fseek(fp,0,SEEK_SET);
            fprintf(fp,"%d\n",balance);
            write(sock,&balance, sizeof(balance));
        }
        else if (choice== 3) { // Display
            write(sock,&balance, sizeof(balance));
        }

        if (fp!=NULL)
            fclose(fp);

        pthread_mutex_unlock(&lock);
    }

    close(sock);
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in addr;//server address structure
    pthread_t tid;

    pthread_mutex_init(&lock, NULL);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;//IPv4
    addr.sin_addr.s_addr = INADDR_ANY;//accept connections from any address
    addr.sin_port = htons(PORT);//convert port number to network byte order

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));//bind socket to address and port
    listen(server_fd, 5);   //check for incoming connections

    printf("ATM Server running...\n");

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);//accept incoming connection

        int *new_sock = malloc(sizeof(int));
        *new_sock = client_fd;

        pthread_create(&tid, NULL, handleClient, new_sock);//create thread for each client
        pthread_detach(tid);//detach thread 
    }
}
