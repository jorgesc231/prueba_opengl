# Demos de programacion grafica con OpenGL

Mi primer intento de aprender a usar OpenGL en Windows.

Por ahora solo funciona en Windows, aunque portarlo a Linux no deberia ser dificil.

Es basicamente un programa que permite agregar demos graficas que usen OpenGL.

Tiene las siguientes pruebas:

- Clear Color (Cambiar el color de la pantalla)
- Carga de Texturas 2D
- Introduccion al Batch Rendering
- Batch rendering con Colores
- Batch rendering con Texturas
- Batch rendering de geometria dinamica
- TODO: Batch renderer para un juego

Usa ImGui para la interfaz.

Aunque usa OpenGL 3.3 Core  pude hacerlo funcionar en mi PC que solo soporta OpenGL 3.1.


## Uso

Descargar de Releases o (Aquí)[https://github.com/jorgesc231/prueba_opengl/releases/download/v0.1.0/prueba_opengl_windows.zip] 
o compilarlo con las instrucciones de mas abajo.

Al ejecutarlo muestra un menu para abrir las demos.

Elegir cualquiera y usar la interfaz grafica para interactuar.


## Dependencias

Todas las dependencias estan incluidas en el repositorio.

- glew (Carga de las fuciones de OpenGL)
- glfw (Abstraccion de la API de Sistema Operativo)
- glm (Matematicas para programacion grafica)
- imgui (Interfaz grafica simple)
- stb_image (Cargar varios formatos de imagen)


## Compilar

1. Clonar el repositorio:

```
git clone https://github.com/jorgesc231/prueba_opengl.git
```

2. Ingresar a la carpeta y abrir la solucion: OpenGL_tests.sln

3. Compilar y ejecutar.


## Por hacer

- Limpiar el codigo
- Terminar la demo del batch renderer
- Agregar demo de sistema de particulas 2D
- Agregar demos de renderizado de texto
- Agregar demos 3D
- Soporte de OpenGL ES

