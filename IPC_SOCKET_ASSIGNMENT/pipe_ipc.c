#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void sort(int a[], int n){
    for(int i=0;i<n-1;i++)
        for(int j=i+1;j<n;j++)
            if(a[i] > a[j]){
                int t=a[i]; a[i]=a[j]; a[j]=t;
            }
}

int main(){
    int fd[2], n, a[20];
    pipe(fd);//creating pipe

    printf("Enter no of elements : ");
    scanf("%d",&n);
    printf("Enter elements:\n");
    for(int i=0;i<n;i++) scanf("%d",&a[i]);

    printf("Before sorting:\n");
    for(int i=0;i<n;i++) printf("%d ",a[i]);
    printf("\n");

    
    if(fork()==0){//child process
        read(fd[0], &n, sizeof(n));//read using pipe
        read(fd[0], a, sizeof(int)*n);

        sort(a,n);

        write(fd[1], a, sizeof(int)*n);//write using pipe
    }
    else{
        write(fd[1], &n, sizeof(n));
        write(fd[1], a, sizeof(int)*n);

        wait(NULL);
        read(fd[0], a, sizeof(int)*n);//read sorted array 

        printf("After sorting:\n");
        for(int i=0;i<n;i++) printf("%d ",a[i]);
        printf("\n");
    }
    return 0;
}
