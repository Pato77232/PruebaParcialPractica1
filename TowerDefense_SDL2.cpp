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

class ListaOleadas {
    NodoOleada* ultimo;
    NodoOleada* actual;
    int         cantidad;
    int         lanzadas;
public:
    ListaOleadas(): ultimo(nullptr), actual(nullptr), cantidad(0), lanzadas(0){}
    ~ListaOleadas(){
        if(!ultimo) return;
        NodoOleada* ini=ultimo->siguiente; NodoOleada* n=ini;
        do{ NodoOleada* s=n->siguiente; delete n; n=s; }while(n!=ini);
    }

    void registrar(Oleada o){
        NodoOleada* n=new NodoOleada(o);
        if(!ultimo){ n->siguiente=n; ultimo=actual=n; }
        else{ n->siguiente=ultimo->siguiente; ultimo->siguiente=n; ultimo=n; }
        cantidad++;
    }

    Oleada* avanzar(){
        if(!actual) return nullptr;
        actual=actual->siguiente;
        lanzadas++;
        return &actual->dato;
    }

    NodoOleada* getPrimero() const { return ultimo ? ultimo->siguiente : nullptr; }
    NodoOleada* getUltimo()  const { return ultimo; }
    Oleada*     oleadaActual(){ return actual ? &actual->dato : nullptr; }
    int         contar()     const { return cantidad; }
    int         getLanzadas()const { return lanzadas; }
    bool        estaVacio()  const { return !ultimo; }
};


//  ESTADO DEL JUEGO
struct EstadoJuego {
    int  vidas;
    int  turno;
    int  oro;
    bool terminado;
    bool victoria;
    EstadoJuego(): vidas(5), turno(0), oro(200), terminado(false), victoria(false){}
};

//  LOG Y MENÚ

struct LogSistema {
    vector<string> lineas;
    void agregar(const string& s){
        lineas.push_back(s);
        if((int)lineas.size()>7) lineas.erase(lineas.begin());
    }
};

enum MenuEstado { MENU_PRINCIPAL, MENU_REG_TORRE, MENU_ELIM_TORRE,
                  MENU_VER_TORRES, MENU_VER_OLEADAS };

struct DatosTorreTemp {
    string nombre, tipo;
    int    posicion, danio, rango, costo, paso;
    DatosTorreTemp(){ nombre=""; tipo=""; posicion=-1; danio=0; rango=0; costo=0; paso=0; }
};

//  CARGA DE 20 OLEADAS PROGRESIVAS

void cargarOleadas(ListaOleadas& ols){
    // n=normales r=rapidos t=tanques vn/vr/vt=vida base
    int def[20][6]={
        {3,0,0, 50,  0,  0},   // Oleada  1
        {5,0,0, 55,  0,  0},   // Oleada  2
        {7,0,0, 60,  0,  0},   // Oleada  3
        {4,4,0, 65, 40,  0},   // Oleada  4
        {5,4,0, 70, 45,  0},   // Oleada  5
        {5,5,0, 75, 50,  0},   // Oleada  6
        {4,5,1, 80, 55,150},   // Oleada  7
        {5,5,1, 85, 60,160},   // Oleada  8
        {6,6,1, 90, 65,170},   // Oleada  9
        {6,6,2, 95, 70,180},   // Oleada 10
        {5,7,2,100, 75,200},   // Oleada 11
        {6,7,2,110, 80,220},   // Oleada 12
        {7,7,3,120, 85,240},   // Oleada 13
        {7,8,3,130, 90,260},   // Oleada 14
        {8,8,3,140, 95,280},   // Oleada 15
        {8,9,4,150,100,300},   // Oleada 16
        {9,9,4,165,110,330},   // Oleada 17
        {9,10,5,180,120,360},  // Oleada 18
        {10,10,5,200,130,400}, // Oleada 19
        {10,12,6,220,140,450}, // Oleada 20
    };
    for(int i=0;i<20;i++){
        Oleada o; o.idOleada=i+1;
        o.cantNormales=def[i][0]; o.cantRapidos=def[i][1]; o.cantTanques=def[i][2];
        o.vidaNormal=def[i][3];   o.vidaRapido=def[i][4];  o.vidaTanque=def[i][5];
        ols.registrar(o);
    }
}

