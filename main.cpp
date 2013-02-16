#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define ESCAPE 27
#define SIZE_SPHERE 0.04

#define HEIGHT_HIP 0.4
#define WIDTH_HIP 0.4
#define LENGTH_HIP 0.11

#define WIDTH_BODY 2.0
#define HEIGHT_BODY  0.8
#define LENGTH_BODY  0.5

#define HEIGHT_FEMUR 0.5
#define WIDTH_FEMUR 0.2
#define LENGTH_FEMUR 0.1

#define HEIGHT_SHIN 0.35
#define WIDTH_SHIN 0.1
#define LENGTH_SHIN 0.08

#define HEIGHT_PAW  0.15
#define BASE_PAW 0.1

#define HEIGHT_NECK  0.4
#define WIDTH_NECK 1.4
#define LENGTH_NECK 0.2

#define HEIGHT_HEAD  0.3
#define WIDTH_HEAD 0.4
#define LENGTH_HEAD 0.15

#define LEFT_FRONT 0
#define RIGHT_FRONT 1
#define LEFT_REAR 2
#define RIGHT_REAR 3

#define HIP 0
#define FEMUR 1
#define SHIN 2
#define PAW 3

// angles[estagio][posicao][quadril/femur/canela/pata]
float anglesCaminhada[6][4][4];
float anglesTrote[6][4][4];

int anglePescocoSubindo = 1;

float angleCabeca;
float angleTronco;
float angleRabo = 0.0;
float anglePescoco = 0.0;
float angleCavalo = 0.0;

float deslocamento = 0.0;
float deslocamentoYTronco = 0.0;

int estagio = 0;
int passo = 0; // 0 - 10
int caminhando = 1;
int movimentarCavalo = 1;
int passoRabo = 0;
int passoRaboSubindo = 1;
int iluminacao = 1;

float xCavalo = 0.0;
float zCavalo = 1.5;

void drawEsphere();
void drawFemur(int posicao);
void drawShin(int posicao);
void drawPaw(int posicao);
void drawHead();
void drawHorse();
void initAngles();
void display();
void timer(int value);

GLUquadricObj *params = gluNewQuadric();

static float angle=0.0,ratio;
static float x=0.0f,y=0.75f,z=5.0f;
static float lx=0.0f,ly=0.0f,lz=-1.0f;

int wd; /* GLUT window handle */

void reshape(int w, int h)
{
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;

    ratio = 1.0f * w / h;
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set the clipping volume
    gluPerspective(45,ratio,1,1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x, y, z, x + lx,y + ly,z + lz,0.0f,1.0f,0.0f);
}

void display(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpe a tela e o buffer

    GLfloat diffuseLight[] = { 1, 1, 1, 1};
    GLfloat ambientLight[] = { 1, 1, 1, 1};
    GLfloat specularLight[] = { 0.3, 0.3, 0.3, 0.3};
    GLfloat lightPos[] = { 300.0f, 2000.0f, -20.0f, 1.0f };

    if (iluminacao) {
        glEnable(GL_LIGHTING);
    } else {
        glDisable(GL_LIGHTING);
    }

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, diffuseLight );
    //glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 50);

    // deseha chão
    glPushMatrix();
    glColor3f(0.1, 1, 0.1);
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, 0.1f, -100.0f);
    glVertex3f(-100.0f, 0.1f,  100.0f);
    glVertex3f( 100.0f, 0.1f,  100.0f);
    glVertex3f( 100.0f, 0.1f, -100.0f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(xCavalo, 0.945,zCavalo);
    glRotatef(angleCavalo,0,1,0);

    drawHorse();

    glPopMatrix();
    glutSwapBuffers();
}

void orientMe(float ang)
{
    lx = sin(ang);
    lz = -cos(ang);
    glLoadIdentity();
    gluLookAt(x, y, z, x + lx,y + ly,z + lz, 0.0f,1.0f,0.0f);
}

void moveMeFlat(int i) {
    x = x + i*(lx)*0.1;
    z = z + i*(lz)*0.1;
    glLoadIdentity();
    gluLookAt(x, y, z, x + lx,y + ly,z + lz, 0.0f,1.0f,0.0f);
}

