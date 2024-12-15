#include <iostream>
#include <raylib.h>
#include <vector>

using namespace std;

typedef enum gameScreen { GAMEPLAY = 0, GAMEOVER} gameScreen;

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

class Star {
public:
    Vector2 position, velocity;
    Texture2D texture;
    Rectangle sourceRec;
    Rectangle destRec;
    Vector2 origin;
    bool active;

    Star(const char* texturePath, float vx, float scale) {
        Image image = LoadImage(texturePath);
        if (image.data == nullptr) {
            cerr << "Failed to load Star texture!" << endl;
            exit(-1);
        }

        texture = LoadTextureFromImage(image);
        UnloadImage(image);

        sourceRec = {0.0f, 0.0f, (float)texture.width, (float)texture.height};
        destRec = {GetScreenWidth() + 50.0f, 0.0f, (float)texture.width, (float)texture.height};
        origin = {0.0f, 0.0f};
        velocity = {vx, 0.0f};
        active = false;
    }

    Star(const Star& other) {
        texture = other.texture; 
        sourceRec = other.sourceRec;
        destRec = other.destRec;
        origin = other.origin;
        velocity = other.velocity;
        position = other.position;
        active = other.active;
    }

    ~Star() {
        UnloadTexture(texture);
    }

    void Update(int& score) {
        if (active) {
            position.x += velocity.x * GetFrameTime();
            destRec.x = position.x;

            if (position.x + destRec.width < 0) {
                active = false;
                score -= 1;
            }
        }
    }

    void Draw() {
        if (active) {
            DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
        }
    }
};

class Polri {
public:
    Vector2 position, velocity;
    Texture2D texture;
    Rectangle sourceRec;
    Rectangle destRec;
    Vector2 origin;
    bool active;

    Polri(const char* texturePath, float vx, float scale) {
        Image image = LoadImage(texturePath);
        if (image.data == nullptr) {
            cerr << "Failed to load Polri texture!" << endl;
            exit(-1);
        }

        texture = LoadTextureFromImage(image);
        UnloadImage(image);

        sourceRec = {0.0f, 0.0f, (float)texture.width, (float)texture.height};
        destRec = {GetScreenWidth() + 50.0f, 0.0f, (float)texture.width, (float)texture.height};
        origin = {0.0f, 0.0f};
        velocity = {vx, 0.0f};
        active = false;
    }

    Polri(const Polri& other) {
        texture = other.texture; 
        sourceRec = other.sourceRec;
        destRec = other.destRec;
        origin = other.origin;
        velocity = other.velocity;
        position = other.position;
        active = other.active;
    }

    ~Polri() {
        UnloadTexture(texture);
    }

    void Update(int& score) {
        if (active) {
            position.x += velocity.x * GetFrameTime();
            destRec.x = position.x;

            if (position.x + destRec.width < 0) {
                active = false;
                score -= 2;
            }
        }
    }

    void Draw() {
        if (active) {
            DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
        }
    }
};

class OPM {
public:
    Vector2 position, velocity;
    Texture2D texture;
    Rectangle sourceRec;
    Rectangle destRec;
    Vector2 origin;
    bool active;

    OPM(const char* texturePath, float vx, float scale) {
        Image image = LoadImage(texturePath);
        if (image.data == nullptr) {
            cerr << "Failed to load OPM texture!" << endl;
            exit(-1);
        }

        texture = LoadTextureFromImage(image);
        UnloadImage(image);

        sourceRec = {0.0f, 0.0f, (float)texture.width, (float)texture.height};
        destRec = {GetScreenWidth() + 50.0f, 0.0f, (float)texture.width, (float)texture.height};
        origin = {0.0f, 0.0f};
        velocity = {vx, 0.0f};
        active = false;
    }

    OPM(const OPM& other) {
        texture = other.texture; 
        sourceRec = other.sourceRec;
        destRec = other.destRec;
        origin = other.origin;
        velocity = other.velocity;
        position = other.position;
        active = other.active;
    }

    ~OPM() {
        UnloadTexture(texture);
    }

    void Update(int& score) {
        if (active) {
            position.x += velocity.x * GetFrameTime();
            destRec.x = position.x;

            if (position.x + destRec.width < 0) {
                active = false;
                score -= 3;
            }
        }
    }

    void Draw() {
        if (active) {
            DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
        }
    }
};

class Gibran {
public:
    Vector2 position, velocity;
    Texture2D texture;
    Rectangle sourceRec;
    Rectangle destRec;
    Vector2 origin;
    bool active;

