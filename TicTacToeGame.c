#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_SIZE 10 // Maximum grid size

// Function prototypes
void initializeBoard(char board[MAX_SIZE][MAX_SIZE], int size);
void printBoard(char board[MAX_SIZE][MAX_SIZE], int size);

int main()
{
    int size;
    printf("Enter the size of the Tic Tac Toe board (max 10): ");
    scanf("%d", &size);
    char board[MAX_SIZE][MAX_SIZE];
    initializeBoard(board, size);
    printBoard(board, size);

    return 0;
}

// Function to initialize the Tic Tac Toe board
void initializeBoard(char board[MAX_SIZE][MAX_SIZE], int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            board[i][j] = ' '; // Initialize each cell with a space
        }
    }
}

void printBoard(char board[MAX_SIZE][MAX_SIZE], int size)
{
    // for loop for the rows
    for (int i = 0; i < size; i++)
    {
        // Print horizontal separator
        // Use if statement to remove first line of dashes
        if (i > 0)
        {
            // for loop to print dashes depending on size
            for (int k = 0; k < size - 1; k++)
            {
                printf("----");
            }
            printf("\n");
        }
        // for loop for the columns
        for (int j = 0; j < size; j++)
        {
            // Initialize each cell with '0' and print it
            printf("%c ", board[i][j]);
            if (j < size - 1)
            {
                // Print vertical separator
                printf("|");
            }
        }
        printf("\n");
    }
}