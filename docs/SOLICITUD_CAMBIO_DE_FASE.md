# Solicitud de cambio de fase

**Fase:** Fase 3 — Sistemas de combate
**Estado:** Autorizada para implementación por el usuario; aceptación manual pendiente

## 1. Fases previas

| Fase | Estado | Evidencia |
| --- | --- | --- |
| Fase 0 — Entorno | Completada | SFML 3 mediante vcpkg; compilación Debug y Release; ventana verificada. |
| Fase 1 — Prototipo jugable | Validada por el usuario | Movimiento, delta time, cámara, límites, paredes/obstáculos y colisión por ejes. |
| Fase 2 — Combate básico | Aprobada por el usuario el 2026-11-12 | Ataque cuerpo a cuerpo, proyectil, enemigo, vida, daño y colisión de proyectiles contra paredes. |

## 2. Alcance de la Fase 3 según el plan

- Sistema de armas.
- Datos configurables.
- Habilidades activas.
- Enfriamientos.
- Efectos de estado.
- Sinergias iniciales.
- Inventario de expedición.

## 3. Criterios de aceptación de la Fase 3

Según la sección 22 del documento maestro:

- Se pueden equipar armas diferentes.
- Los atributos afectan el comportamiento.
- Las habilidades respetan enfriamientos.
- Los efectos se aplican y expiran correctamente.
- Las sinergias funcionan sin duplicar daño accidentalmente.

## 4. Evidencia disponible

- Compilación Debug y Release correctas.
- Pruebas automatizadas `game_tests` en verde en ambas configuraciones.
- Se prueban inventario y ranuras, atributos distintos de armas, alcance y
  enfriamientos melee, proyectiles configurables, Marca abisal, expiración,
  cooldown y sinergia de daño.
- Pruebas manuales de Fase 3: pendientes de ejecución en la ventana del juego.

## 5. Alcance y dependencias pendientes

- La Fase 1 ya fue validada por el usuario y la Fase 3 fue autorizada para
  implementación.
- Las definiciones de armas se mantienen centralizadas en código C++ sin
  introducir un formato externo o dependencia innecesaria.
- La sinergia de marca se verifica para evitar duplicar daño.
- La Fase 3 aún requiere pruebas manuales y aceptación antes de pasar a Fase 4.

## 6. Lista de comprobación manual

### Fase 1

- [ ] El jugador se mueve en las ocho direcciones con WASD o flechas.
- [ ] La velocidad diagonal es igual a la horizontal y vertical.
- [ ] La cámara sigue al jugador sin saltos.
- [ ] El jugador no atraviesa los límites del mundo.
- [ ] El jugador no atraviesa las paredes ni los obstáculos.
- [ ] El jugador se desliza por el borde de una pared al moverse en diagonal.
- [ ] El indicador dorado apunta en la dirección del cursor mientras la cámara se mueve.
- [ ] El juego se cierra con la X sin bloquearse.

### Fase 2

- [ ] El clic izquierdo dispara un proyectil hacia el cursor.
- [ ] El clic derecho ejecuta el ataque cuerpo a cuerpo.
- [ ] El enemigo recibe daño y puede morir.
- [ ] El jugador recibe daño por contacto.
- [ ] Los proyectiles se destruyen al impactar o al agotar su alcance.
- [ ] Los proyectiles se detienen al impactar paredes y no dañan a enemigos detrás de ellas.
- [ ] No se aplican impactos múltiples involuntarios.
- [ ] La barra de salud del HUD refleja el daño.

### Fase 3

- [ ] Las teclas 1 y 2 equipan armas distintas y el HUD muestra la selección.
- [ ] Alcance, daño y enfriamientos de cada arma se comportan como se espera.
- [ ] Q marca al enemigo correcto según dirección y alcance.
- [ ] La marca se muestra y expira; la sinergia aumenta el daño una sola vez.
- [ ] La habilidad vuelve a estar disponible al terminar su enfriamiento.

## 7. Control de avance

La implementación de Fase 3 fue autorizada por el usuario y está en curso.
Este documento no representa su aceptación. No iniciar la Fase 4 hasta que
se completen las pruebas manuales de Fase 3 y el usuario la acepte.
