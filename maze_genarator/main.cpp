#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <deque>
#include <cmath>

using namespace std;
using namespace sf;


const int WIDTH = 800;
const int HEIGHT = 600;
const int CELL_SIZE = 30;
const int COLS = WIDTH / CELL_SIZE;
const int ROWS = (HEIGHT - 60) / CELL_SIZE;  // Reserve space for UI at top
const int UI_HEIGHT = 60;

struct Cell {
    int x, y;
    bool walls[4] = { true, true, true, true }; 
    bool visited = false;
};


struct Solver {
    int x, y;
    float displayX, displayY;  // For smooth animation
};


struct Goal {
    int x, y;
};

// Color scheme
const Color BACKGROUND_COLOR(240, 240, 245);
const Color WALL_COLOR(45, 52, 54);
const Color PLAYER_COLOR(231, 76, 60);
const Color GOAL_COLOR(46, 204, 113);
const Color PATH_COLOR(52, 152, 219);
const Color VISITED_COLOR(200, 214, 229);
const Color UI_BG_COLOR(44, 62, 80);
const Color TEXT_COLOR(236, 240, 241);

vector<Cell> grid;
stack<Cell*> cellStack;
Cell* current;
Solver solver;
Goal goal;

// Game state
int level = 1;
int maxLevels = 5;
bool isPaused = false;
bool mazeGenerated = false;
deque<pair<int, int>> breadcrumbs;  // Track player path

// Input handling
Clock inputClock;
const float INPUT_DELAY = 0.15f;  // Delay between inputs in seconds


void setup(int level) {
    grid.clear();  
    while (!cellStack.empty()) cellStack.pop();
    breadcrumbs.clear();
    mazeGenerated = false;
    
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            Cell cell;
            cell.x = x;
            cell.y = y;
            grid.push_back(cell);
        }
    }
    current = &grid[0];
    current->visited = true;

    solver = { 0, 0, 0.0f, 0.0f };
    solver.displayX = 0.0f;
    solver.displayY = 0.0f;
    
    if (level == 1) {
        goal = { COLS - 1, ROWS - 1 };  
    }
    else if (level == 2) {
        goal = { COLS / 2, ROWS / 2 };  
    }
    else if (level == 3) {
        goal = { COLS / 2 + 3, ROWS / 2 + 3 };  
    }
    else if (level == 4) {
        goal = { COLS - 1, ROWS / 2 };
    }
    else {
        goal = { COLS / 2, ROWS - 1 };
    }
    
    breadcrumbs.push_back({0, 0});
}



Cell* getNeighbor(Cell* cell) {
    vector<Cell*> neighbors;
    int x = cell->x;
    int y = cell->y;

    if (y > 0 && !grid[(y - 1) * COLS + x].visited) neighbors.push_back(&grid[(y - 1) * COLS + x]);
    if (x < COLS - 1 && !grid[y * COLS + (x + 1)].visited) neighbors.push_back(&grid[y * COLS + (x + 1)]);
    if (y < ROWS - 1 && !grid[(y + 1) * COLS + x].visited) neighbors.push_back(&grid[(y + 1) * COLS + x]);
    if (x > 0 && !grid[y * COLS + (x - 1)].visited) neighbors.push_back(&grid[y * COLS + (x - 1)]);

    if (!neighbors.empty()) {
        int r = rand() % neighbors.size();
        return neighbors[r];
    }
    else {
        return nullptr;
    }
}



void removeWalls(Cell* a, Cell* b) {
    int dx = a->x - b->x;
    int dy = a->y - b->y;

    if (dx == 1) {
        a->walls[3] = false;
        b->walls[1] = false;
    }
    else if (dx == -1) {
        a->walls[1] = false;
        b->walls[3] = false;
    }

    if (dy == 1) {
        a->walls[0] = false;
        b->walls[2] = false;
    }
    else if (dy == -1) {
        a->walls[2] = false;
        b->walls[0] = false;
    }
}



void generateMaze() {
    if (mazeGenerated) return;
    
    // Generate maze faster - multiple steps per frame
    for (int i = 0; i < 5; i++) {
        Cell* next = getNeighbor(current);
        if (next) {
            next->visited = true;
            cellStack.push(current);
            removeWalls(current, next);
            current = next;
        }
        else if (!cellStack.empty()) {
            current = cellStack.top();
            cellStack.pop();
        }
        else {
            mazeGenerated = true;
            break;
        }
    }
}



