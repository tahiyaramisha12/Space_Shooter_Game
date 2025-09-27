#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define SZ 100000

using namespace std;

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
int frontCount = -1, cnt = 0;

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
    glLineWidth(4);
    glClearColor(0.42, 0.38, 0.55, 1.0);

    glColor3f(1.0,1.0,1.0);

    renderBitmapString(-1.7f, -0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "SPACE SHOOTER GAME");

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

    renderBitmapString(0.5f, 0.8f, GLUT_BITMAP_TIMES_ROMAN_24, "Start Game");

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

    renderBitmapString(0.5f, 0.45f, GLUT_BITMAP_TIMES_ROMAN_24, "Instruction");

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

    renderBitmapString(0.5f, 0.05f, GLUT_BITMAP_TIMES_ROMAN_24, "High Score");

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

    renderBitmapString(0.5f, -0.35f, GLUT_BITMAP_TIMES_ROMAN_24, "Exit");
}

void instruction(){
    glColor3f(1.0f,1.0f,1.0f);
    renderBitmapString(-0.4f,1.7f,GLUT_BITMAP_TIMES_ROMAN_24,"HOW TO PLAY");

    glBegin(GL_LINES);
        glVertex2f(-0.6f,-1.5f);
        glVertex2f(-0.6f,-1.2f);
        glVertex2f(-0.6f,-1.5f);
        glVertex2f(0.6f,-1.5f);
        glVertex2f(-0.6f,-1.2f);
        glVertex2f(0.6f,-1.2f);
        glVertex2f(0.6f,-1.5f);
        glVertex2f(0.6f,-1.2f);
	glEnd();

	renderBitmapString(-2.7f,1.4f,GLUT_BITMAP_HELVETICA_18,"1. Use Left arrow key to move the shooter Left");
	renderBitmapString(-2.7f,1.2f,GLUT_BITMAP_HELVETICA_18,"2. Use Right arrow key to move the shooter Right");
	renderBitmapString(-2.7f,1.0f,GLUT_BITMAP_HELVETICA_18,"3. Press Space to key to Shoot");
	renderBitmapString(-2.7f,0.8f,GLUT_BITMAP_HELVETICA_18,"4. Kill the enemy and 1 point will be added to the score");
	renderBitmapString(-2.7f,0.6f,GLUT_BITMAP_HELVETICA_18,"5. If enemy passes you, 1 life will be lost");
    renderBitmapString(-2.7f,0.4f,GLUT_BITMAP_HELVETICA_18,"6. Every 10 points, you advance to the next level");
	renderBitmapString(-2.7f,0.2f,GLUT_BITMAP_HELVETICA_18,"6. Press ESC to exit the game");
	renderBitmapString(-0.2f,-1.4f,GLUT_BITMAP_TIMES_ROMAN_24,"BACK");
}

void highScoreMenu(){
    glColor3f(1.0f,1.0f,1.0f);
    renderBitmapString(-0.6f, 1.5f, GLUT_BITMAP_TIMES_ROMAN_24, "HIGH SCORE");

    renderBitmapString(-0.5f, 0.8f, GLUT_BITMAP_TIMES_ROMAN_24, "Best Score: ");
    char str[80];
    sprintf(str,"%d", highScore);
    for(int i=0; i<(int)(strlen(str)); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);

    glBegin(GL_LINES);
        glVertex2f(-0.8f, 0.2f);
        glVertex2f(-0.8f, 0.5f);
        glVertex2f(-0.8f, 0.2f);
        glVertex2f(0.8f, 0.2f);
        glVertex2f(-0.8f, 0.5f);
        glVertex2f(0.8f, 0.5f);
        glVertex2f(0.8f, 0.2f);
        glVertex2f(0.8f, 0.5f);
    glEnd();

    renderBitmapString(-0.3f, 0.3f, GLUT_BITMAP_TIMES_ROMAN_24, "RESET");


    glBegin(GL_LINES);
        glVertex2f(-0.6f, -1.5f);
        glVertex2f(-0.6f, -1.2f);
        glVertex2f(-0.6f, -1.5f);
        glVertex2f(0.6f, -1.5f);
        glVertex2f(-0.6f, -1.2f);
        glVertex2f(0.6f, -1.2f);
        glVertex2f(0.6f, -1.5f);
        glVertex2f(0.6f, -1.2f);
    glEnd();

    renderBitmapString(-0.2f, -1.4f, GLUT_BITMAP_TIMES_ROMAN_24, "BACK");
}

void gameOver(){
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
				frontCount++;
				cnt++;
			}
		}
	}
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

                // Right wing
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

void score(){
	glColor3f(1.0f,1.0f,1.0f);
    renderBitmapString(1.2f,1.8f,GLUT_BITMAP_HELVETICA_18,"SCORE :  ");
	char str[80];
	sprintf(str,"%d",points);
	for(int i=0;i<(int)(strlen(str));i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);
}

void showLevel(){
    glColor3f(1.0f,1.0f,1.0f);
    renderBitmapString(-0.5f,1.8f,GLUT_BITMAP_HELVETICA_18,"LEVEL :  ");
    char str[80];
    sprintf(str,"%d",level);
    for(int i=0;i<(int)(strlen(str));i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);
}

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

