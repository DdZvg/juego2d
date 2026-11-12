# Ecos del Abismo

> Estado: Fases 0, 1 y 2 completadas; Fase 3 en implementación con pruebas
> automatizadas en Debug y Release. Pendiente de comprobación manual; Fase 4
> no iniciada. Consulta `docs/ROADMAP.md` y `docs/SOLICITUD_CAMBIO_DE_FASE.md`.

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

## Fase 1: movimiento, cámara y colisiones

- Mueve al personaje con **WASD** o las **flechas**; las diagonales mantienen la misma velocidad que el movimiento horizontal o vertical.
- La cámara sigue suavemente al jugador y respeta los límites del escenario de 3000 × 3000 unidades.
- El escenario provisional muestra una cuadrícula oscura; el mouse se convierte a coordenadas del mundo mediante la vista actual de la cámara.
- Hay paredes y obstáculos de prueba. El jugador no puede atravesarlos, incluso con desplazamientos grandes, y se desliza por sus bordes al moverse en diagonal.

## Fase 2: combate básico

- **Clic izquierdo:** dispara un proyectil hacia el cursor. Los proyectiles tienen alcance, velocidad y colisión continua para no atravesar enemigos entre fotogramas. También se detienen al impactar paredes u obstáculos y no dañan a enemigos que estén detrás de ellos.
- **Clic derecho:** ataque cuerpo a cuerpo en un arco frontal; el golpe tiene alcance, daño y tiempo de reutilización.
- El enemigo provisional persigue al jugador, recibe daño, muestra su salud y ataca por contacto con enfriamiento. La barra en la esquina superior izquierda muestra la vida del jugador.
- El punto dorado junto al personaje indica hacia dónde apunta el mouse. La posición del cursor se transforma a coordenadas del mundo con la vista actual, incluso mientras se mueve la cámara.

## Fase 3: sistemas de combate (en implementación)

- **1 / 2:** equipa la Hoja herrumbrada o la Vara de ecos; cada arma configura daño, alcance y enfriamientos de sus ataques.
- **Q:** intenta aplicar la Marca abisal al enemigo más cercano dentro del alcance y del arco de apuntado. Si encuentra objetivo, el efecto expira con el tiempo y respeta su enfriamiento; los impactos reciben la bonificación configurada por el arma.
- Los indicadores de ranuras muestran el arma equipada y la barra bajo ellos representa el enfriamiento de la habilidad.
- Esta fase no está declarada aceptada todavía; la fase siguiente no ha comenzado.

## Compilar y probar

Usa los comandos de Debug o Release anteriores y ejecuta `build\Debug\juego.exe` (o `build\Release\juego.exe`). Para ejecutar las pruebas automatizadas en Windows:

```powershell
$ctest = "$vs\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe"
& $cmake --build build --config Debug
& $ctest --test-dir build -C Debug --output-on-failure
```

Las pruebas cubren movimiento y límites, cámara, entrada, vida, alcance y reutilización de ataques, trayectoria y colisión de proyectiles, muerte del enemigo, daño de contacto, inventario/selección de armas, estadísticas configurables y duración, enfriamiento y sinergia de la Marca abisal. La suite pasa en Debug y Release. Comprueba manualmente los controles de Fases 1 a 3 en la ventana, incluidos el apuntado mientras se mueve la cámara, el alcance visual de cada arma, la marca y su indicador de enfriamiento.

## Recursos y documentación

- Plan maestro: [`ECOS_DEL_ABISMO_MASTERPLAN.md`](ECOS_DEL_ABISMO_MASTERPLAN.md).
- Estado por fases: [`docs/ROADMAP.md`](docs/ROADMAP.md).
- Arquitectura actual: [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md).
- Historial de cambios: [`docs/CHANGELOG.md`](docs/CHANGELOG.md).
- Solicitud de cambio de fase: [`docs/SOLICITUD_CAMBIO_DE_FASE.md`](docs/SOLICITUD_CAMBIO_DE_FASE.md).
- Sprites del jugador: [`assets/sprites/player/`](assets/sprites/player/README.md).
- Créditos y licencias de recursos: [`assets/CREDITS.md`](assets/CREDITS.md).

Las imágenes del jugador se colocan en `assets\sprites\player\`; consulta el
README de esa carpeta para el formato y los nombres recomendados.