#include<iostream>
#include<stdio.h>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

Color custom_green = {173,204,96,255};
Color custom_darkGreen = {43,51,24,255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;
double lastUpdateTime = 0;

bool eventTriggered(double interval){
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

bool elementInDeque(Vector2 element, deque<Vector2> deque){
    for(auto bodyBlock:deque){
        if(Vector2Equals(element,bodyBlock)){return true;}
    }
    return false;
}
class Food{
    public:
        Vector2 position;
        Texture2D texture;

        Food(deque<Vector2> snakeBody){
            position = GenerateRandomPos(snakeBody);
            Image image = LoadImage("./Assets/food.png");
            texture = LoadTextureFromImage(image);
            UnloadImage(image);
        };

        ~Food(){
            UnloadTexture(texture);
        }

        void Draw(){
            DrawTexture(texture, position.x*cellSize,position.y*cellSize,WHITE);
        }
        Vector2 GenerateRandomCell(){
            float x = GetRandomValue(0,cellCount-1);
            float y = GetRandomValue(0,cellCount-1);

            return Vector2{x,y};
        }

        Vector2 GenerateRandomPos(deque<Vector2> snakeBody){
            Vector2 position = GenerateRandomCell();

            while(elementInDeque(position,snakeBody)){
                position = GenerateRandomCell();
            }
            
            return position;
        }
};

class Snake{
    public:
        deque <Vector2> body = {Vector2{6,9} , Vector2{5,9} , Vector2{4,9}};
        Vector2 direction = {1,0};
        bool addSegment = false;

        void Draw(){
            for(auto bodyBlock:body){
                Rectangle segment = Rectangle{(float)bodyBlock.x * cellSize , (float) bodyBlock.y*cellSize,(float)cellSize,(float)cellSize};
                DrawRectangleRounded(segment,0.5,6,custom_darkGreen);    
            }
            
        }

        void Update(){
            if(addSegment){
                addSegment = false;
            }else{
                body.pop_back();
            }
            body.push_front(Vector2Add(body[0],direction));
 
        }

        void Reset(){
            body = {Vector2{6,9} , Vector2{5,9} , Vector2{4,9}};
            direction = Vector2{1,0};
        }

};

class Game{
    public:
        Snake snake = Snake();
        Food food = Food(snake.body);
        bool running = true;
        bool isPaused = false;
        int score = 0;

        void Draw(){
            food.Draw();
            snake.Draw();

        }

        void Update(){
            if(running){
                snake.Update();

                CheckCollisionWithEdge();
                CheckCollisionWithFood();
                CheckCollisionWithTail();
            }
            
        }

        void CheckCollisionWithFood(){
            if(Vector2Equals(snake.body[0], food.position)){
                food.position = food.GenerateRandomPos(snake.body);
                snake.addSegment = true;
                score++;
            }
        }

        void CheckCollisionWithEdge(){
            if(snake.body[0].x == cellCount || snake.body[0].x == -1){
                GameOver();
            }

            if(snake.body[0].y == cellCount || snake.body[0].y == -1){
                GameOver();
            }
        }

        void CheckCollisionWithTail(){
            for(int idx = 1; idx<snake.body.size();idx++){
                if(Vector2Equals(snake.body[0], snake.body[idx])){
                    GameOver();
                }
            }
        }

        void GameOver(){
            snake.Reset();
            food.position = food.GenerateRandomPos(snake.body);
            running = false;
            score = 0;
            
        }
};

int main(){
    
    InitWindow(cellSize*cellCount,cellSize*cellCount,"Retro Snake");

    SetTargetFPS(60);

    Game game = Game();

    while(!WindowShouldClose()){
        BeginDrawing();

        
        if(!game.isPaused){
            if(eventTriggered(0.2)){
                game.Update();
            }
            if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
                game.snake.direction = {0,-1};
            }else if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
                game.snake.direction = {0,1};
            }else if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
                game.snake.direction = {-1,0};
            }else if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
                game.snake.direction = {1,0};
            }else if(!game.running && IsKeyPressed(KEY_SPACE)){
                game.running = true;
            }else if(IsKeyPressed(KEY_P)){
                game.isPaused = true;
            }
        }else{
            if(IsKeyPressed(KEY_P)){
                game.isPaused = false;
            }
        }

        ClearBackground(custom_green);

        game.Draw();

        DrawText(TextFormat("SCORE: %d",game.score),10,10,20,custom_darkGreen);

        if(game.isPaused){
            DrawText("Press P to unpause.",((cellSize*cellCount)/2) - 100,(cellSize*cellCount)/2,25,custom_darkGreen);
        }
    
        EndDrawing();
    }

    CloseWindow();
    return 0;
}