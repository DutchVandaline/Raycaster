#include <stdlib.h>
#include <glut.h>
#include <math.h>
#include "sprites.h"

#define PI 3.1415926535

float degToRad(float a) { return a * PI / 180.0; }
float FixAng(float a) { if (a > 359) { a -= 360; } if (a < 0) { a += 360; } return a; }
float distance(ax, ay, bx, by, ang) { return cos(degToRad(ang)) * (bx - ax) - sin(degToRad(ang)) * (by - ay); }
float px, py, pdx, pdy, pa;
float frame1, frame2, fps;

int currentFrame = 0;
float frameDuration = 200; // Duration each frame is displayed in milliseconds
float lastFrameSwitch = 0;
int moving = 0;
int showZoomSprite = 0;
int pressFireSprite = 0;
float animationTimer = 0;

typedef struct
{
    int w, a, d, s;                     
}ButtonKeys; ButtonKeys Keys;



//-----------------------------MAP----------------------------------------------
#define mapX  8      //map width
#define mapY  8      //map height
#define mapS 64      //map cube size

                     //Edit these 3 arrays with values 0-4 to create your own level! 
int mapW[] =          //walls
{
 1,1,1,1,1,3,1,1,
 1,0,0,1,0,0,0,1,
 1,0,0,4,0,2,0,1,
 1,1,4,1,0,0,0,1,
 2,0,0,0,0,0,0,1,
 2,0,0,0,0,1,0,1,
 2,0,0,0,0,0,0,1,
 1,1,3,1,3,1,3,1,
};

int mapF[] =          //floors
{
 0,0,0,0,0,0,0,0,
 0,0,0,0,1,1,0,0,
 0,0,0,0,2,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,2,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,1,1,1,1,0,0,0,
 0,0,0,0,0,0,0,0,
};

int mapC[] =          //ceiling
{
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,1,0,
 0,1,3,1,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
};

//---------------------------Draw Player Animation--------------------------------

void drawPlayerSprite() {
    int spriteSize = 32; // the sprite is 32x32 pixels
    int pixelSize = 15; // size of each 'pixel' of the sprite on the screen
    int screenPosX = 480 - (spriteSize * pixelSize); // center of the sprite on the screen width
    int screenPosY = 640 - (spriteSize * pixelSize);

    int(*sprite)[32][32] = (currentFrame == 0) ? &playerSprite1 : &playerSprite2;

    for (int y = 0; y < spriteSize; y++) {
        for (int x = 0; x < spriteSize; x++) {
            if ((*sprite)[y][x] == 1) { // If the value is 1, color white
                glColor3f(1.0, 1.0, 1.0);
            }
            else if ((*sprite)[y][x] == 2) { // If the value is 2, color black
                glColor3f(0.0, 0.0, 0.0);
            }
            else {
                continue;
            }
            glBegin(GL_QUADS);
            glVertex2i(screenPosX + x * pixelSize, screenPosY + y * pixelSize);
            glVertex2i(screenPosX + x * pixelSize + pixelSize, screenPosY + y * pixelSize);
            glVertex2i(screenPosX + x * pixelSize + pixelSize, screenPosY + y * pixelSize + pixelSize);
            glVertex2i(screenPosX + x * pixelSize, screenPosY + y * pixelSize + pixelSize);
            glEnd();
        }
    }
}

