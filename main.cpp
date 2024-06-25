#include <iostream>
#include <raylib.h>
#include <vector>

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

    void Fly(bool isFlying) {
        if (isFlying) {
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

class Bullet {
public:
    Vector2 position, velocity;
    float radius;
    bool active;

    Bullet(float x, float y) {
        position = {x, y};
        velocity = {500.0f, 0.0f};  // Horizontal bullet movement
        radius = 5.0f;
        active = false;
    }

    Bullet(const Bullet& other) {
        position = other.position;
        velocity = other.velocity;
        radius = other.radius;
        active = other.active;
    }

    void Update() {
        if (active) {
            position.x += velocity.x * GetFrameTime();
            if (position.x > GetScreenWidth()) {
                active = false;
            }
        }
    }

    void Draw() {
        if (active) {
            DrawCircleV(position, radius, WHITE);
        }
    }
};

class BulletPrototype {
public:
    virtual ~BulletPrototype() {}
    virtual Bullet* clone(float x, float y) = 0;
};

class BulletSpawn : public BulletPrototype {
private:
    Bullet* prototypeBullet;

public:
    BulletSpawn(Bullet* bullet) : prototypeBullet(bullet) {}

    Bullet* clone(float x, float y) override {
        Bullet* bullet = new Bullet(*prototypeBullet);
        bullet->position = {x, y};
        bullet->active = true;
        return bullet;
    }
};

class Command {
public:
    virtual ~Command() {}
    virtual void execute() = 0;
};

class FlyCommand : public Command {
private:
    Ship* ship;  // Pointer to the ship instance
    bool isFlying;

public:
    FlyCommand(Ship* ship, bool isFlying) : ship(ship), isFlying(isFlying) {}

    void execute() override {
        ship->Fly(isFlying);
    }
};

class ShootCommand : public Command {
private:
    Ship* ship;
    BulletSpawn* bulletPrototype;
    vector<Bullet*>& bullets;

public:
    ShootCommand(Ship* ship, BulletSpawn* spawnBullet, vector<Bullet*>& bullets)
        : ship(ship), bulletPrototype(spawnBullet), bullets(bullets) {}

    void execute() override {
        Bullet* bullet = bulletPrototype->clone(ship->destRec.x + ship->destRec.width / 2, ship->destRec.y);
        bullets.push_back(bullet);
    }
};

class InputHandler {
private:
    Command* flyCommand;
    Command* fallCommand;
    Command* shootCommand;

public:
    InputHandler(Command* flyCmd, Command* fallCmd, Command* shootCmd)
        : flyCommand(flyCmd), fallCommand(fallCmd), shootCommand(shootCmd) {}

    void handleInput() {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            flyCommand->execute();
        } else {
            fallCommand->execute();
        }
        if (IsKeyPressed(KEY_E)) {  // Use IsKeyPressed for single bullet per key press
            shootCommand->execute();
        }
    }
};

int main() {
    int screenWidth = 1280;
    int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "test");

    Ship ship("src/ship.png", screenWidth, screenHeight);
    Bullet bulletPrototype(0, 0);
    BulletSpawn spawnBullets(&bulletPrototype);

    vector<Bullet*> bullets;

    FlyCommand flyCommand(&ship, true);
    FlyCommand fallCommand(&ship, false);
    ShootCommand shootCommand(&ship, &spawnBullets, bullets);
    InputHandler inputHandler(&flyCommand, &fallCommand, &shootCommand);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        inputHandler.handleInput();

        for (Bullet* bullet : bullets) {
            bullet->Update();
        }

        BeginDrawing();
            ClearBackground(BLACK);

            ship.Draw();

            for (Bullet* bullet : bullets) {
                bullet->Draw();
            }

            DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, RED);
            DrawLine(0, screenHeight / 2, screenWidth, screenHeight / 2, RED);

        EndDrawing();
    }

    for (Bullet* bullet : bullets) {
        delete bullet;
    }

    CloseWindow();
    return 0;
}
