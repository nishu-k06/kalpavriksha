#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 8080

int main(){
    int sock,choice,amount,result;
    struct sockaddr_in addr;//server address structure

    sock = socket(AF_INET,SOCK_STREAM, 0);//create socket 

    addr.sin_family =AF_INET;
    addr.sin_port =htons(PORT);
    addr.sin_addr.s_addr= INADDR_ANY;

    connect(sock,(struct sockaddr*)&addr, sizeof(addr));//connect to server

    do {
        printf("1. Withdraw\n");
        printf("2. Deposit\n");
        printf("3. Display Balance\n");
        printf("4. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        write(sock, &choice, sizeof(choice));

        if (choice ==1 || choice ==2) {// Withdraw or Deposit
            printf("enter amount:");
            scanf("%d", &amount);
            write(sock, &amount,sizeof(amount));

            read(sock, &result,sizeof(result));
            if (result == -1)//in case of insufficient balance
                printf("Insufficient Balance\n");
            else
                printf("Updated Balance: %d\n", result);
        }
        else if (choice == 3) {
            read(sock, &result, sizeof(result));
            printf("Current Balance: %d\n", result);
        }

    } while (choice != 4);

    close(sock);//close socket
    return 0;
}
