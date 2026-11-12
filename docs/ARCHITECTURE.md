# Arquitectura actual

Documento de seguimiento. La arquitectura objetivo completa está en la
sección 16 del `ECOS_DEL_ABISMO_MASTERPLAN.md`; aquí se describe solo lo
que existe hoy.

## Módulos

| Archivo | Responsabilidad |
| --- | --- |
| `src/main.cpp` | Punto de entrada. |
| `src/Game.*` | Ventana, ciclo principal, eventos, actualización, render y HUD provisional. |
| `src/Player.*` | Movimiento, normalización diagonal, colisión, límites, apuntado, vida y dibujo provisional. |
| `src/Enemy.*` | Persecución, vida, daño por contacto y dibujo provisional. |
| `src/Projectile.*` | Trayectoria, alcance, colisión continua contra enemigos y paredes, y expiración. |
| `src/CombatSystem.*` | Ataques cuerpo a cuerpo, proyectiles, habilidad de marca, tiempos de reutilización y resolución de impactos. |
| `src/Weapon.*` | Definiciones de armas y selección por ranura del inventario de expedición. |
| `src/StatusEffectComponent.*` | Duración de efectos de estado y multiplicador de daño de la Marca abisal. |
| `src/HealthComponent.*` | Vida reutilizable para jugador y enemigos. |
| `src/Camera.*` | Seguimiento suavizado y límites de la vista. |
| `src/World.*` | Escenario de 3000 × 3000, cuadrícula, paredes/obstáculos de prueba, límites y colisión por ejes con barrido. |
| `src/InputManager.*` | Teclado, mouse, conversión a coordenadas del mundo y solicitudes de ataque. |

## Dependencias

- C++17.
- SFML 3 (`Graphics`).
- vcpkg mediante el manifiesto `vcpkg.json`.
- CMake 3.24 o superior.

## Estructura de compilación

- `game_core`: biblioteca con la lógica del juego y SFML.
- `juego`: ejecutable que contiene `main.cpp` y `Game.cpp`.
- `game_tests`: pruebas automatizadas de lógica.

## Deuda o huecos conocidos

- Los enemigos todavía no colisionan con las paredes; la navegación
  enemiga se abordará en fases posteriores.
- Los proyectiles ya se detienen en paredes y obstáculos.
- El arte sigue siendo geométrico; los sprites se integrarán más adelante.
- El HUD es provisional.
- No hay un gestor de recursos todavía. Se añadirá cuando el juego cargue
  imágenes o audio.