void processNormalKeys(unsigned char key, int x, int y)
{
    switch(key){
        case ESCAPE:
            exit(0);
            break;
        case 'w': {
            float maiorAngulo = caminhando ? 20.0 : 15.0;
            if (anglePescoco > maiorAngulo || anglePescoco < 0.0) {
                anglePescocoSubindo = !anglePescocoSubindo;
            }
            float incremento = caminhando ? 1.5 : 3.0;
            anglePescoco = anglePescocoSubindo ? anglePescoco + incremento : anglePescoco - incremento;
            if(passo < 10) {
                caminhando ? passo +=2 : passo +=3;
                if(estagio==0 || estagio==2)
                    deslocamentoYTronco += 0.01;
                else if(estagio==1 || estagio==3)
                    deslocamentoYTronco -= 0.01;
            } else {
                passo = 0;
                int estagioFinal = 5;
                if(!caminhando)
                    estagioFinal = 3;
                if(estagio < estagioFinal)
                    estagio++;
                else
                    estagio = 0;
            }
            if(movimentarCavalo){
                float deslocamento = caminhando ? 0.03 : 0.12;
                float angleGraus = angleCavalo*(M_PI/180);
                xCavalo += deslocamento*cos(angleGraus);
                zCavalo -= deslocamento*sin(angleGraus);
            }
            break;
        }
        case 'a':
            angleCavalo += 5;
            break;
        case 'd':
            angleCavalo -= 5;
            break;
        case 'r':
            deslocamentoYTronco = 0.0;
            caminhando = !caminhando;
            break;
        case 'p':
            movimentarCavalo = !movimentarCavalo;
            break;
        case 'i':
            iluminacao = !iluminacao;
            break;
    }
}

void processSpecialKeys(int key, int x, int y) {

    switch (key) {
        case GLUT_KEY_LEFT :
            angle -= 0.05f;
            orientMe(angle);
            break;
        case GLUT_KEY_RIGHT :
            angle +=0.05f;
            orientMe(angle);
            break;
        case GLUT_KEY_UP :
            moveMeFlat(5);
            break;
        case GLUT_KEY_DOWN :
            moveMeFlat(-5);
            break;
    }
}

int main(int argc, char **argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(800,600);

    wd = glutCreateWindow("Modelos hierarquicos - Cavalo");

    glEnable(GL_DEPTH_TEST);

    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(100, timer, 0);

    initAngles();
    glutMainLoop();

    return(0);
}

float pegaAngulo(int posicao, int parte, bool posicaoAtual)
{
    int estagioDoAngulo = estagio;
    int estagioFinal = caminhando ? 5 : 3;
    if(!posicaoAtual){
        if(estagioDoAngulo==estagioFinal)
            estagioDoAngulo = 0;
        else
            estagioDoAngulo++;
    }

    float (*vetor)[4][4] = caminhando ? anglesCaminhada : anglesTrote;

    int estagioDeTroca = caminhando ? 2 : 1;
    if(estagioDoAngulo>estagioDeTroca) {
        switch(posicao){
            case LEFT_FRONT:
                return vetor[estagioDoAngulo-estagioDeTroca-1][RIGHT_FRONT][parte];
            case RIGHT_FRONT:
                return vetor[estagioDoAngulo-estagioDeTroca-1][LEFT_FRONT][parte];
            case LEFT_REAR:
                return vetor[estagioDoAngulo-estagioDeTroca-1][RIGHT_REAR][parte];
            case RIGHT_REAR:
                return vetor[estagioDoAngulo-estagioDeTroca-1][LEFT_REAR][parte];
        }
    } else {
        return vetor[estagioDoAngulo][posicao][parte];
    }
    return 0.0;
}

float pegaAngulo(int posicao, int parte)
{
    float fatorPasso = passo/10.0;
    float angleAtual = pegaAngulo(posicao, parte, true);
    float proximoAngulo = pegaAngulo(posicao, parte, false);

    return angleAtual + (proximoAngulo - angleAtual)*fatorPasso;
}

