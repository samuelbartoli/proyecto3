#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <list>
#include <stdlib.h>
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "nivel.h"

using namespace rapidxml;
using namespace std;

ostream& operator<<(ostream& os, const punto& pt){
    os << "x: "<< pt.x << " y: " << pt.y << " z: " << pt.z <<endl;
    return os;
}

void ParserFile(char *filename, vector<nivel> * niveles){

    int i;
    string input_xml;
    string linea;
    ifstream archivo(filename);
    while(getline(archivo,linea))
        input_xml += linea;

    vector<char> buffer(input_xml.begin(), input_xml.end());
    buffer.push_back('\0');
    rapidxml::xml_document<> doc;
    doc.parse<parse_declaration_node | parse_no_data_nodes>(&buffer[0]);
    
    xml_node<> *nodo = doc.first_node("proyecto");

    int nroniveles = atoi(nodo->first_node("niveles")->value());
 
    nodo = nodo->first_node("nivel");  
    
    //Recorremos los niveles
    for(int i = 0; i < nroniveles ; i++, nodo = nodo->next_sibling("nivel")){
        //declaramos un nivel
        nivel cn ;
        cn.level_id = atoi(nodo->first_node("id")->value());

        cn.Area.x = atoi(nodo->first_node("tablero")->first_node("ancho")->value());
        cn.Area.z = atoi(nodo->first_node("tablero")->first_node("largo")->value());
        
        //JUGADOR
        jugadores jugador;

        jugador.trayec_v = atof(nodo->first_node("jugador")->first_node("Vi")->value());
        jugador.aceleration= atof(nodo->first_node("jugador")->first_node("Aceleration")->value());
        jugador.ataque = 0;

        //guardamos al jugador
        cn.player[0] = jugador ;
        
        //CONTRINCANTES
        xml_node<> *nodocon;
        nodocon = nodo->first_node("contrincantes");
        
        int ncontrincantes = atoi(nodocon->value());
        nodocon = nodo->first_node("contrincante");
        
        cn.nro_jugadores = ncontrincantes + 1;

        //recorremos los contrincantes
        for(int k = 1; k <= ncontrincantes ; k ++ , nodocon = nodocon->next_sibling("contrincante")){
    
            jugadores cont;       
     
            cont.trayec_v = atof(nodocon->first_node("Vi")->value());
            cont.aceleration = atof(nodocon->first_node("Aceleration")->value());
            cont.ptoact.x = atof(nodocon->first_node("Inicio")->first_node("x")->value());
            cont.ptoact.y = 0;
            cont.ptoact.z = atof(nodocon->first_node("Inicio")->first_node("y")->value());
              
 
            int ataque = atoi(nodocon->first_node("Ataque")->first_node("tipo")->value()); 
            if(ataque == 1){
                cont.ataque = 1;
                cont.nro_puntos = 0;
                cn.player[k] = cont;
            }
            else{
                cont.ataque = 2;
                cont.nro_puntos = atoi(nodocon->first_node("Ataque")->first_node("puntos")->value());
                //recorremos los puntos
                xml_node<> * nodopuntos;
                nodopuntos = nodocon->first_node("Ataque")->first_node("punto");
                for(int l = 0; l < cont.nro_puntos ; l++){
                    (cont.points[l]).x = atof(nodopuntos->first_node("x")->value());
                    (cont.points[l]).y = 0;
                    (cont.points[l]).z = atof(nodopuntos->first_node("y")->value());
                    nodopuntos = nodopuntos->next_sibling("punto");
                }
                cn.player[k] = cont ; 
            }
        }

        //OBSTACULOS
        xml_node<> *objetos ;
        objetos = nodo->first_node("obstaculos");
    
        int nobjetos = atoi(objetos->value());
        
        objetos = nodo->first_node("obstaculo");

        for(int j = 0; j < nobjetos ; j++ , objetos = objetos->next_sibling("obstaculo")){
            
            obstaculos obs ;
            obs.archivo = objetos->first_node("archivo")->value();
            obs.trayec_v = atof(objetos->first_node("trayectoria")->first_node("velocidad")->value()); 

            obs.nro_puntos = atof(objetos->first_node("trayectoria")->first_node("puntos")->value());

            //Nodo auxiliar para recorrer los obstaculos
            xml_node<> * aux;
            aux = objetos->first_node("trayectoria")->first_node("punto");

            obs.ptoact.x = atof(aux->first_node("x")->value());
            obs.ptoact.y = 0;
            obs.ptoact.z = atof(aux->first_node("y")->value());

            for(int p = 0; p < obs.nro_puntos; p++, aux = aux->next_sibling("punto")){
                (obs.points[p]).x = atof(aux->first_node("x")->value());
                (obs.points[p]).y = 0;
                (obs.points[p]).z = atof(aux->first_node("y")->value());
            }
 
            //Insertar el obstaculo en el vector de obstaculos
            (cn.objs).push_back(obs);
        }
   
        //Insertamos el nivel en el vector de niveles 
        niveles->push_back(cn);   
    }

}

