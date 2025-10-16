#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct student { 
    int rollNo;
    char name[50];
    float marks[3];
    float totalMarks;
    float averageMarks;
    char grade;
};

// Function prototypes
float calculateTotal(float marks[]);
float calculateAverage(float totalMarks);
char assignGrade(float averageMarks);
void printStars(char grade);
void printRollNumbers(struct student s[], int n, int index);
int isValidName(char str[]);

int main() {
    int n, i, j;
    struct student s[100]; // Maximum 100 students

    // Input number of students
    while (1) {
        printf("Enter number of students (1-100): ");
        if (scanf("%d", &n) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // clear invalid input
            continue;
        }
        if (n <= 0 || n > 100) {
            printf("Number must be between 1 and 100.\n");
            continue;
        }
        break;
    }

    for (i = 0; i < n; i++) {
        printf("\n Enter student %d details \n", i + 1);

        // Roll number input
        int validRoll;
        do {
            validRoll = 1;
            printf("Enter Roll Number: ");
            if (scanf("%d", &s[i].rollNo) != 1) {
                printf("Invalid input! Roll number must be an integer.\n");
                validRoll = 0;
                while (getchar() != '\n'); // clear buffer
                continue;
            }
            if (s[i].rollNo <= 0) {
                printf("Roll number must be positive.\n");
                validRoll = 0;
                continue;
            }
            // Check for duplicate roll number
            for (int k = 0; k < i; k++) {
                if (s[i].rollNo == s[k].rollNo) {
                    printf("Roll number %d already exists. Enter a unique one.\n", s[i].rollNo);
                    validRoll = 0;
                    break;
                }
            }
        } while (!validRoll);

        // Name input
        do {
            printf("Enter Name: ");
            scanf("%s", s[i].name);
            if (!isValidName(s[i].name)) {
                printf("Invalid name! Only letters are allowed.\n");
            }
        } while (!isValidName(s[i].name));

        // Marks input
        for (j = 0; j < 3; j++) {
            int validMark;
            do {
                validMark = 1;
                printf("Enter marks for subject %d: ", j + 1);
                if (scanf("%f", &s[i].marks[j]) != 1) {
                    printf("Invalid input! Marks must be a number.\n");
                    validMark = 0;
                    while (getchar() != '\n'); // clear buffer
                    continue;
                }
                if (s[i].marks[j] < 0 || s[i].marks[j] > 100) {
                    printf("Invalid marks! Must be between 0 and 100.\n");
                    validMark = 0;
                }
            } while (!validMark);
        }

        s[i].totalMarks = calculateTotal(s[i].marks);
        s[i].averageMarks = calculateAverage(s[i].totalMarks);
        s[i].grade = assignGrade(s[i].averageMarks);
    }

    // Display student details
    for (i = 0; i < n; i++) {
        printf("\nRoll: %d", s[i].rollNo);
        printf("\nName: %s", s[i].name);
        printf("\nTotal: %.2f", s[i].totalMarks);
        printf("\nAverage: %.2f", s[i].averageMarks);
        printf("\nGrade: %c", s[i].grade);

        if (s[i].grade != 'F') {
            printf("\nPerformance: ");
            printStars(s[i].grade);
        }
        printf("\n");
    }

    printf("\nList of Roll Numbers (via recursion): ");
    printRollNumbers(s, n, 0);
    printf("\n");

    return 0;
}

// Calculate total marks
float calculateTotal(float marks[]) {
    return marks[0] + marks[1] + marks[2];
}

// Calculate average marks
float calculateAverage(float totalMarks) {
    return totalMarks / 3;
}

// Assign grade based on average marks
char assignGrade(float avgMarks) {
    if (avgMarks >= 85)
        return 'A';
    else if (avgMarks >= 70)
        return 'B';
    else if (avgMarks >= 50)
        return 'C';
    else if (avgMarks >= 35)
        return 'D';
    else
        return 'F';
}

// Print stars for grade
void printStars(char grade) {
    int numStars = 0;
    switch (grade) {
        case 'A': numStars = 5; break;
        case 'B': numStars = 4; break;
        case 'C': numStars = 3; break;
        case 'D': numStars = 2; break;
        default: numStars = 0; break;
    }
    for (int i = 0; i < numStars; i++) {
        printf("*");
    }
}

// Print roll numbers using recursion
void printRollNumbers(struct student s[], int n, int index) {
    if (index == n)
        return;
    printf("%d ", s[index].rollNo);
    printRollNumbers(s, n, index + 1);
}

//name validation function
int isValidName(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalpha(str[i]))
            return 0;
    }
    return 1;
}
