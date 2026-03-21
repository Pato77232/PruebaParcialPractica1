cmake_minimum_required(VERSION 3.20)
project(TowerDefense)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─────────────────────────────────────────────────────────────────────
#  RUTAS DE SDL2  <-- Cambia estas rutas si instalaste en otro lugar
# ─────────────────────────────────────────────────────────────────────
set(SDL2_PATH     "C:/SDL2")
set(SDL2_TTF_PATH "C:/SDL2")   # Si SDL2_ttf está en la misma carpeta

# ─────────────────────────────────────────────────────────────────────
#  EJECUTABLE
# ─────────────────────────────────────────────────────────────────────
add_executable(TowerDefense
        TowerDefense_SDL2.cpp
)

# ─────────────────────────────────────────────────────────────────────
#  INCLUDES  (donde están los .h)
# ─────────────────────────────────────────────────────────────────────
target_include_directories(TowerDefense PRIVATE
        "${SDL2_PATH}/include"
        "${SDL2_PATH}/include/SDL2"
)

# ─────────────────────────────────────────────────────────────────────
#  LIBRERÍAS  (donde están los .a)
# ─────────────────────────────────────────────────────────────────────
target_link_directories(TowerDefense PRIVATE
        "${SDL2_PATH}/lib"
)

target_link_libraries(TowerDefense PRIVATE
        mingw32
        SDL2main
        SDL2
        SDL2_ttf
)

# ─────────────────────────────────────────────────────────────────────
#  COPIAR LOS .dll AL LADO DEL EJECUTABLE AUTOMÁTICAMENTE
#  (Sin los .dll el programa abre y se cierra al instante)
# ─────────────────────────────────────────────────────────────────────
add_custom_command(TARGET TowerDefense POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_PATH}/bin/SDL2.dll"
        $<TARGET_FILE_DIR:TowerDefense>

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_PATH}/bin/SDL2_ttf.dll"
        $<TARGET_FILE_DIR:TowerDefense>
)

# ─────────────────────────────────────────────────────────────────────
#  MENSAJE DE AYUDA AL CONFIGURAR
# ─────────────────────────────────────────────────────────────────────
message(STATUS "SDL2 buscado en: ${SDL2_PATH}")
message(STATUS "Si hay errores de 'not found', verifica que existan:")
message(STATUS "  ${SDL2_PATH}/include/SDL2/SDL.h")
message(STATUS "  ${SDL2_PATH}/lib/libSDL2.a")
message(STATUS "  ${SDL2_PATH}/bin/SDL2.dll")