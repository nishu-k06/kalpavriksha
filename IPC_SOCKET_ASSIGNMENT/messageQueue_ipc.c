#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>

struct msg {//message structure
    long type;
    int n;
    int arr[20];
};

void sort(int a[], int n){
    for(int i=0;i<n-1;i++)
        for(int j=i+1;j<n;j++)
            if(a[i] > a[j]){
                int t=a[i]; a[i]=a[j]; a[j]=t;
            }
}

int main(){
    key_t key = ftok("msg",65);//generate unique key for message queue
    int msgid = msgget(key,0666|IPC_CREAT);//created message queue

    struct msg m;
    printf("Enter no of elements : ");
    scanf("%d",&m.n);//read into message structure

    printf("Enter elements:\n");
    for(int i=0;i<m.n;i++) scanf("%d",&m.arr[i]);//read array elements

    printf("Before sorting:\n");
    for(int i=0;i<m.n;i++) printf("%d ",m.arr[i]);
    printf("\n");

    if(fork()==0){
        msgrcv(msgid,&m,sizeof(m),1,0);//receive message from parent
        sort(m.arr,m.n);
        m.type=2;
        msgsnd(msgid,&m,sizeof(m),0);//send sorted array 
    }
    else{
        m.type=1;
        msgsnd(msgid,&m,sizeof(m),0);//send message to child
        wait(NULL);
        msgrcv(msgid,&m,sizeof(m),2,0);//receive sorted array

        printf("After sorting:\n");
        for(int i=0;i<m.n;i++) printf("%d ",m.arr[i]);
        printf("\n");
        msgctl(msgid,IPC_RMID,NULL);//drop message queue
    }
}
