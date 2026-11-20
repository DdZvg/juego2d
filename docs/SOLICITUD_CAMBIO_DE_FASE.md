# Solicitud de cambio de fase

**Fase:** Fase 4 — Enemigos e IA
**Estado:** Autorizada para implementación por el usuario el 2026-11-20;
aceptación manual pendiente

## 1. Fases previas

| Fase | Estado | Evidencia |
| --- | --- | --- |
| Fase 0 — Entorno | Completada | SFML 3 mediante vcpkg; compilación Debug y Release; ventana verificada. |
| Fase 1 — Prototipo jugable | Validada por el usuario | Movimiento, delta time, cámara, límites, paredes/obstáculos y colisión por ejes. |
| Fase 2 — Combate básico | Aprobada por el usuario el 2026-11-12 | Ataque cuerpo a cuerpo, proyectil, enemigo, vida, daño y colisión de proyectiles contra paredes. |
| Fase 3 — Sistemas de combate | Aceptada por el usuario el 2026-11-20 | Armas configurables, Marca abisal, enfriamientos, efectos y sinergia. |

## 2. Alcance de la Fase 4 según el plan

- Estados de IA.
- Enemigos cuerpo a cuerpo.
- Enemigos a distancia.
- Enemigos resistentes o de apoyo.
- Navegación básica y obstáculos.
- Oleadas y encuentros.

## 3. Criterios de aceptación de la Fase 4

Según la sección 22 del documento maestro:

- Cada arquetipo tiene comportamiento distinguible.
- Los enemigos no atraviesan paredes sin regla explícita.
- La IA no consume recursos excesivos.
- Se puede reiniciar un encuentro de prueba.

## 4. Evidencia disponible

- Compilación Debug y Release correctas.
- Pruebas automatizadas `game_tests` en verde en ambas configuraciones.
- Se prueban las definiciones y la carga de datos de enemigos, los cuatro
  arquetipos (persecución, retirada, disparo con línea de visión,
  resistencia y curación de apoyo), los proyectiles enemigos, la colisión
  de círculos, la línea de visión, la navegación alrededor de paredes y la
  progresión/reinicio de las tres oleadas.
- Pruebas manuales de Fase 4: pendientes de ejecución en la ventana del
  juego.

## 5. Decisiones de implementación

- Cuatro arquetipos: Carroñero (melee), Arquero de ecos (ranged),
  Baluarte (resistente) y Nodriza abisal (apoyo).
- Estados de IA: `Idle`, `Chase`, `Attack`, `Reposition` y `Support`.
- Navegación local con sonda de camino por segmentos, desvío persistente y
  colisión por ejes. No es un pathfinding global; se documenta como deuda.
- Encuentro de prueba de tres oleadas con posiciones relativas fijas y
  retardo entre oleadas. La colocación procedural llegará con la Fase 5.
- Reinicio con R, que restaura la vida del jugador para repetir la prueba.
- Datos de enemigos en `assets/data/enemies.txt` con validación y
  conservación de la configuración anterior si una carga falla.

## 6. Lista de comprobación manual

### Fase 4

- [ ] Los Carroñeros persiguen y atacan por contacto.
- [ ] Los Arqueros de ecos mantienen la distancia, se retiran al acercarse
      y disparan solo con línea de visión.
- [ ] Los proyectiles enemigos dañan al jugador y se detienen en paredes.
- [ ] Los Baluartes resisten más daño y son claramente más lentos.
- [ ] La Nodriza abisal cura a los enemigos cercanos con un pulso visible.
- [ ] Los enemigos rodean las paredes y no las atraviesan.
- [ ] Las tres oleadas avanzan tras despejar la anterior.
- [ ] R reinicia el encuentro y restaura la vida del jugador.
- [ ] El HUD muestra oleada, enemigos restantes y cuenta atrás.

### Fases 1 a 3 (regresión)

- [ ] Movimiento, cámara, límites y obstáculos siguen funcionando.
- [ ] Ataques cuerpo a cuerpo y a distancia, marca y sinergia siguen
      funcionando.
- [ ] El juego se cierra con la X sin bloquearse.

## 7. Control de avance

La implementación de Fase 4 fue autorizada por el usuario y está en curso.
Este documento no representa su aceptación. No iniciar la Fase 5 hasta que
se completen las pruebas manuales de Fase 4 y el usuario la acepte.
