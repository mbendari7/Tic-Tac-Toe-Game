#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Max input size is 15 to prevent the console from looking messy
#define MAX_INPUT_SIZE 15

// --- FIXED: STRUCT DEFINITION MOVED TO TOP ---
// This must be defined BEFORE any functions try to use it.
// This struct holds all the tracking data for Part 2.
typedef struct
{
    int games_played;
    int wins_X;
    int wins_O;
    int draws;
    int *win_patterns;    // Dynamic array: Stores 1 for Row, 2 for Col, 3 for Diag
    int pattern_count;    // How many games stored so far
    int pattern_capacity; // The current size limit of the array
} GameStats;

// Global settings for the AI
int aiEnabled = 0;
char aiPlayer = 'O';

// --- Function Prototypes ---
// Now that GameStats is defined above, these prototypes will work correctly.

// Board Management
char **createBoard(int size);
void freeBoard(char **board, int size);
void initializeBoard(char **board, int size);
void printBoard(char **board, int size);

// Gameplay
int playerMove(char **board, int size, char player);
void aiMove(char **board, int size, char computerChar);

// Win/Draw Checking
int checkWin(char **board, int size, char player);
int checkWinSilent(char **board, int size, char player);
int checkDraw(char **board, int size);

// Statistics and File I/O
GameStats createGameStats();
void updateStats(GameStats *stats, char winner, int win_type);
void freeGameStats(GameStats *stats);
void printStatistics(const GameStats stats);
void saveGame(char **board, int size, char currentPlayer, int mode, const char *filename);
int loadGame(char ***board, int *size, char *currentPlayer, int *mode, const char *filename);
void saveStatistics(const GameStats stats, const char *filename);
void loadStatistics(GameStats *stats, const char *filename);

// --- Main Function ---
int main()
{
    // Seeding the random number generator so the AI plays differently each time
    srand(time(NULL));

    // Initialize our stats tracking and try to load old stats if they exist
    GameStats stats = createGameStats();
    loadStatistics(&stats, "stats.txt");

    int size;
    int playAgain = 1;
    int gameMode = 0;
    int loadedGame = 0; // Flag to track if we are continuing a saved game

    while (playAgain)
    {
        char **board = NULL;
        char currentPlayer = 'X';
        int gameOver = 0;
        int winType = 0;

        // Main Menu Display
        printf("\n--- Tic Tac Toe Part 2 ---\n");
        printf("1. New Game (Player vs Player)\n");
        printf("2. New Game (Player vs AI)\n");
        printf("3. Load Saved Game\n");
        printf("4. View Statistics\n");
        printf("Enter choice: ");
        int choice;

        // Simple validation to make sure they entered a number
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ; // Clear the buffer if they type letters
            choice = 1;
        }

        if (choice == 4)
        {
            // Just print stats and restart the loop to show menu again
            printStatistics(stats);
            continue;
        }
        else if (choice == 3)
        {
            // Part 2: Load functionality
            // We pass the address of 'board' (&board) because the function needs to create it for us
            if (loadGame(&board, &size, &currentPlayer, &gameMode, "game_save.txt"))
            {
                loadedGame = 1;
                aiEnabled = (gameMode == 2) ? 1 : 0; // Restore the AI setting
                printf("Game loaded successfully!\n");
            }
            else
            {
                printf("No saved game found or error loading.\n");
                continue; // Go back to menu if load fails
            }
        }
        else
        {
            // Starting a fresh game
            gameMode = (choice == 2) ? 2 : 1;
            aiEnabled = (gameMode == 2) ? 1 : 0;

            printf("Enter board size (3 to %d): ", MAX_INPUT_SIZE);
            if (scanf("%d", &size) != 1 || size < 3 || size > MAX_INPUT_SIZE)
            {
                printf("Invalid size. Defaulting to 3.\n");
                size = 3;
            }

            // Part 2: Creating the board dynamically based on the chosen size
            board = createBoard(size);
            initializeBoard(board, size);
            loadedGame = 0;
        }

        // --- Main Gameplay Loop ---
        while (!gameOver)
        {
            printBoard(board, size);

            int result = 0;

            // Turn Logic: Check if it's AI's turn
            if (aiEnabled && currentPlayer == aiPlayer)
            {
                printf("AI is thinking...\n");
                aiMove(board, size, aiPlayer);
            }
            else
            {
                // Human turn. Result will be -1 if they chose to Save & Quit
                result = playerMove(board, size, currentPlayer);

                if (result == -1)
                {
                    // Part 2: Save functionality triggered
                    saveGame(board, size, currentPlayer, gameMode, "game_save.txt");
                    printf("Game saved. Exiting to menu.\n");
                    freeBoard(board, size); // Must free memory before leaving!
                    gameOver = 1;           // Break the inner loop
                    playAgain = 1;          // Keep the main program running
                    break;
                }
            }

            // Check for win (winType 1=Row, 2=Col, 3=Diag)
            winType = checkWin(board, size, currentPlayer);
            if (winType > 0)
            {
                printBoard(board, size);
                printf("Player %c Wins!\n", currentPlayer);
                // Update the stats with WHO won and HOW they won
                updateStats(&stats, currentPlayer, winType);
                gameOver = 1;
            }
            else if (checkDraw(board, size))
            {
                printBoard(board, size);
                updateStats(&stats, 'D', 0); // 'D' indicates a Draw
                gameOver = 1;
            }
            else
            {
                // Switch turns if the game isn't over
                if (result != -1)
                    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
            }
        }

        // End of Game Logic
        if (gameOver && winType >= 0)
        {
            // Save the updated stats to the file immediately so we don't lose data
            saveStatistics(stats, "stats.txt");

            // Clean up the board memory if it exists
            if (board != NULL)
                freeBoard(board, size);
        }

        printf("\nBack to Main Menu? (1=Yes, 0=Quit): ");
        if (scanf("%d", &playAgain) != 1)
            playAgain = 0;
    }

    // Program Exiting: Free the dynamic array inside the stats struct
    freeGameStats(&stats);
    return 0;
}

