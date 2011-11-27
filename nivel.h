#include <vector>
#include <string.h>
#include <iostream>

using namespace std;

typedef struct{
    float x;
    float y;
    float z;
}punto;

typedef struct obstaculos{
    string archivo;
    //Trimesh mesh;
    float trayec_v;
    punto ptoact;
    int ptosig;
    int nro_puntos;
    punto points[50];
}obstaculos;

typedef struct jugadores{
    float trayec_v;
    float aceleration;
    punto ptoact;
    int ptosig;
    int ataque;
    int nro_puntos;
    punto points[50];

    public:
    jugadores(){};
    jugadores(int A):nro_puntos(A){}
    jugadores(jugadores &A){
        trayec_v = A.trayec_v;
        aceleration = A.aceleration;
        ptoact = A.ptoact;
        ptosig = A.ptosig;
        ataque = A.ataque;
        nro_puntos = A.nro_puntos;
        memcpy(points,A.points,sizeof(punto)*A.nro_puntos);
    }
}jugadores;

typedef struct nivel{
    int level_id;
    punto Area;
    int nro_jugadores;
    jugadores player[4];
    std::vector<obstaculos> objs;  
   
    public:
    nivel(const nivel &A){
        level_id = A.level_id;
        Area = A.Area;
        nro_jugadores = A.nro_jugadores;
        memcpy(player, A.player, sizeof(jugadores)*4);
        objs = A.objs ;
    }
    nivel(){
        level_id=0;
        memset(player,0,sizeof(jugadores)*4);
    }
    
}nivel;

void ParserFile(char *, vector<nivel> *);
void print_obstaculos(obstaculos);
void print_contrincantes(jugadores);
void print_jugador(jugadores);
void print_nivel(vector<nivel>);
void print_nivel(nivel);
ostream& operator<<(ostream& os, const punto& pt);
