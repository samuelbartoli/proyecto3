#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include "TriMesh.h"
//#include "XForm.h"
#include <vector>
#include <ctime>
#include <cmath>
#include "nivel.h"

#define SQR(a) ((a)*(a))
#define G 0.1
#define RADIO 1
#define MAX_X 102.5
#define MAX_Z 102.5
#define MIN_X 0
#define MIN_Z 0
#define MIN_Y 2.7

using namespace std;

int rotacion_x = 0;
int rotacion_y = 0;

double sx,sy,sz;
vector<nivel> niveles;
nivel lvlactual;
int vidas,puntaje,enemies;
int init=0;
double lvltime;
clock_t oldtime;
GLUquadric * qobj;


/*vector<TriMesh *> meshes;
vector<xform> xforms;
vector<bool> visible;
vector<string> filenames;*/

//Maneja el reescalamiento de la ventana
void handleResize(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	gluPerspective(45.0,(double)w / (double)h, 1.0, 200.0) ;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//Settea opciones de opengl
void initRendering() {
    glutInitWindowSize (800, 500); 
    glutInitWindowPosition (400, 50);
    glutCreateWindow ("Tron");    
	glEnable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    glClearColor(0.0,0.0,0.0,0.0);
    qobj = gluNewQuadric();
    gluQuadricNormals(qobj, GLU_SMOOTH);
}

//Manejador para la tecla Esc que cierra la ventana
void manejador_teclas(unsigned char key, 
					int x, int y) {
	switch (key) {
    	case 27:    //Tecla Esc 
			exit(0); 
    }
}   

//mover camara
void move_cam(int key, int x, int y){
    switch(key) {
        case GLUT_KEY_RIGHT:
            rotacion_y += 2;
            break;
        case GLUT_KEY_LEFT:
            rotacion_y -= 2;
            break;
        case GLUT_KEY_UP:
            rotacion_x += 2;
            break;
        case GLUT_KEY_DOWN:
            rotacion_x -= 2;
            break;
    }  
    glutPostRedisplay();
}

// Dibuja un cuadrado con los tamanos tamx, tamy , tamz 
// de color r , g, b
// en la posicion posx, posy, posz
// de tipo t , donde :
//  t = 1 : se dibuja un cuadrado paralelo al plano yz
//  t = 2 : se dibuja un cuadrado paralelo al plano xz
//  t = 0 : se dibuja un cuadrado paralelo al plano xy
void dibujarcuadrado(float tamx, float tamy, float tamz, float r, float g, float b, float posx, float posy, float posz, int t, float alpha){
   
    glPushMatrix();
    glTranslatef(posx,posy,posz);
    glBegin(GL_QUADS);
       
        glColor4f(r,g,b,alpha);
        
        switch(t){
        
            case 0: 
                glVertex3f(0,0,0);
                glVertex3f(tamx,0,0);
                glVertex3f(tamx,tamy,0);
                glVertex3f(0,tamy,0);
                break;
            case 1:
                glVertex3f(0,0,0);
                glVertex3f(0,tamy,0);
                glVertex3f(0,tamy,tamz);
                glVertex3f(0,0,tamz);
                break;
            case 2:
                glVertex3f(0,0,0);
                glVertex3f(tamx,0,0);
                glVertex3f(tamx,0,tamz);
                glVertex3f(0,0,tamz);
                break; 
        }

    glEnd();
    glPopMatrix();

}

// Dibuja las aristas del cuadrado de  tamanos tamx, tamy , tamz 
// de color r , g, b
// en la posicion posx, posy, posz
// de tipo t , donde :
//  t = 1 : se dibuja un cuadrado paralelo al plano yz
//  t = 2 : se dibuja un cuadrado paralelo al plano xz
//  t = 0 : se dibuja un cuadrado paralelo al plano xy
void dibujararista(float tamx, float tamy, float tamz, float r, float g, float b, float posx, float posy, float posz, int t, float alpha){
   
    glPushMatrix();
    glTranslatef(posx,posy,posz);
    glLineWidth(1);
    glBegin(GL_LINE_LOOP);
        glColor4f(r,g,b,alpha);
        
        switch(t){
        
            case 0: 
                glVertex3f(0,0,0);
                glVertex3f(tamx,0,0);
                glVertex3f(tamx,tamy,0);
                glVertex3f(0,tamy,0);
                break;
            case 1:
                glVertex3f(0,0,0);
                glVertex3f(0,tamy,0);
                glVertex3f(0,tamy,tamz);
                glVertex3f(0,0,tamz);
                break;
           case 2:
                glVertex3f(0,0,0);
                glVertex3f(tamx,0,0);
                glVertex3f(tamx,0,tamz);
                glVertex3f(0,0,tamz);
                break; 
        }

    glEnd();
    glPopMatrix();

}

//Dibuja el area de juego
void dibujarArena(){
    glPushMatrix();
        dibujarcuadrado(105, 0, 105, 0.7, 0.7, 1, -52.5, -0, -52.5, 2, 1);
        dibujarcuadrado(0, 20, 105, 0.7, 0.7, 1, -52.5, 0, -52.5, 1, 0.5);
        dibujarcuadrado(0, 20, 105, 0.7, 0.7, 1, 52.5, 0, -52.5, 1, 0.5);
        dibujarcuadrado(105, 20, 0, 0.7, 0.7, 1, -52.5, 0 , -52.5, 0, 0.5);
        dibujarcuadrado(105, 20, 0, 0.7, 0.7, 1, -52.5, 0 , 52.5, 0, 0.5);
 
        dibujararista(105, 0, 105, 0, 0, 0, -52.5, -0, -52.5, 2, 1);
        dibujararista(0, 20, 105, 0, 0, 0, -52.5, 0, -52.5, 1, 1);
        dibujararista(0, 20, 105, 0, 0, 0, 52.5, 0, -52.5, 1, 1);
        dibujararista(105, 20, 0, 0, 0, 0, -52.5, 0, -52.5, 0, 1);
        dibujararista(105, 20, 0, 0, 0, 0, -52.5, 0, 52.5, 0, 1);
    glPopMatrix();
}


//dibuja una esfera
void dibujar_esfera(){
    glPushMatrix();
    glutSolidSphere(5,100,100);
    glPopMatrix();
}

//dibujar un cubo
void dibujar_cubo(){
    glPushMatrix();
    glutSolidCube(5);
    glColor3f(0,0,0);
    glutWireCube(5);
    glPopMatrix();
}

//Dibujar los ejes de plano
void dibujarEjes(){
    glPushMatrix();
    glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(3,0,0);
    glColor3f(0,1,0);
    glVertex3f(0,0,0);
    glVertex3f(0,3,0);
    glColor3f(0,0,1);
    glVertex3f(0,0,0);
    glVertex3f(0,0,3);
    glEnd();
    glPopMatrix();
    
}

//Dibuja las aristas del poligono segun el orden de los puntos en "puntos"
void dibujarpoligono(int tam, punto A[]){
    glPushMatrix();
    glLineWidth(1.5);
    glBegin(GL_LINE_LOOP);
        for(int i=0;i<tam;i++){
            glVertex3f(A[i].x,2.5,A[i].z);
        }
    glEnd();
    glPopMatrix();
}

//Movimiento de los Obstaculos
void movimiento(double tiempo){
    
    //Mover los personajes
    punto dest,act,uni,delta;
    float dist,vel;
    for(int i=0; i<lvlactual.nro_jugadores; i++){

        if(lvlactual.player[i].nro_puntos==1) continue;

        act = lvlactual.player[i].ptoact;
        dest = lvlactual.player[i].points[lvlactual.player[i].ptosig];
        vel = lvlactual.player[i].trayec_v*50;

        dist = sqrt(SQR(dest.x-act.x)+SQR(dest.z-act.z));
 
        uni.x = (dest.x-act.x)/dist;
        uni.z = (dest.z-act.z)/dist;

        delta.x = tiempo*vel*uni.x;
        delta.z = tiempo*vel*uni.z;

        if(abs(dest.x-act.x)<=abs(delta.x) && abs(dest.z-act.z)<=abs(delta.z)){
            act.x=dest.x;
            act.z=dest.z;
            if(lvlactual.player[i].ptosig==lvlactual.player[i].nro_puntos-1){
                lvlactual.player[i].ptosig = 0;
            }else{
                lvlactual.player[i].ptosig++;
            }
        }else{
            lvlactual.player[i].ptoact.x += delta.x;
            lvlactual.player[i].ptoact.z += delta.z;
        }

    }  
}

//REUSAR PARA LAS COLISIONES
/*

//Movimiento del Disco y colisiones del mismo
void moverdisco(punto dest, double tiempo){

    punto act,delta;
    float vel,deltav;

    for(int i=0; i<lvlactual.nro_jugadores; i++){
        if(lvlactual.player[i].timer <= 5){
            lvlactual.player[i].timer-=tiempo;

            if(lvlactual.player[i].timer<0){
                lvlactual.player[i].timer=0;
            }
            continue;
        }

        act = lvlactual.player[i].disc.pos;
        vel = lvlactual.player[i].disc.v;
      
        delta.x = tiempo*vel.x;
        delta.y = -(G*SQR(tiempo))/2+vel.y*tiempo;
        delta.z = tiempo*vel.z;
        deltav = -G*tiempo;

        //Colisiones
        
        //Paredes
        //Lados
        if(RADIO + vel.x + delta.x > MAX_X){
            lvlactual.player[i].disc.v.x = -vel.x;
            lvlactual.player[i].disc.pos.x = MAX_X - RADIO;
            lvlactual.player[i].disc.rebotes++;
            if(lvlactual.player[i].disc.rebotes == 16){
                lvlactual.player[i].disc.timer = 5;
                lvlactual.player[i].disc.v.x = 0;
                lvlactual.player[i].disc.v.y = 0;
                lvlactual.player[i].disc.v.z = 0;
                lvlactual.player[i].disc.pos.x = 0;
                lvlactual.player[i].disc.pos.y = 0;
                lvlactual.player[i].disc.pos.z = 0;
                lvlactual.player[i].disc.rebotes = 0;
            }
        }else if(-RADIO + vel.x + delta.x < MIN_X){
            lvlactual.player[i].disc.v.x = -vel.x;
            lvlactual.player[i].disc.pos.x = MIN_X + RADIO;
            lvlactual.player[i].disc.rebotes++;
            if(lvlactual.player[i].disc.rebotes == 16){
                lvlactual.player[i].disc.timer = 5;
                lvlactual.player[i].disc.v.x = 0;
                lvlactual.player[i].disc.v.y = 0;
                lvlactual.player[i].disc.v.z = 0;
                lvlactual.player[i].disc.pos.x = 0;
                lvlactual.player[i].disc.pos.y = 0;
                lvlactual.player[i].disc.pos.z = 0;
                lvlactual.player[i].disc.rebotes = 0;
            }
        }

        //Sup e inf
        if(RADIO + vel.z + delta.z > MAX_Z){
            lvlactual.player[i].disc.v.z = -vel.z;
            lvlactual.player[i].disc.pos.z = MAX_Z - RADIO;
            lvlactual.player[i].disc.rebotes++;
            if(lvlactual.player[i].disc.rebotes == 16){
                lvlactual.player[i].disc.timer = 5;
                lvlactual.player[i].disc.v.x = 0;
                lvlactual.player[i].disc.v.y = 0;
                lvlactual.player[i].disc.v.z = 0;
                lvlactual.player[i].disc.pos.x = 0;
                lvlactual.player[i].disc.pos.y = 0;
                lvlactual.player[i].disc.pos.z = 0;
                lvlactual.player[i].disc.rebotes = 0;
            }
        }else if(-RADIO + vel.z + delta.z < MIN_Z){
            lvlactual.player[i].disc.v.z = -vel.z;
            lvlactual.player[i].disc.pos.z = MIN_Z + RADIO;
            lvlactual.player[i].disc.rebotes++;
            if(lvlactual.player[i].disc.rebotes == 16){
                lvlactual.player[i].disc.timer = 5;
                lvlactual.player[i].disc.v.x = 0;
                lvlactual.player[i].disc.v.y = 0;
                lvlactual.player[i].disc.v.z = 0;
                lvlactual.player[i].disc.pos.x = 0;
                lvlactual.player[i].disc.pos.y = 0;
                lvlactual.player[i].disc.pos.z = 0;
                lvlactual.player[i].disc.rebotes = 0;
            }
        }

        //Piso
        if(-RADIO + vel.y + delta.y < MIN_Y){
            lvlactual.player[i].disc.v.y = -vel.y;
            lvlactual.player[i].disc.pos.y = MIN_Y + RADIO;
            lvlactual.player[i].disc.rebotes++;
            if(lvlactual.player[i].disc.rebotes == 16){
                lvlactual.player[i].disc.timer = 5;
                lvlactual.player[i].disc.v.x = 0;
                lvlactual.player[i].disc.v.y = 0;
                lvlactual.player[i].disc.v.z = 0;
                lvlactual.player[i].disc.pos.x = 0;
                lvlactual.player[i].disc.pos.y = 0;
                lvlactual.player[i].disc.pos.z = 0;
                lvlactual.player[i].disc.rebotes = 0;
            }
        }

        //Objetos
        

        //Personajes
        
    }
}
*/

void update(int entero){
    
    clock_t newtime = clock();
    double deltatime = ((double)(newtime-oldtime))/CLOCKS_PER_SEC;
    oldtime = clock();

    lvltime-=deltatime;

    //Chequeo el tiempo del nivel
    if(lvltime<=0){
        printf("Game Over\n");
        exit(0);
    }

    //Mover personajes
    movimiento(deltatime);

    //Mover discos
    //moverdisco();
    
    glutTimerFunc(25, update, 1);
    glutPostRedisplay();
}

void iniciodenivel(){
    //Cargar el nivel
    lvlactual=niveles[lvlactual.level_id];
    //Cargar los jugadores
    for(int i=0; i<lvlactual.nro_jugadores; i++){
        lvlactual.player[i].ptoact = lvlactual.player[i].points[0];
        lvlactual.player[i].ptosig = 1;
        lvlactual.player[i].timer = 0;
    }
    //Inicializar las vidas
        vidas = 3;
    //Inicializar el puntaje
        puntaje = 0;
    //Inicializar el tiempo del nivel
        lvltime = lvlactual.game_time;
    //Inicializar el Contador de enemigos
        enemies = lvlactual.nro_jugadores-1;
    //Tiempo actual que sera el tiempo viejo xD
        oldtime = clock()/CLOCKS_PER_SEC;

    //print_nivel(lvlactual);
    glutPostRedisplay(); 
}

//Funcion que dibuja la escena
void dibujar_escena(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 130.0, 0.0, 0.0, -100.0, 0.0, 1.0, 0.0);
    glPushMatrix();
    glRotatef(rotacion_x,1,0,0);  
    glRotatef(rotacion_y,0,1,0);  

    //Inicializar
    if(init){
        iniciodenivel();
    }

    glPushMatrix();
    glTranslatef(-50,2.7,-50);
    //Dibujar Personajes
    punto dib;
    for(int i=0; i < lvlactual.nro_jugadores; i++){

        glPushMatrix();
            switch(i){
                case 0:
                    glColor3f(0,0,1);
                    break;
                case 1:
                    glColor3f(1,0,0);
                    break;
                case 2:
                    glColor3f(0,1,0);
                    break;
                case 3:
                    glColor3f(1,0.6,0);
                    break;
            }

            dibujarpoligono(lvlactual.player[i].nro_puntos, lvlactual.player[i].points);

            dib = lvlactual.player[i].ptoact;
            glPushMatrix();
                glTranslatef(dib.x,dib.y,dib.z);
                //Aqui se carga la maya
                dibujar_cubo();
            glPopMatrix();
            
        glPopMatrix();
    }

    //Dibujar Obstaculos
    for(int i=0; i < lvlactual.objs.size(); i++){
        dib = lvlactual.objs[i].points;
        glPushMatrix();
        glColor3f(1,1,1);
            if(lvlactual.objs[i].tipo == 0){
                //Cargar la malla del obstaculo
                glTranslatef(dib.x,dib.y,dib.z);
                dibujar_cubo();
            }else if(lvlactual.objs[i].tipo == 1){
                glTranslatef(dib.x,dib.y,dib.z);
                dibujar_cubo();
            }else if(lvlactual.objs[i].tipo == 2){
                glTranslatef(dib.x,dib.y,dib.z);
                dibujar_esfera();
            }
        glPopMatrix();
    }

    //Dibujar Discos
    for(int i=0; i < lvlactual.nro_jugadores; i++){
        if(lvlactual.player[i].timer <= 5){
            continue;
        }
        dib = lvlactual.player[i].disc.pos; 
        glPushMatrix();
        switch(i){
                case 0:
                    glColor3f(0,0,1);
                    break;
                case 1:
                    glColor3f(1,0,0);
                    break;
                case 2:
                    glColor3f(0,1,0);
                    break;
                case 3:
                    glColor3f(1,0.6,0);
                    break;
        }
        glTranslatef(dib.x, dib.y,dib.z);
        dibujarDisco();    
        glPopMatrix();
    }
    glPopMatrix();
    dibujarEjes();
    
    
    //Dibujar el Tablero
    dibujarArena();
    glPopMatrix();

    //Dibuja un
    if(lvlactual.player[0].timer == 0){
        glPushMatrix();
        glColor3f(1,1,1);
        glTranslatef(-30,-18,80);
        glScalef(0.3,0.3,0.3);
        dibujar_esfera();
        glPopMatrix();
    }

	glutSwapBuffers();
}

int main(int argc, char* argv[]) {

    if(argc<2){
        printf("No se especifico ningun archivo de configuracion\nUso: .\\tron <archivo de configuracion>\n");
        exit(0);
    }

    ParserFile(argv[1],&niveles);
    glutInit(&argc,argv);
	initRendering();
/*
    const char *filename = argv[1];
    TriMesh *themesh = TriMesh::read(filename);
    //if(!themesh)
      //  usage(argv[0]);
    themesh->need_normals();
    themesh->need_tstrips();
    themesh->need_bsphere();
    meshes.push_back(themesh);
    xforms.push_back(xform());
    visible.push_back(true);
    filenames.push_back(filename); 
*/
    glutDisplayFunc(dibujar_escena);
    glutKeyboardFunc(manejador_teclas);
    glutSpecialFunc(move_cam);
	glutReshapeFunc(handleResize);
    glutMouseFunc(processMouse);

    iniciodenivel();
    glutTimerFunc(25,update,1);
    glutMainLoop(); 
	return 0;
}
