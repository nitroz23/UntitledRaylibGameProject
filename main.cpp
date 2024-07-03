#include <iostream>
#include <raylib.h>
#include <vector>

using namespace std;
//#####################
//Game objects
//#####################
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
        origin = {0.0f, 0.0f};
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
        DrawRectangleLines(destRec.x, destRec.y, destRec.width, destRec.height, RED);
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

class Asteroid{
public:
    Vector2 position, velocity;
    float radius;
    bool active;

    Asteroid(float y, float vx, float rad) {
        position = {GetScreenWidth() + 50.0f, y};
        velocity = {vx, 0.0f};
        this->radius = rad;
        active = false;
    }

    Asteroid(const Asteroid& other) {
        position = other.position;
        velocity = other.velocity;
        radius = other.radius;
        active = other.active;
    }

    void Update() {
        if (active) {
            position.x += velocity.x * GetFrameTime();
            if (position.x < -50) {
                active = false;
            }
        }
    }

    void Draw() {
        if (active) {
            DrawCircleV(position, radius, DARKGRAY);
        }
    }
};

//#####################
//Prototype
//#####################
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

class AsteroidPrototype{
public:
    virtual ~AsteroidPrototype(){}
    virtual Asteroid* clone(float y, float vx, float rad) = 0;
};

class AsteroidSpawn: public AsteroidPrototype{
private:
    Asteroid* prototypeAsteroid;
public:
    AsteroidSpawn(Asteroid* asteroid): prototypeAsteroid(asteroid){}
    Asteroid* clone(float y, float vx, float rad) override{
        Asteroid* asteroid = new Asteroid(*prototypeAsteroid);
        asteroid->position = {GetScreenWidth() + 50.0f, y};
        asteroid->velocity = {vx, 0.0f};
        asteroid->radius = rad;
        asteroid->active = true;
        return asteroid;
    }
};

//#####################
//Command
//#####################
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
        // Center of the ship
        float bulletX = ship->destRec.x + ship->destRec.width;
        float bulletY = ship->destRec.y + ship->destRec.height / 2;
        Bullet* bullet = bulletPrototype->clone(bulletX, bulletY);
        bullets.push_back(bullet);
    }
};

class SpawnAsteroidCommand : public Command {
private:
    AsteroidSpawn* asteroidPrototype;
    vector<Asteroid*>& asteroids;

public:
    SpawnAsteroidCommand(AsteroidSpawn* spawnAsteroid, vector<Asteroid*>& asteroids)
        : asteroidPrototype(spawnAsteroid), asteroids(asteroids) {}
    void execute() override{
        float y = GetRandomValue(0, GetScreenHeight());
        float vx = GetRandomValue(-2000, -1000)/10.0f;
        float rad = GetRandomValue(10, 50);

        Asteroid* asteroid = asteroidPrototype->clone(y, vx, rad);
        asteroids.push_back(asteroid);
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

//#####################
//Main Game Loop
//#####################
int main() {
    int screenWidth = 1280;
    int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "test");

    Ship ship("src/ship.png", screenWidth, screenHeight);
    Bullet bulletPrototype(0, 0);
    BulletSpawn spawnBullets(&bulletPrototype);

    vector<Bullet*> bullets;

    Asteroid asteroidPrototype(0, 0, 0);
    AsteroidSpawn spawnAsteroids(&asteroidPrototype);
    
    vector<Asteroid*> asteroids;

    FlyCommand flyCommand(&ship, true);
    FlyCommand fallCommand(&ship, false);
    ShootCommand shootCommand(&ship, &spawnBullets, bullets);
    SpawnAsteroidCommand spawnAsteroidCommand(&spawnAsteroids, asteroids);

    InputHandler inputHandler(&flyCommand, &fallCommand, &shootCommand);

    float asteroidSpawnTimer = 0.0f;
    float asteroidSpawnInterval = 0.5f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        inputHandler.handleInput();

        asteroidSpawnTimer += GetFrameTime();
        if (asteroidSpawnTimer >= asteroidSpawnInterval) {
            spawnAsteroidCommand.execute();
            asteroidSpawnTimer = 0.0f;
        }

        for (Asteroid* asteroid : asteroids) {
            asteroid->Update();
        }

        for (Bullet* bullet : bullets) {
            bullet->Update();
        }

        // Collision detection for bullets and asteroids
        for (Asteroid* asteroid : asteroids) {
            if (!asteroid->active) continue;
            for (Bullet* bullet : bullets) {
                if (!bullet->active) continue;
                if (CheckCollisionCircles(asteroid->position, asteroid->radius, bullet->position, bullet->radius)) {
                    asteroid->active = false;
                    bullet->active = false;
                }
            }
        }

        // Collision detection for ship and asteroids
        for (Asteroid* asteroid : asteroids) {
            if (!asteroid->active) continue;
            if (CheckCollisionCircleRec(asteroid->position, asteroid->radius, ship.destRec)) {
                // Handle ship and asteroid collision
                // This can be game over or reducing health, etc.
                cout << "Collision with ship!" << endl;
                // For demonstration, deactivating the asteroid
                asteroid->active = false;
            }
        }

        BeginDrawing();
            ClearBackground(BLACK);

            ship.Draw();

            for (Bullet* bullet : bullets) {
                bullet->Draw();
            }

            for (Asteroid* asteroid : asteroids) {
                asteroid->Draw();
            }

            // DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, RED);
            // DrawLine(0, screenHeight / 2, screenWidth, screenHeight / 2, RED);

        EndDrawing();
    }

    for (Bullet* bullet : bullets) {
        delete bullet;
    }

    for (Asteroid* asteroid : asteroids) {
        delete asteroid;
    }

    CloseWindow();
    return 0;
}