// --- Part 2: Stats Functions ---

// Description: Initializes the statistics structure.
// It uses malloc to create the initial dynamic array for tracking win patterns.
GameStats createGameStats()
{
    GameStats s;
    s.games_played = 0;
    s.wins_X = 0;
    s.wins_O = 0;
    s.draws = 0;
    s.pattern_count = 0;
    s.pattern_capacity = 10; // Start small, we can grow this later

    // Allocate memory for 10 integers to start
    s.win_patterns = (int *)malloc(s.pattern_capacity * sizeof(int));
    return s;
}

// Description: Updates the stats after a game ends.
// If the dynamic array is full, it uses 'realloc' to double the size.
void updateStats(GameStats *stats, char winner, int win_type)
{
    stats->games_played++;

    if (winner == 'X')
        stats->wins_X++;
    else if (winner == 'O')
        stats->wins_O++;
    else
        stats->draws++;

    // Only track win patterns if it wasn't a draw
    if (winner != 'D')
    {
        // Check if our array is full
        if (stats->pattern_count >= stats->pattern_capacity)
        {
            stats->pattern_capacity *= 2; // Double the capacity

            // realloc keeps the old data but gives us more space
            stats->win_patterns = (int *)realloc(stats->win_patterns, stats->pattern_capacity * sizeof(int));
        }
        // Store the pattern and increment the count
        stats->win_patterns[stats->pattern_count++] = win_type;
    }
}

// Description: Prints the current statistics to the console.
// It loops through the win_patterns array to count how many wins were Rows, Cols, or Diagonals.
void printStatistics(const GameStats stats)
{
    printf("\n--- Game Statistics ---\n");
    printf("Total Games: %d\n", stats.games_played);
    printf("X Wins: %d\n", stats.wins_X);
    printf("O Wins: %d\n", stats.wins_O);
    printf("Draws: %d\n", stats.draws);

    int rows = 0, cols = 0, diags = 0;
    // Analyze the dynamic array
    for (int i = 0; i < stats.pattern_count; i++)
    {
        if (stats.win_patterns[i] == 1)
            rows++;
        if (stats.win_patterns[i] == 2)
            cols++;
        if (stats.win_patterns[i] == 3)
            diags++;
    }
    printf("Win Patterns -> Rows: %d, Cols: %d, Diagonals: %d\n", rows, cols, diags);
    printf("-----------------------\n");
}

// Description: Frees the dynamic memory used by the stats structure.
// This prevents memory leaks when the program closes.
void freeGameStats(GameStats *stats)
{
    if (stats->win_patterns != NULL)
    {
        free(stats->win_patterns);
    }
}

// --- Part 2: File I/O Functions ---

