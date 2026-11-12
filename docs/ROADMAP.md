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
| 3. Sistemas de combate | En implementación, autorizada por el usuario | Armas, inventario inicial, Marca abisal, sinergia de daño y enfriamientos; falta comprobación manual y aceptación. |
| 4. Enemigos e IA | No iniciada | No comenzar sin autorización tras cerrar la Fase 3. |
| 4. Enemigos e IA | No iniciada | — |
| 5. Mazmorra procedural | No iniciada | — |
| 6. Recompensas y progresión | No iniciada | — |
| 7. Jefes y dificultad adaptativa | No iniciada | — |
| 8. Muerte, refugio y guardado | No iniciada | — |
| 9. Arte, audio y pulido | No iniciada | Los recursos se pueden ir recopilando en `assets/`. |
| 10. Versión jugable completa | No iniciada | — |

## Aprobaciones y autorizaciones registradas

- **Fase 1:** validada por el usuario.
- **Fase 2:** aprobada por el usuario el 2026-11-12.
- **Fase 3:** el usuario autorizó continuar con la siguiente fase; la
  implementación sigue pendiente de aceptación. No hay autorización para
  iniciar la Fase 4.

## Pendientes inmediatos

1. Comprobar manualmente los controles de combate de Fase 3: alternar armas,
   probar alcance/daño/enfriamiento, apuntar Q a un enemigo, observar la marca,
   su expiración, el daño adicional y la barra de enfriamiento.
2. Confirmar en ejecución que siguen funcionando el movimiento, los límites,
   la cámara, los obstáculos y los controles de combate de Fase 2.
3. Corregir cualquier problema observado y solicitar aceptación antes de
   iniciar la Fase 4.

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
