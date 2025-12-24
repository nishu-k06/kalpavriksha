#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void sort(int a[], int n){//sorting inside shared memory
    for(int i=0;i<n-1;i++)
        for(int j=i+1;j<n;j++)
            if(a[i] > a[j]){
                int t=a[i]; 
                a[i]=a[j]; 
                a[j]=t;
            }
}

int main(){
    key_t key = ftok("shm",65);//generate unique key for shared memory
    int shmid = shmget(key, sizeof(int)*21,0666|IPC_CREAT);//create shared memory 
    int *data = (int*)shmat(shmid,NULL,0);//attach shared memory to a pointer

    int n;
    printf("Enter number of elements: ");
    scanf("%d",&n);
    data[0]=n;

    //writing inside shared memory

    printf("Enter elements:\n");
    for(int i=0;i<n;i++) scanf("%d",&data[i+1]);

    printf("Before sorting:\n");
    for(int i=1;i<=n;i++) printf("%d ",data[i]);
    printf("\n");

    if(fork()==0){
        sort(&data[1], data[0]);//sort array in shared memory
        exit(0);
    }
    else{
        wait(NULL);
        printf("After sorting:\n");
        for(int i=1;i<=n;i++) {
            printf("%d ",data[i]);
        }
        printf("\n");

        shmdt(data);//detach shared memory
        shmctl(shmid,IPC_RMID,NULL);//drop shared memory
    }
}
