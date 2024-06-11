#include <iostream>
#include <raylib.h>

using namespace std;

class Ship {
public:
    Texture2D texture;
    Rectangle sourceRec;
    Rectangle destRec;
    Vector2 origin;
    float rotation;
    float velocity;  // Velocity of the ship
    float acceleration;  // Acceleration rate
    float deceleration;  // Deceleration rate (gravity)

    Ship(const char* texturePath, int screenWidth, int screenHeight) {
        Image image = LoadImage(texturePath);
        if (image.data == nullptr) {
            cerr << "Failed to load image!" << endl;
            exit(-1);  // Exit if the image fails to load
        }

        texture = LoadTextureFromImage(image);
        UnloadImage(image);

        int shipWidth = texture.width;
        int shipHeight = texture.height;

        sourceRec = {0.0f, 0.0f, (float)shipWidth, (float)shipHeight};
        destRec = {
            screenWidth - 1080.0f,
            screenHeight / 2.0f,
            shipWidth / 3.0f,
            shipHeight / 3.0f
        };
        origin = {destRec.width / 2.0f, destRec.height / 2.0f};
        rotation = 0.0f;
        velocity = 0.0f;
        acceleration = 1500.0f;  // Adjust for desired upward acceleration
        deceleration = 1500.0f;  // Adjust for desired downward acceleration
    }

    ~Ship() {
        UnloadTexture(texture);
    }

    void Fly() {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
            velocity -= acceleration * GetFrameTime();  // Increase upward velocity
        } else {
            velocity += deceleration * GetFrameTime();  // Increase downward velocity
        }

        destRec.y += velocity * GetFrameTime();  // Update position based on velocity

        // Prevent the ship from moving off-screen
        if (destRec.y < 0) {
            destRec.y = 0;
            velocity = 0;
        } else if (destRec.y + destRec.height > GetScreenHeight()) {
            destRec.y = GetScreenHeight() - destRec.height;
            velocity = 0;
        }
    }

    void Draw() {
        DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
    }
};

int main () {
    int screenWidth = 1280;
    int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "test");

    Ship ship("src/ship.png", screenWidth, screenHeight);

    //Texture2D ship = LoadTexture("src/ship.png");

    // int shipWidth = ship.width;
    // int shipHeight = ship.height;

    // Rectangle sourceRec = {0.0f, 0.0f, (float)shipWidth, (float)shipHeight};                // area of texture
    // Rectangle destRec = {
    //     150.0f,
    //     screenHeight / 2.0f,
    //     shipWidth / 3.0f,
    //     shipHeight / 3.0f
    // };    // texture placement on canvas
    // Vector2 origin = {destRec.width / 2.0f, destRec.height / 2.0f};                                 // idk lol

    // int rotation = 0;

    SetTargetFPS(60);

    while (WindowShouldClose() == false){
        
        ship.Fly();

        //rotation++;
        BeginDrawing();
            ClearBackground(GRAY);

            ship.Draw();

            DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, RED);
            DrawLine(0, screenHeight / 2, screenWidth, screenHeight / 2, RED);

        EndDrawing();

    }

    CloseWindow();
    return 0;
}