// Description: Saves the current game state to a text file.
// It writes the board size, game mode, current player, and the entire board grid.
void saveGame(char **board, int size, char currentPlayer, int mode, const char *filename)
{
    FILE *file = fopen(filename, "w"); // Open for writing ("w")
    if (file == NULL)
    {
        printf("Error saving game.\n");
        return;
    }

    // Write the header information first
    fprintf(file, "%d %d %c\n", size, mode, currentPlayer);

    // Loop through the board and write every character
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            // We replace spaces with '.' to make reading the file easier later
            char c = (board[i][j] == ' ') ? '.' : board[i][j];
            fprintf(file, "%c", c);
        }
        fprintf(file, "\n"); // Newline at end of each row
    }
    fclose(file); // Always close the file!
}

// Description: Loads a game from a text file.
// It reads the size, recreates the board using malloc, and fills it with the saved data.
// Returns 1 if successful, 0 if failed.
int loadGame(char ***board, int *size, char *currentPlayer, int *mode, const char *filename)
{
    FILE *file = fopen(filename, "r"); // Open for reading ("r")
    if (file == NULL)
        return 0; // File doesn't exist yet

    // Read the header info
    if (fscanf(file, "%d %d %c", size, mode, currentPlayer) != 3)
    {
        fclose(file);
        return 0;
    }

    // Create a new dynamic board using the size we just read
    *board = createBoard(*size);

    // Read the grid characters
    char temp;
    for (int i = 0; i < *size; i++)
    {
        for (int j = 0; j < *size; j++)
        {
            // Skip newline characters to find the actual data
            do
            {
                fscanf(file, "%c", &temp);
            } while (temp == '\n');

            // Convert the '.' back into a space ' '
            (*board)[i][j] = (temp == '.') ? ' ' : temp;
        }
    }
    fclose(file);
    return 1;
}

// Description: Saves the statistics to a file.
// It saves the counts and the entire dynamic array of win patterns.
void saveStatistics(const GameStats stats, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
        return;

    // Write the basic counts
    fprintf(file, "%d %d %d %d %d\n", stats.games_played, stats.wins_X, stats.wins_O, stats.draws, stats.pattern_count);

    // Write the array contents so we remember how previous games were won
    for (int i = 0; i < stats.pattern_count; i++)
    {
        fprintf(file, "%d ", stats.win_patterns[i]);
    }
    fclose(file);
}

// Description: Loads statistics from a file.
// It handles reallocating the memory if the saved file has more patterns than our default capacity.
void loadStatistics(GameStats *stats, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return; // It's okay if no file exists yet

    fscanf(file, "%d %d %d %d %d", &stats->games_played, &stats->wins_X, &stats->wins_O, &stats->draws, &stats->pattern_count);

    // If the file has more patterns than our default 10, we need to grow the array immediately
    if (stats->pattern_count > stats->pattern_capacity)
    {
        stats->pattern_capacity = stats->pattern_count + 10;
        stats->win_patterns = (int *)realloc(stats->win_patterns, stats->pattern_capacity * sizeof(int));
    }

    // Read the patterns into the array
    for (int i = 0; i < stats->pattern_count; i++)
    {
        fscanf(file, "%d", &stats->win_patterns[i]);
    }
    fclose(file);
}

// --- Core Game Functions ---

// Description: Allocates memory for the board dynamically.
// Uses a "pointer to pointer" (char**) to create a list of rows.
char **createBoard(int size)
{
    // 1. Allocate the "spine" (array of pointers, one for each row)
    char **board = (char **)malloc(size * sizeof(char *));

    // 2. Allocate the actual character array for each row
    for (int i = 0; i < size; i++)
    {
        board[i] = (char *)malloc(size * sizeof(char));
    }
    return board;
}

// Description: Frees the memory used by the board.
// MUST be done in reverse order of creation to avoid memory leaks.
void freeBoard(char **board, int size)
{
    // 1. Free the individual rows first
    for (int i = 0; i < size; i++)
    {
        free(board[i]);
    }
    // 2. Free the spine last
    free(board);
}

// Description: Fills the board with empty spaces ' '.
void initializeBoard(char **board, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            board[i][j] = ' ';
        }
    }
}

// Description: Displays the board grid to the console.
void printBoard(char **board, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf(" %c ", board[i][j]);
            if (j < size - 1)
                printf("|"); // Vertical divider
        }
        printf("\n");
        // Print horizontal divider line
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