void initAngles()
{
    anglesCaminhada[0][LEFT_REAR][FEMUR] = 7;
    anglesCaminhada[0][LEFT_REAR][SHIN] = -33;
    anglesCaminhada[0][LEFT_REAR][PAW] = 0;
    anglesCaminhada[0][RIGHT_REAR][FEMUR] = 4;
    anglesCaminhada[0][RIGHT_REAR][SHIN] = -4;
    anglesCaminhada[0][RIGHT_REAR][PAW] = 0;
    anglesCaminhada[0][LEFT_FRONT][HIP] = 15;
    anglesCaminhada[0][LEFT_FRONT][FEMUR] = -35;
    anglesCaminhada[0][LEFT_FRONT][SHIN] = 70;
    anglesCaminhada[0][LEFT_FRONT][PAW] = 0;
    anglesCaminhada[0][RIGHT_FRONT][HIP] = -20;
    anglesCaminhada[0][RIGHT_FRONT][FEMUR] = -26;
    anglesCaminhada[0][RIGHT_FRONT][SHIN] = 39;
    anglesCaminhada[0][RIGHT_FRONT][PAW] = 0;

    anglesCaminhada[1][LEFT_REAR][FEMUR] = 43;
    anglesCaminhada[1][LEFT_REAR][SHIN] = -81;
    anglesCaminhada[1][LEFT_REAR][PAW] = 0;
    anglesCaminhada[1][RIGHT_REAR][FEMUR] = -8;
    anglesCaminhada[1][RIGHT_REAR][SHIN] = -6;
    anglesCaminhada[1][RIGHT_REAR][PAW] = -4;
    anglesCaminhada[1][LEFT_FRONT][HIP] = 5;
    anglesCaminhada[1][LEFT_FRONT][FEMUR] = -20;
    anglesCaminhada[1][LEFT_FRONT][SHIN] = 38;
    anglesCaminhada[1][LEFT_FRONT][PAW] = 0;
    anglesCaminhada[1][RIGHT_FRONT][HIP] = -27;
    anglesCaminhada[1][RIGHT_FRONT][FEMUR] = -25;
    anglesCaminhada[1][RIGHT_FRONT][SHIN] = 30;
    anglesCaminhada[1][RIGHT_FRONT][PAW] = 0;

    anglesCaminhada[2][LEFT_REAR][FEMUR] = 35;
    anglesCaminhada[2][LEFT_REAR][SHIN] = -30;
    anglesCaminhada[2][LEFT_REAR][PAW] = 0;
    anglesCaminhada[2][RIGHT_REAR][FEMUR] = -20;
    anglesCaminhada[2][RIGHT_REAR][SHIN] = 0;
    anglesCaminhada[2][RIGHT_REAR][PAW] = 0;
    anglesCaminhada[2][LEFT_FRONT][HIP] = 20;
    anglesCaminhada[2][LEFT_FRONT][FEMUR] = -60;
    anglesCaminhada[2][LEFT_FRONT][SHIN] = 38;
    anglesCaminhada[2][LEFT_FRONT][PAW] = 0;
    anglesCaminhada[2][RIGHT_FRONT][HIP] = 5;
    anglesCaminhada[2][RIGHT_FRONT][FEMUR] = -70;
    anglesCaminhada[2][RIGHT_FRONT][SHIN] = 40;
    anglesCaminhada[2][RIGHT_FRONT][PAW] = 0;

    anglesTrote[0][LEFT_REAR][FEMUR] = 70;
    anglesTrote[0][LEFT_REAR][SHIN] = -70;
    anglesTrote[0][LEFT_REAR][PAW] = -50;
    anglesTrote[0][RIGHT_REAR][FEMUR] = -15;
    anglesTrote[0][RIGHT_REAR][SHIN] = 0;
    anglesTrote[0][RIGHT_REAR][PAW] = -10;
    anglesTrote[0][LEFT_FRONT][HIP] =-5;
    anglesTrote[0][LEFT_FRONT][FEMUR] = -25;
    anglesTrote[0][LEFT_FRONT][SHIN] = 30;
    anglesTrote[0][LEFT_FRONT][PAW] = -20;
    anglesTrote[0][RIGHT_FRONT][HIP] = 33;
    anglesTrote[0][RIGHT_FRONT][FEMUR] = -75;
    anglesTrote[0][RIGHT_FRONT][SHIN] = 95;
    anglesTrote[0][RIGHT_FRONT][PAW] = -05;

    anglesTrote[1][LEFT_REAR][FEMUR] = 45;
    anglesTrote[1][LEFT_REAR][SHIN] = 0;
    anglesTrote[1][LEFT_REAR][PAW] = 0;
    anglesTrote[1][RIGHT_REAR][FEMUR] = 25;
    anglesTrote[1][RIGHT_REAR][SHIN] = -75;
    anglesTrote[1][RIGHT_REAR][PAW] = 15;
    anglesTrote[1][LEFT_FRONT][HIP] = -25;
    anglesTrote[1][LEFT_FRONT][FEMUR] = -35;
    anglesTrote[1][LEFT_FRONT][SHIN] = 75;
    anglesTrote[1][LEFT_FRONT][PAW] = 0;
    anglesTrote[1][RIGHT_FRONT][HIP] = 10;
    anglesTrote[1][RIGHT_FRONT][FEMUR] = -5;
    anglesTrote[1][RIGHT_FRONT][SHIN] = 45;
    anglesTrote[1][RIGHT_FRONT][PAW] = 0;

    angleCabeca = 90.0;
    angleTronco = -3.75;
}

