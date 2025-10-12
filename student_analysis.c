#include <stdio.h>
#include <string.h>

struct student {
    int roll_no;
    char name[50];
    float marks[3];
    float total_marks;
    float average_marks;
    char grade;
};

float Total(float marks[]);
float Average(float total_marks);
char assignGrade(float average_marks);
void Stars(char grade);
void printRollNumbers(struct student s[], int n, int index);

int main() {
    int n, i;
    struct student s[100]; 

    printf("Enter number of students: ");
    scanf("%d", &n);

    for (i = 0; i < n; i++) {
        printf("student %d details : ", i + 1);
        scanf("%d %s %f %f %f", &s[i].roll_no, s[i].name, &s[i].marks[0], &s[i].marks[1], &s[i].marks[2]);

        s[i].total_marks = Total(s[i].marks);
        s[i].average_marks = Average(s[i].total_marks);

        s[i].grade = assignGrade(s[i].average_marks);
    }

    for (i = 0; i < n; i++) {
        printf("\nRoll: %d", s[i].roll_no);
        printf("\nName: %s", s[i].name);
        printf("\nTotal: %.2f", s[i].total_marks);
        printf("\nAverage: %.2f", s[i].average_marks);
        printf("\nGrade: %c", s[i].grade);

        if (s[i].grade == 'F') {
            printf("\n");
            continue;
        }

        printf("\nPerformance: ");
        Stars(s[i].grade);
        printf("\n");
    }
    printf("\nList of Roll Numbers (via recursion): ");
    printRollNumbers(s, n, 0);

    return 0;
}

float Total(float marks[]) {
    return marks[0] + marks[1] + marks[2];
}

float Average(float total_marks) {
    return total_marks / 3;
}

char assignGrade(float avg_marks) {
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

void Stars(char grade) {
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

void printRollNumbers(struct student s[], int n, int index) {
    if (index == n)
        return;
    printf("%d ", s[index].roll_no);
    printRollNumbers(s, n, index + 1);
}
