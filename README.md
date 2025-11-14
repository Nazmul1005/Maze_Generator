# Maze Generator And Solver

A beautiful and interactive maze game built with C++ and SFML. Navigate through procedurally generated mazes across multiple levels!

![Maze Game Screenshot](https://github.com/user-attachments/assets/225aeed8-df63-4603-a718-f6a0a5fc5758)

## Features

### Visual Features
- **Modern Color Scheme**: Beautiful colors with light background and contrasting elements
- **Smooth Animations**: Player movement with smooth interpolation for fluid gameplay
- **Animated Goal**: Pulsing glow effect on the goal to make it stand out
- **Breadcrumb Trail**: Visual trail showing the path you've traveled (last 100 moves)
- **Professional UI**: Top bar displaying level, timer, and controls
- **Visual Feedback**: Highlighted visited cells and generating status indicator

### Gameplay Features
- **5 Progressive Levels**: Increasing difficulty with different goal positions
- **Precise Controls**: Input debouncing prevents accidental double-moves
- **Pause System**: Press 'P' to pause and resume gameplay
- **Restart Option**: Press 'R' to restart the current level
- **Victory Screens**: Elegant completion screens with time display
- **Real-time Timer**: Track your completion time for each level
- **Smooth Gameplay**: 60 FPS cap for consistent performance

## Controls

| Key | Action |
|-----|--------|
| ↑ ↓ ← → | Move player (Arrow Keys) |
| R | Restart current level |
| P | Pause/Unpause game |
| Space | Continue to next level (on victory screen) |
| ESC | Quit game (on victory screen) |

## Building the Game

### Prerequisites
- C++ compiler with C++11 support (g++, clang++)
- SFML 2.x library

### Installation

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install libsfml-dev g++
```

#### macOS (with Homebrew)
```bash
brew install sfml
```

#### Windows
Download SFML from [sfml-dev.org](https://www.sfml-dev.org/download.php)

### Compilation

Navigate to the `maze_genarator` directory and compile:

```bash
cd maze_genarator
g++ main.cpp -o maze_game -lsfml-graphics -lsfml-window -lsfml-system -std=c++11
```

### Running the Game

```bash
./maze_game
```

## How to Play

1. **Start**: The game begins at Level 1 with the player (red circle) in the top-left corner
2. **Goal**: Navigate to the green circle (goal) using arrow keys
3. **Maze Generation**: Watch as the maze generates in real-time at the start
4. **Movement**: Use arrow keys to move through the maze - you cannot pass through walls
5. **Trail**: Your path is marked with blue breadcrumbs
6. **Victory**: Reach the goal to complete the level and proceed to the next
7. **Controls**: Use R to restart if stuck, P to pause if needed

## Game Mechanics

- **Maze Generation**: Uses depth-first search algorithm with backtracking
- **5 Levels**: Each level has a different goal position for variety
- **Input Handling**: 0.15 second delay between inputs for precise control
- **Animation**: Smooth player movement using linear interpolation
- **Performance**: Optimized rendering at 60 FPS

## Technical Details

### Technologies Used
- **Language**: C++11
- **Graphics Library**: SFML (Simple and Fast Multimedia Library)
- **Algorithm**: Recursive backtracking for maze generation

### Code Structure
- Maze generation using depth-first search
- Cell-based grid system
- Event-driven input handling
- State management for game flow
- Smooth animation system using interpolation

## Improvements Made

This version includes significant improvements over the original:

**Visual Improvements:**
- Modern color palette with professional appearance
- Circular player and goal shapes with glow effects
- Animated elements (pulsing goal, smooth player movement)
- Comprehensive UI with level indicator, timer, and instructions
- Breadcrumb trail visualization
- Larger window size (800x600) for better visibility

**Functional Improvements:**
- Input debouncing for better control
- Pause and restart functionality
- Victory screens with proper game flow
- Extended to 5 levels
- Better event handling
- Frame rate limiting for consistent performance
- Improved maze generation speed

## Future Enhancement Ideas

- Add difficulty settings (Easy/Medium/Hard)
- Implement a hint system
- Add sound effects and background music
- Create a leaderboard system
- Add more maze generation algorithms
- Implement different maze themes
- Add power-ups and obstacles

## License

This project is open source. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Feel free to:
- Report bugs
- Suggest new features
- Submit pull requests
- Improve documentation

---

Enjoy solving mazes! 🎮✨
