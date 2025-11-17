#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_SIZE 10 // Maximum grid size

// --- AI support: globals, prototypes and helper functions ---
// Enable/disable AI and which side AI will play ('X' or 'O')
int aiEnabled = 0;
char aiPlayer = 'O'; // default if AI enabled

// Function prototypes
void initializeBoard(char board[MAX_SIZE][MAX_SIZE], int size);
void printBoard(char board[MAX_SIZE][MAX_SIZE], int size);
void playerMove(char board[MAX_SIZE][MAX_SIZE], int size, char player);
int checkWin(char board[MAX_SIZE][MAX_SIZE], int size);
int checkDraw(char board[MAX_SIZE][MAX_SIZE], int size);
void updateScore(char winner, int *scoreX, int *scoreO);

int main()
{
    int size;
    int scoreX = 0, scoreO = 0;
    int playAgain = 1;

    while (playAgain)
    {
        printf("Enter the size of the Tic Tac Toe board (max 10): ");
        if (scanf("%d", &size) != 1)
        {
            fprintf(stderr, "Invalid input for board size.\n");
            return 1;
        }
        if (size < 1 || size > MAX_SIZE)
        {
            fprintf(stderr, "Size must be between 1 and %d.\n", MAX_SIZE);
            return 1;
        }

        char board[MAX_SIZE][MAX_SIZE];
        int gameOver = 0;
        char currentPlayer = 'X';
        char winner = ' ';

        // 1. Initialize game board
        initializeBoard(board, size);

        // 3. Loop until the game is over
        while (!gameOver)
        {
            // a. Display the 2D game board
            printBoard(board, size);

            // b. Player makes a move
            playerMove(board, size, currentPlayer);

            // c. Check if the current player has won
            if (checkWin(board, size))
            {
                winner = currentPlayer;
                printBoard(board, size);
                updateScore(winner, &scoreX, &scoreO);
                gameOver = 1;
            }
            // d. If not, check for a draw
            else if (checkDraw(board, size))
            {
                printBoard(board, size);
                updateScore(' ', &scoreX, &scoreO);
                gameOver = 1;
            }
            // e. Switch players and continue
            else
            {
                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
            }
        }

        // Display current scores
        printf("\n--- Current Scores ---\n");
        printf("Player X: %d\n", scoreX);
        printf("Player O: %d\n", scoreO);
        printf("Draws: %d\n", scoreX + scoreO > 0 ? (scoreX + scoreO) / 2 : 0);

        // Ask if user wants to play again
        printf("\nDo you want to play again? (1 for yes, 0 for no): ");
        if (scanf("%d", &playAgain) != 1)
        {
            playAgain = 0;
        }
        printf("\n");
    }

    printf("Thanks for playing! Final Scores - X: %d, O: %d\n", scoreX, scoreO);
    return 0;
}

// Function to initialize the Tic Tac Toe board
void initializeBoard(char board[MAX_SIZE][MAX_SIZE], int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            board[i][j] = ' ';
        }
    }
}

void printBoard(char board[MAX_SIZE][MAX_SIZE], int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf(" %c ", board[i][j]);
            if (j < size - 1)
                printf("|");
        }
        printf("\n");

        if (i < size - 1)
        {
            for (int k = 0; k < size; k++)
            {
                printf("---");
                if (k < size - 1)
                    printf("+");
            }
            printf("\n");
        }
    }
}

void playerMove(char board[MAX_SIZE][MAX_SIZE], int size, char player)
{
    int row, col;
    while (1)
    {
        printf("Player %c, enter your move (row and column, 0-based indices): ", player);
        if (scanf("%d %d", &row, &col) != 2)
        {
            printf("Invalid input. Please enter two integers.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }
            continue;
        }

        if (row >= 0 && row < size && col >= 0 && col < size)
        {
            if (board[row][col] == ' ')
            {
                board[row][col] = player;
                break;
            }
            else
            {
                printf("Cell is already occupied. Try again.\n");
            }
        }
        else
        {
            printf("Row and column must be between 0 and %d. Try again.\n", size - 1);
        }
    }
}

int checkWin(char board[MAX_SIZE][MAX_SIZE], int size)
{
    for (int i = 0; i < size; i++)
    {
        if (board[i][0] != ' ')
        {
            int rowWin = 1;
            for (int j = 1; j < size; j++)
            {
                if (board[i][j] != board[i][0])
                {
                    rowWin = 0;
                    break;
                }
            }
            if (rowWin)
            {
                printf("Player %c wins!\n", board[i][0]);
                return 1;
            }
        }

        if (board[0][i] != ' ')
        {
            int colWin = 1;
            for (int j = 1; j < size; j++)
            {
                if (board[j][i] != board[0][i])
                {
                    colWin = 0;
                    break;
                }
            }
            if (colWin)
            {
                printf("Player %c wins!\n", board[0][i]);
                return 1;
            }
        }
    }

    if (board[0][0] != ' ')
    {
        int diagWin = 1;
        for (int i = 1; i < size; i++)
        {
            if (board[i][i] != board[0][0])
            {
                diagWin = 0;
                break;
            }
        }
        if (diagWin)
        {
            printf("Player %c wins!\n", board[0][0]);
            return 1;
        }
    }

    if (board[0][size - 1] != ' ')
    {
        int diagWin = 1;
        for (int i = 1; i < size; i++)
        {
            if (board[i][size - 1 - i] != board[0][size - 1])
            {
                diagWin = 0;
                break;
            }
        }
        if (diagWin)
        {
            printf("Player %c wins!\n", board[0][size - 1]);
            return 1;
        }
    }

    return 0;
}

int checkDraw(char board[MAX_SIZE][MAX_SIZE], int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (board[i][j] == ' ')
            {
                return 0;
            }
        }
    }
    printf("The game is a draw!\n");
    return 1;
}

void updateScore(char winner, int *scoreX, int *scoreO)
{
    if (winner == 'X')
    {
        (*scoreX)++;
    }
    else if (winner == 'O')
    {
        (*scoreO)++;
    }
}
