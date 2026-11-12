# Ecos del Abismo

## SFML 3 en Windows

SFML es una biblioteca C++ (no una extensión de Visual Studio). El proyecto usa CMake y el vcpkg incluido con Visual Studio para instalar SFML 3 y enlazarla sin configurar rutas manualmente. El manifiesto `vcpkg.json` fija el baseline del registro de vcpkg y solicita solo el módulo Graphics.

Abre una terminal PowerShell en la carpeta del proyecto y ejecuta:

```powershell
$vs = 'C:\Program Files\Microsoft Visual Studio\18\Community'
$cmake = "$vs\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
$vcpkgToolchain = "$vs\VC\vcpkg\scripts\buildsystems\vcpkg.cmake"

& $cmake -S . -B build -A x64 "-DCMAKE_TOOLCHAIN_FILE=$vcpkgToolchain" -DVCPKG_TARGET_TRIPLET=x64-windows
& $cmake --build build --config Debug
& $cmake --build build --config Release
```

La primera configuración descarga y compila SFML y sus dependencias; requiere conexión a Internet y puede tardar varios minutos. Luego ejecuta `build\Debug\juego.exe` para comprobar que abre la ventana y que se cierra con la X. Para abrirlo en Visual Studio, selecciona **Archivo > Abrir > Carpeta** y elige la carpeta que contiene `CMakeLists.txt`; Visual Studio detectará el proyecto CMake. Si Visual Studio está instalado en otra ubicación, ajusta `$vs` a su carpeta de instalación.

`src/main.cpp` inicia el prototipo jugable de Fase 2, implementado con la API de SFML 3; no se deben mezclar ejemplos o encabezados de SFML 2.

## Fase 1: movimiento y cámara

- Mueve al personaje con **WASD** o las **flechas**; las diagonales mantienen la misma velocidad que el movimiento horizontal o vertical.
- La cámara sigue suavemente al jugador y respeta los límites del escenario de 3000 × 3000 unidades.
- El escenario provisional muestra una cuadrícula oscura; el mouse se convierte a coordenadas del mundo mediante la vista actual de la cámara.

## Fase 2: combate

- **Clic izquierdo:** dispara un proyectil hacia el cursor. Los proyectiles tienen alcance, velocidad y colisión continua para no atravesar enemigos entre fotogramas.
- **Clic derecho:** ataque cuerpo a cuerpo en un arco frontal; el golpe tiene alcance, daño y tiempo de reutilización.
- El enemigo provisional persigue al jugador, recibe daño, muestra su salud y ataca por contacto con enfriamiento. La barra en la esquina superior izquierda muestra la vida del jugador.
- El punto dorado junto al personaje indica hacia dónde apunta el mouse. La posición del cursor se transforma a coordenadas del mundo con la vista actual, incluso mientras se mueve la cámara.

## Compilar y probar

Usa los comandos de Debug o Release anteriores y ejecuta `build\Debug\juego.exe` (o `build\Release\juego.exe`). Para ejecutar las pruebas automatizadas en Windows:

```powershell
$ctest = "$vs\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe"
& $cmake --build build --config Debug
& $ctest --test-dir build -C Debug --output-on-failure
```

Las pruebas cubren movimiento y límites, suavizado y límites de cámara, vida, alcance y reutilización del golpe, trayectoria y colisión de proyectiles, muerte del enemigo y daño de contacto. Comprueba manualmente en la ventana que el indicador dorado sigue al cursor mientras mueves la cámara y que los clics izquierdo y derecho apuntan y atacan correctamente.