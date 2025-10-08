#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define SIZE 200

// function to remove spaces from expression
void removeSpaces(char exp[])
{
    int i, j = 0;
    for (i = 0; exp[i] != '\0'; i++)
    {
        if (!isspace(exp[i]))
        { // skip spaces
            exp[j++] = exp[i];
        }
    }
    exp[j] = '\0';
}

// function that solves * and / first
int evaluate(char exp[], int *err)
{
    int numbers[SIZE];
    char signs[SIZE];
    int numTop = 0, signTop = 0;

    int i = 0;
    while (exp[i] != '\0')
    {
        if (isdigit(exp[i]))
        {
            int val = 0;
            while (isdigit(exp[i]))
            {
                val = val * 10 + (exp[i] - '0');
                i++;
            }
            numbers[numTop++] = val;
        }
        else if (exp[i] == '*' || exp[i] == '/')
        {
            char op = exp[i];
            i++;

            int val = 0;
            while (isdigit(exp[i]))
            {
                val = val * 10 + (exp[i] - '0');
                i++;
            }

            int left = numbers[numTop--]; // take previous number
            if (op == '*')
            {
                numbers[numTop++] = left * val;
            }
            else
            {
                if (val == 0)
                { // division by zero
                    *err = 1;
                    return 0;
                }
                numbers[numTop++] = left / val;
            }
        }
        else if (exp[i] == '+' || exp[i] == '-')
        {
            // just store the + or - to apply later
            signs[signTop++] = exp[i];
            i++;
        }
        else
        {
            *err = 2; // invalid symbol like 'a'
            return 0;
        }
    }

    // now apply + and - in order
    int ans = numbers[0];
    int k = 1;
    for (int j = 0; j <= signTop; j++)
    {
        if (signs[j] == '+')
            ans += numbers[k++];
        else
            ans -= numbers[k++];
    }
    return ans;
}

int main()
{
    char exp[SIZE];
    printf("Enter expression: ");
    fgets(exp, sizeof(exp), stdin);

    removeSpaces(exp);

    int error = 0;
    int result = evaluate(exp, &error);

    if (error == 1)
        printf("Error: Division by zero.\n");
    else if (error == 2)
        printf("Error: Invalid expression.\n");
    else
        printf("Result = %d\n", result);

    return 0;
}