void drawZoomSprite() {
    int spriteSize = 32;
    int pixelSize = 15;
    int screenPosX = 480 - (spriteSize * pixelSize);
    int screenPosY = 640 - (spriteSize * pixelSize);

    int(*zSprite)[32][32] = (currentFrame == 0) ? &zoomSprite1 : &zoomSprite2;

    for (int y = 0; y < spriteSize; y++) {
        for (int x = 0; x < spriteSize; x++) {
            if ((*zSprite)[y][x] == 1) { // If the value is 1, color white
                glColor3f(1.0, 1.0, 1.0);
            }
            else if ((*zSprite)[y][x] == 2) { // If the value is 2, color black
                glColor3f(0.0, 0.0, 0.0);
            }
            else {
                continue;
            }
            glBegin(GL_QUADS);
            glVertex2i(screenPosX + x * pixelSize, screenPosY + y * pixelSize);
            glVertex2i(screenPosX + x * pixelSize + pixelSize, screenPosY + y * pixelSize);
            glVertex2i(screenPosX + x * pixelSize + pixelSize, screenPosY + y * pixelSize + pixelSize);
            glVertex2i(screenPosX + x * pixelSize, screenPosY + y * pixelSize + pixelSize);
            glEnd();
        }
    }
}

void drawFireSprite() {
    int spriteSize = 32;
    int pixelSize = 15;
    int screenPosX = 480 - (spriteSize * pixelSize);
    int screenPosY = 640 - (spriteSize * pixelSize);

    for (int y = 0; y < spriteSize; y++) {
        for (int x = 0; x < spriteSize; x++) {
            if ((fireSprite)[y][x] == 1) {glColor3f(1.0, 1.0, 1.0);} // If the value is 1, color white
            else if ((fireSprite)[y][x] == 2) { glColor3f(0.0, 0.0, 0.0);}// If the value is 2, color black
            else if ((fireSprite)[y][x] == 3) {glColor3f(1.0, 0.0, 0.0);} // If the value is 2, color black
            else if ((fireSprite)[y][x] == 4) {glColor3f(1.0, 1.0, 0.0);} // If the value is 2, color black
            else {continue;}
            glBegin(GL_QUADS);
            glVertex2i(screenPosX + x * pixelSize, screenPosY + y * pixelSize);
            glVertex2i(screenPosX + x * pixelSize + pixelSize, screenPosY + y * pixelSize);
            glVertex2i(screenPosX + x * pixelSize + pixelSize, screenPosY + y * pixelSize + pixelSize);
            glVertex2i(screenPosX + x * pixelSize, screenPosY + y * pixelSize + pixelSize);
            glEnd();

        }
    }
}


typedef struct
{
    int type;       //static, key, enemy
    int state;      //on, off
    int map;        //texture to show
    double x, y, z;    //position
}sprite; sprite sp[4];
int depth[120];


void drawMonsterSprite()
{
    int x, y, s;
    //if (px < sp[0].x + 10 && px > sp[0].x - 10 && py < sp[0].y + 10 && py > sp[0].y - 10) {sp[0].state = 0;} //pick up key

    int spx = (int)sp[0].x >> 6, spy = (int)sp[0].y >> 6;
    int spx_add = ((int)sp[0].x + 15) >> 6, spy_add = ((int)sp[0].y + 15) >> 6;
    int spx_sub = ((int)sp[0].x - 15) >> 6, spy_sub = ((int)sp[0].y - 15) >> 6;

    if (sp[0].x > px && mapW[spy * 8 + spx_sub] == 0) { sp[0].x -= 0.03 * fps; }
    if (sp[0].x < px && mapW[spy * 8 + spx_add] == 0) { sp[0].x += 0.03 * fps; }
    if (sp[0].y > py && mapW[spy_sub * 8 + spx] == 0) { sp[0].y -= 0.03 * fps; }
    if (sp[0].y < py && mapW[spy_add * 8 + spx] == 0) { sp[0].y += 0.03 * fps; }


    for (s = 0; s < 1; s++) {

        float sx = sp[s].x - px;
        float sy = sp[s].y - py;
        float sz = sp[s].z;

        float CS = cos(degToRad(pa)), SN = sin(degToRad(pa)); // rotate based on player side
        float a = sy * CS + sx * SN;
        float b = sx * CS - sy * SN;
        sx = a; sy = b;

        sx = (sx * 108.0 / sy) + (120 / 2); //convert to screen 
        sy = (sz * 108.0 / sy) + (80 / 2);

        int scale = 45 * 80 / b;
        if (scale < 0) { scale = 0; }
        if (scale > 120) { scale = 120; }

        //texture
        float t_x = 0, t_y = 31, t_x_step = 32.0 / (float) scale, t_y_step = 32.0/(float) scale;

        for (x = sx - scale / 2; x < sx + scale / 2; x++)
        {
            t_y = 31;
            for (y = 0; y < scale; y++) 
            {
                if (x > 0 && x < 120 && b < depth[x]) //check the depth and see draw if it's on screen.
                {
                    
                    int pixel = ((int)t_y * 32 + (int)t_x) * 3;
                    int red = monster_walkSprite1[pixel + 0];
                    int green = monster_walkSprite1[pixel + 1];
                    int blue = monster_walkSprite1[pixel + 2];
                    if (red != 255, green != 255, blue != 255) {
                        glPointSize(8);
                        glColor3ub(red, green, blue);
                        glBegin(GL_POINTS);
                        glVertex2i(x * 8, sy * 8 - y * 8);
                        glEnd();
                    }
                    
                    t_y -= t_y_step; if (t_y < 0) { t_y = 0; }
                }
            }
            t_x += t_x_step;
        }
    }
}