void showLife(){
	glColor3f(1.0f,1.0f,1.0f);
    renderBitmapString(-2.6f,1.8f,GLUT_BITMAP_HELVETICA_18,"LIFE :  ");

    for(int i = 0; i < life && i < 5; i++){
        drawHeart(-1.8f + (i * 0.15f), 1.8f, 1.0f);
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
    glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
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
            exit(0);
        }
    }

    else if (button == GLUT_LEFT_BUTTON && game_play==0 && mainmenu==0 && howtoplay==1){
        getCoord(x,y);
        if(mousex>=-0.6 && mousex<=0.6 && mousey>=-1.5 && mousey<=-1.2){
            mainmenu=1;
            howtoplay=0;
        }
    }

    else if (button == GLUT_LEFT_BUTTON && game_play==0 && mainmenu==0 && highscore_menu==1){
        getCoord(x,y);
        if(mousex>=-0.6 && mousex<=0.6 && mousey>=-1.5 && mousey<=-1.2){
            mainmenu=1;
            highscore_menu=0;
        }
        else if(mousex>=-0.8 && mousex<=0.8 && mousey>=0.2 && mousey<=0.5){
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
        }
    }
}

void handleResize(int w, int h) {
    int winw = w;
    int winh = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (double)w / (double)h, 0.1f, 200.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void handleKeypress(unsigned char key, int x, int y){
    if (key == 27) {
        exit(0);
    }
    else if(key==' ' && on>=1000){
		int i;
		for(i=0;i<high;i++){
			if(bullet_stat[i]==0){
				bullet_x[i]=shooter_x;
				bullet_y[i]=shooter_y + 0.15;
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
			bullet_x[high]=shooter_x;
			bullet_y[high]=shooter_y + 0.15;
			tip_x[high]=shooter_x;
			tip_y[high]=shooter_y+0.35;
			bullet_vel_x[high]=0;
			bullet_vel_y[high]=bullet_vel;
			bullet_stat[high]=1;
		}
		on=0;
		bullet=0;
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

void new_update(int value){
    for(int i=0;i<=frontCount;i++){
        if(enemy_y[i]<=-1.6f){
            enemy_vel[i]=0;
            enemy_y[i]=-5.0f;
            enemy_stat[i]=0;
            frontCount--;
            life--;
        }
    }

    if(life<=0){
        game_over=1;
        game_play=0;
        mainmenu=0;
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

    for(int i=0;i<=frontCount;i++){
        for(int j=0;j<=high;j++){
            if(tip_x[j]>enemy_x[i]-0.08f && tip_x[j]<enemy_x[i]+0.08f && tip_y[j]>enemy_y[i]-0.08f && tip_y[j]<enemy_y[i]+0.08f && bullet_stat[j]==1){
                enemy_vel[i]=0;
                enemy_y[i]=-5.0f;
                enemy_stat[i]=0;
                frontCount--;
                bullet_stat[j]=0;
                points++;


                if(points >= pointsForNextLevel) {
                    level++;
                    pointsForNextLevel += 10;
                }
            }
        }
    }

    for(int i=0;i<=frontCount;i++){
        if(enemy_y[i]<0.0f && enemy_stat[i]==0){
            cnt--;
            enemy_stat[i]=1;
        }
        if(enemy_y[i]>=-1.85f){
            enemy_y[i]-=enemy_vel[i];
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
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -5.0f);
            glColor3f(0.0f, 0.0f, 0.0f);
            MainMenu();
        glPopMatrix();
    }
    else if(howtoplay == 1){
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -5.0f);
            instruction();
        glPopMatrix();
    }
    else if(highscore_menu == 1){
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -5.0f);
            highScoreMenu();
        glPopMatrix();
    }
    else if(game_over == 1){
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -5.0f);
            glColor3f(0.0f, 0.0f, 0.0f);
            gameOver();
        glPopMatrix();
    }
    else if(game_play == 1){
        generateEnemies();

        glClearColor(0.42, 0.38, 0.55, 1.0);
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -5.0f);

            for(int i=0;i<=high;i++){
                if(bullet_stat[i]==1){
                    glPushMatrix();
                        glTranslatef(bullet_x[i],bullet_y[i],0.0f);
                        glColor3f(0.0f,0.0f,0.0f);
                        drawBullet();
                    glPopMatrix();
                }
            }

            glPushMatrix();
                glTranslatef(shooter_x,shooter_y,0.0f);
                drawShooter();
            glPopMatrix();

            for(int i=0;i<=frontCount;i++){
                if(enemy_vel[i]!=0){
                    glPushMatrix();
                        glTranslatef(enemy_x[i],enemy_y[i],0.0f);
                        drawEnemy(enemy_type[i]);
                    glPopMatrix();
                }
            }

            glPushMatrix();
                score();
                showLife();
                showLevel();
            glPopMatrix();
        glPopMatrix();
    }

    glutSwapBuffers();
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Space Shooter Game");
    glutFullScreen();
    loadHighScore();
    glutDisplayFunc(drawScene);
    glutIdleFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutSpecialFunc(handleSpecialKeypress);
    glutMouseFunc(handleMouseclick);
    glutReshapeFunc(handleResize);
    glutTimerFunc(5, new_update, 0);
    glutMainLoop();
    return 0;
}
