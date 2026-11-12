# Recursos del proyecto (assets)

Esta carpeta sigue la estructura de la sección 16.1 del
`ECOS_DEL_ABISMO_MASTERPLAN.md`.

## Dónde colocar cada recurso

| Recurso | Ruta |
| --- | --- |
| Sprites del jugador (movimiento, ataque, daño) | `assets/sprites/player/` |
| Sprites de enemigos | `assets/sprites/enemies/` |
| Sprites de jefes | `assets/sprites/bosses/` |
| Armas | `assets/sprites/weapons/` |
| Objetos | `assets/sprites/items/` |
| Entorno | `assets/sprites/environment/` |
| Tilesets | `assets/tilesets/` |
| Datos de animaciones | `assets/animations/` |
| Fuentes | `assets/fonts/` |
| Música | `assets/audio/music/` |
| Efectos de sonido | `assets/audio/sfx/` |
| Datos de juego (armas, enemigos, objetos, salas) | `assets/data/` |

Las carpetas que todavía no existen se crearán cuando la fase
correspondiente las necesite, tal como indica el plan. No es necesario
crear todas las carpetas vacías el primer día.

## Reglas para los recursos

- Usar PNG con transparencia para sprites y tilesets.
- Mantener el mismo tamaño de fotograma dentro de cada animación.
- No usar escalado con interpolación ni bordes suavizados en pixel art.
- Todo recurso debe ser propio o tener una licencia compatible con el
  proyecto. Registrar autor, fuente y licencia en `assets/CREDITS.md`.
- No incorporar recursos de procedencia dudosa ni assets temporales
  presentados como finales.
- No mover ni renombrar un recurso ya cargado por el código sin
  actualizar la ruta correspondiente.