//---------------------------Draw Rays and Walls--------------------------------
void drawRays2D()
{
    int r, mx, my, mp, dof, side;
    float vx, vy, rx, ry, ra, xo, yo, disV, disH;

    ra = FixAng(pa + 30);                                                              //ray set back 30 degrees

    for (r = 0; r < 122; r++)
    {
        int vmt = 0, hmt = 0;                                                              //vertical and horizontal map texture number 
        //---Vertical--- 
        dof = 0; side = 0; disV = 100000;
        float Tan = tan(degToRad(ra));
        if (cos(degToRad(ra)) > 0.001) { rx = (((int)px >> 6) << 6) + 64;
        ry = (px - rx) * Tan + py; 
        xo = 64; yo = -xo * Tan; }//looking left

        else if (cos(degToRad(ra)) < -0.001) { 
            rx = (((int)px >> 6) << 6) - 0.0001; 
            ry = (px - rx) * Tan + py; 
            xo = -64; 
            yo = -xo * Tan; }//looking right

        else { rx = px; ry = py; dof = 8; }                                                  //looking up or down. no hit  

        while (dof < 8)
        {
            mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && mapW[mp]>0) { vmt = mapW[mp] - 1; dof = 8; disV = cos(degToRad(ra)) * (rx - px) - sin(degToRad(ra)) * (ry - py); }//hit         
            else { rx += xo; ry += yo; dof += 1; }                                               //check next horizontal
        }
        vx = rx; vy = ry;

        //---Horizontal---
        dof = 0; disH = 100000;
        Tan = 1.0 / Tan;
        if (sin(degToRad(ra)) > 0.001) { ry = (((int)py >> 6) << 6) - 0.0001; rx = (py - ry) * Tan + px; yo = -64; xo = -yo * Tan; }//looking up 
        else if (sin(degToRad(ra)) < -0.001) { ry = (((int)py >> 6) << 6) + 64;      rx = (py - ry) * Tan + px; yo = 64; xo = -yo * Tan; }//looking down
        else { rx = px; ry = py; dof = 8; }                                                   //looking straight left or right

        while (dof < 8)
        {
            mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && mapW[mp]>0) { hmt = mapW[mp] - 1; dof = 8; disH = cos(degToRad(ra)) * (rx - px) - sin(degToRad(ra)) * (ry - py); }//hit         
            else { rx += xo; ry += yo; dof += 1; }                                               //check next horizontal
        }

        float shade = 1;
        glColor3f(0, 0.8, 0);
        if (disV < disH) { hmt = vmt; shade = 0.5; rx = vx; ry = vy; disH = disV; glColor3f(0, 0.6, 0); }//horizontal hit first

        int ca = FixAng(pa - ra); disH = disH * cos(degToRad(ca));                            //fix fisheye 
        int lineH = (mapS * 640) / (disH);
        float ty_step = 32.0 / (float)lineH;
        float ty_off = 0;
        if (lineH > 640) { ty_off = (lineH - 640) / 2.0; lineH = 640; }                            //line height and limit
        int lineOff = 320 - (lineH >> 1);                                               //line offset

        depth[r] = disH;

        //---draw walls---
        int y;
        float ty = ty_off * ty_step + hmt * 32;
        float tx;
        if (shade == 1) { tx = (int)(rx / 2.0) % 32; if (ra > 180) { tx = 31 - tx; } }
        else { tx = (int)(ry / 2.0) % 32; if (ra > 90 && ra < 270) { tx = 31 - tx; } }
        for (y = 0; y < lineH; y++)
        {
            float c = All_Textures[(int)(ty) * 32 + (int)(tx)] * shade;
            if (hmt == 0) { glColor3f(c / 2.0, c / 2.0, c / 2.0); } //checkerboard red
            if (hmt == 1) { glColor3f(c / 2.0, c / 2.0, c / 2.0); } //Brick yellow
            if (hmt == 2) { glColor3f(c / 2.0, c / 2.0, c / 2.0); } //window blue
            if (hmt == 3) { glColor3f(c / 2.0, c, c / 2.0); } //door green
            glPointSize(8); glBegin(GL_POINTS); glVertex2i(r * 8, y + lineOff); glEnd();//draw vertical wall  
            ty += ty_step;
        }

        //---draw floors---
        for (y = lineOff + lineH; y < 640; y++)
        {
            float dy = y - (640 / 2.0), deg = degToRad(ra), raFix = cos(degToRad(FixAng(pa - ra)));
            tx = px / 2 + cos(deg) * 158 * 2 * 32 / dy / raFix;
            ty = py / 2 - sin(deg) * 158 * 2 * 32 / dy / raFix;
            int mp = mapF[(int)(ty / 32.0) * mapX + (int)(tx / 32.0)] * 32 * 32;
            float c = All_Textures[((int)(ty) & 31) * 32 + ((int)(tx) & 31) + mp] * 0.7;
            glColor3f(c / 1.3, c / 1.3, c); glPointSize(8); glBegin(GL_POINTS); glVertex2i(r * 8, y); glEnd();

            //---draw ceiling---
            mp = mapC[(int)(ty / 32.0) * mapX + (int)(tx / 32.0)] * 32 * 32;
            c = All_Textures[((int)(ty) & 31) * 32 + ((int)(tx) & 31) + mp] * 0.7;
            glColor3f(c , c , c ); glPointSize(8); glBegin(GL_POINTS); glVertex2i(r * 8, 640 - y); glEnd();
        }

        ra = FixAng(ra - 0.5);                                                               //go to next ray, 60 total
    }
}



