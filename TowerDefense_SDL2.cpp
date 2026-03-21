#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

const int ANCHO         = 1200;
const int ALTO          = 700;
const int PANEL_X       = 830;
const int PANEL_W       = 370;
const int HUD_H         = 52;
const int LOG_H         = 110;
const int JUEGO_H       = ALTO - HUD_H - LOG_H;

const int LIMITE_RUTA   = 20;
const int MAX_TORRES    = 20;   // max 1 torre por casilla
const int CELDA_PX      = 37;
const int RUTA_Y_C      = HUD_H + JUEGO_H / 2;
const int RUTA_ALTO     = 50;
const int RUTA_INI_X    = 15;

SDL_Color C_BG       = {15, 16, 24, 255};
SDL_Color C_PANEL    = {22, 23, 36, 255};
SDL_Color C_BORDE    = {55, 58, 90, 255};
SDL_Color C_RUTA     = {35, 40, 58, 255};
SDL_Color C_RUTA_B   = {50, 56, 80, 255};
SDL_Color C_TEXTO    = {215,218,232,255};
SDL_Color C_SEC      = {120,124,155,255};
SDL_Color C_VERDE    = {52, 211,153,255};
SDL_Color C_NARANJA  = {251,146, 60,255};
SDL_Color C_ROJO     = {239, 68, 68,255};
SDL_Color C_MORADO   = {167,100,230,255};
SDL_Color C_ORO      = {251,191, 36,255};
SDL_Color C_AZUL     = { 96,165,250,255};
SDL_Color C_SEL      = { 35, 45, 75,255};
SDL_Color C_RANGO    = { 52,211,153, 28};

struct Torre {
    int    id;
    string nombre;
    string tipo;
    int    posicion;
    int    danio;
    int    rango;
    int    costo;
};

struct Enemigo {
    int    id;
    string tipo;
    int    vida;
    int    vidaMax;
    int    velocidad;
    int    posicion;
    int    recompensa;
};

struct Oleada {
    int idOleada;
    int cantNormales;
    int cantRapidos;
    int cantTanques;
    int vidaNormal;
    int vidaRapido;
    int vidaTanque;
};

struct NodoEnemigo {
    Enemigo      dato;
    NodoEnemigo* anterior;
    NodoEnemigo* siguiente;
    NodoEnemigo(Enemigo e): dato(e), anterior(nullptr), siguiente(nullptr){}
};

struct NodoOleada {
    Oleada      dato;
    NodoOleada* siguiente;
    NodoOleada(Oleada o): dato(o), siguiente(nullptr){}
};

class ListaTorres {
    Torre datos[MAX_TORRES];
    int   cantidad;
    int   nextId;
public:
    ListaTorres(): cantidad(0), nextId(1){}

    string insertar(string nombre, string tipo, int pos,
                    int danio, int rango, int costo){
        if(cantidad >= MAX_TORRES)
            return "Limite alcanzado (max "+to_string(MAX_TORRES)+")";
        if(pos < 0 || pos > LIMITE_RUTA-1)
            return "Posicion invalida (0 a "+to_string(LIMITE_RUTA-1)+")";
        for(int i=0;i<cantidad;i++)
            if(datos[i].posicion==pos)
                return "Casilla "+to_string(pos)+" ya ocupada";
        Torre t; t.id=nextId++; t.nombre=nombre; t.tipo=tipo;
        t.posicion=pos; t.danio=danio; t.rango=rango; t.costo=costo;
        datos[cantidad++]=t;
        return "";
    }

    bool eliminar(int id){
        for(int i=0;i<cantidad;i++) if(datos[i].id==id){
            for(int j=i;j<cantidad-1;j++) datos[j]=datos[j+1];
            cantidad--; return true;
        }
        return false;
    }

    bool casillaLibre(int pos) const {
        for(int i=0;i<cantidad;i++) if(datos[i].posicion==pos) return false;
        return true;
    }

    int    contar()         const { return cantidad; }
    Torre& obtener(int idx)       { return datos[idx]; }
};

class ListaEnemigos {
    NodoEnemigo* primero;
    NodoEnemigo* ultimo;
    int          cantidad;
    int          nextId;
public:
    ListaEnemigos(): primero(nullptr), ultimo(nullptr), cantidad(0), nextId(1){}
    ~ListaEnemigos(){ limpiar(); }

    void limpiar(){
        while(primero){ NodoEnemigo* s=primero->siguiente; delete primero; primero=s; }
        ultimo=nullptr; cantidad=0;
    }

    void insertarAlFinal(string tipo, int vida, int vel, int recomp){
        Enemigo e; e.id=nextId++; e.tipo=tipo; e.vida=vida; e.vidaMax=vida;
        e.velocidad=vel; e.posicion=0; e.recompensa=recomp;
        NodoEnemigo* n=new NodoEnemigo(e);
        if(!ultimo){ primero=ultimo=n; }
        else{ n->anterior=ultimo; ultimo->siguiente=n; ultimo=n; }
        cantidad++;
    }

    bool eliminar(int id){
        NodoEnemigo* a=primero;
        while(a){ if(a->dato.id==id){
            if(a->anterior) a->anterior->siguiente=a->siguiente; else primero=a->siguiente;
            if(a->siguiente) a->siguiente->anterior=a->anterior; else ultimo=a->anterior;
            delete a; cantidad--; return true;
        } a=a->siguiente; }
        return false;
    }

    NodoEnemigo* getPrimero() const { return primero; }
    int          contar()     const { return cantidad; }
    bool         estaVacio()  const { return !primero; }
};
