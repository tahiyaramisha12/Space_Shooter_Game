#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
double enemy_col[SZ];
int frontCount = -1, cnt = 0;

int points = 0;
int game_over = 0;
int game_play = 0;
int mainmenu = 1;
int howtoplay = 0;
int life = 5;

double mousex;
double mousey;

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
        glVertex2f(0.3f, -0.1f);
        glVertex2f(0.3f, 0.2f);
        glVertex2f(0.3f, -0.1f);
        glVertex2f(1.5f, -0.1f);
        glVertex2f(0.3f, 0.2f);
        glVertex2f(1.5f, 0.2f);
        glVertex2f(1.5f, -0.1f);
        glVertex2f(1.5f, 0.2f);
    glEnd();

    renderBitmapString(0.5f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "Instruction");

    glBegin(GL_LINES);
        glVertex2f(0.3f, -0.9f);
        glVertex2f(0.3f, -0.6f);
        glVertex2f(0.3f, -0.9f);
        glVertex2f(1.5f, -0.9f);
        glVertex2f(0.3f, -0.6f);
        glVertex2f(1.5f, -0.6f);
        glVertex2f(1.5f, -0.9f);
        glVertex2f(1.5f, -0.6f);
    glEnd();

    renderBitmapString(0.5f, -0.8f, GLUT_BITMAP_TIMES_ROMAN_24, "Exit");
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
	renderBitmapString(-2.7f,0.4f,GLUT_BITMAP_HELVETICA_18,"6. Press ESC to exit the game");
	renderBitmapString(-0.2f,-1.4f,GLUT_BITMAP_TIMES_ROMAN_24,"BACK");
}

void gameOver(){
    glColor3f(1.0f,1.0f,1.0f);
    renderBitmapString(-0.5f,0.7f,GLUT_BITMAP_TIMES_ROMAN_24,"GAME OVER");
    renderBitmapString(-0.5f,0.5f,GLUT_BITMAP_TIMES_ROMAN_24,"SCORE : ");

    char str[80];
	sprintf(str,"%d",points);
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
				enemy_col[i]=rand()%3;
				frontCount++;
				cnt++;
			}
		}
	}
}

void drawEnemy(){
    glColor3f(1.0,0.0,0.0);
    glBegin(GL_TRIANGLES);
        glVertex2f(0.0f,0.1f);
        glVertex2f(-0.08f,-0.08f);
        glVertex2f(0.08f,-0.08f);
    glEnd();
}

void score(){
	glColor3f(1.0f,1.0f,1.0f);
    renderBitmapString(1.2f,1.8f,GLUT_BITMAP_HELVETICA_18,"SCORE :  ");
	char str[80];
	sprintf(str,"%d",points);
	for(int i=0;i<(int)(strlen(str));i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);
}

void showLife(){
	glColor3f(1.0f,1.0f,1.0f);
    renderBitmapString(-2.6f,1.8f,GLUT_BITMAP_HELVETICA_18,"LIFE :  ");
	char str[80];
	sprintf(str,"%d",life);
	for(int i=0;i<(int)(strlen(str));i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);
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
            frontCount = -1;
            cnt = 0;
            high = -1;
        }

        else if(mousex >= 0.3 && mousex <= 1.5 && mousey >= -0.1 && mousey <= 0.2){
              mainmenu = 0;
              howtoplay = 1;

        }

        else if(mousex >= 0.3 && mousex <= 1.5 && mousey >= -0.9 && mousey <= -0.6){
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
    else if (button == GLUT_LEFT_BUTTON && game_play==0 && mainmenu==0 && game_over==1){
        getCoord(x,y);
        if(mousex>=-0.6 && mousex<=0.6 && mousey>=-0.5 && mousey<=-0.2){
            mainmenu=1;
            game_over=0;
            life = 5;
            points=0;
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
				bullet_y[i]=shooter_y;
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
			bullet_y[high]=shooter_y;
			tip_x[high]=shooter_x;
			tip_y[high]=shooter_y+0.2;
			bullet_vel_x[high]=0;
			bullet_vel_y[high]=bullet_vel;
			bullet_stat[high]=1;
		}
		on=0;
		bullet=0;
	}
}

void drawShooter(){
    glColor3f(0.0,1.0,0.0);
    glBegin(GL_TRIANGLES);
        glVertex2f(0.0f,0.2f);
        glVertex2f(-0.15f,-0.1f);
        glVertex2f(0.15f,-0.1f);
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
        glutSolidSphere(0.03,50,50);
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
                        drawEnemy();
                    glPopMatrix();
                }
            }

            glPushMatrix();
                score();
                showLife();
            glPopMatrix();
        glPopMatrix();
    }

    glutSwapBuffers();
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 700);
    glutInitWindowPosition(220, 50);
    glutCreateWindow("Space Shooter Game");
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