//  LÓGICA DEL TURNO

void procesarTurno(ListaTorres& torres, ListaEnemigos& enemigos,
                   ListaOleadas& oleadas, EstadoJuego& est, LogSistema& log){
    if(est.terminado){ log.agregar("El juego ha terminado."); return; }
    if(enemigos.estaVacio()){ log.agregar("Sin enemigos. Lanza oleada [5]."); return; }

    est.turno++;
    // 1. Mover
    NodoEnemigo* n=enemigos.getPrimero();
    while(n){ n->dato.posicion+=n->dato.velocidad; n=n->siguiente; }

    // 2. Atacar
    for(int i=0;i<torres.contar();i++){
        Torre& t=torres.obtener(i);
        n=enemigos.getPrimero();
        while(n){
            int d=n->dato.posicion-t.posicion; if(d<0)d=-d;
            if(d<=t.rango) n->dato.vida-=t.danio;
            n=n->siguiente;
        }
    }

    // 3. Muertos
    n=enemigos.getPrimero(); int muertos=0;
    while(n){ NodoEnemigo* s=n->siguiente;
        if(n->dato.vida<=0){ est.oro+=n->dato.recompensa; enemigos.eliminar(n->dato.id); muertos++; }
        n=s;
    }

    // 4. Llegaron al final
    n=enemigos.getPrimero(); int llegaron=0;
    while(n){ NodoEnemigo* s=n->siguiente;
        if(n->dato.posicion>=LIMITE_RUTA){ est.vidas--; enemigos.eliminar(n->dato.id); llegaron++; }
        n=s;
    }

    string msg="T"+to_string(est.turno)+": ";
    if(muertos)  msg+="Elim:"+to_string(muertos)+" ";
    if(llegaron) msg+="Pasaron:"+to_string(llegaron)+" Vidas:"+to_string(est.vidas)+" ";
    msg+="Quedan:"+to_string(enemigos.contar());
    log.agregar(msg);

    if(est.vidas<=0){
        est.terminado=true; log.agregar("*** GAME OVER ***");
    } else if(enemigos.estaVacio() && oleadas.getLanzadas()>=oleadas.contar()){
        est.terminado=true; est.victoria=true; log.agregar("*** VICTORIA - 20 oleadas completadas! ***");
    }
}

//  HELPERS SDL2

void setCol(SDL_Renderer* r,SDL_Color c){ SDL_SetRenderDrawColor(r,c.r,c.g,c.b,c.a); }
void fillR(SDL_Renderer* r,int x,int y,int w,int h,SDL_Color c){ setCol(r,c); SDL_Rect rc{x,y,w,h}; SDL_RenderFillRect(r,&rc); }
void drawR(SDL_Renderer* r,int x,int y,int w,int h,SDL_Color c){ setCol(r,c); SDL_Rect rc{x,y,w,h}; SDL_RenderDrawRect(r,&rc); }
void drawL(SDL_Renderer* r,int x1,int y1,int x2,int y2,SDL_Color c){ setCol(r,c); SDL_RenderDrawLine(r,x1,y1,x2,y2); }

void drawTxt(SDL_Renderer* r,TTF_Font* f,const string& s,int x,int y,SDL_Color c,bool ctr=false){
    if(!f||s.empty()) return;
    SDL_Surface* su=TTF_RenderText_Blended(f,s.c_str(),c); if(!su) return;
    SDL_Texture* tx=SDL_CreateTextureFromSurface(r,su);
    int w=su->w,h=su->h; SDL_FreeSurface(su); if(!tx) return;
    SDL_Rect d{ctr?x-w/2:x,y,w,h}; SDL_RenderCopy(r,tx,nullptr,&d); SDL_DestroyTexture(tx);
}

void drawBarra(SDL_Renderer* r,int x,int y,int w,int h,int act,int mx){
    fillR(r,x,y,w,h,{50,15,15,200});
    int ll=mx>0?w*act/mx:0;
    SDL_Color c=act>mx/2?SDL_Color{52,211,153,255}:SDL_Color{239,68,68,255};
    if(ll>0)fillR(r,x,y,ll,h,c);
    drawR(r,x,y,w,h,{80,80,80,180});
}
