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
| `src/Enemy.*` | Arquetipos, estados de IA, persecución, retirada, strafe, disparo, curación de apoyo, navegación local y dibujo provisional. |
| `src/EnemyArchetype.*` | Definiciones configurables de enemigos y carga validada de `assets/data/enemies.txt`. |
| `src/Encounter.*` | Composición de las oleadas del encuentro de prueba. |
| `src/Projectile.*` | Trayectoria, alcance, colisión continua, expiración y facción (jugador o enemigo). |
| `src/CombatSystem.*` | Ataques, proyectiles de ambas facciones, habilidad de marca, resolución de impactos, separación enemiga y progresión de oleadas. |
| `src/Weapon.*` | Definiciones de armas y selección por ranura del inventario de expedición. |
| `src/StatusEffectComponent.*` | Duración de efectos de estado y multiplicador de daño de la Marca abisal. |
| `src/HealthComponent.*` | Vida reutilizable para jugador y enemigos, con daño, curación y restauración. |
| `src/Camera.*` | Seguimiento suavizado y límites de la vista. |
| `src/World.*` | Escenario de 3000 × 3000, cuadrícula, paredes/obstáculos, colisión por ejes, colisión de círculos, línea de visión y sonda de camino. |
| `src/InputManager.*` | Teclado, mouse, conversión a coordenadas del mundo y solicitudes de ataque, habilidad y reinicio. |

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

- La navegación enemiga es evitación local con sonda de camino y desvío
  persistente; no es un pathfinding global. Puede quedarse en bolsas
  cerradas y se sustituirá cuando la mazmorra procedural lo requiera.
- La separación entre enemigos es O(n²), adecuada para oleadas pequeñas
  (hasta seis enemigos por oleada en la Fase 4).
- El encuentro usa posiciones relativas fijas alrededor del centro de la
  arena; la colocación procedural llegará con la Fase 5.
- Los proyectiles ya se detienen en paredes y obstáculos.
- El arte sigue siendo geométrico; los sprites se integrarán más adelante.
- El HUD es provisional y usa una fuente del sistema si no encuentra una
  fuente en `assets/fonts/`.
- No hay un gestor de recursos todavía. Se añadirá cuando el juego cargue
  imágenes o audio.
