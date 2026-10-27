# Chess Engine

A C++ based Chess Engine with a graphical user interface (GUI) built using the Windows API and GDI+. The project employs an Object-Oriented Programming (OOP) approach to manage the game state, pieces, and valid move generation.

## Prerequisites

- Windows Operating System
- A C++ compiler (like `g++` via MinGW)
- GDI+ library (usually included with the Windows SDK)

## How to Compile

To compile the project, you need to link against the GDI+ and GDI32 libraries. You can compile it from the root directory using `g++` with the following command:

```bash
cd .\src\    
g++ *.cpp -o chessboard.exe -lgdi32 -lgdiplus -mwindows -lwinhttp                 
.\chessboard.exe  
```

*Note: The `-mwindows` flag is used to compile it as a GUI application, preventing the console window from appearing in the background.*

## How to Run

Once compiled, you can run the executable directly from your terminal or by double-clicking it:

```bash
./chessboard.exe
```

## Project Structure

- `src/` - Contains the source code (`.cpp`) and header files (`.h`) for the chess engine logic and the Windows GUI.
- `assests/` - Contains the PNG image assets for the chess pieces used by the GUI.