void init()
{
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, 960, 640, 0);
    px = 150; py = 400; pa = 90;
    pdx = cos(degToRad(pa)); pdy = -sin(degToRad(pa));  //init player

    sp[0].type = 1; sp[0].state = 1;sp[0].map= 0;sp[0].x = 1.5 * 64;sp[0].y = 5 * 64;sp[0].z = 20;
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //frames per second
    frame2 = glutGet(GLUT_ELAPSED_TIME);
    fps = (frame2 - frame1);
    frame1 = glutGet(GLUT_ELAPSED_TIME);

    if (Keys.w || Keys.s) {
        moving = 1;
        if (frame2 - lastFrameSwitch > frameDuration) {
            currentFrame = 1 - currentFrame;  // Switch between 0 and 1
            lastFrameSwitch = frame2;
        }
    }
    else {
        moving = 0;
        currentFrame = 0; // Reset to default frame when not moving
    }

    //buttons
    double moveRate = 0.0;
    if (showZoomSprite == 1) moveRate = 0.1; else moveRate = 0.2;                 //slow down when zoom
    if (Keys.a == 1) { pa += moveRate * fps; pa = FixAng(pa); pdx = cos(degToRad(pa)); pdy = -sin(degToRad(pa));}
    if (Keys.d == 1) { pa -= moveRate * fps; pa = FixAng(pa); pdx = cos(degToRad(pa)); pdy = -sin(degToRad(pa)); }

    int xo = 0; if (pdx < 0) { xo = -20; }else { xo = 20; }                                    //x offset to check map
    int yo = 0; if (pdy < 0) { yo = -20; }else { yo = 20; }                                    //y offset to check map
    int ipx = px / 64.0, ipx_add_xo = (px + xo) / 64.0, ipx_sub_xo = (px - xo) / 64.0;         //x position and offset
    int ipy = py / 64.0, ipy_add_yo = (py + yo) / 64.0, ipy_sub_yo = (py - yo) / 64.0;         //y position and offset
    
    if (Keys.w == 1)                                                                  //move forward
    {   
        if (mapW[ipy * mapX + ipx_add_xo] == 0) { px += pdx * moveRate * fps; }
        if (mapW[ipy_add_yo * mapX + ipx] == 0) { py += pdy * moveRate * fps; }
    }
    if (Keys.s == 1)                                                                  //move backward
    {
        if (mapW[ipy * mapX + ipx_sub_xo] == 0) { px -= pdx * moveRate * fps; }
        if (mapW[ipy_sub_yo * mapX + ipx] == 0) { py -= pdy * moveRate * fps; }
    }
    
    drawRays2D();
    drawMonsterSprite();

    
    if (!pressFireSprite || !showZoomSprite) {
        if (showZoomSprite) {
            drawZoomSprite();
        }
        else {
            drawPlayerSprite();
        }
    }
    if (pressFireSprite && showZoomSprite) {
        animationTimer += fps;
        if (animationTimer >= 200) {
            pressFireSprite = 0;
            showZoomSprite = 1;
            animationTimer = 0;
        }
        drawFireSprite();
    }
  
   
    glutPostRedisplay();
    glutSwapBuffers();
}

