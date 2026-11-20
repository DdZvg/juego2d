# Datos configurables

Archivos de texto con formato `clave = valor` y esquema versionado. Los
valores se validan al cargar: un valor fuera de rango conserva el valor
seguro anterior y un archivo con esquema incorrecto no reemplaza la
configuración válida que ya estuviera en memoria.

## `weapons.txt`

- `schema_version = 1`
- Cada arma empieza con una sección `[weapon]` y un `id`
  (`RustyBlade` o `EchoWand`).
- Claves: `display_name`, `melee_damage`, `melee_range`, `melee_cooldown`,
  `projectile_damage`, `projectile_speed`, `projectile_range`,
  `projectile_cooldown`, `marked_damage_multiplier`.

## `enemies.txt`

- `schema_version = 1`
- Cada enemigo empieza con una sección `[enemy]` y un `id`
  (`Carronero`, `ArqueroEcos`, `Baluarte` o `Nodriza`).
- Claves: `display_name`, `maximum_health`, `radius`, `move_speed`,
  `contact_damage`, `contact_cooldown`, `damage_reduction`,
  `attack_range`, `preferred_range`, `projectile_damage`,
  `projectile_speed`, `projectile_range`, `attack_cooldown`,
  `support_radius`, `support_heal`, `support_cooldown`.
- Los colores de cada arquetipo se definen en código.

Los comentarios empiezan con `#` y pueden ocupar una línea completa o ir al
final de una línea.
