# Changelog

## En curso — Fase 3

### Añadido

- Catálogo inicial de Hoja herrumbrada y Vara de ecos con atributos propios,
  inventario de expedición y selección mediante las teclas 1 y 2.
- Habilidad Marca abisal con alcance, arco de apuntado, duración, enfriamiento,
  indicador visual de estado y sinergia de daño configurable por arma.
- Pruebas automatizadas de inventario, estadísticas de armas, melee, enfriamiento,
  marca, expiración y sinergia.

### Validación

- Compilación y `game_tests` pasan en Debug y Release.
- Pruebas manuales y aceptación de Fase 3 pendientes; Fase 4 no iniciada.

## No publicado — 2026-11-12

### Añadido

- Paredes y obstáculos de prueba en el mundo de 3000 × 3000.
- Colisión por ejes con barrido para evitar atravesar paredes a alta
  velocidad.
- Deslizamiento del jugador a lo largo de los bordes de las paredes.
- Pruebas automatizadas de colisión desde los cuatro lados, bloqueo
  continuo, deslizamiento y ausencia de túnel a alta velocidad.
- Pruebas de proyectiles contra paredes y de prioridad del impacto más
  cercano.

### Corregido

- Los proyectiles ya no atraviesan paredes ni obstáculos: se destruyen en
  el primer impacto y no dañan a enemigos que estén detrás de una pared.
- Se resolvió el impacto más cercano entre enemigos y paredes para que un
  enemigo delante de una pared siga recibiendo el daño.

### Aprobado

- Fase 2 aprobada por el usuario el 2026-11-12.

### Documentación

- Estructura inicial de `assets/` con la carpeta de sprites del jugador,
  la carpeta de datos de animaciones y el registro de créditos.
- Hoja de ruta, arquitectura, changelog y solicitud de cambio de fase.
- Copia del documento maestro en la raíz del repositorio.

### Estado

- Fase 1 validada por el usuario.
- Fase 2 aprobada por el usuario.
- Fase 3 autorizada y en implementación; aceptación manual pendiente.

## 2026-11-12 — Fase 2

### Añadido

- Combate básico: ataque cuerpo a cuerpo, proyectil, enemigo de prueba,
  vida, daño, enfriamientos y HUD de salud.
- Pruebas automatizadas de movimiento, cámara, entrada, vida, combate y
  proyectiles.

Commit: `7695814 Add SFML 3 gameplay prototype and tests`.
