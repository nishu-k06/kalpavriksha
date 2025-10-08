#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "users.txt"

typedef struct
{
    int UserId;
    char UserName[50];
    int UserAge;
} User;

void createUser();
void readUsers();
void updateUser();
void deleteUser();
int userExists(int UserId);
int validName(const char *UserName);
int validAge(int UserAge);


int validName(const char *UserName)
{
    for (int i = 0; UserName[i] != '\0'; i++)
    {
        if (!((UserName[i] >= 'A' && UserName[i] <= 'Z') ||
              (UserName[i] >= 'a' && UserName[i] <= 'z') ||
              UserName[i] == ' ')) // allow spaces
        {
            return 0;
        }
    }
    return 1;
}

int validAge(int UserAge)
{
    return UserAge > 0;
}


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

int userExists(int UserId)
{
    FILE *fp = fopen(FILENAME, "r");
    if (!fp)
        return 0;

    User u;
    while (fscanf(fp, "%d %49s %d", &u.UserId, u.UserName, &u.UserAge) != EOF)
    {
        if (u.UserId == UserId)
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
    printf("Enter User ID: ");
    scanf("%d", &u.UserId);

    if (userExists(u.UserId))
    {
        printf("User with ID %d already exists!\n", u.UserId);
        return;
    }

    do
    {
        printf("Enter Name (alphabets only): ");
        scanf("%49s", u.UserName);
        if (!validName(u.UserName))
            printf("Invalid name! Please use letters only.\n");
    } while (!validName(u.UserName));

    do
    {
        printf("Enter Age (> 0): ");
        scanf("%d", &u.UserAge);
        if (!validAge(u.UserAge))
            printf("Invalid age! Must be greater than 0.\n");
    } while (!validAge(u.UserAge));

    FILE *fp = fopen(FILENAME, "a"); 
    if (!fp)
    {
        printf("Error opening file!\n");
        return;
    }
    fprintf(fp, "%d %s %d\n", u.UserId, u.UserName, u.UserAge);
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
    while (fscanf(fp, "%d %49s %d", &u.UserId, u.UserName, &u.UserAge) != EOF)
    {
        printf("ID: %d | Name: %s | Age: %d\n", u.UserId, u.UserName, u.UserAge);
    }
    fclose(fp);
}

void updateUser()
{
    int UserId, found = 0;
    printf("Enter User ID to update: ");
    scanf("%d", &UserId);

    FILE *fp = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp)
    {
        printf("Error opening file!\n");
        if (fp)
            fclose(fp);
        if (temp)
            fclose(temp);
        return;
    }

    User u;
    while (fscanf(fp, "%d %49s %d", &u.UserId, u.UserName, &u.UserAge) != EOF)
    {
        if (u.UserId == UserId)
        {
            found = 1;

            do
            {
                printf("Enter new Name (alphabets only): ");
                scanf("%49s", u.UserName);
                if (!validName(u.UserName))
                    printf("Invalid name! Please use letters only.\n");
            } while (!validName(u.UserName));

            do
            {
                printf("Enter new Age (> 0): ");
                scanf("%d", &u.UserAge);
                if (!validAge(u.UserAge))
                    printf("Invalid age! Must be greater than 0.\n");
            } while (!validAge(u.UserAge));
        }
        fprintf(temp, "%d %s %d\n", u.UserId, u.UserName, u.UserAge);
    }
    fclose(fp);
    fclose(temp);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found)
        printf("User updated successfully!\n");
    else
        printf("User with ID %d not found!\n", UserId);
}

void deleteUser()
{
    int UserId, found = 0;
    printf("Enter User ID to delete: ");
    scanf("%d", &UserId);

    FILE *fp = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp)
    {
        printf("Error opening file!\n");
        if (fp)
            fclose(fp);
        if (temp)
            fclose(temp);
        return;
    }

    User u;
    while (fscanf(fp, "%d %49s %d", &u.UserId, u.UserName, &u.UserAge) != EOF)
    {
        if (u.UserId == UserId)
        {
            found = 1; // Skip writing this user
        }
        else
        {
            fprintf(temp, "%d %s %d\n", u.UserId, u.UserName, u.UserAge);
        }
    }
    fclose(fp);
    fclose(temp);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found)
        printf("User deleted successfully!\n");
    else
        printf("User with ID %d not found!\n", UserId);
}
