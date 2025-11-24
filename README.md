# Advanced Tic-Tac-Toe (Part 2)

## 📌 Introduction
This project is an enhanced C implementation of the classic Tic-Tac-Toe game. Building upon a basic static version, this **Part 2** update introduces advanced C programming concepts including **Dynamic Memory Allocation (DMA)**, **File I/O**, and **Persistent Statistics**.

The game supports variable board sizes (from 3x3 up to 15x15), a smart AI opponent, and the ability to save and reload game states.

## 🚀 Features
* **Dynamic Board Size:** Users can select a grid size between 3x3 and 15x15. The board is created using dynamic memory (`malloc`), replacing fixed-size arrays.
* **Smart AI:** A single-player mode against a computer that prioritizes winning, then blocking, then random moves.
* **Save & Load System:**
    * **Game State:** You can save an unfinished game and reload it later from the main menu.
    * **Statistics:** The game tracks wins, losses, draws, and specific "Win Patterns" (Row vs Column vs Diagonal) persistently across sessions.
* **Dynamic Statistics Tracking:** Uses a dynamic array to store an unlimited history of win patterns, automatically resizing itself (`realloc`) as more games are played.

## 🛠️ Compilation and Usage

To compile the program, use a standard C compiler like `gcc`:

```bash
gcc -o tictactoe main.c

```
## In-Game Controls
Move Input: Enter moves as row col (e.g., 1 1 for the center of a 3x3 board). Note that indices are 0-based.

Save & Quit: Enter -1 -1 during your turn to save the current game state to game_save.txt and return to the main menu.

## ⚙️ Technical Implementation Details
# 1. Dynamic Memory Allocation (char**)
Unlike standard Tic-Tac-Toe games that use a fixed char board[10][10], this project uses a double pointer (char**) to handle variable sizes efficiently.

Creation (createBoard): Memory is allocated for an array of row pointers ("the spine"). Then, the program loops through that array and allocates memory for the character cells for each row.

Cleanup (freeBoard): To prevent memory leaks, memory is freed in reverse order: individual rows are freed first, followed by the array of pointers.

# 2. Dynamic Game Statistics
The GameStats structure tracks the history of play. It uses a dynamic array (int* win_patterns) to store exactly how each game was won (Row, Column, or Diagonal).

Auto-Resizing: The array starts with a capacity of 10. If the user plays more than 10 games, the updateStats function detects this and uses realloc to double the array's capacity automatically. This ensures the game never runs out of space for stats.

# 3. File I/O System
The game uses text files to maintain persistence:

game_save.txt: Stores the board size, game mode, current player turn, and the visual state of the board (spaces are converted to . for easier parsing).

stats.txt: Stores the cumulative counters (wins/losses) and the entire array of win patterns. This is loaded at startup so stats are never lost.

# 4. AI Logic
The AI (aiMove) follows a strict priority system to mimic intelligent play:

Win Check: It simulates placing its piece in every empty spot. If checkWinSilent confirms a win, it takes that move immediately.

Block Check: It simulates the opponent placing a piece in every empty spot. If the opponent would win, the AI steals that spot to block.

Random Move: If no immediate win or loss is detected, it selects a random valid cell.

## 📂 Code Structure & Functions
# Board Management
createBoard(int size): Allocates the heap memory for the board.

freeBoard(char** board, int size): Releases the memory to the system.

initializeBoard: Fills the dynamic grid with empty spaces.

# Gameplay
playerMove: Handles input validation and checks for the -1 -1 save command.

aiMove: Executes the 3-step AI strategy (Win -> Block -> Random).

checkWin: Analyzes rows, columns, and diagonals to determine a winner. Returns the type of win (1=Row, 2=Col, 3=Diag) for statistical tracking.

# Statistics & Data
updateStats: Increments counters and handles the realloc logic for the dynamic pattern array.

saveGame / loadGame: Handles serialization of the board state to/from text files.
