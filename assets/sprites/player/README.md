# Sprites del jugador

Coloca aquí las hojas de sprites del personaje: reposo, caminata, ataque,
daño y muerte. La ruta completa es:

```text
assets\sprites\player\
```

Las imágenes originales proporcionadas por el usuario están en `reference/`.
Las hojas derivadas y usadas por el juego son `sheet_locomotion_8dir.png`
(6 fotogramas para cada una de las 8 direcciones) y `sheet_idle_8dir.png`
(una pose por dirección). Se recortaron las poses del personaje para excluir
rótulos y otras animaciones de las láminas; los originales se conservaron.

- Formato de las hojas integradas: PNG RGBA con transparencia.
- Tamaño de fotograma: **128 × 128 px**.
- Direcciones: `up`, `up-right`, `right`, `down-right`, `down`, `down-left`, `left`, `up-left`.
- `idle` usa una pose por dirección; `walk` usa 6 fotogramas por dirección.

## Distribución de las hojas integradas

Las hojas derivadas usan fotogramas de **128 × 128 px** con transparencia:

- `sheet_idle_8dir.png`: una fila por dirección en el orden indicado arriba.
- `sheet_locomotion_8dir.png`: ocho filas en el mismo orden; seis fotogramas
  de izquierda a derecha en cada fila.
- El juego carga estas hojas si están disponibles y conserva las hojas
  anteriores como respaldo para reposo y caminata.

## Carga

El juego carga estas hojas desde `assets/sprites/player/`. Si faltan, usa
las hojas anteriores como respaldo. Las animaciones de ataque, daño y muerte
siguen usando sus hojas existentes. Los originales y las hojas derivadas se
registran en `assets/CREDITS.md`.