void drawHead()
{
    glPushMatrix();
    glRotatef(45-anglePescoco, 0,0,1);
    glPushMatrix();

    glScalef(WIDTH_NECK,HEIGHT_NECK, LENGTH_NECK);

    glutSolidCube(0.5);

    glPopMatrix();
    glTranslatef(WIDTH_NECK*0.19, -HEIGHT_NECK*0.4,0);
    glRotatef(angleCabeca, 0,0,1);

    glScalef(WIDTH_HEAD,HEIGHT_HEAD, LENGTH_HEAD);

    glutSolidCube(0.5);
    glPopMatrix();
}

void drawTail()
{
    float angle = caminhando ? 0.0 : 5.0;
    float angle2 = caminhando ? passoRabo  : passoRabo/2;

    // primeiro segmento
    glutSolidSphere(SIZE_SPHERE/1.25,8,8);
    glRotatef(90-angle2,0,1,0);
    glRotatef(-45+angle,1,0,0);
    glTranslatef(0,0,-0.20);
    gluCylinder(params,0.020,0.03,0.20,15,2);

    // segundo segmento
    glutSolidSphere(SIZE_SPHERE/1.5,8,8);
    glRotatef(-angle2*2,0,1,0);
    glRotatef(-24+angle/2,1,0,0);
    glTranslatef(0,0,-0.15);
    gluCylinder(params,0.015,0.020,0.15,15,2);

    // terceiro segmento
    glutSolidSphere(SIZE_SPHERE/2,8,8);
    glRotatef(-angle2*3,0,1,0);
    glTranslatef(0,0,-0.25);
    gluCylinder(params,0.0,0.015,0.25,15,2);
}

void drawHip(int posicao)
{
    glPushMatrix();
    glRotatef(pegaAngulo(posicao,HIP),0,0,1);
    glTranslatef(0.0,-SIZE_SPHERE,0.0);
    glPushMatrix();
    glScalef(WIDTH_HIP,HEIGHT_HIP, LENGTH_HIP);

    glRotatef(90,1,0,0);

    gluCylinder(params,0.3,0.3,0.5,15,2);

    glPopMatrix();
    glTranslatef(0.0,-HEIGHT_HIP*0.5-SIZE_SPHERE,0.0);

    drawEsphere();
    drawFemur(posicao);

    glPopMatrix();
}

void drawFemur(int posicao)
{
    glPushMatrix();
    glRotatef(pegaAngulo(posicao, FEMUR),0,0,1);
    glTranslatef(0.0,-SIZE_SPHERE,0.0);
    glPushMatrix();
    glScalef(WIDTH_FEMUR,HEIGHT_FEMUR, LENGTH_FEMUR);

    glRotatef(90,1,0,0);

    gluCylinder(params,0.3,0.3,0.5,15,2);

    glPopMatrix();
    glTranslatef(0.0,-HEIGHT_FEMUR*0.5-SIZE_SPHERE,0.0);

    drawEsphere();

    drawShin(posicao);

    glPopMatrix();
}

void drawShin(int posicao)
{
    glPushMatrix();
    glRotatef(pegaAngulo(posicao,SHIN),0,0,1);
    glTranslatef(0.0,-SIZE_SPHERE,0.0);
    glPushMatrix();
    glScalef(WIDTH_SHIN,HEIGHT_SHIN, LENGTH_SHIN);

    glRotatef(90,1,0,0);

    gluCylinder(params,0.3,0.3,0.5,15,2);

    glPopMatrix();
    glTranslatef(0.0,-HEIGHT_SHIN*0.5-SIZE_SPHERE,0.0);

    drawEsphere();
    drawPaw(posicao);

    glPopMatrix();
}

