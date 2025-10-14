#include <stdio.h>
#include <string.h>

struct student { //creating student structure to store student details
    int roll_no;
    char name[50];
    float marks[3];
    float total_marks;
    float average_marks;
    char grade;
};

float Total(float marks[]); //function prototypes
float Average(float total_marks);
char assignGrade(float average_marks);
void Stars(char grade);
void printRollNumbers(struct student s[], int n, int index);

int main() { //main function
    int n, i, j;
    struct student s[100]; //array of student structures

    printf("Enter number of students: ");
    scanf("%d", &n);

    for (i = 0; i < n; i++) { //loop to input student details
        printf("\nstudent %d details: ", i + 1);
        int flag;;
        do {
            flag = 0;
            printf("\nEnter Roll Number: ");
            scanf("%d", &s[i].roll_no);//roll number input

            // Check for negative or zero(0) roll number
            if (s[i].roll_no <= 0) {
                printf("Roll number must be positive, please enter again: \n");
                flag = 1;
                continue;
            }

            // Check for repeated roll number
            for (int k = 0; k < i; k++) {
                if (s[i].roll_no == s[k].roll_no) {
                    printf("Roll Number %d already exists, please enter a unique Roll Number.\n", s[i].roll_no);
                    flag = 1;
                    break;
                }
            }
        } while (flag);

        printf("Enter Name: ");
        scanf("%s", s[i].name);

        for (j = 0; j < 3; j++) {//marks input with validation
            do {
                printf("Enter marks for subject %d : ", j + 1);
                scanf("%f", &s[i].marks[j]);

                if (s[i].marks[j] < 0 || s[i].marks[j] > 100) {
                    printf("Invalid marks, please enter again: \n");
                }
            } while (s[i].marks[j] < 0 || s[i].marks[j] > 100);
        }

        s[i].total_marks = Total(s[i].marks);
        s[i].average_marks = Average(s[i].total_marks);
        s[i].grade = assignGrade(s[i].average_marks);
    }

    for (i = 0; i < n; i++) {//loop to display student details
        printf("\nRoll: %d", s[i].roll_no);
        printf("\nName: %s", s[i].name);
        printf("\nTotal: %.2f", s[i].total_marks);
        printf("\nAverage: %.2f", s[i].average_marks);
        printf("\nGrade: %c", s[i].grade);

        if (s[i].grade != 'F') {//print stars only for passed students
            printf("\nPerformance: ");
            Stars(s[i].grade);
        }
        printf("\n");
    }

    printf("\nList of Roll Numbers (via recursion): ");
    printRollNumbers(s, n, 0);
    printf("\n");

    return 0;
}

float Total(float marks[]) { //function to calculate total marks
    return marks[0] + marks[1] + marks[2];
}

float Average(float total_marks) { //function to calculate average marks
    return total_marks / 3;
}

char assignGrade(float avg_marks) { //function to assign grade based on average marks
    if (avg_marks >= 85)
        return 'A';
    else if (avg_marks >= 70)
        return 'B';
    else if (avg_marks >= 50)
        return 'C';
    else if (avg_marks >= 35)
        return 'D';
    else
        return 'F';
}

void Stars(char grade) { //function to print stars based on grade
    int stars = 0;
    switch (grade) {
        case 'A': stars = 5; break;
        case 'B': stars = 4; break;
        case 'C': stars = 3; break;
        case 'D': stars = 2; break;
        default: stars = 0; break;
    }
    for (int i = 0; i < stars; i++) {
        printf("*");
    }
}

void printRollNumbers(struct student s[], int n, int index) { //function to print roll numbers using recursion
    if (index == n)
        return;
    printf("%d ", s[index].roll_no);
    printRollNumbers(s, n, index + 1);
}