    Gibran(const char* texturePath, float vx, float scale) {
        Image image = LoadImage(texturePath);
        if (image.data == nullptr) {
            cerr << "Failed to load Gibran texture!" << endl;
            exit(-1);
        }

        texture = LoadTextureFromImage(image);
        UnloadImage(image);

        sourceRec = {0.0f, 0.0f, (float)texture.width, (float)texture.height};
        destRec = {GetScreenWidth() + 50.0f, 0.0f, (float)texture.width, (float)texture.height};
        origin = {0.0f, 0.0f};
        velocity = {vx, 0.0f};
        active = false;
    }

    Gibran(const Gibran& other) {
        texture = other.texture; 
        sourceRec = other.sourceRec;
        destRec = other.destRec;
        origin = other.origin;
        velocity = other.velocity;
        position = other.position;
        active = other.active;
    }

    ~Gibran() {
        UnloadTexture(texture);
    }

    void Update(int& score) {
        if (active) {
            position.x += velocity.x * GetFrameTime();
            destRec.x = position.x;

            if (position.x + destRec.width < 0) {
                active = false;
                score -= 4;
            }
        }
    }

    void Draw() {
        if (active) {
            DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
        }
    }
};

class MA {
public:
    Vector2 position, velocity;
    Texture2D texture;
    Rectangle sourceRec;
    Rectangle destRec;
    Vector2 origin;
    bool active;

    MA(const char* texturePath, float vx, float scale) {
        Image image = LoadImage(texturePath);
        if (image.data == nullptr) {
            cerr << "Failed to load MA texture!" << endl;
            exit(-1);
        }

        texture = LoadTextureFromImage(image);
        UnloadImage(image);

        sourceRec = {0.0f, 0.0f, (float)texture.width, (float)texture.height};
        destRec = {GetScreenWidth() + 50.0f, 0.0f, (float)texture.width, (float)texture.height};
        origin = {0.0f, 0.0f};
        velocity = {vx, 0.0f};
        active = false;
    }

    MA(const MA& other) {
        texture = other.texture; 
        sourceRec = other.sourceRec;
        destRec = other.destRec;
        origin = other.origin;
        velocity = other.velocity;
        position = other.position;
        active = other.active;
    }

    ~MA() {
        UnloadTexture(texture);
    }

    void Update(int& score) {
        if (active) {
            position.x += velocity.x * GetFrameTime();
            destRec.x = position.x;

            if (position.x + destRec.width < 0) {
                active = false;
                score -= 5;
            }
        }
    }