void drawPaw(int posicao)
{
    glPushMatrix();
    glRotatef(pegaAngulo(posicao, PAW), 0, 0, 1);
    glTranslatef(0.0, -SIZE_SPHERE, 0.0);
    glTranslatef(0.0, -HEIGHT_PAW*0.35, 0.0);
    glScalef(BASE_PAW, HEIGHT_PAW, BASE_PAW);
    glRotatef(-90, 1, 0, 0);

    glutSolidCone(0.5, 0.6, 8, 6);

    glPopMatrix();
}

void drawBody()
{
    if (!caminhando) {
        glTranslatef(0.0,deslocamentoYTronco,0.0);
    }

    glPushMatrix();

    glColor3f(0.4,0.2,0.14);

    glPushMatrix();
    glTranslatef(-WIDTH_BODY*0.2,0,0.0);
    glPushMatrix();
    glScalef(1,HEIGHT_BODY*1.5, 1);

    glutSolidSphere(LENGTH_BODY*0.335,8,8);

    glPopMatrix();
    glScalef(WIDTH_BODY,HEIGHT_BODY, LENGTH_BODY);
    glRotatef(90,0,1,0);

    gluCylinder(params,0.25,0.25,0.4,15,2);

    glPopMatrix();
    glPushMatrix();
    glTranslatef(WIDTH_BODY*0.2,0,0.0);
    glScalef(1,HEIGHT_BODY*1.5, 0.75);

    glutSolidSphere(LENGTH_BODY*0.335,8,8);

    glPopMatrix();

    //glColor3f(0.5, 0.45, 0.4);
    glPopMatrix();
}

void drawLeg(int posicao)
{
    glPushMatrix();
    if (posicao == LEFT_FRONT || posicao == RIGHT_FRONT) {
        drawHip(posicao);
    } else {
        drawFemur(posicao);
    }
    glPopMatrix();
}

void drawEsphere()
{
    glutSolidSphere(SIZE_SPHERE, 8, 8);
}

void drawHorse()
{
    drawBody();

    glPushMatrix();
    glTranslatef(-WIDTH_BODY*0.17, HEIGHT_BODY*0.1, LENGTH_BODY*0.3);

    drawEsphere();

    drawLeg(LEFT_FRONT);

    glPopMatrix();
    glPushMatrix();
    glTranslatef(WIDTH_BODY*0.22, -HEIGHT_BODY*0.2, LENGTH_BODY*0.2);

    drawEsphere();

    drawLeg(LEFT_REAR);

    glPopMatrix();
    glPushMatrix();
    glTranslatef(WIDTH_BODY*0.22, -HEIGHT_BODY*0.2, -LENGTH_BODY*0.2);

    drawEsphere();

    drawLeg(RIGHT_REAR);

    glPopMatrix();
    glPushMatrix();
    glTranslatef(-WIDTH_BODY*0.17, HEIGHT_BODY*0.1, -LENGTH_BODY*0.3);

    drawEsphere();

    drawLeg(RIGHT_FRONT);

    glPopMatrix();
    glPushMatrix();
    glTranslatef(WIDTH_BODY*0.25, HEIGHT_BODY*0.2, 0.0);

    drawHead();

    glPopMatrix();
    glPushMatrix();
    glTranslatef(-WIDTH_BODY*0.28, HEIGHT_BODY*0.1, 0.0);

    drawTail();

    glPopMatrix();
}

void timer(int value)
{
    if(passoRabo > 15){
        passoRaboSubindo = 0;
        if (caminhando) {
            passoRabo--;
        } else {
            passoRabo -= 2;
        }
    } else if(passoRabo < -15){
        passoRaboSubindo = 1;
        if (caminhando) {
            passoRabo++;
        } else {
            passoRabo += 2;
        }
    } else {
        if (caminhando) {
            passoRaboSubindo ? passoRabo++ : passoRabo--;
        } else {
            passoRaboSubindo ? passoRabo += 2 : passoRabo -=2;
        }
    }

    display();
    glutTimerFunc(50, timer, 0);
}
