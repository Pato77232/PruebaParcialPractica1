<img width="1468" height="865" alt="image" src="https://github.com/user-attachments/assets/ce98466c-99c4-4011-bfd9-1355840d3728" /># PruebaParcialPractica1

#Para el correcto funcionamiento de este proyecto se necesita instalar SDL2 y SDL2_ttf  
#Se recomienda usar el IDE CLion para ejecutar el código fuente.  
#Se incluye un archivo rar con los archivos de SDL2 y las instrucciones de instalación — descomprimir en C:\

---

# Tower Defense — Estructura de Datos en C++

> Asignatura: Estructura de Datos | Evaluación Práctica Primer Parcial

---

## Integrantes
Edwin Caraguay (Lider)
Emmanuel Acosta (Colaborador)
David Cuenca (Colaborador)
Michael Rosillo (Colaborador

---

## Descripción

Sistema de juego **Tower Defense** implementado en C++ puro que demuestra el uso correcto de tres estructuras de datos lineales implementadas manualmente, sin usar ninguna colección de la STL.

El juego consiste en defender una ruta de posiciones `0→20` colocando torres defensivas que atacan automáticamente a oleadas de enemigos que avanzan hacia la base del jugador.

La interfaz gráfica está construida con **SDL2**, pero la lógica de datos (listas, nodos, oleadas) es completamente independiente de la capa visual.

---

## Estructura del Proyecto

```
TowerDefense/
├── TowerDefense_SDL2.cpp   # Código fuente completo (lógica + SDL2)
├── CMakeLists.txt          # Configuración de compilación para CLion + MinGW
└── README.md               # Este archivo
```

> El proyecto fue desarrollado en **CLion** con compilador **MinGW (64-bit)** sobre Windows.

---

## Instalación de SDL2 (Windows + CLion)

1. Descomprimir el `.rar` adjunto en `C:\` — quedará la carpeta `C:\SDL2\`
2. Abrir el proyecto en CLion
3. CLion detecta el `CMakeLists.txt` automáticamente
4. Hacer clic en **File → Reload CMake Project**
5. Compilar y ejecutar con el botón ▶

La estructura esperada dentro de `C:\SDL2\` es:
```
C:\SDL2\
├── include\SDL2\     # headers (.h)
├── lib\              # librerías (.a)
└── bin\              # SDL2.dll y SDL2_ttf.dll
```

---

## Estructuras de Datos Implementadas

### 1. Lista Secuencial — Torres Defensivas

Arreglo estático manual de capacidad **20** (una por casilla máximo). Sin `std::vector`.

```cpp
struct Torre {
    int    id;
    string nombre;
    string tipo;       // Arquero | Canon | Mago
    int    posicion;   // 0 a 19
    int    danio;
    int    rango;
    int    costo;
};
```

| Método | Descripción |
|--------|-------------|
| `insertar(...)` | Valida casilla libre y límite de 20 torres |
| `eliminar(id)` | Elimina por ID y desplaza el arreglo |
| `buscar(id)` | Retorna puntero a Torre o `nullptr` |
| `casillaLibre(pos)` | Verifica si una posición está disponible |
| `contar()` | Cantidad de torres activas |
| `obtener(idx)` | Acceso directo por índice para la lógica del turno |

---

### 2. Lista Doblemente Enlazada — Enemigos Activos

Nodos con punteros `anterior` y `siguiente`. Referencias a `primero` y `ultimo` para acceso O(1) en ambos extremos.

```
nullptr ← [E1] ⇆ [E2] ⇆ [E3] → nullptr
          ↑                 ↑
        primero           ultimo
```

```cpp
struct NodoEnemigo {
    Enemigo      dato;        // id, tipo, vida, vidaMax, velocidad, posicion, recompensa
    NodoEnemigo* anterior;
    NodoEnemigo* siguiente;
};
```

| Método | Descripción |
|--------|-------------|
| `insertarAlFinal(...)` | Reconecta `ultimo` en O(1) |
| `eliminar(id)` | Reconecta vecinos y libera memoria |
| `getPrimero()` | Acceso directo al primer nodo |
| `estaVacio()` | Verifica si hay enemigos en campo |
| `limpiar()` | Libera todos los nodos al reiniciar |

---

### 3. Lista Circular Simplemente Enlazada — Oleadas

Referencia únicamente al `ultimo`. `ultimo->siguiente` siempre apunta al primer nodo, permitiendo ciclo continuo entre las 20 oleadas.

```
        ┌──────────────────────────────────┐
        ↓                                  │
      [O1] → [O2] → ... → [O19] → [O20] ──┘
                                    ↑
                                  ultimo
```

```cpp
struct NodoOleada {
    Oleada      dato;    // idOleada, cantNormales, cantRapidos, cantTanques, vidas...
    NodoOleada* siguiente;
};
```

| Método | Descripción |
|--------|-------------|
| `registrar(oleada)` | Inserta al final, mantiene circularidad |
| `avanzar()` | Mueve `actual` al siguiente nodo |
| `oleadaActual()` | Retorna puntero a la oleada en curso |
| `getLanzadas()` | Cantidad de oleadas ya iniciadas |

---

## Progresión de las 20 Oleadas

| Oleadas | Normales | Rápidos | Tanques | Dificultad |
|---------|----------|---------|---------|------------|
| 1 – 3   | 3 → 7    | —       | —       | Baja |
| 4 – 6   | 4 – 5    | 4 – 5   | —       | Media |
| 7 – 10  | 4 – 6    | 5 – 6   | 1 – 2   | Alta |
| 11 – 15 | 5 – 8    | 7 – 8   | 2 – 3   | Muy alta |
| 16 – 20 | 8 – 10   | 9 – 12  | 4 – 6   | Extrema |

> Los enemigos tipo **Tanque** tienen triple de vida y otorgan mayor recompensa de oro.

---

## Diagrama de Clases

<img width="1509" height="1019" alt="image" src="https://github.com/user-attachments/assets/ec30cc03-bf44-45eb-af04-8c86e6b5085c" />


## Lógica de cada turno

Cada vez que se presiona **[6] Avanzar turno**, el sistema ejecuta:

```
1. Mover enemigos     →  posicion += velocidad
2. Torres atacan      →  si |pos_enemigo - pos_torre| ≤ rango → vida -= daño
3. Eliminar muertos   →  si vida ≤ 0 → delete nodo + oro al jugador
4. Verificar meta     →  si posicion ≥ 20 → delete nodo + vidas--
5. Verificar fin      →  vidas == 0       → GAME OVER
                          oleadas == 20   → VICTORIA
```

---

## Menú (Panel lateral SDL2)

```
╔══════════════════════════════════════╗
║       TOWER DEFENSE — MENÚ           ║
╠══════════════════════════════════════╣
║  [1]  Registrar torre defensiva      ║
║  [2]  Mostrar torres registradas     ║
║  [3]  Eliminar torre                 ║
║  [4]  Mostrar oleadas                ║
║  [5]  Iniciar siguiente oleada       ║
║  [6]  Avanzar turno                  ║
║  [ESC] Salir                         ║
╚══════════════════════════════════════╝
```

> El menú está integrado como **panel lateral dentro de la ventana SDL2** (1200×700 px).  
> Cada operación actualiza la visualización del juego en tiempo real.

---

## Reglas del Juego

- La ruta tiene posiciones de `0` a `20`.
- Los enemigos aparecen en posición `0` y avanzan cada turno según su velocidad.
- Las torres atacan automáticamente a cualquier enemigo dentro de su rango.
- Un enemigo destruido otorga oro al jugador.
- Un enemigo que cruza la meta descuenta **1 vida**.
- El jugador comienza con **5 vidas** y **200 de oro**.
- La partida termina al perder todas las vidas o al completar las 20 oleadas.

---

## Caso de Prueba (Sección 10 del documento guía)

**Torres precargadas al iniciar:**

| # | Nombre | Tipo | Posición | Daño | Rango | Costo |
|---|--------|------|----------|------|-------|-------|
| 1 | Arquero | Arquero | 3 | 20 | 2 | 50 |
| 2 | Cañón | Canon | 8 | 35 | 3 | 80 |

**Oleadas 1 y 2 precargadas:**

| # | Tipo | Cantidad | Vida | Velocidad |
|---|------|----------|------|-----------|
| 1 | Básico | 3 enemigos | 50 HP | 1 |
| 2 | Rápido | 2 enemigos | 40 HP | 2 |

**Secuencia de demostración:**
```
1. Ejecutar el programa  →  caso de prueba se carga automáticamente
2. Opción [5]            →  Iniciar Oleada 1 (3 enemigos básicos)
3. Opción [6] × 3        →  Avanzar turnos (torres atacan, enemigos avanzan)
4. Opción [5]            →  Iniciar Oleada 2 (2 enemigos rápidos)
5. Opción [6] × 5        →  Avanzar turnos hasta limpiar la oleada
```

---

## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)
project(TowerDefense)
set(CMAKE_CXX_STANDARD 11)

set(SDL2_PATH "C:/SDL2")

add_executable(TowerDefense TowerDefense_SDL2.cpp)

target_include_directories(TowerDefense PRIVATE
    "${SDL2_PATH}/include"
    "${SDL2_PATH}/include/SDL2"
)
target_link_directories(TowerDefense PRIVATE "${SDL2_PATH}/lib")
target_link_libraries(TowerDefense PRIVATE mingw32 SDL2main SDL2 SDL2_ttf)

add_custom_command(TARGET TowerDefense POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_PATH}/bin/SDL2.dll" $<TARGET_FILE_DIR:TowerDefense>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_PATH}/bin/SDL2_ttf.dll" $<TARGET_FILE_DIR:TowerDefense>
)

```
---
## Conclusiones

- La lista secuencial es adecuada para torres porque el numero maximo es fijo (20) y el acceso O(1) por indice es necesario para el renderer, que itera todas las torres en cada frame.
- La lista doblemente enlazada es la mejor eleccion para enemigos activos porque las eliminaciones ocurren frecuentemente y la reconexion de punteros O(1) evita el costo de desplazar elementos que tendria un arreglo.
- La lista circular simplifica el control de oleadas: el puntero actual avanza naturalmente y la propiedad circular permite reiniciar el ciclo sin logica adicional de limites
- La separacion entre logica del juego (juego.h) y presentacion grafica (main.cpp) facilita el mantenimiento y permite reemplazar la interfaz SDL2 sin modificar las estructuras de datos.
- SDL_ttf permitio presentar informacion dinamica de forma legible, mejorando significativamente la experiencia de evaluacion respecto a una version de consola
•

## Anexos
<img width="1468" height="865" alt="image" src="https://github.com/user-attachments/assets/1f7cfc01-e633-4db2-aeda-4318d1bb8fa5" />

<img width="1480" height="852" alt="image" src="https://github.com/user-attachments/assets/c4e3a8f3-3075-4df3-973f-11628aca8164" />

<img width="1484" height="847" alt="image" src="https://github.com/user-attachments/assets/026d80b4-d5fb-4337-9008-c284cb964657" />


