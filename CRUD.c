#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "users.txt"

typedef struct
{
    int id;
    char name[50];
    int age;
} User;

void createUser();
void readUsers();
void updateUser();
void deleteUser();
int userExists(int id);

int main()
{
    int choice;
    while (1)
    {
        printf("\n=== USER MANAGEMENT SYSTEM ===\n");
        printf("1. Add User (Create)\n");
        printf("2. Display Users (Read)\n");
        printf("3. Update User (Update)\n");
        printf("4. Delete User (Delete)\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            createUser();
            break;
        case 2:
            readUsers();
            break;
        case 3:
            updateUser();
            break;
        case 4:
            deleteUser();
            break;
        case 5:
            exit(0);
        default:
            printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}

// Function to check if user already exists
int userExists(int id)
{
    FILE *fp = fopen(FILENAME, "r");
    if (!fp)
        return 0;

    User u;
    while (fscanf(fp, "%d %s %d", &u.id, u.name, &u.age) != EOF)
    {
        if (u.id == id)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void createUser()
{
    User u;
    printf("Enter ID: ");
    scanf("%d", &u.id);

    if (userExists(u.id))
    {
        printf("User with ID %d already exists!\n", u.id);
        return;
    }

    printf("Enter Name: ");
    scanf("%s", u.name);
    printf("Enter Age: ");
    scanf("%d", &u.age);

    FILE *fp = fopen(FILENAME, "a"); // Append mode
    if (!fp)
    {
        printf("Error opening file!\n");
        return;
    }
    fprintf(fp, "%d %s %d\n", u.id, u.name, u.age);
    fclose(fp);
    printf("User added successfully!\n");
}

void readUsers()
{
    FILE *fp = fopen(FILENAME, "r");
    if (!fp)
    {
        printf("No users found! File doesn't exist yet.\n");
        return;
    }

    User u;
    printf("\n--- User Records ---\n");
    while (fscanf(fp, "%d %s %d", &u.id, u.name, &u.age) != EOF)
    {
        printf("ID: %d | Name: %s | Age: %d\n", u.id, u.name, u.age);
    }
    fclose(fp);
}

void updateUser()
{
    int id, found = 0;
    printf("Enter ID of user to update: ");
    scanf("%d", &id);

    FILE *fp = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp)
    {
        printf("Error opening file!\n");
        return;
    }

    User u;
    while (fscanf(fp, "%d %s %d", &u.id, u.name, &u.age) != EOF)
    {
        if (u.id == id)
        {
            found = 1;
            printf("Enter new Name: ");
            scanf("%s", u.name);
            printf("Enter new Age: ");
            scanf("%d", &u.age);
        }
        fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
    }
    fclose(fp);
    fclose(temp);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found)
        printf("User updated successfully!\n");
    else
        printf("User with ID %d not found!\n", id);
}

void deleteUser()
{
    int id, found = 0;
    printf("Enter ID of user to delete: ");
    scanf("%d", &id);

    FILE *fp = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp)
    {
        printf("Error opening file!\n");
        return;
    }

    User u;
    while (fscanf(fp, "%d %s %d", &u.id, u.name, &u.age) != EOF)
    {
        if (u.id == id)
        {
            found = 1; // Skip writing this user
        }
        else
        {
            fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
        }
    }
    fclose(fp);
    fclose(temp);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found)
        printf("User deleted successfully!\n");
    else
        printf("User with ID %d not found!\n", id);
}