void drawMaze(RenderWindow& window) {
    // Draw visited cells with light background
    for (const auto& cell : grid) {
        if (cell.visited && mazeGenerated) {
            RectangleShape cellBg(Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
            cellBg.setPosition(cell.x * CELL_SIZE + 1, cell.y * CELL_SIZE + UI_HEIGHT + 1);
            cellBg.setFillColor(VISITED_COLOR);
            window.draw(cellBg);
        }
    }
    
    // Draw breadcrumbs (path traveled)
    if (mazeGenerated && breadcrumbs.size() > 1) {
        for (size_t i = 0; i < breadcrumbs.size(); i++) {
            float alpha = 50 + (i * 150.0f / breadcrumbs.size());
            CircleShape crumb(3);
            crumb.setPosition(breadcrumbs[i].first * CELL_SIZE + CELL_SIZE / 2 - 3,
                            breadcrumbs[i].second * CELL_SIZE + UI_HEIGHT + CELL_SIZE / 2 - 3);
            crumb.setFillColor(Color(PATH_COLOR.r, PATH_COLOR.g, PATH_COLOR.b, (Uint8)alpha));
            window.draw(crumb);
        }
    }
    
    // Draw walls
    for (const auto& cell : grid) {
        int x = cell.x * CELL_SIZE;
        int y = cell.y * CELL_SIZE + UI_HEIGHT;

        if (cell.walls[0]) {
            RectangleShape line(Vector2f(CELL_SIZE, 2));
            line.setPosition(x, y);
            line.setFillColor(WALL_COLOR);
            window.draw(line);
        }
        if (cell.walls[1]) {
            RectangleShape line(Vector2f(2, CELL_SIZE));
            line.setPosition(x + CELL_SIZE, y);
            line.setFillColor(WALL_COLOR);
            window.draw(line);
        }
        if (cell.walls[2]) {
            RectangleShape line(Vector2f(CELL_SIZE, 2));
            line.setPosition(x, y + CELL_SIZE);
            line.setFillColor(WALL_COLOR);
            window.draw(line);
        }
        if (cell.walls[3]) {
            RectangleShape line(Vector2f(2, CELL_SIZE));
            line.setPosition(x, y);
            line.setFillColor(WALL_COLOR);
            window.draw(line);
        }
    }
}



void drawSolver(RenderWindow& window) {
    // Smooth animation
    const float smoothSpeed = 0.3f;
    solver.displayX += (solver.x - solver.displayX) * smoothSpeed;
    solver.displayY += (solver.y - solver.displayY) * smoothSpeed;
    
    // Draw player with gradient effect (outer glow)
    CircleShape glow(CELL_SIZE / 2 + 2);
    glow.setPosition(solver.displayX * CELL_SIZE - 2, solver.displayY * CELL_SIZE + UI_HEIGHT - 2);
    glow.setFillColor(Color(PLAYER_COLOR.r, PLAYER_COLOR.g, PLAYER_COLOR.b, 100));
    window.draw(glow);
    
    // Draw main player circle
    CircleShape solverShape(CELL_SIZE / 2 - 3);
    solverShape.setPosition(solver.displayX * CELL_SIZE + 3, solver.displayY * CELL_SIZE + UI_HEIGHT + 3);
    solverShape.setFillColor(PLAYER_COLOR);
    window.draw(solverShape);
    
    // Add a highlight
    CircleShape highlight(CELL_SIZE / 4);
    highlight.setPosition(solver.displayX * CELL_SIZE + 8, solver.displayY * CELL_SIZE + UI_HEIGHT + 8);
    highlight.setFillColor(Color(255, 255, 255, 150));
    window.draw(highlight);
}



void drawGoal(RenderWindow& window) {
    // Animated pulsing effect for goal
    static Clock pulseClock;
    float pulse = 0.5f + 0.5f * sin(pulseClock.getElapsedTime().asSeconds() * 3);
    
    // Draw goal with glow effect
    CircleShape glow(CELL_SIZE / 2 + 2 + pulse * 4);
    glow.setPosition(goal.x * CELL_SIZE - 2 - pulse * 4, goal.y * CELL_SIZE + UI_HEIGHT - 2 - pulse * 4);
    glow.setFillColor(Color(GOAL_COLOR.r, GOAL_COLOR.g, GOAL_COLOR.b, 80));
    window.draw(glow);
    
    // Draw main goal star shape (using circle for simplicity)
    CircleShape goalShape(CELL_SIZE / 2 - 3);
    goalShape.setPosition(goal.x * CELL_SIZE + 3, goal.y * CELL_SIZE + UI_HEIGHT + 3);
    goalShape.setFillColor(GOAL_COLOR);
    window.draw(goalShape);
    
    // Add a highlight
    CircleShape highlight(CELL_SIZE / 4);
    highlight.setPosition(goal.x * CELL_SIZE + 8, goal.y * CELL_SIZE + UI_HEIGHT + 8);
    highlight.setFillColor(Color(255, 255, 255, 200));
    window.draw(highlight);
}



void drawUI(RenderWindow& window, Font& font, Time elapsedTime) {
    // UI background
    RectangleShape uiBg(Vector2f(WIDTH, UI_HEIGHT));
    uiBg.setPosition(0, 0);
    uiBg.setFillColor(UI_BG_COLOR);
    window.draw(uiBg);
    
    // Level text
    Text levelText;
    levelText.setFont(font);
    levelText.setString("Level: " + to_string(level) + "/" + to_string(maxLevels));
    levelText.setCharacterSize(20);
    levelText.setFillColor(TEXT_COLOR);
    levelText.setPosition(20, 20);
    window.draw(levelText);
    
    // Timer text
    Text timerText;
    timerText.setFont(font);
    stringstream ss;
    ss.precision(1);
    ss << fixed << "Time: " << elapsedTime.asSeconds() << "s";
    timerText.setString(ss.str());
    timerText.setCharacterSize(20);
    timerText.setFillColor(TEXT_COLOR);
    timerText.setPosition(WIDTH / 2 - 60, 20);
    window.draw(timerText);
    
    // Instructions
    Text instructText;
    instructText.setFont(font);
    instructText.setString("Arrow Keys: Move | R: Restart | P: Pause");
    instructText.setCharacterSize(14);
    instructText.setFillColor(Color(TEXT_COLOR.r, TEXT_COLOR.g, TEXT_COLOR.b, 180));
    instructText.setPosition(20, 45);
    window.draw(instructText);
    
    // Status text
    Text statusText;
    statusText.setFont(font);
    if (!mazeGenerated) {
        statusText.setString("Generating...");
        statusText.setCharacterSize(16);
        statusText.setFillColor(Color(52, 152, 219));
    } else if (isPaused) {
        statusText.setString("PAUSED");
        statusText.setCharacterSize(24);
        statusText.setFillColor(Color(241, 196, 15));
    }
    statusText.setPosition(WIDTH - 150, 15);
    window.draw(statusText);
}

void handleInput() {
    if (!mazeGenerated || isPaused) return;
    
    // Input debouncing
    if (inputClock.getElapsedTime().asSeconds() < INPUT_DELAY) return;
    
    bool moved = false;
    
    if (Keyboard::isKeyPressed(Keyboard::Up)) {
        if (solver.y > 0 && !grid[solver.y * COLS + solver.x].walls[0]) {
            solver.y--;
            moved = true;
        }
    }
    else if (Keyboard::isKeyPressed(Keyboard::Down)) {
        if (solver.y < ROWS - 1 && !grid[solver.y * COLS + solver.x].walls[2]) {
            solver.y++;
            moved = true;
        }
    }
    else if (Keyboard::isKeyPressed(Keyboard::Left)) {
        if (solver.x > 0 && !grid[solver.y * COLS + solver.x].walls[3]) {
            solver.x--;
            moved = true;
        }
    }
    else if (Keyboard::isKeyPressed(Keyboard::Right)) {
        if (solver.x < COLS - 1 && !grid[solver.y * COLS + solver.x].walls[1]) {
            solver.x++;
            moved = true;
        }
    }
    
    if (moved) {
        inputClock.restart();
        breadcrumbs.push_back({solver.x, solver.y});
        // Keep breadcrumbs limited
        if (breadcrumbs.size() > 100) {
            breadcrumbs.pop_front();
        }
    }
}



bool checkVictory() {
    return solver.x == goal.x && solver.y == goal.y;
}

void showVictoryScreen(RenderWindow& window, Font& font, Time elapsedTime) {
    // Semi-transparent overlay
    RectangleShape overlay(Vector2f(WIDTH, HEIGHT));
    overlay.setFillColor(Color(0, 0, 0, 180));
    window.draw(overlay);
    
    // Victory message
    Text victoryText;
    victoryText.setFont(font);
    if (level < maxLevels) {
        victoryText.setString("Level Complete!");
    } else {
        victoryText.setString("Congratulations!\nAll Levels Complete!");
    }
    victoryText.setCharacterSize(48);
    victoryText.setFillColor(GOAL_COLOR);
    victoryText.setStyle(Text::Bold);
    FloatRect textBounds = victoryText.getLocalBounds();
    victoryText.setPosition((WIDTH - textBounds.width) / 2, HEIGHT / 2 - 100);
    window.draw(victoryText);
    
    // Time text
    Text timeText;
    timeText.setFont(font);
    stringstream ss;
    ss.precision(2);
    ss << fixed << "Time: " << elapsedTime.asSeconds() << " seconds";
    timeText.setString(ss.str());
    timeText.setCharacterSize(24);
    timeText.setFillColor(TEXT_COLOR);
    textBounds = timeText.getLocalBounds();
    timeText.setPosition((WIDTH - textBounds.width) / 2, HEIGHT / 2);
    window.draw(timeText);
    
    // Continue instructions
    Text continueText;
    continueText.setFont(font);
    if (level < maxLevels) {
        continueText.setString("Press SPACE to continue to next level\nPress ESC to quit");
    } else {
        continueText.setString("Press ESC to quit");
    }
    continueText.setCharacterSize(20);
    continueText.setFillColor(Color(TEXT_COLOR.r, TEXT_COLOR.g, TEXT_COLOR.b, 200));
    textBounds = continueText.getLocalBounds();
    continueText.setPosition((WIDTH - textBounds.width) / 2, HEIGHT / 2 + 60);
    window.draw(continueText);
}

void nextLevel() {
    if (level < maxLevels) {
        level++;
        cout << "\nProceeding to level " << level << "...\n";
        setup(level);  
    }
}



int main() {
    srand(static_cast<unsigned>(time(0)));
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Maze Generator And Solver");
    window.setFramerateLimit(60);

    // Load font (using default system font)
    Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        cerr << "Error loading font! Using default rendering." << endl;
    }
    
    Clock clock;
    Time elapsedTime;
    bool victoryState = false;
    Clock victoryClock;

    setup(level);  

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            
            if (event.type == Event::KeyPressed) {
                // Restart current level
                if (event.key.code == Keyboard::R) {
                    clock.restart();
                    setup(level);
                    victoryState = false;
                }
                // Pause/unpause
                else if (event.key.code == Keyboard::P && mazeGenerated && !victoryState) {
                    isPaused = !isPaused;
                }
                // Handle victory screen inputs
                else if (victoryState) {
                    if (event.key.code == Keyboard::Space && level < maxLevels) {
                        clock.restart();
                        nextLevel();
                        victoryState = false;
                    }
                    else if (event.key.code == Keyboard::Escape) {
                        window.close();
                    }
                }
            }
        }

        // Check for victory
        if (checkVictory() && !victoryState && mazeGenerated) {
            cout << "\nVictory! You solved level " << level << " in " << elapsedTime.asSeconds() << " seconds!" << endl;
            victoryState = true;
            victoryClock.restart();
        }

        // Update elapsed time only when not paused
        if (!isPaused) {
            elapsedTime = clock.getElapsedTime();
        }

        // Handle input
        if (!victoryState) {
            handleInput();
        }
        
        // Generate maze if not complete
        if (!mazeGenerated) {
            generateMaze();
        }

        // Render
        window.clear(BACKGROUND_COLOR);
        drawMaze(window);
        drawGoal(window);
        drawSolver(window);
        drawUI(window, font, elapsedTime);
        
        // Draw victory screen overlay
        if (victoryState) {
            showVictoryScreen(window, font, elapsedTime);
        }
        
        window.display();
    }

    return 0;
}
