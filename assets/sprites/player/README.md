# Sprites del jugador

Coloca aquí las imágenes del personaje: reposo, caminata, ataque, daño y
muerte. La ruta completa es:

```text
assets\sprites\player\
```

## Formato recomendado (provisional)

Estos valores son una propuesta para mantener el pixel art coherente. Se
confirmarán antes de integrar los sprites en el código.

- Formato: PNG con fondo transparente.
- Tamaño de cada fotograma: **48 × 48 px**.
- Todos los fotogramas de una animación deben medir lo mismo.
- Personaje centrado en el lienzo y sin escalado ni antialiasing.
- Direcciones: `down`, `left`, `right`, `up`.
- Fotogramas recomendados:
  - `idle`: 2 fotogramas.
  - `walk`: 4 a 6 fotogramas.
  - `attack`: 3 a 4 fotogramas (para la Fase 2/3).
  - `hurt` y `death`: cuando se integren vida y muerte definitivas.

## Hojas de animación (recomendado)

Una hoja por animación:

```text
player_idle.png
player_walk.png
player_attack.png
player_hurt.png
player_death.png
```

Distribución de la hoja:

- Filas, en este orden: `down`, `left`, `right`, `up`.
- Columnas: fotogramas de la animación, de izquierda a derecha.
- La hoja de caminata quedaría, por ejemplo, de 4 filas × 4 columnas.

## Fotogramas individuales (alternativa)

Si prefieres archivos separados, usa este patrón:

```text
player_walk_down_00.png
player_walk_down_01.png
player_walk_left_00.png
player_walk_left_01.png
...
```

Si ya tienes las imágenes con otro nombre o distribución, no las
renombres todavía: avísame y adaptamos el cargador a tu formato.

## Importante

- El juego actual dibuja al jugador con formas geométricas; los sprites
  todavía no se cargan. La integración se hará en una tarea autorizada y
  con su correspondiente compilación y prueba.
- Registra el origen y la licencia de cada imagen en
  `assets/CREDITS.md`.