// Description: Handles human player input.
// Returns 1 if move was successful, -1 if user entered the code to Save/Quit.
int playerMove(char **board, int size, char player)
{
    int row, col;
    while (1)
    {
        printf("Player %c (row col) OR (-1 -1 to Save/Quit): ", player);
        if (scanf("%d %d", &row, &col) != 2)
        {
            printf("Invalid input.\n");
            while (getchar() != '\n')
                ; // Clear garbage input
            continue;
        }

        // Check for the special "Save Game" code
        if (row == -1 && col == -1)
            return -1;

        // Validate the move is within bounds
        if (row >= 0 && row < size && col >= 0 && col < size)
        {
            if (board[row][col] == ' ')
            {
                board[row][col] = player;
                return 1; // Move accepted
            }
            else
                printf("Occupied.\n");
        }
        else
            printf("Out of bounds.\n");
    }
}

// Description: A helper function for the AI.
// It checks if someone won, but does NOT print anything to the screen.
// The AI uses this to "imagine" outcomes.
int checkWinSilent(char **board, int size, char player)
{
    // Check Rows and Cols
    for (int i = 0; i < size; i++)
    {
        int r = 1, c = 1;
        for (int j = 0; j < size; j++)
        {
            if (board[i][j] != player)
                r = 0;
            if (board[j][i] != player)
                c = 0;
        }
        if (r || c)
            return 1;
    }
    // Check Diagonals
    int d1 = 1, d2 = 1;
    for (int i = 0; i < size; i++)
    {
        if (board[i][i] != player)
            d1 = 0;
        if (board[i][size - 1 - i] != player)
            d2 = 0;
    }
    return (d1 || d2);
}

// Description: The main AI logic.
// Follows Priority: 1. Win Immediately -> 2. Block Opponent -> 3. Random Move
void aiMove(char **board, int size, char computerChar)
{
    char opponentChar = (computerChar == 'X') ? 'O' : 'X';

    // 1. WIN CHECK
    // Loop through every empty spot and see if placing our piece there wins.
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (board[i][j] == ' ')
            {
                board[i][j] = computerChar;
                if (checkWinSilent(board, size, computerChar))
                    return;        // Found a win!
                board[i][j] = ' '; // Reset if not
            }
        }
    }

    // 2. BLOCK CHECK
    // Loop through every empty spot and see if the opponent would win there.
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (board[i][j] == ' ')
            {
                board[i][j] = opponentChar; // Pretend opponent moves here
                if (checkWinSilent(board, size, opponentChar))
                {
                    board[i][j] = computerChar; // Block them!
                    return;
                }
                board[i][j] = ' '; // Reset
            }
        }
    }

    // 3. RANDOM MOVE
    // If no strategic moves are found, pick a random empty spot.
    int r, c;
    do
    {
        r = rand() % size;
        c = rand() % size;
    } while (board[r][c] != ' ');
    board[r][c] = computerChar;
}

// Description: Checks if a player has won.
// Returns 0 for no win, 1 for Row Win, 2 for Col Win, 3 for Diagonal Win.
// This helps us track the "Win Pattern" for statistics.
int checkWin(char **board, int size, char player)
{
    for (int i = 0; i < size; i++)
    {
        // Check rows
        int rowWin = 1;
        for (int j = 0; j < size; j++)
        {
            if (board[i][j] != player)
                rowWin = 0;
        }
        if (rowWin)
            return 1; // Return 1 for ROW win

        // Check cols
        int colWin = 1;
        for (int j = 0; j < size; j++)
        {
            if (board[j][i] != player)
                colWin = 0;
        }
        if (colWin)
            return 2; // Return 2 for COL win
    }

    // Check diagonals
    int d1 = 1;
    for (int i = 0; i < size; i++)
    {
        if (board[i][i] != player)
            d1 = 0;
    }
    if (d1)
        return 3; // Return 3 for DIAG win

    int d2 = 1;
    for (int i = 0; i < size; i++)
    {
        if (board[i][size - 1 - i] != player)
            d2 = 0;
    }
    if (d2)
        return 3; // Return 3 for DIAG win

    return 0; // No win
}

// Description: Checks if the board is full (Draw).
// Returns 1 if draw, 0 if spaces remain.
int checkDraw(char **board, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (board[i][j] == ' ')
                return 0; // Found an empty spot, keep playing
        }
    }
    printf("The game is a draw!\n");
    return 1;
}