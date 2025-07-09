#include <iostream>
#include "raylib.h"
#include <deque>
#include "raymath.h"
#include <string.h>

Color light = {129, 116, 160, 255};
Color dark = {168, 136, 181, 255};
Color Overlay = {239, 182, 200, 128};
Color Screen = {239, 182, 200, 255};
Color Background = {255, 210, 160, 255};

int cellSize{20};
int cellCount{32};
int offset{64};

typedef enum {
    START,
    PLAYING,
    PAUSED,
    OVER
} State;

bool ElementInDeque(std::deque<Vector2> deq, Vector2 elem) {
    for (auto vec: deq) {
        if(Vector2Equals(vec, elem))
            return true;
    }
    return false;
}

class Food {
    public:
        Vector2 pos;
        Food(std::deque<Vector2> snake_body) {
            pos = RandomPosition(snake_body);
        }

        void Draw() {
            Rectangle apple {pos.x * cellSize + offset, pos.y*cellSize + offset, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(apple, 0.7, 11, RED);
        }

        Vector2 RandomCell() {
            float x = GetRandomValue(4, 28);
            float y = GetRandomValue(4, 28);
            return Vector2{x, y};
        }

        Vector2 RandomPosition(std::deque<Vector2> snake_body){
            Vector2 vec = RandomCell();
            while (ElementInDeque(snake_body, vec)) {
                vec = RandomCell();
            }
            return vec;
        }

        void UpdateFoodPosition(std::deque<Vector2> snake_body) {
            pos = RandomPosition(snake_body);
        }
};

class Snake {
    
    public:
        std::deque<Vector2> body;
        Vector2 direction;
        bool addSegment {false};

        void Draw() {
            for (unsigned int i = 0; i < body.size(); i++) {
                Rectangle segment {body[i].x * cellSize + offset, body[i].y*cellSize + offset, (float)cellSize, (float)cellSize};
                float round = i == 0 ? 0.8 : 0.4;
                DrawRectangleRounded(segment, round, 6, i % 2 == 1 ? light : dark);
            }
        }

        void MoveSnake() {
            body.push_front(body[0] + direction);
            if(addSegment) {
                addSegment = false;
            } else {
                body.pop_back();
            }
        }

        Vector2 GetSnakeHead() {
            return body[0];
        }

        void Reset() {
            body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
            direction = {1, 0};
        }
};

class Game {
    private:
        Snake snake = Snake();
        Food food = Food(snake.body);
        
    public:
        State state = START;
        int score = 0;
        void Draw() {
            if (state == START) {
                snake.Reset();
                DrawRectangle(offset, offset, cellCount*cellSize,cellCount*cellSize, Screen);
                DrawText("Snake Game! \nPress [Enter] to start!", 300, 300, 20, BLACK);
            } else if (state != OVER) {
                food.Draw();
                snake.Draw();
                if (state == PAUSED) {
                    DrawRectangle(offset, offset, cellCount*cellSize,cellCount*cellSize, Overlay);
                    DrawText("Paused.. \nPress [Space] to resume.", 300, 300, 20, BLACK);
                }
            } else {
                DrawRectangle(offset, offset, cellCount*cellSize,cellCount*cellSize, Screen);
                DrawText("Game Over! \nPress [Enter] to restart!", 300, 300, 20, BLACK);
            }
        }

        void Update(int key) {
            if (state == PLAYING) {
                if((key == KEY_D || key == KEY_RIGHT) && snake.direction.x != -1) {            
                    snake.direction = {1, 0};
                }
                if((key == KEY_A || key == KEY_LEFT) && snake.direction.x != 1) {            
                    snake.direction = {-1, 0};
                }
                if((key == KEY_S || key == KEY_DOWN) && snake.direction.y != -1) {            
                    snake.direction = {0, 1};
                }
                if((key == KEY_W || key == KEY_UP) && snake.direction.y != 1) {            
                    snake.direction = {0, -1};
                }
                snake.MoveSnake();
                EdgeCollision();
                SelfCollision();
                CheckFoodEaten();
            }
        }

        void CheckFoodEaten() {
            if (Vector2Equals(snake.GetSnakeHead(), food.pos)) {
                snake.addSegment = true;
                food.UpdateFoodPosition(snake.body);
                score += 10;
            }
        }

        void EdgeCollision() {
            if (snake.body[0].x == cellCount) {
                snake.body[0].x = 0;
            } else if (snake.body[0].x == -1) {
                snake.body[0].x = cellCount - 1;
            }

            if (snake.body[0].y == cellCount) {
                snake.body[0].y = 0;
            } else if (snake.body[0].y == -1) {
                snake.body[0].y = cellCount - 1;
            }
        }

        void SelfCollision() {
            std::deque<Vector2> headlessBody = snake.body;
            headlessBody.pop_front();
            if (ElementInDeque(headlessBody, snake.body[0])){
                state = OVER;
                snake.Reset();
            }
            
        }
};
int main() {

    std::cout << "Starting Game..." << std::endl;


    int winSize { 2 * offset + cellCount * cellSize };
    InitWindow(winSize, winSize, "Snake Game");
    SetTargetFPS(10);

    Game game = Game();
    while (!WindowShouldClose()) {
        BeginDrawing();
        if (IsKeyPressed(KEY_ENTER) && (game.state == START || game.state == OVER)) {
            game.score = 0;
            game.state = PLAYING;
        }
        if (IsKeyPressed(KEY_SPACE) && game.state != OVER) {
            game.state = game.state == PAUSED ? PLAYING : PAUSED;
        }
        game.Update(GetKeyPressed());
        
        ClearBackground(Background);
        Rectangle borders = {(float) offset - 4, (float)offset - 4, (float)cellSize*cellCount + 8, (float)cellSize*cellCount + 8};
        DrawRectangleLinesEx(borders, 5, BLACK);
        DrawText("Snake Game", offset - 4, 20, 40, BLACK);
        DrawText(TextFormat("%i", game.score), offset + (cellCount * cellSize) - 16, offset + (cellCount * cellSize) + 12, 40, BLACK);
        game.Draw();
        EndDrawing();
    }

    return 0;
}