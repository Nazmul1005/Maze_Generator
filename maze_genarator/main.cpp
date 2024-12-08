#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

using namespace std;
using namespace sf;


const int WIDTH = 600;
const int HEIGHT = 400;
const int CELL_SIZE = 25;
const int COLS = WIDTH / CELL_SIZE;
const int ROWS = HEIGHT / CELL_SIZE;

struct Cell {
    int x, y;
    bool walls[4] = { true, true, true, true }; 
    bool visited = false;
};


struct Solver {
    int x, y;
};


struct Goal {
    int x, y;
};

vector<Cell> grid;
stack<Cell*> cellStack;
Cell* current;
Solver solver;
Goal goal;


int level = 1;
int maxLevels = 3;


void setup(int level) {
    grid.clear();  
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

    solver = { 0, 0 };
    if (level == 1) {
        goal = { COLS - 1, ROWS - 1 };  
    }
    else if (level == 2) {
        goal = { COLS / 2, ROWS / 2 };  
    }
    else if (level == 3) {
        goal = { COLS / 2 + 3, ROWS / 2 + 3 };  
    }
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
}



void drawMaze(RenderWindow& window) {
    for (const auto& cell : grid) {
        int x = cell.x * CELL_SIZE;
        int y = cell.y * CELL_SIZE;

        if (cell.walls[0]) {
            Vertex line[] = {
                Vertex(Vector2f(x, y)),
                Vertex(Vector2f(x + CELL_SIZE, y))
            };
            window.draw(line, 2, Lines);
        }
        if (cell.walls[1]) {
            Vertex line[] = {
                Vertex(Vector2f(x + CELL_SIZE, y)),
                Vertex(Vector2f(x + CELL_SIZE, y + CELL_SIZE))
            };
            window.draw(line, 2, Lines);
        }
        if (cell.walls[2]) {
            Vertex line[] = {
                Vertex(Vector2f(x + CELL_SIZE, y + CELL_SIZE)),
                Vertex(Vector2f(x, y + CELL_SIZE))
            };
            window.draw(line, 2, Lines);
        }
        if (cell.walls[3]) {
            Vertex line[] = {
                Vertex(Vector2f(x, y + CELL_SIZE)),
                Vertex(Vector2f(x, y))
            };
            window.draw(line, 2, Lines);
        }
    }
}



void drawSolver(RenderWindow& window) {
    RectangleShape solverShape(Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    solverShape.setPosition(solver.x * CELL_SIZE + 1, solver.y * CELL_SIZE + 1);
    solverShape.setFillColor(Color::Red);
    window.draw(solverShape);
}



void drawGoal(RenderWindow& window) {
    RectangleShape goalShape(Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    goalShape.setPosition(goal.x * CELL_SIZE + 1, goal.y * CELL_SIZE + 1);
    goalShape.setFillColor(Color::Green);
    window.draw(goalShape);
}



void handleInput() {
    if (Keyboard::isKeyPressed(Keyboard::Up)) {
        if (!grid[solver.y * COLS + solver.x].walls[0]) {
            solver.y--;
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Down)) {
        if (!grid[solver.y * COLS + solver.x].walls[2]) {
            solver.y++;
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        if (!grid[solver.y * COLS + solver.x].walls[3]) {
            solver.x--;
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        if (!grid[solver.y * COLS + solver.x].walls[1]) {
            solver.x++;
        }
    }
}



bool checkVictory() {
    return solver.x == goal.x && solver.y == goal.y;
}

void nextLevel() {
    if (level < maxLevels) {
        level++;
        cout << "\nProceeding to level " << level << "...\n";
        setup(level);  
    }
    else {
        cout << "\nCongratulations! You completed all levels!\n";
        exit(0);  
    }
}



int main() {
    srand(static_cast<unsigned>(time(0)));
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Maze Genarator And Solver");

    
    Clock clock;
    Time elapsedTime;


    setup(level);  


    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        
        elapsedTime = clock.getElapsedTime();
        stringstream ss;
        ss << "Maze Genarator And Solver Time: " << elapsedTime.asSeconds() << "s";

        
        window.setTitle(ss.str());


        if (checkVictory()) {
            cout << "\nVictory! You solved the maze in " << elapsedTime.asSeconds() << " seconds!" << endl;
            clock.restart();  
            nextLevel();      
        }

        handleInput();
        generateMaze();

        window.clear();
        drawMaze(window);
        drawGoal(window);     
        drawSolver(window); 
        window.display();
    }

    return 0;
}
