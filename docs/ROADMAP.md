# Hoja de ruta y estado

Seguimiento del `ECOS_DEL_ABISMO_MASTERPLAN.md`. La regla de avance es no
iniciar una fase hasta que la anterior cumpla sus criterios de aceptación
y el usuario la autorice.

## Estado actual

| Fase | Estado | Notas |
| --- | --- | --- |
| 0. Preparación del entorno | Completada | SFML 3 vía vcpkg; Debug y Release compilan; ventana verificada. |
| 1. Prototipo jugable | Completada y validada por el usuario | Movimiento, delta time, cámara, límites, paredes/obstáculos y colisión por ejes. |
| 2. Combate básico | Aprobada por el usuario | Ataque cuerpo a cuerpo, proyectil, enemigo, vida, daño y colisión de proyectiles contra paredes. |
| 3. Sistemas de combate | Aceptada por el usuario | Armas equipables con estadísticas propias, inventario de expedición, Marca abisal con enfriamiento, sinergia de daño y datos configurables en `assets/data/weapons.txt`. |
| 4. Enemigos e IA | Implementada, aceptación manual pendiente | Cuatro arquetipos con estados de IA, proyectiles enemigos, navegación con evitación de obstáculos, encuentro de tres oleadas y reinicio con R. Datos en `assets/data/enemies.txt`. |
| 5. Mazmorra procedural | No iniciada | No comenzar sin autorización tras cerrar la Fase 4. |
| 6. Recompensas y progresión | No iniciada | — |
| 7. Jefes y dificultad adaptativa | No iniciada | — |
| 8. Muerte, refugio y guardado | No iniciada | — |
| 9. Arte, audio y pulido | No iniciada | Los recursos se pueden ir recopilando en `assets/`. |
| 10. Versión jugable completa | No iniciada | — |

## Aprobaciones y autorizaciones registradas

- **Fase 1:** validada por el usuario.
- **Fase 2:** aprobada por el usuario el 2026-11-12.
- **Fase 3:** aceptada por el usuario el 2026-11-20 al autorizar el inicio
  de la Fase 4.
- **Fase 4:** autorizada por el usuario el 2026-11-20; implementación en
  curso y pendiente de aceptación. No hay autorización para iniciar la
  Fase 5.

## Pendientes inmediatos

1. Comprobar manualmente la Fase 4: distinguir los cuatro arquetipos
   (Carroñero, Arquero de ecos, Baluarte y Nodriza abisal), verificar que
   los enemigos no atraviesan paredes, que la Nodriza cura aliados y que
   las tres oleadas avanzan y se reinician con R.
2. Confirmar en ejecución que siguen funcionando el movimiento, la cámara,
   los límites, los obstáculos, el combate y los controles de las fases
   anteriores.
3. Corregir cualquier problema observado y solicitar aceptación antes de
   iniciar la Fase 5.

## Pruebas automatizadas

El proyecto tiene `tests/GameTests.cpp`. En Windows se ejecuta con:

```powershell
$vs = 'C:\Program Files\Microsoft Visual Studio\18\Community'
$cmake = "$vs\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
$ctest = "$vs\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe"

& $cmake --build build --config Debug
& $ctest --test-dir build -C Debug --output-on-failure
& $cmake --build build --config Release
& $ctest --test-dir build -C Release --output-on-failure
```