void print_obstaculos(obstaculos m){
        cout << "Archivo: " << m.archivo << endl;
        printf("\tvelocidad de trayectoria %f\n", m.trayec_v );
        printf("Puntos\n");
        for(int i = 0; i < m.nro_puntos ; i++){
            cout << m.points[i];
        }
}   

void print_contrincantes(jugadores m){
    printf("\tvelocidad de trayectoria %f\n", m.trayec_v );
    printf("Aceleracion: %f\n", m.aceleration );
    cout <<"Inicio  " << m.ptoact ; 

    if(m.ataque == 1)
        printf("Ataque: Heuristica\n");
    else{
        printf("Puntos\n");
        for(int i = 0; i < m.nro_puntos ; i++){
            cout << m.points[i];
        }
    }
}

void print_jugador(jugadores m){
    printf("velocidad de trayectoria: %f\n", m.trayec_v );
    printf("Aceleracion: %f\n", m.aceleration );
}

void print_nivel(vector<nivel> l){
    for(int i = 0 ; i < l.size() ; i++){
        printf("\nnivel id: %d\n", l[i].level_id);
        printf("nro jugadores: %d\n", l[i].nro_jugadores);
        cout << "Area " << l[i].Area ;
        
        printf("--- JUGADOR\n");
        print_jugador(l[i].player[0]);

        int j = 1;
        printf("\t--- CONTRINCANTES \n");
        for(int j = 1; j < l[i].nro_jugadores; j++ ){
        //    printf("el contrincante %d tiene %d puntos \n", j,(int) l[i].player[j].nro_puntos);
            print_contrincantes(l[i].player[j]);
        }

        printf("\t--- OBSTACULOS: %d\n",(int) l[i].objs.size());
        for(int k = 0; k < (int)l[i].objs.size() ; k++){
            print_obstaculos(l[i].objs[k]);
        }
    }
}

void print_nivel(nivel l){
        printf("\nnivel id: %d\n", l.level_id);
        printf("nro jugadores: %d\n", l.nro_jugadores); 
        
        printf("--- JUGADOR\n");
        print_jugador(l.player[0]);

        int j = 1;
        printf("\t--- CONTRINCANTES \n");
        for(int j = 1; j < l.nro_jugadores; j++ ){
            //printf("el contrincante %d tiene %d puntos \n", j,(int) l.player[j].nro_puntos);
            print_contrincantes(l.player[j]);
        }

        printf("\t--- OBSTACULOS: %d\n",(int) l.objs.size());
        for(int k = 0; k < (int)l.objs.size() ; k++){
            print_obstaculos(l.objs[k]);
        }
}

/*
int main (int argc, char * argv[]){
    vector<nivel> niveles;   

    ParserFile("config.xml",&niveles);
    int tamano = niveles.size();

    print_nivel(niveles);
}
*/
