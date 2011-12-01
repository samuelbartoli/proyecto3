#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "TriMesh.h"
#include <vector>
#include <ctime>
#include <cmath>
#include "nivel.h"

#define SQR(a) ((a)*(a))
#define G 0.1
using namespace std;

int rotacion_x = 0;
int rotacion_y = 0;

double sx,sy,sz;
vector<nivel> niveles;
vector<punto> jstela;

nivel lvlactual;
int vidas,puntaje,enemies,cam_mode,dir,vstate;
int init=0;
double lvltime,vtop,vmax;
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
	gluPerspective(45.0,(double)w / (double)h, 1, 1000.0) ;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//Settea opciones de opengl
void initRendering() {
    glutInitWindowSize (1300, 700); 
    glutInitWindowPosition (20, 20);
    glutCreateWindow ("Tron");    
	//gluPerspective(45.0, 16/9, 1.0, 1) ;
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
        case 100:   //Tecla d
            cam_mode = 0;
            break;
        case 114:   //Tecla r
            cam_mode = 1;
            break;
        case 101:   //Tecla e
            cam_mode = 2;
            break;
        case 116:   //Tecla t
            cam_mode = 3;
            break;
        case 104:   //Tecla h
        case 32:    //Barra espaciadora
            vstate = 1;
            break;
    	case 27:    //Tecla Esc 
			exit(0);
            break; 
    }
}   

