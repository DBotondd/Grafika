#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include "camera.h"
#include "shader.h"
#include "model.h"
#include "fog.h"
#include "texture.h"
#include "stb_image.h"

#define SCR_WIDTH   800
#define SCR_HEIGHT  600
#define GROUND_EXTENT 100.0f
#define ARROW_LENGTH 5.0f
#define MAX_PARTICLES 200
#define SCORE_TO_WIN 5
#define ARC_SEGMENTS 64

const float BALL_RADIUS        = 0.5f;
const float GRAVITY            = 9.81f;
const float FRICTION           = 0.8f;
const float BOUND              = 10.0f;
const float CAM_SPEED          = 2.5f;
const float KICK_FORCE         = 10.0f;
const float GOAL_RESET_TIME    = 2.0f;
const float GOAL_TEXT_DURATION = 1.5f;
const float MISS_TEXT_DURATION = 1.5f;
const float WIN_TEXT_DURATION  = 2.0f;
const float KEEPER_SPEED       = 2.0f;
const float SPOT_Z             = -8.0f + (11.0f/16.5f)*5.0f + 2.0f;
const float boxFrontZ          = -8.0f + (16.5f/16.5f)*5.0f + 2.0f;




typedef struct { vec3 pos, vel; float life; } Particle;
typedef struct {
    Particle particles[MAX_PARTICLES];
    int count;
} ParticleSystem;

void ParticleSystem_Spawn(ParticleSystem* ps, vec3 cp, int N) {
    for(int i = 0; i < N && ps->count < MAX_PARTICLES; i++) {
        Particle *p = &ps->particles[ps->count++];
        glm_vec3_copy(cp, p->pos);
        p->vel[0] = ((rand()/(float)RAND_MAX)*2.0f - 1.0f)*0.5f;
        p->vel[1] = 1.0f + (rand()/(float)RAND_MAX)*0.5f;
        p->vel[2] = ((rand()/(float)RAND_MAX)*2.0f - 1.0f)*0.5f;
        p->life   = 1.0f;
    }
}
void ParticleSystem_Update(ParticleSystem* ps, float dt) {
    int dst = 0;
    for(int i = 0; i < ps->count; i++) {
        Particle *p = &ps->particles[i];
        p->life -= dt;
        if(p->life > 0.0f) {
            vec3 d;
            glm_vec3_scale(p->vel, dt, d);
            glm_vec3_add(p->pos, d, p->pos);
            ps->particles[dst++] = *p;
        }
    }
    ps->count = dst;
}
typedef struct {
    vec3 pos;
    vec3 vel;
    bool scored;
    bool missed;
    bool arrowVisible;
} Ball;
void Ball_Reset(Ball* b, float spot_z) {
    glm_vec3_copy((vec3){0, BALL_RADIUS, spot_z}, b->pos);
    glm_vec3_zero(b->vel);
    b->scored = false;
    b->missed = false;
    b->arrowVisible = true;
}
void Ball_Kick(Ball* b, float force, float yaw, float pitch) {
    float yR = glm_rad(yaw), pR = glm_rad(pitch);
    vec3 aimDir = {
        cosf(pR)*cosf(yR),
        sinf(pR),
        cosf(pR)*sinf(yR)
    };
    glm_vec3_normalize(aimDir);
    glm_vec3_scale(aimDir, force, b->vel);
    b->arrowVisible = false;
}
typedef struct {
    vec3 pos;
    float dir;
} Keeper;
void Keeper_Reset(Keeper* k) {
    glm_vec3_copy((vec3){0, 0, -7.5f}, k->pos);
    k->dir = 1.0f;
}
void Keeper_Move(Keeper* k, float dt) {
    k->pos[0] += k->dir * KEEPER_SPEED * dt;
    if(k->pos[0] > 2.0f){ k->pos[0] = 2.0f; k->dir = -1; }
    if(k->pos[0] < -2.0f){ k->pos[0] = -2.0f; k->dir = +1; }
}
typedef struct {
    Camera camera;
    Ball ball;
    Keeper keeper;
    ParticleSystem smoke;
    float aimYaw;
    float aimPitch;
    int playerScore, keeperScore;
    bool gameOver, showWin, showLose;
    float winLoseTimer;
    float goalTimer, missTimer;
    bool helpActive;
    float brightness;
} Game;
void Game_Reset(Game* g) {
    Ball_Reset(&g->ball, SPOT_Z);
    Keeper_Reset(&g->keeper);
    Camera_Init(&g->camera, (vec3){0,1,5}, (vec3){0,1,0}, CAMERA_YAW, CAMERA_PITCH);
    g->aimYaw = -90.0f;
    g->aimPitch = 0.0f;
    g->showWin = false;
    g->showLose = false;
    g->gameOver = false;
    g->playerScore = g->keeperScore = 0;
    g->winLoseTimer = 0.0f;
    g->goalTimer = 0.0f;
    g->missTimer = 0.0f;
    g->helpActive = false;
    
    g->smoke.count = 0;
    g->brightness = 1.0f;

}
void UpdateWindowTitle(GLFWwindow* window, int playerScore, int keeperScore) {
    char title[128];
    snprintf(title, sizeof(title), "3D Soccer – %d : %d", playerScore, keeperScore);
    glfwSetWindowTitle(window, title);
}

