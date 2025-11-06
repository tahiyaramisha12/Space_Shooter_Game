#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

#define SZ 100000

using namespace std;

GLuint backgroundTexture;
GLuint menuTexture;
int backgroundWidth, backgroundHeight;
unsigned char* backgroundImage = NULL;

// Music variables
bool menuMusicPlaying = false;
bool gameMusicPlaying = false;
bool musicEnabled = true;

double shooter_x = 0.0f;
double shooter_y = -1.9f;
double shooter_min_x = -1.9f;
double shooter_max_x = 1.9f;

double bullet_x[SZ];
double bullet_y[SZ];
double bullet_vel = 0.05f;
int bullet_stat[SZ] = {0};
double bullet_vel_x[SZ];
double bullet_vel_y[SZ];
double tip_x[SZ];
double tip_y[SZ];
int bullet = 0;
int on = 1000;
int high = -1;

double enemy_x[SZ];
double enemy_y[SZ];
double enemy_vel[SZ];
double enemy_stat[SZ] = {0};
int enemy_type[SZ];
// NEW: Enemy movement variables
double enemy_move_dir[SZ] = {0}; // -1 for left, 1 for right, 0 for down only
double enemy_move_speed[SZ] = {0}; // Horizontal movement speed
int frontCount = -1, cnt = 0;

// Explosion variables
double explosion_x[SZ];
double explosion_y[SZ];
double explosion_radius[SZ];
int explosion_active[SZ] = {0};
int explosion_lifetime[SZ] = {0};
const int EXPLOSION_MAX_LIFETIME = 18;

// NEW: Gift variables
double gift_x[SZ];
double gift_y[SZ];
double gift_vel[SZ];
int gift_stat[SZ] = {0};
int gift_type[SZ]; // 0: weapon power, 1: extra life, 2: slow motion
int gift_count = 0;
bool gift_active = false;

// Weapon power system
int weapon_power = 1; // 1: single shot, 2: double shot, 3: triple shot
int weapon_timer = 0;
const int WEAPON_POWER_DURATION = 1000; // frames

// Slow motion system
bool slow_motion_active = false;
int slow_motion_timer = 0;
const int SLOW_MOTION_DURATION = 800; // frames
float slow_motion_factor = 0.5f; // 50% speed reduction

int points = 0;
int game_over = 0;
int game_play = 0;
int mainmenu = 1;
int highscore_menu = 0;
int howtoplay = 0;
int life = 5;

int highScore = 0;

int level = 1;
int pointsForNextLevel = 10;

double mousex;
double mousey;

// Function to play menu background music
void playMenuMusic() {
    if (musicEnabled && !menuMusicPlaying) {
        if (gameMusicPlaying) {
            PlaySound(NULL, NULL, 0);
            gameMusicPlaying = false;
        }

        bool success = PlaySound("E:\\L-3 S-1\\Computer graphics\\Lab work\\project opengl\\space_shooter\\background_music.wav",
                               NULL, SND_ASYNC | SND_LOOP | SND_FILENAME);

        if (!success) {
            success = PlaySound("background_music.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME);
            if (!success) {
                PlaySound(NULL, NULL, 0);
                MessageBeep(MB_ICONASTERISK);
            }
        }

        menuMusicPlaying = true;
        gameMusicPlaying = false;
    }
}

// Function to play game background music (g.wav)
void playGameMusic() {
    if (musicEnabled && !gameMusicPlaying) {
        if (menuMusicPlaying) {
            PlaySound(NULL, NULL, 0);
            menuMusicPlaying = false;
        }

        bool success = PlaySound("E:\\L-3 S-1\\Computer graphics\\Lab work\\project opengl\\space_shooter\\g.wav",
                               NULL, SND_ASYNC | SND_LOOP | SND_FILENAME);

        if (!success) {
            success = PlaySound("g.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME);
            if (!success) {
                PlaySound(NULL, NULL, 0);
                MessageBeep(MB_ICONASTERISK);
            }
        }

        gameMusicPlaying = true;
        menuMusicPlaying = false;
    }
}

// Function to stop all background music
void stopAllMusic() {
    PlaySound(NULL, NULL, 0);
    menuMusicPlaying = false;
    gameMusicPlaying = false;
}

// Function to toggle music on/off
void toggleMusic() {
    musicEnabled = !musicEnabled;
    if (musicEnabled) {
        if (mainmenu == 1) {
            playMenuMusic();
        } else if (game_play == 1) {
            playGameMusic();
        }
    } else {
        stopAllMusic();
    }
}

// NEW: Function to play death sound when enemy reaches bottom
void playDeathSound() {
    if (musicEnabled) {
        bool success = PlaySound("E:\\L-3 S-1\\Computer graphics\\Lab work\\project opengl\\space_shooter\\death.wav",
                               NULL, SND_ASYNC | SND_FILENAME);

        if (!success) {
            success = PlaySound("death.wav", NULL, SND_ASYNC | SND_FILENAME);
            if (!success) {
                PlaySound(NULL, NULL, 0);
                MessageBeep(MB_ICONASTERISK);
            }
        }
    }
}

// BMP loading function
GLuint loadBMPTexture(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Could not open BMP file %s\n", filename);
        return 0;
    }

    unsigned char header[54];
    fread(header, 1, 54, file);

    if (header[0] != 'B' || header[1] != 'M') {
        printf("Error: Not a valid BMP file\n");
        fclose(file);
        return 0;
    }

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int dataPos = *(int*)&header[10];
    int imageSize = *(int*)&header[34];

    printf("BMP Info: %dx%d, DataPos: %d, ImageSize: %d\n", width, height, dataPos, imageSize);

    unsigned char* data = (unsigned char*)malloc(imageSize);
    fseek(file, dataPos, SEEK_SET);
    fread(data, 1, imageSize, file);
    fclose(file);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    unsigned char* rgbData = (unsigned char*)malloc(width * height * 3);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int srcIndex = (y * width + x) * 3;
            int dstIndex = ((height - 1 - y) * width + x) * 3;

            rgbData[dstIndex] = data[srcIndex + 2];
            rgbData[dstIndex + 1] = data[srcIndex + 1];
            rgbData[dstIndex + 2] = data[srcIndex];
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbData);

    free(data);
    free(rgbData);

    printf("BMP texture loaded successfully: %dx%d\n", width, height);
    return textureID;
}

void loadMenuBackground() {
    menuTexture = loadBMPTexture("E:\\L-3 S-1\\Computer graphics\\Lab work\\project opengl\\space_shooter\\s1.bmp");

    if (menuTexture == 0) {
        printf("Creating fallback background...\n");

        const int width = 1024;
        const int height = 1024;
        unsigned char* menuImage = (unsigned char*)malloc(width * height * 3);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int idx = (y * width + x) * 3;
                float fx = (float)x / width;
                float fy = (float)y / height;

                menuImage[idx] = 20 + (int)(50 * sin(fx * 10));
                menuImage[idx+1] = 10 + (int)(30 * cos(fy * 8));
                menuImage[idx+2] = 80 + (int)(60 * sin(fx * 6));
            }
        }

        for (int i = 0; i < 1000; i++) {
            int x = rand() % width;
            int y = rand() % height;
            int pixel = (y * width + x) * 3;
            int brightness = 150 + rand() % 106;
            menuImage[pixel] = brightness;
            menuImage[pixel+1] = brightness;
            menuImage[pixel+2] = brightness;
        }

        glGenTextures(1, &menuTexture);
        glBindTexture(GL_TEXTURE_2D, menuTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, menuImage);

        free(menuImage);
    }
}

void drawMenuBackground() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, menuTexture);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float screenWidth = viewport[2];
    float screenHeight = viewport[3];

    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, screenWidth, 0, screenHeight);

    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_DEPTH_TEST);

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(screenWidth, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(screenWidth, screenHeight);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, screenHeight);
    glEnd();

    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_TEXTURE_2D);
}

void generateSpaceBackground() {
    backgroundWidth = 512;
    backgroundHeight = 512;

    if (backgroundImage) free(backgroundImage);
    backgroundImage = (unsigned char*)malloc(backgroundWidth * backgroundHeight * 3);

    for (int i = 0; i < backgroundWidth * backgroundHeight * 3; i += 3) {
        backgroundImage[i] = 10;
        backgroundImage[i+1] = 15;
        backgroundImage[i+2] = 40;
    }

    for (int i = 0; i < 500; i++) {
        int x = rand() % backgroundWidth;
        int y = rand() % backgroundHeight;
        int pixel = (y * backgroundWidth + x) * 3;

        int brightness = 150 + rand() % 106;
        backgroundImage[pixel] = brightness;
        backgroundImage[pixel+1] = brightness;
        backgroundImage[pixel+2] = brightness;
    }
}