//mover jugador
void move_player(int key, int x, int y){
    switch(key) {
        case GLUT_KEY_RIGHT:
            if(dir!=3){ 
                dir = 1;
            }
            break;
        case GLUT_KEY_LEFT:
            if(dir!=1){
                dir=3;
            }
            break;
        case GLUT_KEY_UP:
            if(dir!=2){
                dir=0;
            }
            break;
        case GLUT_KEY_DOWN:
            if(dir!=0){
                dir=2;
            }
            break;
    }
  
    (lvlactual.player[0].estela).push_back(lvlactual.player[0].ptoact);
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
        dibujarcuadrado(lvlactual.Area.x + 5, 0, lvlactual.Area.z+5, 0.7, 0.7, 1, -(lvlactual.Area.x+5)/2, -0, -(lvlactual.Area.z+5)/2, 2, 1);
        dibujarcuadrado(0, 20, lvlactual.Area.z+5, 0.7, 0.7, 1, -(lvlactual.Area.x+5)/2, 0, -(lvlactual.Area.z+5)/2, 1, 0.5);
        dibujarcuadrado(0, 20, lvlactual.Area.z +5 , 0.7, 0.7, 1, (lvlactual.Area.x+5)/2, 0, -(lvlactual.Area.z+5)/2, 1, 0.5);
        dibujarcuadrado(lvlactual.Area.x+5, 20, 0, 0.7, 0.7, 1, -(lvlactual.Area.x+5)/2, 0 , -(lvlactual.Area.z+5)/2, 0, 0.5);
        dibujarcuadrado(lvlactual.Area.x+5, 20, 0, 0.7, 0.7, 1, -(lvlactual.Area.x+5)/2, 0 , (lvlactual.Area.z+5)/2, 0, 0.5);
 
        dibujararista(lvlactual.Area.x+5, 0, lvlactual.Area.z+5, 0, 0, 0, (lvlactual.Area.x+5)/2, -0, -(lvlactual.Area.z+5)/2, 2, 1);
        dibujararista(0, 20, lvlactual.Area.z+5, 0, 0, 0, -(lvlactual.Area.x+5)/2, 0, -(lvlactual.Area.z+5)/2, 1, 1);
        dibujararista(0, 20, lvlactual.Area.z+5, 0, 0, 0, (lvlactual.Area.x+5)/2, 0, -(lvlactual.Area.z+5)/2, 1, 1);
        dibujararista(lvlactual.Area.x+5, 20, 0, 0, 0, 0, -(lvlactual.Area.x+5)/2, 0, -(lvlactual.Area.z+5)/2, 0, 1);
        dibujararista(lvlactual.Area.x+5, 20, 0, 0, 0, 0, -(lvlactual.Area.x+5)/2, 0, (lvlactual.Area.z+5)/2, 0, 1);
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

void auxestela(int r, int g, int b, punto u, punto v, float alpha){
    glPushMatrix();
    glLineWidth(1.5);
    glColor4f(r,g,b,alpha);
    glBegin(GL_QUADS);
    glVertex3f(u.x,u.y,u.z);
    glVertex3f(v.x,v.y,v.z);
    glVertex3f(v.x,v.y+5,v.z);
    glVertex3f(u.x,u.y+5,u.z);
    glEnd();
    glPopMatrix();
}

//Dibuja la estela 
void dibujarestela(){
    punto aux;
    int i;
    for(i = 0; i < lvlactual.player[0].estela.size()-2; i++){
            auxestela(1,0,0, lvlactual.player[0].estela[i], 
                             lvlactual.player[0].estela[i+1], 0.5);
    }

    switch(dir){
        case 0:
            aux.x = lvlactual.player[0].estela[i+1].x;
            aux.z = lvlactual.player[0].estela[i+1].z+3;
            auxestela(1,0,0, lvlactual.player[0].estela[i], 
                             aux, 0.5);
            break;
        case 1:
            aux.x = lvlactual.player[0].estela[i+1].x-3;
            aux.z = lvlactual.player[0].estela[i+1].z;
            auxestela(1,0,0, lvlactual.player[0].estela[i], 
                             aux, 0.5);
            break;
        case 2:
            aux.x = lvlactual.player[0].estela[i+1].x;
            aux.z = lvlactual.player[0].estela[i+1].z-3;
            auxestela(1,0,0, lvlactual.player[0].estela[i], 
                             aux, 0.5);
            break;
        case 3:
            aux.x = lvlactual.player[0].estela[i+1].x+3;
            aux.z = lvlactual.player[0].estela[i+1].z;
            auxestela(1,0,0, lvlactual.player[0].estela[i], 
                             aux, 0.5);
            break;
    }
    
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

//Movimiento
void movimiento(double tiempo){
    
    //Mover los obstaculos
    punto dest,act,uni,delta,deltav;
    float dist,vel,ace;

    //Mover el jugador
    act = lvlactual.player[0].ptoact;
    ace = lvlactual.player[0].aceleration;
    vel = lvlactual.player[0].vact*50;

    delta.x = tiempo*vel+(ace*SQR(tiempo))/2;
    delta.y = ace*tiempo;

    if(vstate == 0){
        if(vel+delta.y>vtop){
            lvlactual.player[0].vact = vtop;    //aceleracion hasta vtop
            vstate = 4;
        }else{
            lvlactual.player[0].vact += delta.y;
        }
    }else if(vstate == 1){
        if(vel+delta.y>vmax){
            lvlactual.player[0].vact = vmax;    //aceleracion hasta vmax
            vstate = 4;
        }else{
            lvlactual.player[0].vact += delta.y;
        }
    }else if(vstate == 2){
        if(vel-delta.y<vtop){
            lvlactual.player[0].vact = vtop;    //desaceleracion hasta vtop
            vstate = 4;
        }else{
            lvlactual.player[0].vact -= delta.y;
        }
    }

    switch(dir){
        case 0:
            lvlactual.player[0].ptoact.z -= delta.x;
            break;
        case 1:
            lvlactual.player[0].ptoact.x += delta.x;
            break;
        case 2:
            lvlactual.player[0].ptoact.z += delta.x;
            break;
        case 3:
            lvlactual.player[0].ptoact.x -= delta.x;
            break;
    }
    
    lvlactual.player[0].estela[lvlactual.player[0].estela.size()-1]=lvlactual.player[0].ptoact;

    //Mover los obstaculos
    for(int i=0; i<lvlactual.objs.size(); i++){

        if(lvlactual.objs[i].nro_puntos==1) continue;

        act = lvlactual.objs[i].ptoact;
        dest = lvlactual.objs[i].points[lvlactual.objs[i].ptosig];
        vel = lvlactual.objs[i].trayec_v*50;

        dist = sqrt(SQR(dest.x-act.x)+SQR(dest.z-act.z));
 
        uni.x = (dest.x-act.x)/dist;
        uni.z = (dest.z-act.z)/dist;

        delta.x = tiempo*vel*uni.x;
        delta.z = tiempo*vel*uni.z;

        if(abs(dest.x-act.x)<=abs(delta.x) && abs(dest.z-act.z)<=abs(delta.z)){
            act.x=dest.x;
            act.z=dest.z;
            if(lvlactual.objs[i].ptosig==lvlactual.objs[i].nro_puntos-1){
                lvlactual.objs[i].ptosig = 0;
            }else{
                lvlactual.objs[i].ptosig++;
            }
        }else{
            lvlactual.objs[i].ptoact.x += delta.x;
            lvlactual.objs[i].ptoact.z += delta.z;
        }

    }  
}

//colisiones
void colisiones(double tiempo){

    punto act,delta;
    float vel,ace;

    //for(int i=0; i<lvlactual.nro_jugadores; i++){

        act = lvlactual.player[0].ptoact;
        ace = lvlactual.player[0].aceleration;
        vel = lvlactual.player[0].vact*50;

        delta.x = tiempo*vel+(ace*SQR(tiempo))/2;
        delta.y = ace*tiempo;


        //Paredes
        switch(dir){
        case 0:
            if(act.z - delta.x < 0){ // pared superior
                lvlactual.player[0].ptoact.z = 0;
                cout << "juego terminado presione cualquier tecla" << endl;
                scanf(":");
                exit(0);
                
            }
            break;
        case 1:
            if(act.x + delta.x > lvlactual.Area.x){ //pared derecha
                lvlactual.player[0].ptoact.x = lvlactual.Area.x;
                cout << "juego terminado presione cualquier tecla" << endl;
                scanf(":");
                exit(0);
            }
            break;
        case 2:
            if(act.z + delta.x > lvlactual.Area.z){ //pared inferior
                lvlactual.player[0].ptoact.z = lvlactual.Area.z;
                cout << "juego terminado presione cualquier tecla" << endl;
                scanf(":");
                exit(0);
            }
            break;
        case 3:
            if(act.x - delta.x < 0){ //pared izquierda
                lvlactual.player[0].ptoact.x = 0;
                cout << "juego terminado presione cualquier tecla" << endl;
                scanf(":");
                exit(0);
            }
            break;
        }



        //Objetos



    //}
}

void update(int entero){
    
    clock_t newtime = clock();
    double deltatime = ((double)(newtime-oldtime))/CLOCKS_PER_SEC;
    oldtime = clock();

    //Manejo de colisiones
    colisiones(deltatime);
    //Mover obstaculos, jugador
    movimiento(deltatime);
    
    glutTimerFunc(25, update, 1);
    glutPostRedisplay();
}

void iniciodenivel(){
    //Cargar el nivel
    lvlactual=niveles[lvlactual.level_id];
    //Cargar los jugadores
    for(int i=0; i<lvlactual.nro_jugadores; i++){
        if(i==0){
            lvlactual.player[i].ptoact.x = 0; //pto inicio del jugador
            lvlactual.player[i].ptoact.z = lvlactual.Area.z; //pto inicio del jugador
            vmax = lvlactual.player[i].trayec_v*2;
            vtop = lvlactual.player[i].trayec_v;
            lvlactual.player[i].vact = 0;
            vstate=0;
            (lvlactual.player[i].estela).push_back(lvlactual.player[i].ptoact);
            (lvlactual.player[i].estela).push_back(lvlactual.player[i].ptoact);
        }

        lvlactual.player[i].ptosig = 1;
    }
    //Inicializa la trayectoria del jugador
    dir=0;
    //Inicializar las trayectorias de los obstaculos
    for(int i=0; i<lvlactual.objs.size();i++){
        if(lvlactual.objs[i].nro_puntos>1){
            lvlactual.objs[i].ptosig = 1;
        }
    }
    //Inicializar las vidas
        vidas = 3;
    //Inicializar el puntaje
        puntaje = 0;
    //Inicializar el Contador de enemigos
        enemies = lvlactual.nro_jugadores-1;
    //Tiempo actual que sera el tiempo viejo xD
        oldtime = clock()/CLOCKS_PER_SEC;
    //Modo de camara por default
        cam_mode=0;

    //print_nivel(lvlactual);
    glutPostRedisplay(); 
}

//Funcion que dibuja la escena
void dibujar_escena(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if(cam_mode==0){
        //gluLookAt(0,80,lvlactual.Area.z+15, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        gluLookAt(0,280,0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);
    }else if(cam_mode==1){
        
    }else if(cam_mode==2){

    }else if(cam_mode==3){

    } 

    //Inicializar
    if(init){
        iniciodenivel();
    }

    glPushMatrix();
    glTranslatef(-lvlactual.Area.x/2,2.7,-lvlactual.Area.z/2);
    //Dibujar Personajes
    punto dib;
    for(int i=0; i < lvlactual.nro_jugadores; i++){
        glPushMatrix();
            switch(i){
                case 0: //Jugado de color azul
                    glColor3f(0,0,1);
                    break;
                case 1: //Contrincantes de color rojo
                    glColor3f(1,0,0);
                    break;
                case 2:
                    glColor3f(0,1,0);
                    break;
                case 3:
                    glColor3f(1,0.6,0);
                    break;
            }

            dib = lvlactual.player[i].ptoact;
            glPushMatrix();
                glTranslatef(dib.x,dib.y,dib.z);
                //Aqui se carga la maya. mientras se dibujan cubos como
                //personajes
                dibujar_cubo();
            glPopMatrix();
            
        glPopMatrix();
    }

    //Crear la estela
    dibujarestela();

    dib.x = 0;
    dib.y = 0;
    dib.z = 0;

    //Dibujar Obstaculos
    for(int i=0; i < lvlactual.objs.size(); i++){
        dib = lvlactual.objs[i].ptoact;
        glPushMatrix();
        glColor3f(1,1,1);
                //Cargar la malla del obstaculo, mientras se dibuja una esfera
                glTranslatef(dib.x,dib.y,dib.z);
                dibujar_esfera();
        glPopMatrix();
    }

    glPopMatrix();
    dibujarEjes();
    
    
    //Dibujar el Tablero
    dibujarArena();

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

/*    const char *filename = argv[2];
    TriMesh *themesh = TriMesh::read(argv[2]);
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
    glutSpecialFunc(move_player);
	glutReshapeFunc(handleResize);

    iniciodenivel();
    glutTimerFunc(25,update,1);
    glutMainLoop(); 
	return 0;
}