    void Draw() {
        if (active) {
            DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
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

class StarPrototype {
public:
    virtual ~StarPrototype() {}
    virtual Star* clone(float y, float vx, float scale) = 0;
};

class StarSpawn : public StarPrototype {
private:
    Star* prototypeStar;

public:
    StarSpawn(Star* star) : prototypeStar(star) {}

    Star* clone(float y, float vx, float scale) override {
        Star* star = new Star(*prototypeStar);
        star->position = {GetScreenWidth() + 50.0f, y};
        star->velocity = {vx, 0.0f};
        star->destRec.y = y;
        star->destRec.width = prototypeStar->texture.width * scale;
        star->destRec.height = prototypeStar->texture.height * scale;
        star->active = true;
        return star;
    }
};

class PolriPrototype {
public:
    virtual ~PolriPrototype() {}
    virtual Polri* clone(float y, float vx, float scale) = 0;
};

class PolriSpawn : public PolriPrototype {
private:
    Polri* prototypePolri;

public:
    PolriSpawn(Polri* polri) : prototypePolri(polri) {}

    Polri* clone(float y, float vx, float scale) override {
        Polri* polri = new Polri(*prototypePolri);
        polri->position = {GetScreenWidth() + 50.0f, y};
        polri->velocity = {vx, 0.0f};
        polri->destRec.y = y;
        polri->destRec.width = prototypePolri->texture.width * scale;
        polri->destRec.height = prototypePolri->texture.height * scale;
        polri->active = true;
        return polri;
    }
};

class OPMPrototype {
public:
    virtual ~OPMPrototype() {}
    virtual OPM* clone(float y, float vx, float scale) = 0;
};

class OPMSpawn : public OPMPrototype {
private:
    OPM* prototypeOPM;

public:
    OPMSpawn(OPM* opm) : prototypeOPM(opm) {}

    OPM* clone(float y, float vx, float scale) override {
        OPM* opm = new OPM(*prototypeOPM);
        opm->position = {GetScreenWidth() + 50.0f, y};
        opm->velocity = {vx, 0.0f};
        opm->destRec.y = y;
        opm->destRec.width = prototypeOPM->texture.width * scale;
        opm->destRec.height = prototypeOPM->texture.height * scale;
        opm->active = true;
        return opm;
    }
};

class GibranPrototype {
public:
    virtual ~GibranPrototype() {}
    virtual Gibran* clone(float y, float vx, float scale) = 0;
};

class GibranSpawn : public GibranPrototype {
private:
    Gibran* prototypeGibran;

public:
    GibranSpawn(Gibran* gibran) : prototypeGibran(gibran) {}

    Gibran* clone(float y, float vx, float scale) override {
        Gibran* gibran = new Gibran(*prototypeGibran);
        gibran->position = {GetScreenWidth() + 50.0f, y};
        gibran->velocity = {vx, 0.0f};
        gibran->destRec.y = y;
        gibran->destRec.width = prototypeGibran->texture.width * scale;
        gibran->destRec.height = prototypeGibran->texture.height * scale;
        gibran->active = true;
        return gibran;
    }
};

class MAPrototype {
public:
    virtual ~MAPrototype() {}
    virtual MA* clone(float y, float vx, float scale) = 0;
};

class MASpawn : public MAPrototype {
private:
    MA* prototypeMA;

public:
    MASpawn(MA* ma) : prototypeMA(ma) {}

    MA* clone(float y, float vx, float scale) override {
        MA* ma = new MA(*prototypeMA);
        ma->position = {GetScreenWidth() + 50.0f, y};
        ma->velocity = {vx, 0.0f};
        ma->destRec.y = y;
        ma->destRec.width = prototypeMA->texture.width * scale;
        ma->destRec.height = prototypeMA->texture.height * scale;
        ma->active = true;
        return ma;
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

class SpawnStarCommand : public Command {
private:
    StarSpawn* StarPrototype;
    std::vector<Star*>& stars;

public:
    SpawnStarCommand(StarSpawn* spawnStar, std::vector<Star*>& stars)
        : StarPrototype(spawnStar), stars(stars) {}

    void execute() override {
        float y = GetRandomValue(0, GetScreenHeight());
        float vx = GetRandomValue(-2000, -1000) / 10.0f;
        float scale = GetRandomValue(20, 50) / 100.0f;

        Star* star = StarPrototype->clone(y, vx, scale);
        stars.push_back(star);
    }
};

class SpawnPolriCommand : public Command {
private:
    PolriSpawn* PolriPrototype;
    std::vector<Polri*>& polris;

public:
    SpawnPolriCommand(PolriSpawn* spawnPolri, std::vector<Polri*>& polris)
        : PolriPrototype(spawnPolri), polris(polris) {}

    void execute() override {
        float y = GetRandomValue(0, GetScreenHeight());
        float vx = GetRandomValue(-2000, -1000) / 10.0f;
        float scale = GetRandomValue(20, 50) / 500.0f;

        Polri* polri = PolriPrototype->clone(y, vx, scale);
        polris.push_back(polri);
    }
};

class SpawnOPMCommand : public Command {
private:
    OPMSpawn* OPMPrototype;
    std::vector<OPM*>& opms;

public:
    SpawnOPMCommand(OPMSpawn* spawnOPM, std::vector<OPM*>& opms)
        : OPMPrototype(spawnOPM), opms(opms) {}

    void execute() override {
        float y = GetRandomValue(0, GetScreenHeight());
        float vx = GetRandomValue(-2000, -1000) / 10.0f;
        float scale = GetRandomValue(20, 50) / 100.0f;

        OPM* opm = OPMPrototype->clone(y, vx, scale);
        opms.push_back(opm);
    }
};

class SpawnGibranCommand : public Command {
private:
    GibranSpawn* GibranPrototype;
    std::vector<Gibran*>& gibrans;

public:
    SpawnGibranCommand(GibranSpawn* spawnGibran, std::vector<Gibran*>& gibrans)
        : GibranPrototype(spawnGibran), gibrans(gibrans) {}

    void execute() override {
        float y = GetRandomValue(0, GetScreenHeight());
        float vx = GetRandomValue(-2000, -1000) / 10.0f;
        float scale = GetRandomValue(20, 30) / 100.0f;

        Gibran* gibran = GibranPrototype->clone(y, vx, scale);
        gibrans.push_back(gibran);
    }
};

class SpawnMACommand : public Command {
private:
    MASpawn* MAPrototype;
    std::vector<MA*>& mas;

public:
    SpawnMACommand(MASpawn* spawnMA, std::vector<MA*>& mas)
        : MAPrototype(spawnMA), mas(mas) {}

    void execute() override {
        float y = GetRandomValue(0, GetScreenHeight());
        float vx = GetRandomValue(-2000, -1000) / 10.0f;
        float scale = GetRandomValue(20, 50) / 500.0f;

        MA* ma = MAPrototype->clone(y, vx, scale);
        mas.push_back(ma);
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
// Function to reset all obstacles
void ResetObstacles(std::vector<Star*>& stars, std::vector<Polri*>& polris,
                    std::vector<OPM*>& opms, std::vector<Gibran*>& gibrans,
                    std::vector<MA*>& mas) {
    for (auto* star : stars) {
        star->active = false;
        star->position = {GetScreenWidth() + 50.0f, 0.0f};
        star->destRec.x = star->position.x;
        star->destRec.y = 0.0f; 
    }

    for (auto* polri : polris) {
        polri->active = false;
        polri->position = {GetScreenWidth() + 50.0f, 0.0f};
        polri->destRec.x = polri->position.x;
        polri->destRec.y = 0.0f; 
    }

    for (auto* opm : opms) {
        opm->active = false;
        opm->position = {GetScreenWidth() + 50.0f, 0.0f};
        opm->destRec.x = opm->position.x;
        opm->destRec.y = 0.0f; 
    }

    for (auto* gibran : gibrans) {
        gibran->active = false;
        gibran->position = {GetScreenWidth() + 50.0f, 0.0f};
        gibran->destRec.x = gibran->position.x;
        gibran->destRec.y = 0.0f; 
    }

    for (auto* ma : mas) {
        ma->active = false;
        ma->position = {GetScreenWidth() + 50.0f, 0.0f};
        ma->destRec.x = ma->position.x;
        ma->destRec.y = 0.0f; 
    }
    
}


void ResetGame(Ship& ship, vector<Bullet*>& bullets, vector<Asteroid*>& asteroids, 
               vector<Star*>& stars, vector<Polri*>& polris, vector<OPM*>& opms,
               vector<Gibran*>& gibrans, vector<MA*>& mas) {
    ship.Reset();

    for (Bullet* bullet : bullets) {
        delete bullet;
    }
    bullets.clear();

    for (Asteroid* asteroid : asteroids) {
        delete asteroid;
    }
    asteroids.clear();

    ResetObstacles(stars, polris, opms, gibrans, mas);
}

int main() {
    int screenWidth = 1280;
    int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "GARUDA PANCASILA");

    gameScreen currentScreen = GAMEPLAY;

    Ship ship("src/ship.png", screenWidth, screenHeight);
    Bullet bulletPrototype(0, 0);
    BulletSpawn spawnBullets(&bulletPrototype);

    vector<Bullet*> bullets;

    Asteroid asteroidPrototype(0, 0, 0);
    AsteroidSpawn spawnAsteroids(&asteroidPrototype);

    vector<Asteroid*> asteroids;

    Star starPrototype("src/star.png", 0, 0.1f); 
    StarSpawn spawnStars(&starPrototype);
    
    vector<Star*> stars;

    Polri PolriPrototype("src/polri.png", 0, 0.1f);
    PolriSpawn spawnPolris(&PolriPrototype);

    vector<Polri*> polris;

    OPM OPMPrototype("src/opm.png", 0, 0.1f);
    OPMSpawn spawnOPMS(&OPMPrototype);

    vector<OPM*> opms;

    Gibran GibranPrototype("src/gibran.png", 0, 0.1f);
    GibranSpawn spawnGibrans(&GibranPrototype);

    vector<Gibran*> gibrans;

    MA MAPrototype("src/MA.png", 0, 0.1f);
    MASpawn spawnMAs(&MAPrototype);

    vector<MA*> mas;

    FlyCommand flyCommand(&ship, true);
    FlyCommand fallCommand(&ship, false);
    ShootCommand shootCommand(&ship, &spawnBullets, bullets);
    //SpawnAsteroidCommand spawnAsteroidCommand(&spawnAsteroids, asteroids);
    SpawnStarCommand spawnStarCommand(&spawnStars, stars);
    SpawnPolriCommand spawnPolriCommand(&spawnPolris, polris);
    SpawnOPMCommand spawnOPMCommand(&spawnOPMS, opms);
    SpawnGibranCommand spawnGibranCommand(&spawnGibrans, gibrans);
    SpawnMACommand spawnMACommand(&spawnMAs, mas);

    InputHandler inputHandler(&flyCommand, &fallCommand, &shootCommand);

    /* float asteroidSpawnTimer = 0.0f;
    float asteroidSpawnInterval = 0.5f; */

    float starSpawnTimer = 0.0f;
    float starSpawnInterval = 1.0f;

    float polriSpawnTimer = 0.0f;
    float polriSpawnInterval = 2.0f;

    float opmSpawnTimer = 0.0f;
    float opmSpawnInterval = 3.0f;

    float gibranSpawnTimer = 0.0f;
    float gibranSpawnInterval = 4.0f;
    
    float maSpawnTimer = 0.0f;
    float maSpawnInterval = 5.0f;
    
    int score = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        switch (currentScreen) {
            case GAMEPLAY: {
                inputHandler.handleInput();

               /*  for (Obstacle* obstacle : obstacles) {
                    if (obstacle->active) {
                        cout << "Obstacle position: " << obstacle->position.x << ", " << obstacle->position.y << endl;
                        DrawCircle(obstacle->position.x, obstacle->position.y, 5, RED);
                    }
                } */


              /*   asteroidSpawnTimer += GetFrameTime();
                if (asteroidSpawnTimer >= asteroidSpawnInterval) {
                    spawnAsteroidCommand.execute();
                    asteroidSpawnTimer = 0.0f;
                }

                for (Asteroid* asteroid : asteroids) {
                    asteroid->Update();
                } */

                starSpawnTimer += GetFrameTime();
                if(starSpawnTimer >= starSpawnInterval){
                    spawnStarCommand.execute();
                    starSpawnTimer = 0.0f;
                }

                for(Star* star : stars){
                    star->Update(score);
                }
                
                polriSpawnTimer += GetFrameTime();
                if(polriSpawnTimer >= polriSpawnInterval){
                    spawnPolriCommand.execute();
                    polriSpawnTimer = 0.0f;
                }

                for(Polri* polri : polris){
                    polri->Update(score);
                }

                opmSpawnTimer += GetFrameTime();
                if(opmSpawnTimer >= opmSpawnInterval){
                    spawnOPMCommand.execute();
                    opmSpawnTimer = 0.0f;
                }

                for(OPM* opm : opms){
                    opm->Update(score);
                }

                gibranSpawnTimer += GetFrameTime();
                if(gibranSpawnTimer >= gibranSpawnInterval){
                    spawnGibranCommand.execute();
                    gibranSpawnTimer = 0.0f;
                }

                for(Gibran* gibran : gibrans){
                    gibran->Update(score);
                }

                maSpawnTimer += GetFrameTime();
                if(maSpawnTimer >= maSpawnInterval){
                    spawnMACommand.execute();
                    maSpawnTimer = 0.0f;
                }

                for(MA* ma : mas){
                    ma->Update(score);
                }

                for (Bullet* bullet : bullets) {
                    bullet->Update();
                }


                /* for (Asteroid* asteroid : asteroids) {
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
                        asteroid->active = false;
                        currentScreen = GAMEOVER;
                    }
                } */

                for (Star* star : stars) {
                    if (!star->active) continue;

                    for (Bullet* bullet : bullets) {
                        if (!bullet->active) continue;
                        Rectangle bulletRec = {bullet->position.x - bullet->radius, bullet->position.y - bullet->radius, bullet->radius * 2, bullet->radius * 2};

                        if (CheckCollisionRecs(star->destRec, bulletRec)) {
                            star->active = false;
                            bullet->active = false;
                            score += 1;
                            break;
                        }
                    }
                }

                for(Star* star : stars){
                    if(!star->active) continue;
                    if(CheckCollisionRecs(star->destRec, ship.destRec)){
                        star->active = false;
                        currentScreen = GAMEOVER;
                    }
                }

                for(Polri* polri : polris){
                    if(!polri->active) continue;

                    for(Bullet* bullet : bullets){
                        if(!bullet->active) continue;
                        Rectangle bulletRec = {bullet->position.x - bullet->radius, bullet->position.y - bullet->radius, bullet->radius * 2, bullet->radius * 2};

                        if(CheckCollisionRecs(polri->destRec, bulletRec)){
                            polri->active = false;
                            bullet->active = false;
                            score += 2;
                            break;
                        }
                    }
                }

                for(Polri* polri : polris){
                    if(!polri->active) continue;
                    if(CheckCollisionRecs(polri->destRec, ship.destRec)){
                        polri->active = false;
                        currentScreen = GAMEOVER;
                    }
                }

                for(OPM* opm : opms){
                    if(!opm->active) continue;

                    for(Bullet* bullet : bullets){
                        if(!bullet->active) continue;
                        Rectangle bulletRec = {bullet->position.x - bullet->radius, bullet->position.y - bullet->radius, bullet->radius * 2, bullet->radius * 2};

                        if(CheckCollisionRecs(opm->destRec, bulletRec)){
                            opm->active = false;
                            bullet->active = false;
                            score += 3;
                            break;
                        }
                    }
                }

                for(OPM* opm : opms){
                    if(!opm->active) continue;
                    if(CheckCollisionRecs(opm->destRec, ship.destRec)){
                        opm->active = false;
                        currentScreen = GAMEOVER;
                    }
                }

                for(Gibran* gibran : gibrans){
                    if(!gibran->active) continue;

                    for(Bullet* bullet : bullets){
                        if(!bullet->active) continue;
                        Rectangle bulletRec = {bullet->position.x - bullet->radius, bullet->position.y - bullet->radius, bullet->radius * 2, bullet->radius * 2};

                        if(CheckCollisionRecs(gibran->destRec, bulletRec)){
                            gibran->active = false;
                            bullet->active = false;
                            score += 4;
                            break;
                        }
                    }
                }

                for(Gibran* gibran : gibrans){
                    if(!gibran->active) continue;
                    if(CheckCollisionRecs(gibran->destRec, ship.destRec)){
                        gibran->active = false;
                        currentScreen = GAMEOVER;
                    }
                }

                for(MA* ma : mas){
                    if(!ma->active) continue;

                    for(Bullet* bullet : bullets){
                        if(!bullet->active) continue;
                        Rectangle bulletRec = {bullet->position.x - bullet->radius, bullet->position.y - bullet->radius, bullet->radius * 2, bullet->radius * 2};

                        if(CheckCollisionRecs(ma->destRec, bulletRec)){
                            ma->active = false;
                            bullet->active = false;
                            score += 5;
                            break;
                        }
                    }
                }

                for(MA* ma : mas){
                    if(!ma->active) continue;
                    if(CheckCollisionRecs(ma->destRec, ship.destRec)){
                        ma->active = false;
                        currentScreen = GAMEOVER;
                    }
                }

            } break;
            case GAMEOVER: {
                if (IsKeyPressed(KEY_R)) {
                    ResetGame(ship, bullets, asteroids, stars, polris, opms, gibrans, mas);
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

                /* for (Asteroid* asteroid : asteroids) {
                    asteroid->Draw();
                } */
                 
                for(Star* star : stars){
                    star->Draw();
                }

                for(Polri* polri : polris){
                    polri->Draw();
                }

                for(OPM* opm : opms){
                    opm->Draw();
                }

                for(Gibran* gibran : gibrans){
                    gibran->Draw();
                }

                for(MA* ma : mas){
                    ma->Draw();
                }

               //DrawTexture(obstaclePrototype.texture, screenWidth - obstaclePrototype.texture.width - 10, 10, WHITE);

            } break;
            case GAMEOVER: {
                DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 50) / 2, screenHeight / 2 - 20, 50, PINK);
                DrawText(TextFormat("SCORE: %d", score), screenWidth / 2 - MeasureText(TextFormat("SCORE: %d", score), 25) / 2, screenHeight / 2 + 30, 25, PINK);
                DrawText("PRESS 'R' TO RETRY || PRESS 'ESC' TO QUIT", screenWidth / 2 - MeasureText("PRESS 'R' TO RETRY || PRESS 'ESC' TO QUIT", 20) / 2, screenHeight / 2 + 75, 20, PINK);
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

    for(Star* star : stars){
        delete star;
    }

    for(Polri* polri : polris){
        delete polri;
    }

    for(OPM* opm : opms){
        delete opm;
    }

    for(Gibran* gibran : gibrans){
        delete gibran;
    }

    for(MA* ma : mas){
        delete ma;
    }

    CloseWindow();
    return 0;
}