// VAO/VBO/TEXTÚRA GLOBÁLISOK
unsigned groundVAO, groundVBO;
unsigned helpVAO, helpVBO, helpTex;
unsigned goalTex, missTex, winTex, loseTex;
unsigned keeperVAO, keeperVBO;
unsigned netVAO, netVBO;
unsigned smokeVAO, smokeVBO;
unsigned lineVAO, lineVBO;
unsigned boxVAO, boxVBO;
unsigned arcVAO, arcVBO;
unsigned cubeVAO, cubeVBO;
unsigned spotVAO, spotVBO;
unsigned goalLineVAO, goalLineVBO;

// CALLBACKOK ÉS EGÉR
void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    (void)window;
    glViewport(0, 0, w, h);
}
float lastX = SCR_WIDTH/2.0f, lastY = SCR_HEIGHT/2.0f;
bool firstMouse = true;
Camera* globalCam = NULL;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    if(firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoff = (float)xpos - lastX;
    float yoff = lastY - (float)ypos;
    lastX = xpos;
    lastY = ypos;
    if (globalCam) Camera_ProcessMouseMovement(globalCam, xoff, yoff, true);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window; (void)xoffset;
    if (globalCam) Camera_ProcessMouseScroll(globalCam, (float)yoffset);
}


// NET VAO/VBO setup (kapuháló)
enum { NET_R = 10, NET_C = 20 };
int netCount;
void SetupNet() {
    netCount = (NET_R + 1 + NET_C + 1) * 2;
    float *net = malloc(netCount * 3 * sizeof(float));
    int idx = 0;
    for(int i = 0; i <= NET_R; i++){
        float y = i * (2.0f / NET_R);
        net[idx++] = -3; net[idx++] = y; net[idx++] = -8;
        net[idx++] =  3; net[idx++] = y; net[idx++] = -8;
    }
    for(int j = 0; j <= NET_C; j++){
        float x = -3 + j * (6.0f / NET_C);
        net[idx++] = x; net[idx++] = 0; net[idx++] = -8;
        net[idx++] = x; net[idx++] = 2; net[idx++] = -8;
    }
    glGenVertexArrays(1, &netVAO);
    glGenBuffers(1, &netVBO);
    glBindVertexArray(netVAO);
        glBindBuffer(GL_ARRAY_BUFFER, netVBO);
        glBufferData(GL_ARRAY_BUFFER, netCount * 3 * sizeof(float), net, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    free(net);
}

// BOX VAO/VBO setup
void SetupBox() {
    float boxVerts[] = {
        -3.0f, 0.01f, -8.0f,   3.0f, 0.01f, -8.0f,
         3.0f, 0.01f,  boxFrontZ,  -3.0f, 0.01f,  boxFrontZ
    };
    glGenVertexArrays(1, &boxVAO);
    glGenBuffers(1, &boxVBO);
    glBindVertexArray(boxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof boxVerts, boxVerts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// ARC (ív) VAO/VBO setup
void SetupArc() {
    float arcVerts[ARC_SEGMENTS * 3];
    float arcR    = (9.15f/16.5f) * 5.0f;
    float dz      = boxFrontZ - SPOT_Z;
    float phi0    = acosf(dz / arcR);
    for(int i = 0; i < ARC_SEGMENTS; i++){
        float t = -phi0 + 2 * phi0 * (i / (float)(ARC_SEGMENTS - 1));
        arcVerts[3*i + 0] =  sinf(t) * arcR;
        arcVerts[3*i + 1] =  0.01f;
        arcVerts[3*i + 2] =  SPOT_Z + cosf(t) * arcR;
    }
    glGenVertexArrays(1, &arcVAO);
    glGenBuffers(1, &arcVBO);
    glBindVertexArray(arcVAO);
        glBindBuffer(GL_ARRAY_BUFFER, arcVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof arcVerts, arcVerts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// GOAL LINE VAO/VBO
void SetupGoalLine() {
    float goalLineVerts[] = {
        -BOUND, 0.01f, -8.0f,
         BOUND, 0.01f, -8.0f
    };
    glGenVertexArrays(1, &goalLineVAO);
    glGenBuffers(1, &goalLineVBO);
    glBindVertexArray(goalLineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, goalLineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof goalLineVerts, goalLineVerts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// SPOT VAO/VBO (büntetőpont)
void SetupSpot() {
    float spotVert[] = { 0.0f, 0.01f, SPOT_Z };
    glGenVertexArrays(1, &spotVAO);
    glGenBuffers(1, &spotVBO);
    glBindVertexArray(spotVAO);
        glBindBuffer(GL_ARRAY_BUFFER, spotVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof spotVert, spotVert, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// CUBE VAO/VBO (kapufa)
void SetupCube() {
    static const float cubeMesh[] = {
        -0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f,-0.5f,-0.5f,
         0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f, -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
         0.5f,-0.5f,-0.5f,  0.5f, 0.5f, 0.5f,  0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,
         0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f, 0.5f,-0.5f, -0.5f, 0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f
    };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof cubeMesh, cubeMesh, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// ARROW (irány nyíl) VAO/VBO
void SetupArrow() {
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindVertexArray(lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, 2 * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// KEEPER VAO/VBO
void SetupKeeper() {
    float keeperVerts[] = {
        -0.5f, 0.0f,  0.0f, 0.0f,
         0.5f, 0.0f,  1.0f, 0.0f,
         0.5f, 1.2f,  1.0f, 1.0f,
        -0.5f, 1.2f,  0.0f, 1.0f
    };
    glGenVertexArrays(1, &keeperVAO);
    glGenBuffers(1, &keeperVBO);
    glBindVertexArray(keeperVAO);
        glBindBuffer(GL_ARRAY_BUFFER, keeperVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof keeperVerts, keeperVerts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

// UI QUAD VAO/VBO
void SetupUIQuad() {
    float uiVerts[] = {
        -1,  1,  0, 1,
         1,  1,  1, 1,
         1, -1,  1, 0,
        -1, -1,  0, 0
    };
    glGenVertexArrays(1, &helpVAO);
    glGenBuffers(1, &helpVBO);
    glBindVertexArray(helpVAO);
        glBindBuffer(GL_ARRAY_BUFFER, helpVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof uiVerts, uiVerts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

// SMOKE VAO/VBO
void SetupSmoke() {
    glGenVertexArrays(1, &smokeVAO);
    glGenBuffers(1, &smokeVBO);
    glBindVertexArray(smokeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, smokeVBO);
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(float), NULL, GL_STREAM_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// GROUND VAO/VBO
void SetupGround() {
    float groundVerts[] = {
        -GROUND_EXTENT, 0, -GROUND_EXTENT,
        -GROUND_EXTENT, 0,  GROUND_EXTENT,
         GROUND_EXTENT, 0,  GROUND_EXTENT,
         GROUND_EXTENT, 0, -GROUND_EXTENT
    };
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glBindVertexArray(groundVAO);
        glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof groundVerts, groundVerts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// --- A főfüggvényben a setupok
void SetupAllVAOs() {
    SetupGround();
    SetupUIQuad();
    SetupKeeper();
    SetupSmoke();
    SetupNet();
    SetupBox();
    SetupArc();
    SetupGoalLine();
    SetupSpot();
    SetupCube();
    SetupArrow();
}

void processInput(GLFWwindow* w, Game* game, float deltaTime);

int main(void) {
    srand((unsigned)time(NULL));

    if(!glfwInit()) return EXIT_FAILURE;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Soccer", NULL, NULL);
    if(!window){ glfwTerminate(); return EXIT_FAILURE; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) return EXIT_FAILURE;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glClearColor(0.4f, 0.6f, 0.9f, 1.0f);

    UpdateWindowTitle(window, 0, 0);

    // SHADEREK, TEXTÚRÁK, MODELL
    Shader sceneSh  = Shader_New("shaders/vertex.glsl",       "shaders/fragment.glsl");
    Shader groundSh = Shader_New("shaders/ground_vert.glsl",  "shaders/ground_frag.glsl");
    Shader uiSh     = Shader_New("shaders/ui_vert.glsl",      "shaders/ui_frag.glsl");
    Shader smokeSh  = Shader_New("shaders/smoke_vertex.glsl", "shaders/smoke_fragment.glsl");
    Shader lineSh   = Shader_New("shaders/line_vert.glsl",    "shaders/line_frag.glsl");
    Shader spriteSh = Shader_New("shaders/sprite_vert.glsl",  "shaders/sprite_frag.glsl");

    Shader_Use(&sceneSh);  Shader_SetInt(&sceneSh, "texture_diffuse1", 0);
    Shader_Use(&uiSh);     Shader_SetInt(&uiSh,    "uiTexture",       0);

    SetupAllVAOs();

    stbi_set_flip_vertically_on_load(true);
    helpTex   = Texture_FromFile("assets/help.png");
    goalTex   = Texture_FromFile("assets/goal.png");
    missTex   = Texture_FromFile("assets/miss.png");
    winTex    = Texture_FromFile("assets/win.png");
    loseTex   = Texture_FromFile("assets/lose.png");
    unsigned keeperTex   = Texture_FromFile("assets/keeper.png");
    unsigned stadiumTex  = Texture_FromFile("assets/stadium.png");
    unsigned ballTex = Texture_FromFile("assets/ball_diffuse.png");

    Model ballModel;
    if(!Model_Load(&ballModel, "assets/ball_uv.obj")) return EXIT_FAILURE;

    Fog fog; Fog_Init(&fog, 0.045f, 1.5f);

    Game game;
    Game_Reset(&game);
    globalCam = &game.camera;

    float lastFrame = 0.0f;

    

    // --- JÁTÉKCIKLUS ---
    while(!glfwWindowShouldClose(window)) {
        float now = (float)glfwGetTime();
        float deltaTime = now - lastFrame; lastFrame = now;

        // --- INPUT ---
        processInput(window, &game, deltaTime);

        // --- KAMERA KORLÁTOK ---
        game.camera.Position[0] = fmaxf(-BOUND, fminf(BOUND, game.camera.Position[0]));
        game.camera.Position[2] = fmaxf(-BOUND, fminf(BOUND, game.camera.Position[2]));
        game.camera.Position[1] = fmaxf(1.0f, game.camera.Position[1]);

        // --- GAME LOGIKA ---
        if(game.gameOver) {
            game.winLoseTimer -= deltaTime;
            if(game.winLoseTimer <= 0.0f) {
                game.playerScore = game.keeperScore = 0;
                game.showWin = game.showLose = game.gameOver = false;
                game.winLoseTimer = 0.0f;
                Ball_Reset(&game.ball, SPOT_Z);
                Camera_Init(&game.camera, (vec3){0,1,5}, (vec3){0,1,0}, CAMERA_YAW, CAMERA_PITCH);
                game.aimYaw = -90.0f;
                game.aimPitch = 0.0f;
                Keeper_Reset(&game.keeper);
                UpdateWindowTitle(window, game.playerScore, game.keeperScore);
            }
        } else {
            Keeper_Move(&game.keeper, deltaTime);

            // LABDARÚGÁS
            if(!game.ball.scored && game.missTimer <= 0.0f && game.goalTimer <= 0.0f &&
                glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                Ball_Kick(&game.ball, KICK_FORCE, game.aimYaw, game.aimPitch);
            }

            // GRAVITÁCIÓ, MOZGÁS
            game.ball.vel[1] -= GRAVITY * deltaTime;
            vec3 dv; glm_vec3_scale(game.ball.vel, deltaTime, dv);
            glm_vec3_add(game.ball.pos, dv, game.ball.pos);
            if(game.ball.pos[1] < BALL_RADIUS){
                game.ball.pos[1] = BALL_RADIUS;
                game.ball.vel[1] *= -0.5f;
                game.ball.vel[0] *= FRICTION; game.ball.vel[2] *= FRICTION;
            }
            game.ball.pos[0] = fmaxf(-BOUND, fminf(BOUND, game.ball.pos[0]));
            game.ball.pos[2] = fmaxf(-BOUND, fminf(BOUND, game.ball.pos[2]));

            ParticleSystem_Update(&game.smoke, deltaTime);

            // GÓL/MISS DETEKTÁLÁS 
            if(game.ball.pos[2] - BALL_RADIUS < game.keeper.pos[2] + 0.2f &&
               fabsf(game.ball.pos[0] - game.keeper.pos[0]) < 0.5f &&
               game.ball.pos[1] < 2.0f &&
               game.missTimer <= 0.0f && game.goalTimer <= 0.0f && !game.ball.scored)
            {
                game.missTimer = MISS_TEXT_DURATION;
                game.ball.missed = true;
                game.keeperScore++;
                printf("MISS!!! 😕 Kapus: %d Játékos: %d\n", game.keeperScore, game.playerScore);
                UpdateWindowTitle(window, game.playerScore, game.keeperScore);
                glm_vec3_zero(game.ball.vel);
            }
            else if(game.ball.pos[2] - BALL_RADIUS < -8.0f &&
                    (game.ball.pos[0] < -3.0f || game.ball.pos[0] > 3.0f) &&
                    game.missTimer <= 0.0f && game.goalTimer <= 0.0f && !game.ball.scored)
            {
                game.ball.missed = true;
                game.missTimer = MISS_TEXT_DURATION;
                game.keeperScore++;
                printf("MISS!!! 😕 Kapus: %d Játékos: %d\n", game.keeperScore, game.playerScore);
                UpdateWindowTitle(window, game.playerScore, game.keeperScore);
                glm_vec3_zero(game.ball.vel);
            }
            else if (!game.ball.scored && !game.ball.missed
                     && game.ball.pos[2] - BALL_RADIUS < -8.0f
                     && (game.ball.pos[0] <= -3.0f || game.ball.pos[0] >= 3.0f || game.ball.pos[1] >= 2.0f))
            {
                game.ball.missed = true;
                game.missTimer = MISS_TEXT_DURATION;
                game.keeperScore++;
                printf("MISS!!! 😢 Kapus: %d Játékos: %d\n", game.keeperScore, game.playerScore);
                UpdateWindowTitle(window, game.playerScore, game.keeperScore);
            }
            if (!game.ball.scored && !game.ball.missed
                && game.ball.pos[2] - BALL_RADIUS < -8.0f
                && game.ball.pos[0] > -3.0f && game.ball.pos[0] < 3.0f
                && game.ball.pos[1] < 2.0f)
            {
                game.ball.scored   = true;
                game.goalTimer = GOAL_TEXT_DURATION;
                ParticleSystem_Spawn(&game.smoke, game.ball.pos, 200);
                game.playerScore++;
                printf("GOAL!!! 🥳 Játékos: %d Kapus: %d\n", game.playerScore, game.keeperScore);
                UpdateWindowTitle(window, game.playerScore, game.keeperScore);
            }
            if(game.goalTimer > 0.0f) game.goalTimer -= deltaTime;
            if(game.missTimer > 0.0f) game.missTimer -= deltaTime;

            if((game.goalTimer <= 0.0f && game.ball.scored) ||
               (game.missTimer <= 0.0f && game.ball.missed))
            {
                if(game.playerScore >= SCORE_TO_WIN) {
                    game.showWin = true;
                    game.gameOver = true;
                    game.winLoseTimer = WIN_TEXT_DURATION;
                    printf("=== JÁTÉKOS NYERT! ===\n");
                } else if(game.keeperScore >= SCORE_TO_WIN) {
                    game.showLose = true;
                    game.gameOver = true;
                    game.winLoseTimer = WIN_TEXT_DURATION;
                    printf("=== KAPUS NYERT! ===\n");
                }
                if(!game.gameOver){
                    Ball_Reset(&game.ball, SPOT_Z);
                    Camera_Init(&game.camera, (vec3){0,1,5}, (vec3){0,1,0}, CAMERA_YAW, CAMERA_PITCH);
                    game.aimYaw = -90.0f;
                    game.aimPitch = 0.0f;
                }
            }
        }

        // --- RENDER ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. HÁTTÉR STADION UI
        glDisable(GL_DEPTH_TEST);
        Shader_Use(&uiSh);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stadiumTex);
        glBindVertexArray(helpVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glEnable(GL_DEPTH_TEST);

        // 2. KAMERA, PROJ/VIEW
        mat4 proj, view;
        int winWidth, winHeight;
        glfwGetFramebufferSize(window, &winWidth, &winHeight);
        glm_perspective(glm_rad(game.camera.Zoom), (float)winWidth / winHeight, 0.1f, 100.0f, proj);
        Camera_GetViewMatrix(&game.camera, view);

        // 3. TALAJ
        {
            mat4 M; glm_mat4_identity(M);
            Shader_Use(&groundSh);
            Shader_SetFloat(&groundSh, "brightness", game.brightness);
            Shader_SetMat4(&groundSh, "view", view);
            Shader_SetMat4(&groundSh, "projection", proj);
            Shader_SetMat4(&groundSh, "model", M);
            glBindVertexArray(groundVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }

        // 4. VONALAK, BOX, ÍV, PONT
        {
            Shader_Use(&lineSh);
            Shader_SetFloat(&lineSh, "brightness", game.brightness);
            Shader_SetVec3(&lineSh, "objectColor", (vec3){1,1,1});
            Shader_SetMat4(&lineSh, "view", view);
            Shader_SetMat4(&lineSh, "projection", proj);

            // Gólvonal
            glBindVertexArray(goalLineVAO);
            glDrawArrays(GL_LINES, 0, 2);

            // Doboz kontúr
            glBindVertexArray(boxVAO);
            glDrawArrays(GL_LINE_LOOP, 0, 4);

            // Pont a büntetőponthoz
            glPointSize(6.0f);
            glBindVertexArray(spotVAO);
            glDrawArrays(GL_POINTS, 0, 1);
            glPointSize(1.0f);

            // Ív
            glBindVertexArray(arcVAO);
            glDrawArrays(GL_LINE_STRIP, 0, ARC_SEGMENTS);

            glBindVertexArray(0);
        }

        // 5. LABDA
        {
            mat4 M; glm_mat4_identity(M);
            glm_translate(M, game.ball.pos);
            glm_scale(M, (vec3){BALL_RADIUS, BALL_RADIUS, BALL_RADIUS});
            Shader_Use(&sceneSh);
            Shader_SetFloat(&sceneSh, "brightness", game.brightness);
            Shader_SetInt(&sceneSh,   "useTexture",     1);
            Shader_SetInt(&sceneSh,   "texture_diffuse1", 0);
            Shader_SetMat4(&sceneSh, "model", M);
            Shader_SetMat4(&sceneSh, "view", view);
            Shader_SetMat4(&sceneSh, "projection", proj);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ballTex);
            Fog_Apply(&fog, &sceneSh);
            Model_Draw(&ballModel);
        }

        // 6. FÜST RÉSZECSEK
        {
            float *buf = malloc(game.smoke.count * 4 * sizeof(float));
            for(int i = 0; i < game.smoke.count; i++){
                buf[4*i + 0] = game.smoke.particles[i].pos[0];
                buf[4*i + 1] = game.smoke.particles[i].pos[1];
                buf[4*i + 2] = game.smoke.particles[i].pos[2];
                buf[4*i + 3] = game.smoke.particles[i].life;
            }
            glBindBuffer(GL_ARRAY_BUFFER, smokeVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, game.smoke.count * 4 * sizeof(float), buf);
            free(buf);
            Shader_Use(&smokeSh);
            Shader_SetFloat(&smokeSh, "brightness", game.brightness);
            Shader_SetMat4(&smokeSh, "view", view);
            Shader_SetMat4(&smokeSh, "projection", proj);
            glBindVertexArray(smokeVAO);
            glDrawArrays(GL_POINTS, 0, game.smoke.count);
        }

        // 7. KAPUFA - TARTÓOSZLOPOK, RÚD
        {
            Shader_Use(&sceneSh);
            mat4 M;

            // Bal oszlop
            glm_mat4_identity(M);
            glm_translate(M, (vec3){-3, 1, -8});
            glm_scale(M, (vec3){0.2f, 2.5f, 0.2f});
            Shader_SetMat4(&sceneSh, "model", M);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Jobb oszlop
            glm_mat4_identity(M);
            glm_translate(M, (vec3){3, 1, -8});
            glm_scale(M, (vec3){0.2f, 2.5f, 0.2f});
            Shader_SetMat4(&sceneSh, "model", M);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Vízszintes rúd
            glm_mat4_identity(M);
            glm_translate(M, (vec3){0, 2.25f, -8});
            glm_scale(M, (vec3){6.0f, 0.2f, 0.2f});
            Shader_SetMat4(&sceneSh, "model", M);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // 8. KAPUHÁLÓ SZÁLAK
        {
            Shader_Use(&lineSh);
            Shader_SetVec3(&lineSh, "objectColor", (vec3){1,1,1});
            Shader_SetMat4(&lineSh, "view", view);
            Shader_SetMat4(&lineSh, "projection", proj);
            glBindVertexArray(netVAO);
            glDrawArrays(GL_LINES, 0, netCount);
        }

        // 9. KAPUS SPRITE
        {
            mat4 M;
            glm_mat4_identity(M);
            glm_translate(M, game.keeper.pos);
            glm_scale(M, (vec3){1.0f, 1.8f, 1.0f});
            Shader_Use(&spriteSh);
            Shader_SetFloat(&spriteSh, "brightness", game.brightness);
            Shader_SetMat4(&spriteSh, "model",      M);
            Shader_SetMat4(&spriteSh, "view",       view);
            Shader_SetMat4(&spriteSh, "projection", proj);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, keeperTex);
            glBindVertexArray(keeperVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }

        // 10. NYÍL (célirányzó) DRAW
        if (game.ball.arrowVisible) {
            vec3 start, end;
            glm_vec3_copy(game.ball.pos, start);
            float yR = glm_rad(game.aimYaw), pR = glm_rad(game.aimPitch);
            end[0] = start[0] + cosf(pR)*cosf(yR) * ARROW_LENGTH;
            end[1] = start[1] + sinf(pR) * ARROW_LENGTH;
            end[2] = start[2] + cosf(pR)*sinf(yR) * ARROW_LENGTH;
            float verts[6] = {
                start[0], start[1], start[2],
                end[0],   end[1],   end[2]
            };
            glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof verts, verts);
            Shader_Use(&lineSh);
            Shader_SetVec3(&lineSh, "objectColor", (vec3){1,0,0});
            Shader_SetMat4(&lineSh, "view", view);
            Shader_SetMat4(&lineSh, "projection", proj);
            glBindVertexArray(lineVAO);
            glDrawArrays(GL_LINES, 0, 2);
        }

        // 11. UI, SEGÍTSÉG
        if(game.helpActive){
            glDisable(GL_DEPTH_TEST);
            Shader_Use(&uiSh);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, helpTex);
            glBindVertexArray(helpVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glEnable(GL_DEPTH_TEST);
        }

        // 12. GÓL/MISS OVERLAY
        if(game.goalTimer > 0.0f || game.missTimer > 0.0f){
            glDisable(GL_DEPTH_TEST);
            Shader_Use(&uiSh);
            glBindTexture(GL_TEXTURE_2D, game.goalTimer > 0.0f ? goalTex : missTex);
            glBindVertexArray(helpVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glEnable(GL_DEPTH_TEST);
        }

        // 13. WIN/LOSE OVERLAY
        if(game.showWin && game.gameOver && game.winLoseTimer > 0.0f){
            glDisable(GL_DEPTH_TEST);
            Shader_Use(&uiSh);
            glBindTexture(GL_TEXTURE_2D, winTex);
            glBindVertexArray(helpVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glEnable(GL_DEPTH_TEST);
        }
        if(game.showLose && game.gameOver && game.winLoseTimer > 0.0f){
            glDisable(GL_DEPTH_TEST);
            Shader_Use(&uiSh);
            glBindTexture(GL_TEXTURE_2D, loseTex);
            glBindVertexArray(helpVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glEnable(GL_DEPTH_TEST);
        }

        // 14. Konzol HUD
        printf("\rJátékos: %d    Kapus: %d         ", game.playerScore, game.keeperScore);
        fflush(stdout);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- CLEANUP ---
    Model_Cleanup(&ballModel);
    Shader_Delete(&sceneSh);
    Shader_Delete(&groundSh);
    Shader_Delete(&uiSh);
    Shader_Delete(&smokeSh);
    Shader_Delete(&lineSh);
    Shader_Delete(&spriteSh);

    glDeleteVertexArrays(1, &groundVAO);  glDeleteBuffers(1, &groundVBO);
    glDeleteVertexArrays(1, &netVAO);     glDeleteBuffers(1, &netVBO);
    glDeleteVertexArrays(1, &cubeVAO);    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &smokeVAO);   glDeleteBuffers(1, &smokeVBO);
    glDeleteVertexArrays(1, &lineVAO);    glDeleteBuffers(1, &lineVBO);
    glDeleteVertexArrays(1, &helpVAO);    glDeleteBuffers(1, &helpVBO);
    glDeleteVertexArrays(1, &keeperVAO);  glDeleteBuffers(1, &keeperVBO);
    glDeleteVertexArrays(1, &goalLineVAO); glDeleteBuffers(1, &goalLineVBO);
    glDeleteVertexArrays(1, &spotVAO);    glDeleteBuffers(1, &spotVBO);
    glDeleteVertexArrays(1, &boxVAO);     glDeleteBuffers(1, &boxVBO);
    glDeleteVertexArrays(1, &arcVAO);     glDeleteBuffers(1, &arcVBO);

    glfwTerminate();
    return EXIT_SUCCESS;
}

void processInput(GLFWwindow* w, Game* game, float deltaTime)
{
    // ESC -> kilépés
    if(glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(w, true);

    // Kamera WASD
    Camera_ProcessKeyboard(&game->camera, CAMERA_MV_FORWARD,
        deltaTime * ((glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) ? CAM_SPEED : 1.0f));
    Camera_ProcessKeyboard(&game->camera, CAMERA_MV_BACKWARD,
        deltaTime * ((glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) ? CAM_SPEED : 1.0f));
    Camera_ProcessKeyboard(&game->camera, CAMERA_MV_LEFT,
        deltaTime * ((glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) ? CAM_SPEED : 1.0f));
    Camera_ProcessKeyboard(&game->camera, CAMERA_MV_RIGHT,
        deltaTime * ((glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) ? CAM_SPEED : 1.0f));

    // Célirányzó billentyűk
    float spd = 60.0f * deltaTime;
    if(glfwGetKey(w, GLFW_KEY_LEFT)  == GLFW_PRESS)  game->aimYaw   -= spd;
    if(glfwGetKey(w, GLFW_KEY_RIGHT) == GLFW_PRESS)  game->aimYaw   += spd;
    if(glfwGetKey(w, GLFW_KEY_UP)    == GLFW_PRESS)  game->aimPitch  = fminf(89.0f,  game->aimPitch + spd);
    if(glfwGetKey(w, GLFW_KEY_DOWN)  == GLFW_PRESS)  game->aimPitch  = fmaxf(-89.0f, game->aimPitch - spd);

    
    // Fényerő (brightness)
    if(glfwGetKey(w, GLFW_KEY_KP_ADD) == GLFW_PRESS || glfwGetKey(w, GLFW_KEY_EQUAL) == GLFW_PRESS)
    {
        game->brightness = fminf(2.0f, game->brightness + 0.05f);
        printf("Brightness: %.0f%%\n", 100 * game->brightness);
    }
    if(glfwGetKey(w, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS || glfwGetKey(w, GLFW_KEY_MINUS) == GLFW_PRESS)
    {
        game->brightness = fmaxf(0.1f, game->brightness - 0.05f);
        printf("Brightness: %.0f%%\n", 100 * game->brightness);
    }


    // Segítség overlay váltó
    static bool f1last = false;
    bool f1now = (glfwGetKey(w, GLFW_KEY_F1) == GLFW_PRESS);
    if(f1now && !f1last) game->helpActive = !game->helpActive;
    f1last = f1now;

    // Kamera forgatás J/K
    float rotSpeed = 60.0f * deltaTime;
    if(glfwGetKey(w, GLFW_KEY_J) == GLFW_PRESS) {
        Camera_ProcessMouseMovement(&game->camera, -rotSpeed, 0.0f, true);
    }
    if(glfwGetKey(w, GLFW_KEY_K) == GLFW_PRESS) {
        Camera_ProcessMouseMovement(&game->camera,  rotSpeed, 0.0f, true);
    }

    // Reset gomb (X)
    static bool x_was_down = false;
    bool x_is_down = (glfwGetKey(w, GLFW_KEY_X) == GLFW_PRESS);
    if (x_is_down && !x_was_down) {
        Game_Reset(game);
    }
    x_was_down = x_is_down;

    // FULLSCREEN váltás F11-re
    static bool fullscreen_now = false;
    static int windowed_x = 100, windowed_y = 100, windowed_width = SCR_WIDTH, windowed_height = SCR_HEIGHT;
    static bool f11_last = false;
    bool f11_now = (glfwGetKey(w, GLFW_KEY_F11) == GLFW_PRESS);
    if (f11_now && !f11_last) {
        if (!fullscreen_now) {
            fullscreen_now = true;
            glfwGetWindowPos(w, &windowed_x, &windowed_y);
            glfwGetWindowSize(w, &windowed_width, &windowed_height);
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(w, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            fullscreen_now = false;
            glfwSetWindowMonitor(w, NULL, windowed_x, windowed_y, windowed_width, windowed_height, 0);
        }
        
        int width, height;
        glfwGetFramebufferSize(w, &width, &height);
        glViewport(0, 0, width, height);
    }
    f11_last = f11_now;
}
