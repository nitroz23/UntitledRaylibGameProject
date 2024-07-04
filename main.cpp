#include <iostream>
#include <raylib.h>
#include <vector>

using namespace std;

typedef enum gameScreen { GAMEPLAY = 0, GAMEOVER } gameScreen;

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
    float velocity;  
    float acceleration;  
    float deceleration;  
    Vector2 initialPosition;

    Ship(const char* texturePath, int screenWidth, int screenHeight) {
        Image image = LoadImage(texturePath);
        if (image.data == nullptr) {
            cerr << "Failed to load image!" << endl;
            exit(-1);
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
        acceleration = 1500.0f;  
        deceleration = 1500.0f;
        initialPosition = {screenWidth - 1080.0f, screenHeight / 2.0f};
    }

    ~Ship() {
        UnloadTexture(texture);
    }

    void Fly(bool isFlying) {
        if (isFlying) {
            velocity -= acceleration * GetFrameTime();  
        } else {
            velocity += deceleration * GetFrameTime(); 
        }

        destRec.y += velocity * GetFrameTime();  

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

    void Reset() {
        destRec.x = initialPosition.x;
        destRec.y = initialPosition.y;
        velocity = 0.0f;
    }
};

class Bullet {
public:
    Vector2 position, velocity;
    float radius;
    bool active;

    Bullet(float x, float y) {
        position = {x, y};
        velocity = {500.0f, 0.0f};  
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

class Asteroid {
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

class AsteroidPrototype {
public:
    virtual ~AsteroidPrototype() {}
    virtual Asteroid* clone(float y, float vx, float rad) = 0;
};

class AsteroidSpawn : public AsteroidPrototype {
private:
    Asteroid* prototypeAsteroid;
public:
    AsteroidSpawn(Asteroid* asteroid) : prototypeAsteroid(asteroid) {}
    Asteroid* clone(float y, float vx, float rad) override {
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
    Ship* ship;  
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
    void execute() override {
        float y = GetRandomValue(0, GetScreenHeight());
        float vx = GetRandomValue(-2000, -1000) / 10.0f;
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
        if (IsKeyPressed(KEY_E)) { 
            shootCommand->execute();
        }
    }
};

//#####################
//Main Game Loop
//#####################
void ResetGame(Ship& ship, vector<Bullet*>& bullets, vector<Asteroid*>& asteroids) {
    ship.Reset();

    for (Bullet* bullet : bullets) {
        delete bullet;
    }
    bullets.clear();

    for (Asteroid* asteroid : asteroids) {
        delete asteroid;
    }
    asteroids.clear();
}

int main() {
    int screenWidth = 1280;
    int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "test");

    gameScreen currentScreen = GAMEPLAY;

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

    int score = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        switch (currentScreen) {
            case GAMEPLAY: {
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

                for (Asteroid* asteroid : asteroids) {
                    if (!asteroid->active) continue;
                    for (Bullet* bullet : bullets) {
                        if (!bullet->active) continue;
                        if (CheckCollisionCircles(asteroid->position, asteroid->radius, bullet->position, bullet->radius)) {
                            asteroid->active = false;
                            bullet->active = false;
                            score += 1;
                        }
                    }
                }
                for (Asteroid* asteroid : asteroids) {
                    if (!asteroid->active) continue;
                    if (CheckCollisionCircleRec(asteroid->position, asteroid->radius, ship.destRec)) {
                        cout << "Collision with ship!" << endl;
                        asteroid->active = false;
                        currentScreen = GAMEOVER;
                    }
                }
            } break;
            case GAMEOVER: {
                if (IsKeyPressed(KEY_R)) {
                    ResetGame(ship, bullets, asteroids);
                    score = 0;
                    currentScreen = GAMEPLAY;
                }
            } break;
            default:
                break;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        switch (currentScreen) {
            case GAMEPLAY: {
                DrawText(TextFormat("SCORE: %d", score), 10, 10, 20, WHITE);
                ship.Draw();

                for (Bullet* bullet : bullets) {
                    bullet->Draw();
                }

                for (Asteroid* asteroid : asteroids) {
                    asteroid->Draw();
                }

            } break;
            case GAMEOVER: {
                DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 50) / 2, screenHeight / 2 - 20, 50, PINK);
                DrawText(TextFormat("SCORE: %d", score), screenWidth / 2 - MeasureText(TextFormat("SCORE: %d", score), 25) / 2, screenHeight / 2 + 30, 25, PINK);
                DrawText("PRESS 'R' TO RETRY || PRESS 'ESC' TO QUIT", screenWidth / 2 - MeasureText("PRESS 'R' TO RETRY || PRESS 'ESC' TO QUIT", 20) / 2, screenHeight / 2 + 75, 20, PINK);
                DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, RED);
            }
        }
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
