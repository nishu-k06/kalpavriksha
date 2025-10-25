#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//function prototypes
void printMatrix(int *matrix, int n);
void rotateMatrix90Clockwise(int *matrix, int n);
void matrixSmoothingFilter(int *matrix, int n);

int main(){
    int n ,matrix[10][10];
    printf("Enter matrix size (2-10): ");
    
    // Input validation for matrix size
    while(1){
        if( scanf("%d", &n) != 1){
            printf("Invalid input, please enter a number between 2 and 10:  ");
            while(getchar() != '\n'); // clear buffer
            continue;
        }
        if(n >= 2 && n <= 10) break;
        printf("Invalid size, Please  enter a number between 2 and 10: ");
    }
    srand(time(0));// Seed for random number generation
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            *((int *)matrix + i * n + j) = (rand() % 255) + 1;
        }
    }
    printf("\nOriginal Randomly Generated Matrix:\n");
    
    printMatrix((int *)matrix, n);// Print the original matrix

    printf("Matrix after 90 degree Clockwise Rotation:\n");

    rotateMatrix90Clockwise((int *)matrix, n); // Rotate the matrix 90 degrees clockwise

    printMatrix((int *)matrix, n); // Print the rotated matrix
    printf("\n");

    printf("Matrix after Applying 3x3 Smoothing Filter:\n");

    matrixSmoothingFilter((int *)matrix, n); // Apply 3x3 smoothing filter

    printMatrix((int *)matrix, n); // Print the smoothed matrix 

    return 0;
}

// Function to print the original matrix
void printMatrix(int *matrix, int n){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            printf("%4d ", *(matrix+i*n+j));
        }
        printf("\n");
    }
    printf("\n");
}

// Function to rotate the matrix 90 degrees clockwise
void rotateMatrix90Clockwise(int *matrix, int n){
    int temp;
    
    // Transpose the matrix
    for(int i = 0; i < n; i++){
        for(int j = i + 1 ; j < n; j++){
            temp = *(matrix + i*n + j);
            *(matrix + i*n + j) = *(matrix + j * n + i);
            *(matrix + j * n + i) = temp;
        }
    }

    // Reverse each row
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n / 2; j++){
            temp = *(matrix + i*n + j);
            *(matrix + i*n + j) = *(matrix + i*n + (n - 1 - j));
            *(matrix + i*n + (n - 1 - j)) = temp;
        }
    }


}

// Function to apply a 3x3 smoothing filter
void matrixSmoothingFilter(int *matrix, int n){
    
    //iterate through each element of the matrix
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            
            int sum = 0;
            int count = 0;

            // Iterate through the 3x3 neighborhood
            for(int rowOffset = -1; rowOffset <= 1; rowOffset++){
                for(int colOffset = -1; colOffset <= 1; colOffset++){
                    int adjacentRow = i + rowOffset;
                    int adjacentCol = j + colOffset;
                    if(adjacentRow >= 0 && adjacentRow < n && adjacentCol >= 0 && adjacentCol < n){
                        sum += *(matrix + adjacentRow*n + adjacentCol);
                        count++;
                    }
                }
            }
            *(matrix + i*n + j) = sum / count;// Update with average value
        }
    } 

}
