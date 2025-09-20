#include <iostream>
#include <conio.h>
#include <windows.h>
#include <random>
#include<fstream>
#include<string>
using namespace std;

void setcolor(int textcolor) {
    //console color
    cout << "\033[" << textcolor << "m";
}

// Constants, Global variable
const int WIDTH = 80;
const int HEIGHT = 20;
const int MAX_OBSTACLES = 50;
const int MAX_FOODS = 10;
const int MIN_SPEED = 02;


void nam(string name) {
    ofstream outFile("score.txt", ios::app);
    if (outFile) {
        outFile << "Name: " << name << endl;

        outFile.close();
    }
}

void high(int score) {
    ofstream outFile("score.txt", ios::app);
    if (outFile) {
        outFile << "score: " << score << endl;
        
        outFile << "-------------------- " << endl;
        outFile.close();
    }
}

// Structures
struct Obstacle {
    int x;
    int top;
    int bottom;
};

struct Food {
    int x;
    int y;
    bool collected;
};

// Helper Functions
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void clearScreen() {
    system("cls");
}

void displayMenu() {
    clearScreen();
    cout << "                                ----------------------------------" << endl;
    cout << "                                          HELICPOTER GAME            " << endl;
    cout << "                                 ----------------------------------" << endl;
    cout << endl;
    cout << "Welcome to Helicopter Game!" << endl;
    cout << "1. Play" << endl;
    cout << "2. Exit" << endl;
}

void drawObstacle(int x, int top, int bottom) {
    for (int y = top; y <= bottom; ++y) {
        gotoxy(x, y);
        cout << "#";
    }
}

void drawFood(const Food& food) {
    gotoxy(food.x, food.y);
    cout << "O";
}

// Gameplay Functions
void moveFoods(Food foods[], int& foodCount) {
    for (int i = 0; i < foodCount; ++i) {
        if (!foods[i].collected) {
            foods[i].x--;
            if (foods[i].x < 0) {
                foods[i].x = WIDTH - 1;
                foods[i].y = rand() % HEIGHT;
            }
        }
    }
}

void spawnNewFood(Food foods[], int& foodCount) {
    if (foodCount < MAX_FOODS && rand() % 100 < 5) {
        foods[foodCount] = { WIDTH - 1, rand() % HEIGHT, false };
        ++foodCount;
    }
}

void removeCollectedFoods(Food foods[], int& foodCount) {
    for (int i = 0; i < foodCount; ++i) {
        if (foods[i].collected) {
            for (int j = i; j < foodCount - 1; ++j) {
                foods[j] = foods[j + 1];
            }
            --foodCount;
            --i;
        }
    }
}

void addObstacle(Obstacle obstacles[], int& obstacleCount, int x, int top, int bottom) {
    if (obstacleCount < MAX_OBSTACLES) {
        obstacles[obstacleCount] = { x, top, bottom };
        ++obstacleCount;
    }
}

void moveObstacles(Obstacle obstacles[], int& obstacleCount) {
    for (int i = 0; i < obstacleCount; ++i) {
        obstacles[i].x--;
    }
    for (int i = 0; i < obstacleCount; ++i) {
        if (obstacles[i].x < 0) {
            for (int j = i; j < obstacleCount - 1; ++j) {
                obstacles[j] = obstacles[j + 1];
            }
            --obstacleCount;
            --i;
        }
    }
}

bool checkCollision(int heliX, int heliY, const Obstacle obstacles[], int obstacleCount) {
    for (int i = 0; i < obstacleCount; ++i) {
        if (heliX >= obstacles[i].x && heliX <= obstacles[i].x + 1 &&
            heliY >= obstacles[i].top && heliY <= obstacles[i].bottom) {
            return true;
        }
    }
    return false;
}

bool checkFoodCollision(int heliX, int heliY, Food foods[], int& foodCount, int& score) {
    for (int i = 0; i < foodCount; ++i) {
        if (!foods[i].collected && heliX == foods[i].x && heliY == foods[i].y) {
            foods[i].collected = true;
            score++;
            return true;
        }
    }
    return false;
}

void displayLevel(int level) {
    gotoxy(WIDTH / 2 - 5, 0); // Display level at the top center
    cout << "Level: " << level;
}

// Main Game Loop

void playGame() {

    clearScreen();


    // Helicopter position
    int heliX = 5;
    int heliY = HEIGHT / 2;

    // Gameplay variables
    bool gameOver = false;
    int speed = 40;
    int score = 0;
    int level = 1;

    Obstacle obstacles[MAX_OBSTACLES];
    int obstacleCount = 0;

    Food foods[MAX_FOODS];
    int foodCount = 0;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(2, HEIGHT - 3);

    // Initial spawn
    for (int i = 20; i < WIDTH; i += 10) {
        int height = distrib(gen);
        addObstacle(obstacles, obstacleCount, i, height, height + 3);
    }
    for (int i = 0; i < 3; ++i) {
        foods[i] = { WIDTH - 1, rand() % HEIGHT, false };
        ++foodCount;
    }

    while (!gameOver) {
        if (_kbhit()) {
            char input = _getch();
            if (input == 72 && heliY > 0) heliY--; // Up arrow
            else if (input == 80 && heliY < HEIGHT - 1) heliY++; // Down arrow
        }

        clearScreen();
        gotoxy(heliX, heliY);
        cout << ">-";

        // Draw obstacles
        for (int i = 0; i < obstacleCount; ++i) {
            drawObstacle(obstacles[i].x, obstacles[i].top, obstacles[i].bottom);
        }

        // Draw and move foods
        for (int i = 0; i < foodCount; ++i) {
            drawFood(foods[i]);
        }

        moveFoods(foods, foodCount);
        spawnNewFood(foods, foodCount);
        removeCollectedFoods(foods, foodCount);

        // Collision checks
        if (checkCollision(heliX, heliY, obstacles, obstacleCount)) gameOver = true;
        checkFoodCollision(heliX, heliY, foods, foodCount, score);

        // Move obstacles
        moveObstacles(obstacles, obstacleCount);
        if (obstacleCount < 10) {
            for (int i = WIDTH; i < WIDTH + 40; i += 10) {
                int height = distrib(gen);
                addObstacle(obstacles, obstacleCount, i, height, height + 3);
            }
        }

        // Display score and level
        displayLevel(level);
        gotoxy(WIDTH / 2 - 5, HEIGHT - 1); // Bottom center
        cout << "Score: " << score;

        // Increase difficulty as score grows
        if (score >= level * 5) { // Level up every 5 points
            level++;
            if (speed > MIN_SPEED) speed -= 10; // Increase game speed
        }

        Sleep(speed);
    }

    clearScreen();

    high(score);
    cout << "Game Over!" << endl;
    cout << "Your Score : " << score << endl;
    cout << "Final Level: " << level << endl;
    Sleep(5000);
}

int main() {
    setcolor(94);
    char choice;
    string  name;
    cout << "                                ----------------------------------" << endl;
    cout << "                                          HELICPOTER GAME            " << endl;
    cout << "                                ----------------------------------" << endl;
    cout << endl;
    cout << " Enter your Name: ";
    getline(cin, name);
    do {
        displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == '1') {
            nam(name);
            playGame();
        }
        else if (choice != '2') {
            cout << "Invalid choice. Try again!" << endl;
            Sleep(100);
        }
    } while (choice != '2');

    cout << "Exiting..." << endl;
    return 0;
}