void ButtonDown(unsigned char key, int x, int y)                                  //keyboard button pressed down
{
    if (key == 'a') { Keys.a = 1; }
    if (key == 'd') { Keys.d = 1; }
    if (key == 'w') { Keys.w = 1; }
    if (key == 's') { Keys.s = 1; }
    if (key == 'q') { showZoomSprite = !showZoomSprite;}
    if (key == 'r' && showZoomSprite == 1) { pressFireSprite= !pressFireSprite; }
    if (key == 'e')             //open doors
    {
        int xo = 0; if (pdx < 0) { xo = -25; }
        else { xo = 25; }
        int yo = 0; if (pdy < 0) { yo = -25; }
        else { yo = 25; }
        int ipx = px / 64.0, ipx_add_xo = (px + xo) / 64.0;
        int ipy = py / 64.0, ipy_add_yo = (py + yo) / 64.0;
        if (mapW[ipy_add_yo * mapX + ipx_add_xo] == 4) { mapW[ipy_add_yo * mapX + ipx_add_xo] = 0; }
    }

    glutPostRedisplay();
}

void ButtonUp(unsigned char key, int x, int y)                                    //keyboard button pressed up
{
    if (key == 'a') { Keys.a = 0; }
    if (key == 'd') { Keys.d = 0; }
    if (key == 'w') { Keys.w = 0; }
    if (key == 's') { Keys.s = 0; }
    glutPostRedisplay();
}

void resize(int w, int h)                                                        //screen window rescaled, snap back
{
    glutReshapeWindow(960, 640);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(960, 640);
    glutInitWindowPosition(glutGet(GLUT_SCREEN_HEIGHT) / 2 -960/2, glutGet(GLUT_SCREEN_HEIGHT) / 2 - 640 / 2);
    glutCreateWindow("Doom");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(ButtonDown);
    glutKeyboardUpFunc(ButtonUp);
    glutMainLoop();
}
