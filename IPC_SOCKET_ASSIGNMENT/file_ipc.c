#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

//declaring file name
#define FILE_NAME "data.txt"//

//sorting function
void sort(int a[], int n){
    for(int i=0;i<n-1;i++)
        for(int j=i+1;j<n;j++)
            if(a[i]> a[j]){
                int t= a[i];
                a[i]= a[j];
                a[j]= t;
            }
}


int main(){
    int n, a[20];
    printf("Enter number of elements:");
    scanf("%d",&n);

    printf("Enter elements:\n");
    for(int i=0;i<n;i++){
        scanf("%d",&a[i]);

    printf("before sorting:\n");
    for(int i=0;i<n;i++){
        printf("%d ", a[i]);
    }
    printf("\n");

    FILE *fp = fopen(FILE_NAME,"w");//create and open file

    fprintf(fp,"%d\n",n);//writing into file
    for(int i=0;i<n;i++){
        fprintf(fp,"%d ",a[i]);
    }
    fclose(fp);

    if(fork()==0){//child process
        fp = fopen(FILE_NAME,"r");//open file for reading
        fscanf(fp,"%d",&n);
        for(int i=0;i<n;i++){
            fscanf(fp,"%d",&a[i]);//reading from file
        }
        fclose(fp);

        sort(a,n);

        fp = fopen(FILE_NAME,"w");//writing sorted array into file 
        fprintf(fp,"%d\n",n);
        for(int i=0;i<n;i++){
            fprintf(fp,"%d ",a[i]);
        }
        fclose(fp);
        exit(0);
    }
    else{
        wait(NULL);//waits for child process completion 

        //read and display sorted array
        fp = fopen(FILE_NAME,"r");
        fscanf(fp,"%d",&n);
        for(int i=0;i<n;i++){
            fscanf(fp,"%d",&a[i]);
        }
        fclose(fp);

        printf("after sorting:\n");
        for(int i=0;i<n;i++){
            printf("%d ", a[i]);
        }
        printf("\n");
    }
    return 0;
}