void loadBackgroundTexture() {
    generateSpaceBackground();

    glGenTextures(1, &backgroundTexture);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, backgroundWidth, backgroundHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, backgroundImage);
}

void drawSpaceBackground() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float aspectRatio = (float)viewport[2] / (float)viewport[3];

    float right = aspectRatio * 3.0f;
    float left = -right;
    float top = 3.0f;
    float bottom = -3.0f;

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(left, bottom, -5.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(right, bottom, -5.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(right, top, -5.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(left, top, -5.5f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void loadHighScore() {
    FILE *file = fopen("highscore.txt", "r");
    if(file != NULL) {
        fscanf(file, "%d", &highScore);
        fclose(file);
    }
    else {
        highScore = 0;
    }
}

void saveHighScore() {
    FILE *file = fopen("highscore.txt", "w");
    if(file != NULL) {
        fprintf(file, "%d", highScore);
        fclose(file);
    }
}

void renderBitmapString(float x, float y, void *font,const char *string){
    const char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void MainMenu(){
    drawMenuBackground();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-3, 3, -2, 2);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    glLineWidth(4);
    glColor3f(1.0,1.0,1.0);

    renderBitmapString(-1.7f, 1.5f, GLUT_BITMAP_TIMES_ROMAN_24, "SPACE SHOOTER GAME");

    glBegin(GL_LINES);
        glVertex2f(0.3f, 0.7f);
        glVertex2f(0.3f, 1.0f);
        glVertex2f(0.3f, 0.7f);
        glVertex2f(1.5f, 0.7f);
        glVertex2f(0.3f, 1.0f);
        glVertex2f(1.5f, 1.0f);
        glVertex2f(1.5f, 0.7f);
        glVertex2f(1.5f, 1.0f);
    glEnd();

    renderBitmapString(0.5f, 0.8f, GLUT_BITMAP_TIMES_ROMAN_24, "START GAME");

    glBegin(GL_LINES);
        glVertex2f(0.3f, 0.3f);
        glVertex2f(0.3f, 0.6f);
        glVertex2f(0.3f, 0.3f);
        glVertex2f(1.5f, 0.3f);
        glVertex2f(0.3f, 0.6f);
        glVertex2f(1.5f, 0.6f);
        glVertex2f(1.5f, 0.3f);
        glVertex2f(1.5f, 0.6f);
    glEnd();

    renderBitmapString(0.5f, 0.45f, GLUT_BITMAP_TIMES_ROMAN_24, "INSTRUCTIONS");

    glBegin(GL_LINES);
        glVertex2f(0.3f, -0.1f);
        glVertex2f(0.3f, 0.2f);
        glVertex2f(0.3f, -0.1f);
        glVertex2f(1.5f, -0.1f);
        glVertex2f(0.3f, 0.2f);
        glVertex2f(1.5f, 0.2f);
        glVertex2f(1.5f, -0.1f);
        glVertex2f(1.5f, 0.2f);
    glEnd();

    renderBitmapString(0.5f, 0.05f, GLUT_BITMAP_TIMES_ROMAN_24, "HIGH SCORE");

    glBegin(GL_LINES);
        glVertex2f(0.3f, -0.5f);
        glVertex2f(0.3f, -0.2f);
        glVertex2f(0.3f, -0.5f);
        glVertex2f(1.5f, -0.5f);
        glVertex2f(0.3f, -0.2f);
        glVertex2f(1.5f, -0.2f);
        glVertex2f(1.5f, -0.5f);
        glVertex2f(1.5f, -0.2f);
    glEnd();

    renderBitmapString(0.5f, -0.35f, GLUT_BITMAP_TIMES_ROMAN_24, "EXIT");

    glBegin(GL_LINES);
        glVertex2f(-1.5f, -1.5f);
        glVertex2f(-1.5f, -1.2f);
        glVertex2f(-1.5f, -1.5f);
        glVertex2f(-0.3f, -1.5f);
        glVertex2f(-1.5f, -1.2f);
        glVertex2f(-0.3f, -1.2f);
        glVertex2f(-0.3f, -1.5f);
        glVertex2f(-0.3f, -1.2f);
    glEnd();

    if (musicEnabled) {
        renderBitmapString(-1.4f, -1.35f, GLUT_BITMAP_TIMES_ROMAN_24, "MUSIC: ON");
    } else {
        renderBitmapString(-1.4f, -1.35f, GLUT_BITMAP_TIMES_ROMAN_24, "MUSIC: OFF");
    }

    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// UPDATED: Enhanced instruction screen with better layout
void instruction(){
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-3, 3, -2, 2);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    // Modern background overlay
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.1f, 0.2f, 0.85f);
    glBegin(GL_QUADS);
        glVertex2f(-2.8f, -1.8f);
        glVertex2f(2.8f, -1.8f);
        glVertex2f(2.8f, 1.8f);
        glVertex2f(-2.8f, 1.8f);
    glEnd();
    glDisable(GL_BLEND);

    // Modern title with gradient effect
    glColor3f(0.2f, 0.8f, 1.0f);
    renderBitmapString(-0.4f, 1.6f, GLUT_BITMAP_TIMES_ROMAN_24, "HOW TO PLAY");

    // Decorative line under title
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glColor3f(0.2f, 0.8f, 1.0f);
        glVertex2f(-1.0f, 1.5f);
        glVertex2f(1.0f, 1.5f);
    glEnd();

    // Instructions container with border
    glColor3f(0.2f, 0.3f, 0.4f);
    glBegin(GL_QUADS);
        glVertex2f(-2.7f, -1.4f);
        glVertex2f(2.7f, -1.4f);
        glVertex2f(2.7f, 1.3f);
        glVertex2f(-2.7f, 1.3f);
    glEnd();

    // Container border
    glLineWidth(2.0f);
    glColor3f(0.4f, 0.6f, 0.8f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-2.7f, -1.4f);
        glVertex2f(2.7f, -1.4f);
        glVertex2f(2.7f, 1.3f);
        glVertex2f(-2.7f, 1.3f);
    glEnd();

    // Instructions with better formatting - TWO COLUMN LAYOUT
    float leftCol = -2.5f;
    float rightCol = 0.0f;
    float startY = 1.1f;
    float lineHeight = -0.15f;

    // LEFT COLUMN: Controls & Basic Game Rules
    glColor3f(0.9f, 0.9f, 1.0f);
    renderBitmapString(leftCol, startY, GLUT_BITMAP_HELVETICA_18, "CONTROLS");
    glColor3f(1.0f, 1.0f, 1.0f);

    // Control instructions
    glColor3f(0.8f, 1.0f, 0.8f);
    renderBitmapString(leftCol, startY + lineHeight, GLUT_BITMAP_HELVETICA_18, "\u2190 \u2192");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(leftCol + 0.3f, startY + lineHeight, GLUT_BITMAP_HELVETICA_18, "Arrow Keys - Move");

    glColor3f(0.8f, 1.0f, 0.8f);
    renderBitmapString(leftCol, startY + 2*lineHeight, GLUT_BITMAP_HELVETICA_18, "SPACE");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(leftCol + 0.3f, startY + 2*lineHeight, GLUT_BITMAP_HELVETICA_18, "Fire Bullets");

    glColor3f(0.8f, 1.0f, 0.8f);
    renderBitmapString(leftCol, startY + 3*lineHeight, GLUT_BITMAP_HELVETICA_18, "M");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(leftCol + 0.3f, startY + 3*lineHeight, GLUT_BITMAP_HELVETICA_18, "Toggle Music");

    glColor3f(0.8f, 1.0f, 0.8f);
    renderBitmapString(leftCol, startY + 4*lineHeight, GLUT_BITMAP_HELVETICA_18, "ESC");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(leftCol + 0.3f, startY + 4*lineHeight, GLUT_BITMAP_HELVETICA_18, "Exit Game");

    // Game rules section
    glColor3f(0.9f, 0.9f, 1.0f);
    renderBitmapString(leftCol, startY + 6*lineHeight, GLUT_BITMAP_HELVETICA_18, "GAME RULES");
    glColor3f(1.0f, 1.0f, 1.0f);

    glColor3f(1.0f, 0.8f, 0.8f);
    renderBitmapString(leftCol, startY + 7*lineHeight, GLUT_BITMAP_HELVETICA_18, "\u2665");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(leftCol + 0.3f, startY + 7*lineHeight, GLUT_BITMAP_HELVETICA_18, "5 Lives - Start with");

    glColor3f(1.0f, 0.8f, 0.8f);
    renderBitmapString(leftCol, startY + 8*lineHeight, GLUT_BITMAP_HELVETICA_18, "\u2605");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(leftCol + 0.3f, startY + 8*lineHeight, GLUT_BITMAP_HELVETICA_18, "1 Point per enemy");

    glColor3f(1.0f, 0.8f, 0.8f);
    renderBitmapString(leftCol, startY + 9*lineHeight, GLUT_BITMAP_HELVETICA_18, "\u2191");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(leftCol + 0.3f, startY + 9*lineHeight, GLUT_BITMAP_HELVETICA_18, "Level up every 10 pts");

    // RIGHT COLUMN: Power-ups & Weapon System
    glColor3f(0.9f, 0.9f, 1.0f);
    renderBitmapString(rightCol, startY, GLUT_BITMAP_HELVETICA_18, "POWER-UPS");
    glColor3f(1.0f, 1.0f, 1.0f);

    glColor3f(0.2f, 0.4f, 1.0f);
    renderBitmapString(rightCol, startY + lineHeight, GLUT_BITMAP_HELVETICA_18, "\u25A0");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(rightCol + 0.3f, startY + lineHeight, GLUT_BITMAP_HELVETICA_18, "Blue - Weapon Power");

    glColor3f(1.0f, 0.2f, 0.2f);
    renderBitmapString(rightCol, startY + 2*lineHeight, GLUT_BITMAP_HELVETICA_18, "\u25A0");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(rightCol + 0.3f, startY + 2*lineHeight, GLUT_BITMAP_HELVETICA_18, "Red - Extra Life");

    glColor3f(0.2f, 1.0f, 0.2f);
    renderBitmapString(rightCol, startY + 3*lineHeight, GLUT_BITMAP_HELVETICA_18, "\u25A0");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(rightCol + 0.3f, startY + 3*lineHeight, GLUT_BITMAP_HELVETICA_18, "Green - Slow Motion");

    // Weapon System
    glColor3f(0.9f, 0.9f, 1.0f);
    renderBitmapString(rightCol, startY + 5*lineHeight, GLUT_BITMAP_HELVETICA_18, "WEAPON SYSTEM");
    glColor3f(1.0f, 1.0f, 1.0f);

    glColor3f(0.8f, 0.8f, 1.0f);
    renderBitmapString(rightCol, startY + 6*lineHeight, GLUT_BITMAP_HELVETICA_18, "1.");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(rightCol + 0.3f, startY + 6*lineHeight, GLUT_BITMAP_HELVETICA_18, "Single Shot - Default");

    glColor3f(0.8f, 0.8f, 1.0f);
    renderBitmapString(rightCol, startY + 7*lineHeight, GLUT_BITMAP_HELVETICA_18, "2.");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(rightCol + 0.3f, startY + 7*lineHeight, GLUT_BITMAP_HELVETICA_18, "Double Shot - 2 bullets");

    glColor3f(0.8f, 0.8f, 1.0f);
    renderBitmapString(rightCol, startY + 8*lineHeight, GLUT_BITMAP_HELVETICA_18, "3.");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(rightCol + 0.3f, startY + 8*lineHeight, GLUT_BITMAP_HELVETICA_18, "Triple Shot - 3 bullets");

    // MOVED PRO STRATEGIES SECTION UP - Now in the middle section
    glColor3f(0.9f, 0.9f, 1.0f);
    renderBitmapString(-1.3f, startY + 11*lineHeight, GLUT_BITMAP_HELVETICA_18, "PRO STRATEGIES");
    glColor3f(1.0f, 1.0f, 1.0f);

    glColor3f(1.0f, 1.0f, 0.8f);
    renderBitmapString(-2.5f, startY + 12*lineHeight, GLUT_BITMAP_HELVETICA_18, "\u2728");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(-2.2f, startY + 12*lineHeight, GLUT_BITMAP_HELVETICA_18, "Prioritize weapon power gifts");

    glColor3f(1.0f, 1.0f, 0.8f);
    renderBitmapString(-2.5f, startY + 13*lineHeight, GLUT_BITMAP_HELVETICA_18, "\u269B");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(-2.2f, startY + 13*lineHeight, GLUT_BITMAP_HELVETICA_18, "Use slow motion for tough waves");

    glColor3f(1.0f, 1.0f, 0.8f);
    renderBitmapString(0.0f, startY + 12*lineHeight, GLUT_BITMAP_HELVETICA_18, "\u23F1");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(0.3f, startY + 12*lineHeight, GLUT_BITMAP_HELVETICA_18, "Time shots for moving enemies");

    glColor3f(1.0f, 1.0f, 0.8f);
    renderBitmapString(0.0f, startY + 13*lineHeight, GLUT_BITMAP_HELVETICA_18, "\u2191");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(0.3f, startY + 13*lineHeight, GLUT_BITMAP_HELVETICA_18, "Higher levels = more gifts");

    // Enemy Movement Info (moved down to make space)
    glColor3f(0.9f, 0.9f, 1.0f);
    renderBitmapString(-1.3f, startY + 14*lineHeight, GLUT_BITMAP_HELVETICA_18, "ENEMY MOVEMENT");
    glColor3f(1.0f, 1.0f, 1.0f);

    glColor3f(1.0f, 1.0f, 0.8f);
    renderBitmapString(-2.5f, startY + 16*lineHeight, GLUT_BITMAP_HELVETICA_18, "\u2193");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(-2.2f, startY + 15*lineHeight, GLUT_BITMAP_HELVETICA_18, "Level 1 - Straight down");

    glColor3f(1.0f, 1.0f, 0.8f);
    renderBitmapString(-2.5f, startY + 17*lineHeight, GLUT_BITMAP_HELVETICA_18, "\u2194");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(-2.2f, startY + 16*lineHeight, GLUT_BITMAP_HELVETICA_18, "Level 2+ - Left/Right movement");

    glColor3f(1.0f, 1.0f, 0.8f);
    renderBitmapString(0.0f, startY + 16*lineHeight, GLUT_BITMAP_HELVETICA_18, "\u26A0");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(0.3f, startY + 15*lineHeight, GLUT_BITMAP_HELVETICA_18, "Higher levels = faster");

    glColor3f(1.0f, 1.0f, 0.8f);
    renderBitmapString(0.0f, startY + 17*lineHeight, GLUT_BITMAP_HELVETICA_18, "\u2728");
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(0.3f, startY + 16*lineHeight, GLUT_BITMAP_HELVETICA_18, "10+ unique enemy designs");

    // Back button - modern rectangular design
    glColor3f(0.3f, 0.5f, 0.8f);
    glBegin(GL_QUADS);
        glVertex2f(-0.9f, -1.7f);
        glVertex2f(0.9f, -1.7f);
        glVertex2f(0.9f, -1.5f);
        glVertex2f(-0.9f, -1.5f);
    glEnd();

    // Back button border
    glLineWidth(2.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.9f, -1.7f);
        glVertex2f(0.9f, -1.7f);
        glVertex2f(0.9f, -1.5f);
        glVertex2f(-0.9f, -1.5f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(-0.2f, -1.62f, GLUT_BITMAP_HELVETICA_18, "BACK TO MENU");

    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void highScoreMenu(){
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-3, 3, -2, 2);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.1f, 0.2f, 0.85f);
    glBegin(GL_QUADS);
        glVertex2f(-2.8f, -1.8f);
        glVertex2f(2.8f, -1.8f);
        glVertex2f(2.8f, 1.8f);
        glVertex2f(-2.8f, 1.8f);
    glEnd();
    glDisable(GL_BLEND);

    glColor3f(0.2f, 0.8f, 1.0f);
    renderBitmapString(-0.3f, 1.5f, GLUT_BITMAP_TIMES_ROMAN_24, "HIGH SCORES");

    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glColor3f(0.2f, 0.8f, 1.0f);
        glVertex2f(-1.0f, 1.4f);
        glVertex2f(1.0f, 1.4f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(-1.2f, 1.0f, GLUT_BITMAP_HELVETICA_18, "BEST SCORE:");

    char str[80];
    sprintf(str,"%d", highScore);

    glColor3f(1.0f, 0.8f, 0.2f);
    glRasterPos2f(-0.3f, 1.0f);
    for(int i=0; i<(int)(strlen(str)); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);

    if(highScore >= 100) {
        glColor3f(1.0f, 0.8f, 0.0f);
        renderBitmapString(0.8f, 1.0f, GLUT_BITMAP_HELVETICA_18, "ELITE");
    } else if(highScore >= 50) {
        glColor3f(0.7f, 0.7f, 0.9f);
        renderBitmapString(0.8f, 1.0f, GLUT_BITMAP_HELVETICA_18, "PRO");
    } else if(highScore >= 25) {
        glColor3f(0.9f, 0.6f, 0.2f);
        renderBitmapString(0.8f, 1.0f, GLUT_BITMAP_HELVETICA_18, "VETERAN");
    }

    glColor3f(0.9f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
        glVertex2f(-0.9f, 0.5f);
        glVertex2f(0.9f, 0.5f);
        glVertex2f(0.9f, 0.7f);
        glVertex2f(-0.9f, 0.7f);
    glEnd();

    glLineWidth(2.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.9f, 0.5f);
        glVertex2f(0.9f, 0.5f);
        glVertex2f(0.9f, 0.7f);
        glVertex2f(-0.9f, 0.7f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(-0.25f, 0.58f, GLUT_BITMAP_HELVETICA_18, "RESET SCORE");

    glColor3f(0.3f, 0.5f, 0.8f);
    glBegin(GL_QUADS);
        glVertex2f(-0.9f, -1.0f);
        glVertex2f(0.9f, -1.0f);
        glVertex2f(0.9f, -0.8f);
        glVertex2f(-0.9f, -0.8f);
    glEnd();

    glLineWidth(2.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.9f, -1.0f);
        glVertex2f(0.9f, -1.0f);
        glVertex2f(0.9f, -0.8f);
        glVertex2f(-0.9f, -0.8f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(-0.2f, -0.92f, GLUT_BITMAP_HELVETICA_18, "BACK TO MENU");



    int estimatedLevel = (highScore / 10) + 1;
    if(estimatedLevel > 1) {
        glColor3f(0.6f, 0.9f, 0.6f);
        char levelStr[50];
        sprintf(levelStr, "Estimated Level: %d", estimatedLevel);
        renderBitmapString(-0.3f, 0.0f, GLUT_BITMAP_HELVETICA_18, levelStr);
    }

    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void gameOver(){
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-3, 3, -2, 2);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    glColor3f(1.0f,1.0f,1.0f);
    renderBitmapString(-0.5f,0.9f,GLUT_BITMAP_TIMES_ROMAN_24,"GAME OVER");

    renderBitmapString(-0.5f,0.6f,GLUT_BITMAP_TIMES_ROMAN_24,"SCORE : ");
    char str[80];
    sprintf(str,"%d",points);
    for(int i=0;i<(int)(strlen(str));i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);

    renderBitmapString(-0.5f,0.4f,GLUT_BITMAP_TIMES_ROMAN_24,"LEVEL REACHED : ");
    sprintf(str,"%d",level);
    for(int i=0;i<(int)(strlen(str));i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);

    if(points > highScore){
        highScore = points;
        saveHighScore();
    }

    renderBitmapString(-0.5f,0.2f,GLUT_BITMAP_TIMES_ROMAN_24,"HIGH SCORE : ");
    sprintf(str,"%d",highScore);
    for(int i=0;i<(int)(strlen(str));i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);

    glBegin(GL_LINES);
        glVertex2f(-0.6f,-0.5f);
        glVertex2f(-0.6f,-0.2f);
        glVertex2f(-0.6f,-0.5f);
        glVertex2f(0.6f,-0.5f);
        glVertex2f(-0.6f,-0.2f);
        glVertex2f(0.6f,-0.2f);
        glVertex2f(0.6f,-0.5f);
        glVertex2f(0.6f,-0.2f);
	glEnd();

	renderBitmapString(-0.2f,-0.4f,GLUT_BITMAP_TIMES_ROMAN_24,"MENU");

    frontCount=-1;
    cnt=0;
    high=-1;
    weapon_power = 1; // Reset weapon power
    weapon_timer = 0;
    slow_motion_active = false;
    slow_motion_timer = 0;

    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// NEW: Function to create gifts when enemies are destroyed
void createGift(double x, double y) {
    for(int i = 0; i < SZ; i++) {
        if(!gift_stat[i]) {
            gift_x[i] = x;
            gift_y[i] = y;
            gift_vel[i] = 0.02f;
            gift_stat[i] = 1;
            gift_type[i] = rand() % 3; // 0: weapon, 1: life, 2: slow motion
            gift_count++;
            gift_active = true;
            break;
        }
    }
}

// NEW: Function to draw different types of gifts
void drawGift(int type) {
    switch(type) {
        case 0: // Weapon power gift (blue)
            glColor3f(0.2f, 0.4f, 1.0f);
            glBegin(GL_QUADS);
                glVertex2f(-0.04f, 0.04f);
                glVertex2f(0.04f, 0.04f);
                glVertex2f(0.04f, -0.04f);
                glVertex2f(-0.04f, -0.04f);
            glEnd();
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_LINES);
                glVertex2f(0.0f, 0.04f);
                glVertex2f(0.0f, -0.04f);
                glVertex2f(-0.04f, 0.0f);
                glVertex2f(0.04f, 0.0f);
            glEnd();
            break;

        case 1: // Extra life gift (red)
            glColor3f(1.0f, 0.2f, 0.2f);
            glBegin(GL_QUADS);
                glVertex2f(-0.04f, 0.04f);
                glVertex2f(0.04f, 0.04f);
                glVertex2f(0.04f, -0.04f);
                glVertex2f(-0.04f, -0.04f);
            glEnd();
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_LINES);
                glVertex2f(-0.02f, 0.02f);
                glVertex2f(0.02f, -0.02f);
                glVertex2f(-0.02f, -0.02f);
                glVertex2f(0.02f, 0.02f);
            glEnd();
            break;

        case 2: // Slow motion gift (green)
            glColor3f(0.2f, 1.0f, 0.2f);
            glBegin(GL_QUADS);
                glVertex2f(-0.04f, 0.04f);
                glVertex2f(0.04f, 0.04f);
                glVertex2f(0.04f, -0.04f);
                glVertex2f(-0.04f, -0.04f);
            glEnd();
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_LINES);
                glVertex2f(-0.03f, 0.0f);
                glVertex2f(0.03f, 0.0f);
                glVertex2f(0.0f, -0.03f);
                glVertex2f(0.0f, 0.03f);
            glEnd();
            break;
    }
}

void generateEnemies(){
    if(frontCount==-1){
        cnt=0;
    }
    if(frontCount==0){
        cnt=1;
    }
	if(frontCount<100000){
		for(int i=0; cnt<=2; i++){
			if(i>frontCount || enemy_y[i]<=-5.0f){
                enemy_x[i]=((rand()%100)*1.0f/100.0)+((rand()%4)*1.0f)-2;
                enemy_y[i]=2.3f;
				enemy_vel[i]=0.003f + 3*((rand()%10*1.0f)/10000);
				enemy_stat[i]=0;
				enemy_type[i] = rand() % 10;

                // NEW: Enable left-right movement for higher levels
                if(level >= 2) {
                    enemy_move_dir[i] = (rand() % 2 == 0) ? -1.0f : 1.0f;
                    enemy_move_speed[i] = 0.01f + (level * 0.002f);
                } else {
                    enemy_move_dir[i] = 0.0f;
                    enemy_move_speed[i] = 0.0f;
                }

				frontCount++;
				cnt++;
			}
		}
	}
}

void createExplosion(double x, double y) {
    for(int i = 0; i < SZ; i++) {
        if(!explosion_active[i]) {
            explosion_x[i] = x;
            explosion_y[i] = y;
            explosion_radius[i] = 0.12f;
            explosion_active[i] = 1;
            explosion_lifetime[i] = EXPLOSION_MAX_LIFETIME;

            // NEW: Random chance to drop gift (20% chance)
            if(rand() % 5 == 0) {
                createGift(x, y);
            }

            break;
        }
    }
}

void drawExplosionFire(double x, double y, double radius, int lifetime) {
    float progress = 1.0f - ((float)lifetime / (float)EXPLOSION_MAX_LIFETIME);
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();
    glTranslatef(x, y, 0.0f);

    for(int i = 0; i < 2; i++) {
        float scaleFactor = 1.0f + progress * 0.6f;
        float currentRadius = radius * scaleFactor * (1.0f - i * 0.25f);
        float alpha = (1.0f - progress) * (0.9f - i * 0.3f) + 0.1f;

        if(i == 0) {
            glColor4f(1.0f, 0.9f, 0.4f, alpha);
        } else {
            glColor4f(1.0f, 0.5f, 0.12f, alpha * 0.9f);
        }

        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);
        for(int j = 0; j <= 360; j += 10) {
            float angle = j * 3.14159f / 180.0f;
            float randOffset = 1.0f + (sin(angle * 3 + lifetime * 0.5f) * 0.08f);
            glVertex2f(cos(angle) * currentRadius * randOffset,
                       sin(angle) * currentRadius * randOffset);
        }
        glEnd();
    }

    glColor4f(1.0f, 0.85f, 0.2f, 0.55f * (1.0f - progress));
    for(int i = 0; i < 6; i++) {
        float angle = (i * 60 + lifetime * 8) * 3.14159f / 180.0f;
        float particleLength = radius * (1.0f + progress * 1.8f);
        float particleWidth = radius * 0.12f * (1.0f - progress);

        glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(cos(angle - 0.12f) * particleWidth, sin(angle - 0.12f) * particleWidth);
        glVertex2f(cos(angle) * particleLength, sin(angle) * particleLength);
        glEnd();
    }

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void drawEnemy(int type){
    float scale = 1.0f;

    switch(type) {
        case 0: // Classic Space Invader
            glColor3f(0.0f, 1.0f, 1.0f);
            glBegin(GL_QUADS);
                glVertex2f(-0.06f*scale, 0.06f*scale);
                glVertex2f(0.06f*scale, 0.06f*scale);
                glVertex2f(0.06f*scale, -0.02f*scale);
                glVertex2f(-0.06f*scale, -0.02f*scale);

                glVertex2f(-0.08f*scale, 0.02f*scale);
                glVertex2f(-0.06f*scale, 0.02f*scale);
                glVertex2f(-0.06f*scale, -0.06f*scale);
                glVertex2f(-0.08f*scale, -0.06f*scale);

                glVertex2f(0.06f*scale, 0.02f*scale);
                glVertex2f(0.08f*scale, 0.02f*scale);
                glVertex2f(0.08f*scale, -0.06f*scale);
                glVertex2f(0.06f*scale, -0.06f*scale);
            glEnd();
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_QUADS);
                glVertex2f(-0.04f*scale, 0.02f*scale);
                glVertex2f(-0.02f*scale, 0.02f*scale);
                glVertex2f(-0.02f*scale, 0.04f*scale);
                glVertex2f(-0.04f*scale, 0.04f*scale);

                glVertex2f(0.02f*scale, 0.02f*scale);
                glVertex2f(0.04f*scale, 0.02f*scale);
                glVertex2f(0.04f*scale, 0.04f*scale);
                glVertex2f(0.02f*scale, 0.04f*scale);
            glEnd();
            break;

        case 1: // Octopus-like
            glColor3f(1.0f, 0.2f, 0.2f);
            glBegin(GL_POLYGON);
                glVertex2f(0.0f*scale, 0.08f*scale);
                glVertex2f(-0.04f*scale, 0.06f*scale);
                glVertex2f(-0.06f*scale, 0.02f*scale);
                glVertex2f(-0.06f*scale, -0.02f*scale);
                glVertex2f(-0.04f*scale, -0.06f*scale);
                glVertex2f(-0.02f*scale, -0.08f*scale);
                glVertex2f(0.02f*scale, -0.08f*scale);
                glVertex2f(0.04f*scale, -0.06f*scale);
                glVertex2f(0.06f*scale, -0.02f*scale);
                glVertex2f(0.06f*scale, 0.02f*scale);
                glVertex2f(0.04f*scale, 0.06f*scale);
            glEnd();
            glBegin(GL_QUADS);
                for(int i = -2; i <= 2; i++) {
                    glVertex2f((i*0.02f-0.01f)*scale, -0.06f*scale);
                    glVertex2f((i*0.02f+0.01f)*scale, -0.06f*scale);
                    glVertex2f((i*0.02f+0.01f)*scale, -0.1f*scale);
                    glVertex2f((i*0.02f-0.01f)*scale, -0.1f*scale);
                }
            glEnd();
            break;

        case 2: // Crab-like
            glColor3f(1.0f, 0.5f, 0.0f);
            glBegin(GL_POLYGON);
                glVertex2f(-0.05f*scale, 0.06f*scale);
                glVertex2f(0.05f*scale, 0.06f*scale);
                glVertex2f(0.07f*scale, 0.02f*scale);
                glVertex2f(0.05f*scale, -0.04f*scale);
                glVertex2f(-0.05f*scale, -0.04f*scale);
                glVertex2f(-0.07f*scale, 0.02f*scale);
            glEnd();

            glBegin(GL_TRIANGLES);
                glVertex2f(-0.07f*scale, 0.04f*scale);
                glVertex2f(-0.1f*scale, 0.06f*scale);
                glVertex2f(-0.09f*scale, 0.02f*scale);

                glVertex2f(0.07f*scale, 0.04f*scale);
                glVertex2f(0.1f*scale, 0.06f*scale);
                glVertex2f(0.09f*scale, 0.02f*scale);
            glEnd();
            break;

        case 3: // Squid-like Enemy
            glColor3f(0.6f, 0.2f, 0.8f);
            glBegin(GL_QUADS);
                glVertex2f(-0.05f*scale, 0.08f*scale);
                glVertex2f(0.05f*scale, 0.08f*scale);
                glVertex2f(0.05f*scale, 0.02f*scale);
                glVertex2f(-0.05f*scale, 0.02f*scale);
            glEnd();

            glBegin(GL_QUADS);
                glVertex2f(-0.03f*scale, 0.08f*scale);
                glVertex2f(-0.01f*scale, 0.08f*scale);
                glVertex2f(-0.01f*scale, 0.1f*scale);
                glVertex2f(-0.03f*scale, 0.1f*scale);

                glVertex2f(0.01f*scale, 0.08f*scale);
                glVertex2f(0.03f*scale, 0.08f*scale);
                glVertex2f(0.03f*scale, 0.1f*scale);
                glVertex2f(0.01f*scale, 0.1f*scale);
            glEnd();

            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_QUADS);
                glVertex2f(-0.04f*scale, 0.04f*scale);
                glVertex2f(-0.02f*scale, 0.04f*scale);
                glVertex2f(-0.02f*scale, 0.06f*scale);
                glVertex2f(-0.04f*scale, 0.06f*scale);

                glVertex2f(0.02f*scale, 0.04f*scale);
                glVertex2f(0.04f*scale, 0.04f*scale);
                glVertex2f(0.04f*scale, 0.06f*scale);
                glVertex2f(0.02f*scale, 0.06f*scale);
            glEnd();

            glColor3f(0.6f, 0.2f, 0.8f);
            glBegin(GL_QUADS);
                glVertex2f(-0.06f*scale, 0.02f*scale);
                glVertex2f(-0.04f*scale, 0.02f*scale);
                glVertex2f(-0.05f*scale, -0.04f*scale);
                glVertex2f(-0.07f*scale, -0.04f*scale);

                glVertex2f(-0.05f*scale, -0.04f*scale);
                glVertex2f(-0.03f*scale, -0.04f*scale);
                glVertex2f(-0.02f*scale, -0.08f*scale);
                glVertex2f(-0.04f*scale, -0.08f*scale);

                glVertex2f(-0.02f*scale, 0.02f*scale);
                glVertex2f(0.0f*scale, 0.02f*scale);
                glVertex2f(0.01f*scale, -0.04f*scale);
                glVertex2f(-0.01f*scale, -0.04f*scale);

                glVertex2f(0.01f*scale, -0.04f*scale);
                glVertex2f(0.03f*scale, -0.04f*scale);
                glVertex2f(0.04f*scale, -0.08f*scale);
                glVertex2f(0.02f*scale, -0.08f*scale);

                glVertex2f(0.0f*scale, 0.02f*scale);
                glVertex2f(0.02f*scale, 0.02f*scale);
                glVertex2f(0.01f*scale, -0.04f*scale);
                glVertex2f(-0.01f*scale, -0.04f*scale);

                glVertex2f(0.04f*scale, 0.02f*scale);
                glVertex2f(0.06f*scale, 0.02f*scale);
                glVertex2f(0.07f*scale, -0.04f*scale);
                glVertex2f(0.05f*scale, -0.04f*scale);

                glVertex2f(0.05f*scale, -0.04f*scale);
                glVertex2f(0.07f*scale, -0.04f*scale);
                glVertex2f(0.08f*scale, -0.08f*scale);
                glVertex2f(0.06f*scale, -0.08f*scale);
            glEnd();
            break;

        case 4: // Spider-like
            glColor3f(0.6f, 0.2f, 0.8f);

            glBegin(GL_POLYGON);
                for(int i = 0; i < 8; i++) {
                    float angle = 2.0f * 3.14159f * i / 8;
                    glVertex2f(0.04f*scale*cos(angle), 0.04f*scale*sin(angle));
                }
            glEnd();

            glBegin(GL_LINES);
                glLineWidth(2);
                for(int i = 0; i < 8; i++) {
                    float angle = 2.0f * 3.14159f * i / 8;
                    glVertex2f(0.04f*scale*cos(angle), 0.04f*scale*sin(angle));
                    glVertex2f(0.08f*scale*cos(angle), 0.08f*scale*sin(angle));
                }
            glEnd();
            break;

        case 5: // Diamond Enemy
            glColor3f(1.0f, 1.0f, 0.2f);
            glBegin(GL_POLYGON);
                glVertex2f(0.0f*scale, 0.08f*scale);
                glVertex2f(-0.06f*scale, 0.0f*scale);
                glVertex2f(0.0f*scale, -0.08f*scale);
                glVertex2f(0.06f*scale, 0.0f*scale);
            glEnd();

            glColor3f(0.8f, 0.8f, 0.0f);
            glBegin(GL_POLYGON);
                glVertex2f(0.0f*scale, 0.04f*scale);
                glVertex2f(-0.03f*scale, 0.0f*scale);
                glVertex2f(0.0f*scale, -0.04f*scale);
                glVertex2f(0.03f*scale, 0.0f*scale);
            glEnd();
            break;

        case 6: // Star Enemy
            glColor3f(1.0f, 0.4f, 0.8f);
            glBegin(GL_TRIANGLES);
                for(int i = 0; i < 5; i++) {
                    float angle1 = 2.0f * 3.14159f * i / 5 - 3.14159f/2;
                    float angle2 = 2.0f * 3.14159f * (i+0.5f) / 5 - 3.14159f/2;
                    float angle3 = 2.0f * 3.14159f * (i+1) / 5 - 3.14159f/2;

                    glVertex2f(0.0f, 0.0f);
                    glVertex2f(0.09f*scale*cos(angle1), 0.09f*scale*sin(angle1));
                    glVertex2f(0.045f*scale*cos(angle2), 0.045f*scale*sin(angle2));

                    glVertex2f(0.0f, 0.0f);
                    glVertex2f(0.045f*scale*cos(angle2), 0.045f*scale*sin(angle2));
                    glVertex2f(0.09f*scale*cos(angle3), 0.09f*scale*sin(angle3));
                }
            glEnd();
            break;

        case 7: // Hexagon Enemy
            glColor3f(0.4f, 0.8f, 1.0f);
            glBegin(GL_POLYGON);
                for(int i = 0; i < 6; i++) {
                    float angle = 2.0f * 3.14159f * i / 6;
                    glVertex2f(0.09f*scale*cos(angle), 0.09f*scale*sin(angle));
                }
            glEnd();
            glColor3f(0.2f, 0.6f, 0.8f);
            glBegin(GL_POLYGON);
                for(int i = 0; i < 6; i++) {
                    float angle = 2.0f * 3.14159f * i / 6;
                    glVertex2f(0.045f*scale*cos(angle), 0.045f*scale*sin(angle));
                }
            glEnd();
            break;

        case 8: // Spaceship Enemy
            glColor3f(0.5f, 1.0f, 0.0f);
            glBegin(GL_POLYGON);
                glVertex2f(0.0f*scale, 0.08f*scale);
                glVertex2f(-0.03f*scale, 0.04f*scale);
                glVertex2f(-0.05f*scale, 0.0f*scale);
                glVertex2f(-0.03f*scale, -0.06f*scale);
                glVertex2f(0.03f*scale, -0.06f*scale);
                glVertex2f(0.05f*scale, 0.0f*scale);
                glVertex2f(0.03f*scale, 0.04f*scale);
            glEnd();

            glColor3f(0.4f, 0.8f, 0.0f);
            glBegin(GL_TRIANGLES);
                glVertex2f(-0.05f*scale, 0.02f*scale);
                glVertex2f(-0.09f*scale, 0.0f*scale);
                glVertex2f(-0.05f*scale, -0.02f*scale);

                glVertex2f(0.05f*scale, 0.02f*scale);
                glVertex2f(0.09f*scale, 0.0f*scale);
                glVertex2f(0.05f*scale, -0.02f*scale);
            glEnd();

            glColor3f(0.2f, 0.2f, 0.8f);
            glBegin(GL_POLYGON);
                glVertex2f(0.0f*scale, 0.05f*scale);
                glVertex2f(-0.015f*scale, 0.02f*scale);
                glVertex2f(0.015f*scale, 0.02f*scale);
            glEnd();

            glColor3f(1.0f, 0.5f, 0.0f);
            glBegin(GL_TRIANGLES);
                glVertex2f(-0.02f*scale, -0.06f*scale);
                glVertex2f(-0.015f*scale, -0.09f*scale);
                glVertex2f(-0.01f*scale, -0.06f*scale);

                glVertex2f(0.01f*scale, -0.06f*scale);
                glVertex2f(0.015f*scale, -0.09f*scale);
                glVertex2f(0.02f*scale, -0.06f*scale);
            glEnd();
            break;

        case 9: // Triangle Swarm
            glColor3f(0.3f, 0.7f, 1.0f);

            glBegin(GL_TRIANGLES);
                glVertex2f(0.0f*scale, 0.08f*scale);
                glVertex2f(-0.06f*scale, -0.04f*scale);
                glVertex2f(0.06f*scale, -0.04f*scale);

                glVertex2f(-0.04f*scale, 0.02f*scale);
                glVertex2f(-0.08f*scale, -0.02f*scale);
                glVertex2f(-0.06f*scale, -0.06f*scale);

                glVertex2f(0.04f*scale, 0.02f*scale);
                glVertex2f(0.08f*scale, -0.02f*scale);
                glVertex2f(0.06f*scale, -0.06f*scale);
            glEnd();
            break;
    }
}

// UPDATED SCORE DISPLAY - Top Right
void score(){
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(1.5f, 1.8f, GLUT_BITMAP_HELVETICA_18, "SCORE: ");
    char str[80];
    sprintf(str, "%d", points);
    glRasterPos2f(1.5f + 0.7f, 1.8f);
    for(int i = 0; i < (int)(strlen(str)); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
}

// UPDATED LEVEL DISPLAY - Top Center
void showLevel(){
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(-0.3f, 1.8f, GLUT_BITMAP_HELVETICA_18, "LEVEL: ");
    char str[80];
    sprintf(str, "%d", level);
    glRasterPos2f(-0.3f + 0.6f, 1.8f);
    for(int i = 0; i < (int)(strlen(str)); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
}

// NEW: Weapon power display
void showWeaponPower(){
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(1.5f, 1.6f, GLUT_BITMAP_HELVETICA_18, "WEAPON: ");

    char powerStr[20];
    if(weapon_power == 1) strcpy(powerStr, "SINGLE");
    else if(weapon_power == 2) strcpy(powerStr, "DOUBLE");
    else strcpy(powerStr, "TRIPLE");

    if(weapon_timer > 0) {
        char timerStr[30];
        sprintf(timerStr, "%s (%d)", powerStr, weapon_timer / 50);
        glRasterPos2f(1.5f + 0.8f, 1.6f);
        for(int i = 0; i < (int)(strlen(timerStr)); i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timerStr[i]);
    } else {
        glRasterPos2f(1.5f + 0.8f, 1.6f);
        for(int i = 0; i < (int)(strlen(powerStr)); i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, powerStr[i]);
    }
}

// NEW: Slow motion display
void showSlowMotion(){
    if(slow_motion_timer > 0) {
        glColor3f(0.2f, 1.0f, 0.2f);
        char slowStr[30];
        sprintf(slowStr, "SLOW MOTION: %d", slow_motion_timer / 40);
        renderBitmapString(-0.8f, 1.6f, GLUT_BITMAP_HELVETICA_18, slowStr);
    }
}

// UPDATED HEART DRAWING FUNCTION
void drawHeart(float x, float y, float size){
    glColor3f(1.0f, 0.0f, 0.0f);

    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(size, size, 1.0f);

    glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, -0.08f);
        glVertex2f(-0.06f, 0.02f);
        glVertex2f(0.06f, 0.02f);
    glEnd();

    glPushMatrix();
    glTranslatef(-0.03f, 0.03f, 0.0f);
    glutSolidSphere(0.03, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.03f, 0.03f, 0.0f);
    glutSolidSphere(0.03, 10, 10);
    glPopMatrix();

    glPopMatrix();
}

// UPDATED LIFE DISPLAY - Top Left
void showLife(){
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(-2.8f, 1.8f, GLUT_BITMAP_HELVETICA_18, "LIFE: ");

    for(int i = 0; i < life && i < 5; i++){
        drawHeart(-2.3f + (i * 0.15f), 1.8f, 1.0f);
    }
}

void getCoord(int x, int y){
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    winZ = 0;
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
    mousex = posX * 100 / 2;
    mousey = (posY * 100) / 2;
    glutPostRedisplay();
}

void handleMouseclick(int button, int state, int x, int y){
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && mainmenu == 1){
        getCoord(x, y);

        if(mousex >= 0.3 && mousex <= 1.5 && mousey >= 0.7 && mousey <= 1.0){
            game_play = 1;
            mainmenu = 0;
            game_over = 0;
            life = 5;
            points = 0;
            level = 1;
            pointsForNextLevel = 10;
            frontCount = -1;
            cnt = 0;
            high = -1;
            weapon_power = 1;
            weapon_timer = 0;
            slow_motion_active = false;
            slow_motion_timer = 0;
            playGameMusic();
        }

        else if(mousex >= 0.3 && mousex <= 1.5 && mousey >= 0.3 && mousey <= 0.6){
              mainmenu = 0;
              howtoplay = 1;
        }

        else if(mousex >= 0.3 && mousex <= 1.5 && mousey >= -0.1 && mousey <= 0.2){
              mainmenu = 0;
              highscore_menu = 1;
        }

        else if(mousex >= 0.3 && mousex <= 1.5 && mousey >= -0.5 && mousey <= -0.2){
            stopAllMusic();
            exit(0);
        }

        else if(mousex >= -1.5 && mousex <= -0.3 && mousey >= -1.5 && mousey <= -1.2){
            toggleMusic();
        }
    }

    else if (button == GLUT_LEFT_BUTTON && game_play==0 && mainmenu==0 && howtoplay==1){
        getCoord(x,y);
        if(mousex>=-0.9 && mousex<=0.9 && mousey>=-1.7 && mousey<=-1.5){
            mainmenu=1;
            howtoplay=0;
        }
    }

    else if (button == GLUT_LEFT_BUTTON && game_play==0 && mainmenu==0 && highscore_menu==1){
        getCoord(x,y);
        if(mousex>=-0.9 && mousex<=0.9 && mousey>=-1.0 && mousey<=-0.8){
            mainmenu=1;
            highscore_menu=0;
        }
        else if(mousex>=-0.9 && mousex<=0.9 && mousey>=0.5 && mousey<=0.7){
            highScore = 0;
            saveHighScore();
            glutPostRedisplay();
        }
    }

    else if (button == GLUT_LEFT_BUTTON && game_play==0 && mainmenu==0 && game_over==1){
        getCoord(x,y);
        if(mousex>=-0.6 && mousex<=0.6 && mousey>=-0.5 && mousey<=-0.2){
            mainmenu=1;
            game_over=0;
            life = 5;
            points=0;
            level = 1;
            pointsForNextLevel = 10;
            weapon_power = 1;
            weapon_timer = 0;
            slow_motion_active = false;
            slow_motion_timer = 0;
            playMenuMusic();
        }
    }
}

void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (double)w / (double)h, 0.1f, 200.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// UPDATED: Enhanced bullet firing for weapon power
void fireBullet(double x, double y) {
    int i;
    for(i=0;i<high;i++){
        if(bullet_stat[i]==0){
            bullet_x[i]=x;
            bullet_y[i]=y;
            tip_x[i]=bullet_x[i];
            tip_y[i]=bullet_y[i]+0.2;
            bullet_vel_x[i]=0;
            bullet_vel_y[i]=bullet_vel;
            bullet_stat[i]=1;
            bullet=1;
            break;
        }
    }
    if(bullet==0){
        high++;
        bullet_x[high]=x;
        bullet_y[high]=y;
        tip_x[high]=x;
        tip_y[high]=y+0.35;
        bullet_vel_x[high]=0;
        bullet_vel_y[high]=bullet_vel;
        bullet_stat[high]=1;
    }
    bullet=0;
}

void handleKeypress(unsigned char key, int x, int y){
    if (key == 27) {
        stopAllMusic();
        exit(0);
    }
    else if(key == 'm' || key == 'M') {
        toggleMusic();
    }
    else if(key==' ' && on>=1000){
        PlaySound("E:\\L-3 S-1\\Computer graphics\\Lab work\\project opengl\\space_shooter\\bullet.wav", NULL, SND_ASYNC);

        // NEW: Enhanced bullet firing based on weapon power
        if(weapon_power == 1) {
            // Single shot
            fireBullet(shooter_x, shooter_y + 0.15);
        }
        else if(weapon_power == 2) {
            // Double shot
            fireBullet(shooter_x - 0.05, shooter_y + 0.15);
            fireBullet(shooter_x + 0.05, shooter_y + 0.15);
        }
        else if(weapon_power == 3) {
            // Triple shot
            fireBullet(shooter_x, shooter_y + 0.15);
            fireBullet(shooter_x - 0.08, shooter_y + 0.12);
            fireBullet(shooter_x + 0.08, shooter_y + 0.12);
        }

        on=0;
    }
}

void drawShooter(){
    float scale = 1.5f;

    glColor3f(0.85, 0.87, 0.9);
    glBegin(GL_POLYGON);
        glVertex2f(0.0f * scale, 0.12f * scale);
        glVertex2f(-0.04f * scale, 0.0f * scale);
        glVertex2f(-0.04f * scale, -0.08f * scale);
        glVertex2f(0.04f * scale, -0.08f * scale);
        glVertex2f(0.04f * scale, 0.0f * scale);
    glEnd();

    glColor3f(0.2, 0.8, 1.0);
    glBegin(GL_POLYGON);
        glVertex2f(0.0f * scale, 0.08f * scale);
        glVertex2f(-0.02f * scale, 0.02f * scale);
        glVertex2f(0.02f * scale, 0.02f * scale);
    glEnd();

    glColor3f(0.6, 0.65, 0.7);
    glBegin(GL_POLYGON);
        glVertex2f(-0.04f * scale, 0.02f * scale);
        glVertex2f(-0.12f * scale, -0.02f * scale);
        glVertex2f(-0.12f * scale, -0.06f * scale);
        glVertex2f(-0.04f * scale, -0.04f * scale);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(0.04f * scale, 0.02f * scale);
        glVertex2f(0.12f * scale, -0.02f * scale);
        glVertex2f(0.12f * scale, -0.06f * scale);
        glVertex2f(0.04f * scale, -0.04f * scale);
    glEnd();

    glColor3f(1.0, 0.5, 0.0);
    glBegin(GL_QUADS);
        glVertex2f(-0.08f * scale, -0.02f * scale);
        glVertex2f(-0.06f * scale, -0.02f * scale);
        glVertex2f(-0.06f * scale, -0.05f * scale);
        glVertex2f(-0.08f * scale, -0.05f * scale);

        glVertex2f(0.06f * scale, -0.02f * scale);
        glVertex2f(0.08f * scale, -0.02f * scale);
        glVertex2f(0.08f * scale, -0.05f * scale);
        glVertex2f(0.06f * scale, -0.05f * scale);
    glEnd();

    glColor3f(1.0, 0.6, 0.0);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.08f * scale, -0.08f * scale);
        glVertex2f(-0.1f * scale, -0.12f * scale);
        glVertex2f(-0.06f * scale, -0.08f * scale);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2f(-0.02f * scale, -0.08f * scale);
        glVertex2f(-0.025f * scale, -0.12f * scale);
        glVertex2f(-0.015f * scale, -0.08f * scale);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2f(0.015f * scale, -0.08f * scale);
        glVertex2f(0.025f * scale, -0.12f * scale);
        glVertex2f(0.02f * scale, -0.08f * scale);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2f(0.06f * scale, -0.08f * scale);
        glVertex2f(0.1f * scale, -0.12f * scale);
        glVertex2f(0.08f * scale, -0.08f * scale);
    glEnd();

    glColor3f(1.0, 1.0, 0.3);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.08f * scale, -0.08f * scale);
        glVertex2f(-0.08f * scale, -0.10f * scale);
        glVertex2f(-0.07f * scale, -0.08f * scale);

        glVertex2f(-0.02f * scale, -0.08f * scale);
        glVertex2f(-0.02f * scale, -0.10f * scale);
        glVertex2f(-0.018f * scale, -0.08f * scale);

        glVertex2f(0.018f * scale, -0.08f * scale);
        glVertex2f(0.02f * scale, -0.10f * scale);
        glVertex2f(0.02f * scale, -0.08f * scale);

        glVertex2f(0.07f * scale, -0.08f * scale);
        glVertex2f(0.08f * scale, -0.10f * scale);
        glVertex2f(0.08f * scale, -0.08f * scale);
    glEnd();
}

void handleSpecialKeypress(int key, int x, int y){
    if (key == GLUT_KEY_LEFT){
        if(shooter_x > shooter_min_x){
            shooter_x -= 0.05;
        }
    }
    else if (key == GLUT_KEY_RIGHT){
        if(shooter_x < shooter_max_x){
            shooter_x += 0.05;
        }
    }
}

void drawBullet(){
    glPushMatrix();
        glColor3f(1.0f, 1.0f, 0.8f);
        glBegin(GL_QUADS);
            glVertex2f(-0.01f, 0.04f);
            glVertex2f(0.01f, 0.04f);
            glVertex2f(0.01f, -0.04f);
            glVertex2f(-0.01f, -0.04f);
        glEnd();

        glBegin(GL_TRIANGLES);
            glVertex2f(0.0f, 0.06f);
            glVertex2f(-0.01f, 0.04f);
            glVertex2f(0.01f, 0.04f);
        glEnd();

        glColor4f(0.5f, 1.0f, 0.2f, 0.3f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_QUADS);
            glVertex2f(-0.02f, 0.05f);
            glVertex2f(0.02f, 0.05f);
            glVertex2f(0.02f, -0.05f);
            glVertex2f(-0.02f, -0.05f);
        glEnd();

        glDisable(GL_BLEND);
    glPopMatrix();
}

// UPDATED: Enhanced update function with gifts and enemy movement
void new_update(int value){
    // Update weapon power timer
    if(weapon_timer > 0) {
        weapon_timer--;
        if(weapon_timer == 0) {
            weapon_power = 1; // Reset to single shot
        }
    }

    // Update slow motion timer
    if(slow_motion_timer > 0) {
        slow_motion_timer--;
        if(slow_motion_timer == 0) {
            slow_motion_active = false;
        }
    }

    // NEW: Check if enemies touch the bottom and play death sound
    bool enemyReachedBottom = false;
    for(int i=0;i<=frontCount;i++){
        if(enemy_y[i]<=-1.6f){
            enemy_vel[i]=0;
            enemy_y[i]=-5.0f;
            enemy_stat[i]=0;
            frontCount--;
            life--;
            enemyReachedBottom = true;
        }
    }

    // NEW: Play death sound when enemy reaches bottom
    if(enemyReachedBottom) {
        playDeathSound();
    }

    if(life<=0){
        game_over=1;
        game_play=0;
        mainmenu=0;
        playMenuMusic();
    }

	for(int i=0;i<=high;i++){
		if(bullet_stat[i]==1){
			bullet_x[i]-=bullet_vel_x[i];
			bullet_y[i]+=bullet_vel_y[i];
			tip_x[i]-=bullet_vel_x[i];
			tip_y[i]+=bullet_vel_y[i];
			if(bullet_y[i]>=2.0f){
				bullet_stat[i]=0;
			}
		}
	}

    // NEW: Update gifts
    for(int i = 0; i < SZ; i++) {
        if(gift_stat[i] == 1) {
            gift_y[i] -= gift_vel[i];
            if(gift_y[i] <= -2.0f) {
                gift_stat[i] = 0;
                gift_count--;
            }

            // Check gift collision with shooter
            if(gift_x[i] > shooter_x - 0.1f && gift_x[i] < shooter_x + 0.1f &&
               gift_y[i] > shooter_y - 0.1f && gift_y[i] < shooter_y + 0.1f) {

                PlaySound("E:\\L-3 S-1\\Computer graphics\\Lab work\\project opengl\\space_shooter\\gift.wav", NULL, SND_ASYNC);

                // Apply gift effect based on type
                switch(gift_type[i]) {
                    case 0: // Weapon power
                        weapon_power = 2 + (rand() % 2); // Level 2 or 3
                        weapon_timer = WEAPON_POWER_DURATION;
                        break;
                    case 1: // Extra life
                        if(life < 5) life++;
                        break;
                    case 2: // Slow motion
                        slow_motion_active = true;
                        slow_motion_timer = SLOW_MOTION_DURATION;
                        break;
                }

                gift_stat[i] = 0;
                gift_count--;
            }
        }
    }

    // Collision detection
    if(frontCount >= 0 && high >= 0) {
        for(int i=0;i<=frontCount;i++){
            for(int j=0;j<=high;j++){
                if(bullet_stat[j]==1 &&
                   tip_x[j]>enemy_x[i]-0.08f && tip_x[j]<enemy_x[i]+0.08f &&
                   tip_y[j]>enemy_y[i]-0.08f && tip_y[j]<enemy_y[i]+0.08f) {

                    double ex = enemy_x[i];
                    double ey = enemy_y[i];

                    enemy_vel[i]=0;
                    enemy_y[i]=-5.0f;
                    enemy_stat[i]=0;
                    frontCount--;
                    bullet_stat[j]=0;
                    points++;

                    createExplosion(ex, ey);

                    PlaySound("E:\\L-3 S-1\\Computer graphics\\Lab work\\project opengl\\space_shooter\\explosion.wav", NULL, SND_ASYNC);

                    if(points >= pointsForNextLevel) {
                        level++;
                        pointsForNextLevel += 10;
                    }
                }
            }
        }
    }

    // NEW: Enhanced enemy movement with left-right motion for higher levels
    for(int i=0;i<=frontCount;i++){
        if(enemy_y[i]<0.0f && enemy_stat[i]==0){
            cnt--;
            enemy_stat[i]=1;
        }

        // Apply slow motion if active
        float current_vel = enemy_vel[i];
        float current_move_speed = enemy_move_speed[i];
        if(slow_motion_active) {
            current_vel *= slow_motion_factor;
            current_move_speed *= slow_motion_factor;
        }

        if(enemy_y[i]>=-1.85f){
            enemy_y[i] -= current_vel;

            // Left-right movement for level 2 and above
            if(level >= 2 && enemy_move_dir[i] != 0) {
                enemy_x[i] += enemy_move_dir[i] * current_move_speed;

                // Bounce at screen edges
                if(enemy_x[i] < -2.0f) {
                    enemy_x[i] = -2.0f;
                    enemy_move_dir[i] = 1.0f;
                } else if(enemy_x[i] > 2.0f) {
                    enemy_x[i] = 2.0f;
                    enemy_move_dir[i] = -1.0f;
                }
            }
        }
    }

    // Update explosions
    for(int i = 0; i < SZ; i++) {
        if(explosion_active[i]) {
            explosion_lifetime[i]--;
            explosion_radius[i] += 0.01f;

            if(explosion_lifetime[i] <= 0) {
                explosion_active[i] = 0;
            }
        }
    }

	on+=100;
    glutTimerFunc(20, new_update, 0);
}

void drawScene(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if(mainmenu == 1){
        MainMenu();
    }
    else if(howtoplay == 1){
        instruction();
    }
    else if(highscore_menu == 1){
        highScoreMenu();
    }
    else if(game_over == 1){
        gameOver();
    }
    else if(game_play == 1){
        generateEnemies();

        drawSpaceBackground();

        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -5.0f);

            // Draw bullets
            for(int i=0;i<=high;i++){
                if(bullet_stat[i]==1){
                    glPushMatrix();
                        glTranslatef(bullet_x[i],bullet_y[i],0.0f);
                        glColor3f(0.0f,0.0f,0.0f);
                        drawBullet();
                    glPopMatrix();
                }
            }

            // Draw shooter
            glPushMatrix();
                glTranslatef(shooter_x,shooter_y,0.0f);
                drawShooter();
            glPopMatrix();

            // Draw enemies
            for(int i=0;i<=frontCount;i++){
                if(enemy_vel[i]!=0){
                    glPushMatrix();
                        glTranslatef(enemy_x[i],enemy_y[i],0.0f);
                        drawEnemy(enemy_type[i]);
                    glPopMatrix();
                }
            }

            // NEW: Draw gifts
            for(int i = 0; i < SZ; i++) {
                if(gift_stat[i] == 1) {
                    glPushMatrix();
                        glTranslatef(gift_x[i], gift_y[i], 0.0f);
                        drawGift(gift_type[i]);
                    glPopMatrix();
                }
            }

            // Draw explosions
            for(int i = 0; i < SZ; i++) {
                if(explosion_active[i]) {
                    drawExplosionFire(explosion_x[i], explosion_y[i],
                                     explosion_radius[i], explosion_lifetime[i]);
                }
            }

            // Display UI elements
            glPushMatrix();
                showLife();
                showLevel();
                score();
                showWeaponPower(); // NEW: Show weapon power
                showSlowMotion();  // NEW: Show slow motion timer
            glPopMatrix();
        glPopMatrix();
    }

    glutSwapBuffers();
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Space Shooter Game");
    glutFullScreen();

    glEnable(GL_DEPTH_TEST);

    loadMenuBackground();
    loadBackgroundTexture();
    loadHighScore();
    playMenuMusic();

    glutDisplayFunc(drawScene);
    glutIdleFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutSpecialFunc(handleSpecialKeypress);
    glutMouseFunc(handleMouseclick);
    glutReshapeFunc(handleResize);
    glutTimerFunc(5, new_update, 0);
    glutMainLoop();

    stopAllMusic();
    if(backgroundImage) free(backgroundImage);
    return 0;
}
