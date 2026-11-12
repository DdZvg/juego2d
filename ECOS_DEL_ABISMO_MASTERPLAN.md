# ECOS DEL ABISMO

## Documento maestro de diseño, especificaciones técnicas y prompt para IA de desarrollo

**Versión del documento:** 1.0\
**Estado:** Especificación inicial aprobada\
**Proyecto:** Videojuego 2D tipo Dungeon Crawler / Roguelite\
**Lenguaje:** C++17 o superior compatible con las dependencias
seleccionadas\
**IDE principal:** Visual Studio 2022\
**Tecnología gráfica prevista:** SFML 3.x (por confirmar e instalar)\
**Plataforma inicial:** Windows 10/11, PC\
**Modo:** Un jugador\
**Idioma de este documento:** Español

------------------------------------------------------------------------

# PARTE I. VISIÓN DEL PROYECTO

## 1. Concepto general

**Ecos del Abismo** es un videojuego 2D de acción y exploración tipo
dungeon crawler roguelite, con cámara top-down, estética pixel art y
ambientación de fantasía oscura.

El jugador explora un mundo hostil formado por zonas y mazmorras
generadas proceduralmente. Cada expedición combina habitaciones,
caminos, enemigos, tesoros, eventos, secretos y jefes. Algunas
habitaciones especiales estarán diseñadas manualmente y se integrarán en
los mapas generados para garantizar encuentros interesantes y variedad
controlada.

El combate combinará ataques manuales cuerpo a cuerpo, armas a distancia
y habilidades activas. El objetivo es permitir estilos de juego
distintos y crear combinaciones interesantes entre armas, objetos y
habilidades.

La dificultad será adaptativa: el juego podrá ajustar la presión y el
comportamiento de los enemigos de acuerdo con el rendimiento del
jugador, pero nunca deberá sentirse arbitrario, tramposo o imposible.

El juego será para un solo jugador. El alcance buscado es **mediano y
pulido**: priorizar una experiencia sólida, ampliable y divertida antes
que intentar producir cientos de contenidos sin terminar los sistemas
fundamentales.

## 2. Pilares de diseño

1.  **Combate satisfactorio:** controles precisos, respuesta inmediata,
    telegráficos claros, impacto visual y sonoro.
2.  **Exploración con descubrimientos:** habitaciones, caminos
    alternativos, secretos, cofres y eventos.
3.  **Variedad de construcciones (builds):** armas, pasivas y
    habilidades que se complementan.
4.  **Rejugabilidad:** generación procedural con composición de niveles
    coherente.
5.  **Riesgo y recompensa:** decisiones sobre explorar, gastar recursos,
    luchar o continuar.
6.  **Progresión justa:** la muerte debe tener consecuencias, pero cada
    expedición debe contribuir al progreso a largo plazo.
7.  **Calidad técnica:** arquitectura modular, código mantenible,
    rendimiento estable y pruebas.

## 3. Referencias de diseño

Se pueden estudiar ideas generales de juegos como *The Binding of
Isaac*, *Enter the Gungeon*, *Dead Cells* y *Hades*: ritmo de combate,
variedad de objetos, salas con identidad, progresión y claridad visual.

**No copiar** personajes, nombres, mapas, sprites, música, interfaces,
textos, historias ni diseños protegidos. Ecos del Abismo debe tener
identidad visual y mecánica propia.

## 4. Decisiones aprobadas

  -----------------------------------------------------------------------
  Área                                Decisión
  ----------------------------------- -----------------------------------
  Cámara                              Top-down 2D

  Movimiento                          Libre por el mapa

  Combate                             Mixto: ataques manuales y
                                      habilidades activas

  Estética                            Pixel Art

  Ambientación                        Fantasía oscura, misterio y terror

  Dificultad                          Adaptativa

  Mazmorras                           Procedurales con habitaciones
                                      especiales diseñadas a mano

  Muerte                              Pérdida parcial de recursos y botín
                                      temporal

  Progresión                          Híbrida: niveles durante la partida
                                      y desbloqueos permanentes

  Multijugador                        Un jugador

  Controles                           Teclado + mouse; WASD y apuntado
                                      con mouse

  Alcance                             Mediano, priorizando pulido

  IDE                                 Visual Studio 2022

  SFML                                No instalada o versión desconocida;
                                      verificar antes de configurar
  -----------------------------------------------------------------------

------------------------------------------------------------------------

# PARTE II. EXPERIENCIA DE JUEGO

## 5. Bucle principal de juego

El bucle principal debe ser fácil de entender y suficientemente
profundo:

1.  El jugador llega al refugio o pantalla inicial.
2.  Revisa desbloqueos, mejoras permanentes y opciones.
3.  Inicia una expedición.
4.  Explora habitaciones y zonas conectadas.
5.  Combate enemigos y evita trampas.
6.  Recoge monedas, consumibles, armas y objetos.
7.  Decide si toma rutas opcionales, visita tiendas o busca secretos.
8.  Derrota al jefe de la zona.
9.  Avanza a una zona más peligrosa o completa la expedición.
10. Si muere, regresa al refugio con las recompensas permanentes que
    haya conseguido.
11. Desbloquea contenido y vuelve a intentarlo con nuevas posibilidades.

La expedición debe generar decisiones interesantes; no debe convertirse
en recorrer habitaciones vacías para acumular tiempo.

## 6. Cámara y mundo

-   Cámara ortográfica top-down que siga al jugador suavemente.
-   El mundo será más grande que la ventana.
-   La cámara debe limitarse a los bordes del mapa.
-   La resolución de ventana inicial sugerida es 1280 × 720.
-   El tamaño interno de referencia del pixel art debe definirse de
    forma consistente.
-   Usar escalado entero cuando sea posible para evitar pixel art
    borroso.
-   El juego debe admitir pantalla completa y modo ventana en una etapa
    posterior.
-   Separar coordenadas del mundo de coordenadas de pantalla.
-   El apuntado con mouse debe convertir correctamente la posición de
    pantalla a coordenadas del mundo.

## 7. Movimiento del jugador

-   WASD para movimiento; flechas como alternativa opcional.
-   Movimiento en ocho direcciones.
-   Normalizar el vector diagonal para evitar que el jugador se mueva
    más rápido en diagonal.
-   Movimiento basado en delta time.
-   Colisiones sólidas con paredes, obstáculos y límites del mundo.
-   Evitar que el jugador atraviese paredes a altas velocidades.
-   La velocidad, aceleración y respuesta deben ser configurables.
-   Considerar una pequeña tolerancia de colisión para evitar que el
    personaje se atasque en esquinas.
-   No implementar física compleja si no aporta a la jugabilidad.

## 8. Combate

### 8.1 Reglas generales

El combate debe combinar: - Ataques cuerpo a cuerpo. - Armas a
distancia. - Proyectiles. - Habilidades activas. - Efectos de estado. -
Esquiva o dash, si las pruebas de jugabilidad lo justifican. -
Enfriamientos, consumo de recursos o restricciones que eviten el uso
indiscriminado de habilidades.

El jugador debe recibir retroalimentación clara al atacar, acertar,
recibir daño, bloquear o activar una habilidad.

### 8.2 Controles iniciales propuestos

  Acción                 Control inicial
  ---------------------- -----------------
  Movimiento             WASD
  Apuntar                Mouse
  Ataque principal       Clic izquierdo
  Habilidad secundaria   Clic derecho
  Interactuar            E
  Esquivar               Espacio
  Inventario             Tab
  Pausa                  Escape

Todos los controles deben poder configurarse posteriormente. No asumir
que este esquema es definitivo si las pruebas revelan conflictos.

### 8.3 Armas

Diseñar un sistema de armas extensible. Categorías iniciales sugeridas:

-   Espadas y armas rápidas.
-   Hachas y armas lentas de alto daño.
-   Lanzas o armas de alcance.
-   Arcos o ballestas.
-   Bastones mágicos.
-   Varitas y armas de proyectiles.
-   Armas especiales o malditas.

Cada arma puede definir: - Identificador único. - Nombre y
descripción. - Categoría. - Daño base. - Velocidad de ataque. - Alcance
o velocidad de proyectil. - Retroceso. - Probabilidad de crítico, si
aplica. - Rareza. - Efectos especiales. - Animación, sonido y recursos
visuales. - Requisitos o condiciones de desbloqueo.

Los valores deben residir en datos configurables cuando sea razonable;
evitar llenar el código de números mágicos.

### 8.4 Habilidades y sinergias

El sistema debe permitir que armas, habilidades y objetos produzcan
sinergias. Ejemplos originales provisionales:

-   **Eco eléctrico:** un proyectil eléctrico salta a un enemigo
    cercano.
-   **Marca abisal:** los enemigos marcados reciben daño adicional al
    ser golpeados.
-   **Corte espectral:** un ataque cuerpo a cuerpo deja una onda de
    energía.
-   **Llama voraz:** los enemigos afectados por fuego pueden propagar
    quemaduras.
-   **Fragmentación:** algunos proyectiles se dividen al impactar.

Las sinergias deben ser legibles y equilibrables. Evitar combinaciones
que generen daño infinito, bloqueen el juego o destruyan el rendimiento.

## 9. Enemigos y jefes

### 9.1 Enemigos comunes

Crear arquetipos con comportamientos diferenciados: - Perseguidor cuerpo
a cuerpo. - Enemigo que ataca a distancia. - Enemigo rápido y frágil. -
Enemigo resistente. - Enemigo de apoyo o invocador. - Enemigo que coloca
trampas o controla zonas.

Cada enemigo debe tener estados de IA claros, por ejemplo: - Idle /
patrulla. - Detectar jugador. - Perseguir. - Preparar ataque. -
Atacar. - Recuperarse. - Recibir daño. - Morir.

No todos los enemigos deben perseguir al jugador de la misma manera. La
variedad de patrones importa más que aumentar únicamente la vida y el
daño.

### 9.2 Jefes

Cada zona importante debe culminar en un jefe con: - Identidad visual
propia. - Presentación y arena. - Varios patrones de ataque. - Señales
visuales antes de ataques peligrosos. - Fases o cambios de
comportamiento. - Ventanas de vulnerabilidad. - Recompensas únicas. -
Condiciones de victoria y transición claras.

Los jefes no deben depender exclusivamente de tener muchísima vida.
Deben exigir aprender patrones y tomar decisiones.

## 10. Objetos, rarezas y recompensas

Categorías sugeridas: - Armas. - Objetos pasivos. - Consumibles. -
Reliquias. - Mejoras de estadísticas. - Llaves y objetos de misión. -
Monedas y recursos. - Objetos malditos de alto riesgo y recompensa.

Rarezas iniciales: - Común. - Poco común. - Raro. - Épico. - Legendario.

La rareza no debe ser la única medida de utilidad. Los objetos deben
tener funciones diferentes y situacionales.

Cada objeto debe tener identificador estable, nombre, descripción, tipo,
rareza, efecto, icono o sprite y reglas de aparición.

## 11. Mazmorras procedurales

### 11.1 Enfoque híbrido

Combinar: - Generación procedural de estructura y conexiones. -
Habitaciones especiales diseñadas manualmente. - Variantes de decoración
y distribución. - Reglas para asegurar recorridos válidos y dificultad
coherente.

La generación debe ser reproducible mediante una semilla (seed), útil
para depuración y pruebas.

### 11.2 Tipos de habitaciones

-   Inicio.
-   Combate.
-   Élite.
-   Tesoro.
-   Tienda.
-   Descanso o curación.
-   Evento.
-   Secreto.
-   Jefe.
-   Salida o transición.

No todas deben aparecer en todas las expediciones. Definir reglas de
frecuencia, accesibilidad y distancia.

### 11.3 Reglas de generación

-   Todas las habitaciones esenciales deben ser alcanzables.
-   Evitar conexiones duplicadas o inválidas.
-   Evitar habitaciones bloqueadas sin llave disponible.
-   Garantizar un camino hacia el objetivo principal.
-   Colocar jefes al final de una zona.
-   Distribuir recompensas según riesgo.
-   Controlar la cantidad de encuentros seguidos.
-   Evitar mapas excesivamente vacíos o laberínticos.
-   Permitir guardar la seed y los datos necesarios para reproducir
    errores.

### 11.4 Secretos

-   Paredes rompibles o entradas ocultas.
-   Habitaciones escondidas.
-   Cofres con condiciones especiales.
-   Eventos poco frecuentes.
-   Rutas alternativas.
-   Recompensas por explorar.
-   Pistas visuales y sonoras sutiles.

Los secretos deben poder descubrirse mediante observación o
experimentación, no depender únicamente de azar opaco.

## 12. Dificultad adaptativa

La dificultad adaptativa debe ser un sistema medible, limitado y
transparente en sus resultados.

Puede observar: - Daño recibido recientemente. - Salud restante. -
Tiempo para superar encuentros. - Cantidad de muertes recientes. -
Rendimiento en combate. - Recursos consumidos. - Mejoras y poder de la
build.

Puede ajustar de forma controlada: - Composición de grupos enemigos. -
Frecuencia de enemigos de élite. - Agresividad o selección de
patrones. - Ritmo de aparición de encuentros. - Oportunidades de
recuperación. - Calidad o cantidad de recompensas dentro de límites
definidos.

**Restricciones importantes:** - No modificar vida o daño de enemigos de
forma invisible durante un combate. - No castigar al jugador por jugar
bien con aumentos repentinos e injustos. - No generar picos de
dificultad imposibles. - Establecer límites mínimos y máximos. -
Suavizar los cambios en el tiempo. - Mantener la semilla y reglas
reproducibles para depuración. - Dar prioridad a ajustar composición y
ritmo antes que inflar estadísticas. - La dificultad no debe invalidar
las reglas de los jefes.

La IA de desarrollo debe proponer métricas y valores iniciales,
documentarlos y permitir ajustarlos desde configuración.

## 13. Muerte y progresión

### 13.1 Muerte parcial

Al morir: - Se pierde el botín temporal de la expedición, según las
reglas definidas. - Se conservan desbloqueos permanentes. - Se conservan
recursos permanentes confirmados. - Se reinicia el nivel temporal de la
expedición. - Se registran estadísticas de la partida. - Se puede
mostrar un resumen de la expedición.

No implementar pérdida de progreso permanente sin confirmación explícita
del diseño.

### 13.2 Progresión híbrida

Durante una expedición: - Ganar experiencia. - Subir de nivel. - Elegir
entre varias mejoras. - Crear una build. - Obtener armas y objetos
temporales.

Entre expediciones: - Desbloquear armas, personajes cosméticos o
variantes si se añaden. - Desbloquear objetos que pueden aparecer en
futuras partidas. - Abrir nuevas zonas o eventos. - Mejorar opciones
iniciales de forma moderada. - Completar retos y logros.

La progresión permanente debe ampliar opciones sin eliminar el desafío
ni convertir el juego en una victoria automática por acumulación de
estadísticas.

## 14. Refugio, interfaz y menús

Pantallas previstas: - Menú principal. - Selección / inicio de
expedición. - Refugio. - HUD durante el juego. - Inventario o resumen de
build. - Pausa y configuración. - Pantalla de muerte. - Resumen de
expedición. - Desbloqueos.

HUD mínimo: - Salud. - Recurso de habilidad, si existe. - Arma
equipada. - Habilidad activa. - Monedas. - Nivel y experiencia. -
Indicador de interacción. - Estado de efectos relevantes.

La interfaz debe ser clara, legible y coherente con la ambientación
oscura. No sacrificar legibilidad por decoración.

------------------------------------------------------------------------

# PARTE III. ESPECIFICACIONES TÉCNICAS

## 15. Entorno y dependencias

### 15.1 Entorno inicial

-   Windows 10/11.
-   Visual Studio 2022.
-   C++17 como estándar mínimo inicial.
-   CMake.
-   SFML 3.x, si se confirma compatible con el entorno.
-   Git para control de versiones.

### 15.2 Verificación obligatoria de SFML

El usuario no sabe si tiene SFML instalado ni qué versión. La IA debe:

1.  Inspeccionar el equipo/proyecto y preguntar o comprobar si SFML está
    disponible.
2.  No asumir que SFML está instalada.
3.  No mezclar encabezados o librerías de SFML 2 con SFML 3.
4.  Preferir SFML 3 estable y compatible con Visual Studio 2022 si no
    existe una dependencia previa.
5.  Explicar opciones de instalación: gestor de paquetes compatible o
    instalación oficial, según el entorno real.
6.  Configurar CMake para localizar SFML.
7.  Verificar arquitectura, configuración Debug/Release y dependencias
    DLL cuando corresponda.
8.  Probar un programa mínimo que abra una ventana antes de implementar
    el juego.
9.  Documentar los pasos exactos para reproducir la compilación.

No inventar rutas de instalación ni afirmar que una dependencia está
instalada sin verificarla.

### 15.3 Dependencias

Mantener las dependencias externas al mínimo. SFML debe cubrir ventana,
gráficos, entrada y audio en la primera etapa. Añadir bibliotecas para
físicas, serialización, UI o scripting únicamente si existe una
necesidad justificada y se aprueba su impacto.

## 16. Arquitectura de software

Aplicar programación orientada a objetos con responsabilidades pequeñas
y claras. Evitar una clase gigantesca `Game` y evitar que `main.cpp`
contenga la lógica completa.

Separar: - Inicialización. - Bucle principal. - Entrada. -
Actualización. - Física y colisiones. - IA. - Renderizado. - Audio. -
UI. - Datos y guardado. - Generación procedural.

### 16.1 Estructura de carpetas propuesta

``` text
EcosDelAbismo/
│
├── CMakeLists.txt
├── README.md
├── .gitignore
├── docs/
│   ├── GAME_DESIGN.md
│   ├── ARCHITECTURE.md
│   ├── ROADMAP.md
│   └── CHANGELOG.md
│
├── assets/
│   ├── sprites/
│   │   ├── player/
│   │   ├── enemies/
│   │   ├── bosses/
│   │   ├── weapons/
│   │   ├── items/
│   │   └── environment/
│   ├── tilesets/
│   ├── animations/
│   ├── fonts/
│   ├── audio/
│   │   ├── music/
│   │   └── sfx/
│   └── data/
│       ├── weapons/
│       ├── enemies/
│       ├── items/
│       └── rooms/
│
├── include/
│   ├── core/
│   │   ├── Game.h
│   │   ├── GameConfig.h
│   │   ├── GameState.h
│   │   └── Time.h
│   ├── entities/
│   │   ├── Entity.h
│   │   ├── Player.h
│   │   ├── Enemy.h
│   │   ├── Boss.h
│   │   └── Projectile.h
│   ├── combat/
│   │   ├── Weapon.h
│   │   ├── WeaponSystem.h
│   │   ├── Ability.h
│   │   ├── StatusEffect.h
│   │   └── DamageInfo.h
│   ├── world/
│   │   ├── World.h
│   │   ├── Dungeon.h
│   │   ├── Room.h
│   │   ├── RoomTemplate.h
│   │   ├── TileMap.h
│   │   └── DungeonGenerator.h
│   ├── systems/
│   │   ├── CollisionSystem.h
│   │   ├── CombatSystem.h
│   │   ├── AISystem.h
│   │   ├── DifficultyDirector.h
│   │   ├── ProgressionSystem.h
│   │   ├── LootSystem.h
│   │   └── SaveSystem.h
│   ├── input/
│   │   └── InputManager.h
│   ├── rendering/
│   │   ├── Camera.h
│   │   ├── Animation.h
│   │   └── Renderer.h
│   ├── ui/
│   │   ├── HUD.h
│   │   ├── MainMenu.h
│   │   └── PauseMenu.h
│   └── audio/
│       └── AudioManager.h
│
├── src/
│   ├── main.cpp
│   └── [mismos módulos y subcarpetas que include]
│
├── tests/
│   ├── CMakeLists.txt
│   ├── test_dungeon_generator.cpp
│   ├── test_progression.cpp
│   └── test_combat.cpp
│
└── build/                 # Generado; no subir a Git
```

Esta estructura es una arquitectura objetivo, no una obligación de crear
todos los archivos vacíos el primer día. Crear módulos cuando la etapa
correspondiente los necesite.

### 16.2 Responsabilidades principales

  Clase / módulo         Responsabilidad
  ---------------------- ------------------------------------------------
  `Game`                 Coordinar ciclo de vida y estados generales
  `Player`               Estado y acciones del jugador
  `Entity`               Base común para entidades cuando sea útil
  `Enemy`                Datos y comportamiento de enemigos
  `Boss`                 Fases y patrones especiales
  `Weapon`               Definición y comportamiento de un arma
  `Projectile`           Movimiento, impacto y vida útil de proyectiles
  `InputManager`         Leer y traducir controles
  `Camera`               Seguir al jugador y limitar vista
  `CollisionSystem`      Consultar y resolver colisiones
  `DungeonGenerator`     Crear estructura reproducible
  `RoomTemplate`         Definición de habitaciones especiales
  `CombatSystem`         Resolver daño, impactos y efectos
  `DifficultyDirector`   Medir rendimiento y proponer ajustes
  `LootSystem`           Seleccionar y generar recompensas
  `ProgressionSystem`    Experiencia y desbloqueos
  `SaveSystem`           Persistir progreso y configuración
  `AudioManager`         Música y efectos de sonido
  `HUD`                  Mostrar información durante el juego

### 16.3 Reglas de diseño

-   Usar composición antes que herencia profunda.
-   Usar herencia solo cuando exista una relación real de tipo.
-   Mantener encapsulados los datos.
-   Preferir `enum class` para estados y categorías.
-   Usar `const` y referencias cuando corresponda.
-   Evitar variables globales mutables.
-   Evitar números mágicos: usar constantes o configuración.
-   Usar `std::unique_ptr` / `std::shared_ptr` solo cuando la propiedad
    lo justifique.
-   Evitar punteros crudos propietarios.
-   Evitar ciclos de dependencias entre módulos.
-   Mantener los encabezados ligeros y usar declaraciones adelantadas
    cuando ayuden.
-   Cada `.h` debe tener su `.cpp` cuando el tamaño y la responsabilidad
    lo justifiquen.
-   No crear abstracciones innecesarias.
-   No usar `using namespace std;` en encabezados.
-   Usar nombres consistentes en inglés para código y español para
    documentación dirigida al usuario, salvo decisión posterior.
-   Comentar decisiones no obvias, no cada línea.

## 17. Bucle principal y tiempo

-   Separar procesamiento de eventos, actualización y renderizado.
-   Usar `deltaTime` para movimiento y temporizadores.
-   Considerar un paso fijo para simulación/colisiones si las pruebas lo
    requieren.
-   Limitar delta time tras pausas o pérdida de foco.
-   Evitar lógica de juego dependiente del número de FPS.
-   No realizar trabajo pesado de generación procedural dentro del
    renderizado.
-   Mantener una tasa de actualización estable.
-   No limitar FPS de forma arbitraria sin medir el resultado.

## 18. Colisiones y movimiento

-   Empezar con formas simples (AABB/círculos) y ampliar si se necesita.
-   Separar colisiones del renderizado.
-   Resolver movimiento por ejes o mediante un método que evite
    atravesar obstáculos.
-   Evitar que entidades se queden atrapadas en paredes.
-   Definir capas o categorías de colisión.
-   Proyectiles deben tener reglas claras de impacto y destrucción.
-   Los ataques cuerpo a cuerpo deben usar hitboxes temporales y no
    depender solo de la apariencia del sprite.

## 19. Cámara y coordenadas

-   Mantener una cámara `sf::View` o solución equivalente.
-   Convertir posición del mouse de coordenadas de ventana a coordenadas
    del mundo usando la vista actual.
-   Aplicar suavizado configurable.
-   Limitar la cámara al mundo.
-   Probar correctamente en distintas resoluciones y tamaños de ventana.

## 20. Datos y guardado

-   Separar definiciones de contenido de la lógica cuando sea razonable.
-   Usar un formato legible y seguro para armas, enemigos, objetos y
    habitaciones.
-   JSON puede ser una opción, pero añadir una biblioteca solo si se
    justifica.
-   Guardar versión del esquema para permitir migraciones futuras.
-   Guardar progreso permanente y configuración por separado del estado
    temporal de una expedición.
-   Hacer guardado atómico o con respaldo para reducir riesgo de
    corrupción.
-   Validar datos al cargar.
-   No guardar rutas absolutas de la máquina del desarrollador.
-   No guardar secretos ni información personal.

## 21. Rendimiento

Objetivo inicial: funcionamiento fluido en PC de gama baja/media
razonable.

-   Evitar crear y destruir muchos objetos gráficos cada frame.
-   Reutilizar texturas y recursos.
-   Usar un gestor de recursos para texturas, fuentes y audio.
-   Dibujar solo lo visible cuando el tamaño del mundo lo justifique.
-   Evitar búsquedas costosas repetidas en el bucle principal.
-   Controlar la cantidad de proyectiles, partículas y entidades.
-   Usar pooling solo si el perfilado muestra que hace falta.
-   Medir antes de optimizar prematuramente.
-   Evitar fugas de memoria y recursos duplicados.

------------------------------------------------------------------------

# PARTE IV. PLAN DE DESARROLLO

## 22. Etapas y criterios de aceptación

El proyecto se debe implementar por etapas pequeñas. Cada etapa debe
compilar y funcionar antes de comenzar la siguiente.

### Fase 0 --- Preparación del entorno

**Tareas** - Inspeccionar Visual Studio 2022. - Verificar compilador C++
y CMake. - Comprobar si SFML está instalada. - Si no está, instalar una
versión estable compatible. - Crear proyecto mínimo. - Abrir una ventana
y cerrarla correctamente. - Documentar compilación y ejecución.

**Aceptación** - Compila en Debug y Release. - Abre una ventana SFML. -
No hay errores de DLL o arquitectura. - README contiene pasos
reproducibles.

### Fase 1 --- Prototipo jugable

**Tareas** - Ventana y ciclo principal. - Jugador provisional. -
Movimiento en ocho direcciones. - Delta time. - Mundo de prueba. -
Cámara que sigue al jugador. - Conversión mouse/mundo. - Límites y
colisiones básicas.

**Aceptación** - El jugador se mueve en todas las direcciones. - La
velocidad diagonal es correcta. - La cámara sigue al jugador sin
saltos. - No atraviesa límites ni paredes de prueba. - El juego se puede
cerrar sin bloquearse.

### Fase 2 --- Combate básico

**Tareas** - Ataque cuerpo a cuerpo. - Apuntado con mouse. - Proyectil
de prueba. - Enemigo de prueba. - Vida y daño. - Invulnerabilidad breve
tras recibir daño, si el diseño lo necesita. - Feedback visual y sonoro
provisional.

**Aceptación** - Se puede atacar y acertar. - El enemigo recibe daño y
puede morir. - El jugador puede recibir daño. - Los proyectiles se
destruyen correctamente. - No se aplican impactos múltiples
involuntarios.

### Fase 3 --- Sistemas de combate

**Tareas** - Sistema de armas. - Datos configurables. - Habilidades
activas. - Enfriamientos. - Efectos de estado. - Sinergias iniciales. -
Inventario de expedición.

**Aceptación** - Se pueden equipar armas diferentes. - Los atributos
afectan el comportamiento. - Las habilidades respetan enfriamientos. -
Los efectos se aplican y expiran correctamente. - Las sinergias
funcionan sin duplicar daño accidentalmente.

### Fase 4 --- Enemigos e IA

**Tareas** - Estados de IA. - Enemigos cuerpo a cuerpo. - Enemigos a
distancia. - Enemigos resistentes o de apoyo. - Navegación básica y
obstáculos. - Oleadas y encuentros.

**Aceptación** - Cada arquetipo tiene comportamiento distinguible. - Los
enemigos no atraviesan paredes sin regla explícita. - La IA no consume
recursos excesivos. - Se puede reiniciar un encuentro de prueba.

### Fase 5 --- Mazmorra procedural

**Tareas** - Modelo de habitación. - Conexiones. - Generación con
seed. - Camino principal. - Habitaciones especiales manuales. -
Colocación de enemigos y recompensas. - Transiciones.

**Aceptación** - Cada mapa es reproducible por seed. - Existe camino
válido hasta el objetivo. - Las habitaciones esenciales son
alcanzables. - No hay conexiones rotas. - Se puede generar una batería
de mapas de prueba.

### Fase 6 --- Recompensas y progresión

**Tareas** - Cofres. - Tienda. - Rarezas. - Experiencia. - Mejoras por
nivel. - Monedas. - Recursos permanentes. - Desbloqueos.

**Aceptación** - Las recompensas respetan las reglas de aparición. - El
jugador puede construir distintas builds. - Los desbloqueos persisten al
reiniciar el juego. - No se duplican recompensas al cargar una partida.

### Fase 7 --- Jefes y dificultad adaptativa

**Tareas** - Arena y jefe inicial. - Fases y patrones. - Medición de
rendimiento. - Director de dificultad. - Límites y suavizado de
ajustes. - Pruebas con jugadores o escenarios simulados.

**Aceptación** - El jefe tiene patrones legibles. - Los ataques se
pueden esquivar o contrarrestar. - La dificultad no cambia de forma
injusta durante un ataque. - Los ajustes permanecen dentro de límites
documentados. - El sistema adaptativo no castiga de forma excesiva a
jugadores hábiles.

### Fase 8 --- Muerte, refugio y guardado

**Tareas** - Pantalla de muerte. - Resumen de expedición. - Separación
de recursos temporales y permanentes. - Refugio. - Desbloqueos
persistentes. - Guardado y carga robustos.

**Aceptación** - Morir aplica exactamente las reglas de pérdida
parcial. - Los desbloqueos permanentes se conservan. - Los datos no se
duplican al cargar. - Un archivo inválido produce un error controlado y
no bloquea el juego.

### Fase 9 --- Arte, audio y pulido

**Tareas** - Sprites originales o con licencia válida. - Animaciones. -
Tilesets. - Efectos de partículas. - Música y efectos. - HUD final. -
Menús y opciones. - Balance y pruebas.

**Aceptación** - Estilo visual consistente. - Audio configurable. - UI
legible. - No hay errores críticos conocidos. - Rendimiento estable en
escenarios de alta carga.

### Fase 10 --- Versión jugable completa

**Tareas** - Integración final. - Pruebas de regresión. - Corrección de
errores. - Documentación. - Empaquetado para Windows. - Instrucciones de
instalación.

**Aceptación** - Se puede iniciar, jugar, morir, progresar y volver a
iniciar una expedición. - No se requieren herramientas de desarrollo
para ejecutar la versión empaquetada. - README y controles están
actualizados. - No quedan errores bloqueantes conocidos.

------------------------------------------------------------------------

# PARTE V. CALIDAD Y CONTROL DEL PROYECTO

## 23. Reglas obligatorias para la IA

1.  Antes de editar, inspeccionar los archivos existentes.
2.  No sobrescribir código funcional sin entender su propósito.
3.  No crear todo el juego en una sola respuesta o cambio masivo.
4.  Trabajar por fases y tareas pequeñas.
5.  Antes de modificar, explicar brevemente qué archivos cambiará y por
    qué.
6.  Mantener compilación funcional al terminar cada tarea.
7.  Ejecutar la compilación real cuando las herramientas lo permitan.
8.  Corregir errores introducidos por los cambios antes de declarar la
    tarea terminada.
9.  No afirmar que una prueba pasó si no se ejecutó.
10. Si no puede ejecutar una prueba, indicar exactamente qué queda
    pendiente.
11. Actualizar README, CHANGELOG y documentación técnica cuando
    corresponda.
12. No inventar APIs de SFML ni funciones de bibliotecas.
13. Consultar los encabezados o documentación de la versión instalada.
14. No mezclar SFML 2 y SFML 3.
15. Evitar dependencias innecesarias.
16. Mantener nombres y estilo consistentes.
17. No dejar pseudocódigo en una función que se presenta como terminada.
18. No ocultar errores con `catch (...)` vacío.
19. No introducir datos de prueba en el juego final sin identificarlos.
20. Preguntar antes de tomar decisiones que cambien pilares aprobados.

## 24. Pruebas mínimas

Crear pruebas para lógica que no dependa directamente de la ventana: -
Normalización de movimiento. - Cálculo de daño. - Enfriamientos. -
Aplicación y expiración de efectos. - Generación procedural y
conectividad. - Reglas de recompensas. - Muerte y persistencia. -
Límites de dificultad adaptativa. - Guardado y carga.

Las pruebas visuales/manuales deben documentarse por separado.

## 25. Git y control de versiones

-   Inicializar repositorio Git.
-   Crear commits pequeños y descriptivos.
-   No subir carpetas `build/`, binarios temporales, archivos de Visual
    Studio generados ni datos locales de usuario.
-   Incluir `.gitignore` apropiado para Visual Studio, CMake y C++.
-   Crear una rama por funcionalidad si el flujo lo justifica.
-   No reescribir historial compartido sin autorización.
-   No borrar cambios del usuario.
-   Revisar `git status` antes de cambios grandes.

Convención sugerida de commits: - `feat: add player movement` -
`fix: resolve diagonal speed` - `refactor: separate camera logic` -
`test: add dungeon connectivity tests` -
`docs: update build instructions`

## 26. Seguridad y licencias de recursos

-   No descargar ni incluir assets de procedencia dudosa.
-   Preferir recursos propios o con licencia que permita el uso
    previsto.
-   Registrar autor, fuente y licencia de cada recurso externo.
-   No incorporar ejecutables o scripts de terceros sin revisar.
-   No incluir credenciales, tokens o datos privados en el repositorio.
-   No presentar recursos temporales como assets finales.

------------------------------------------------------------------------

# PARTE VI. PROMPT MAESTRO PARA LA IA DE VISUAL STUDIO

## 27. Instrucciones principales

Copia el siguiente bloque y entrégalo a GitHub Copilot Chat u otra IA
integrada en Visual Studio 2022. También puedes usarlo como
instrucciones de proyecto.

------------------------------------------------------------------------

### PROMPT MAESTRO

Actúa como un desarrollador senior especializado en C++, arquitectura de
videojuegos 2D, SFML, diseño de sistemas roguelite y herramientas de
desarrollo para Windows.

Tu responsabilidad es ayudarme a desarrollar **Ecos del Abismo**, un
videojuego 2D dungeon crawler roguelite en C++.

Debes tratar el archivo `ECOS_DEL_ABISMO_MASTERPLAN.md` como la
especificación principal del proyecto. Léelo completo antes de proponer
cambios.

#### Contexto del desarrollador

Soy estudiante de Ingeniería en Sistemas y tengo conocimientos
básicos/intermedios de C++. He trabajado con clases, objetos, archivos
`.h` y `.cpp`, arreglos, matrices, ciclos, condicionales y proyectos de
consola.

Necesito código de buena calidad, pero también explicaciones que me
permitan entenderlo. No asumas que conozco patrones avanzados de
arquitectura o APIs complejas.

#### Objetivo

Construir un juego funcional, modular, extensible y visualmente
atractivo, no una demostración desechable.

Decisiones aprobadas: - C++17 o superior compatible con las
dependencias. - Visual Studio 2022. - SFML 3.x preferida, pendiente de
verificar/instalar. - Cámara top-down. - Movimiento libre. - Combate
cuerpo a cuerpo y a distancia. - Teclado + mouse. - Pixel Art. -
Fantasía oscura. - Mazmorras procedurales con habitaciones especiales
manuales. - Dificultad adaptativa. - Muerte con pérdida parcial. -
Progresión híbrida. - Un jugador. - Alcance mediano, con prioridad en
pulido. - Muchas armas, secretos, jefes, sinergias y desbloqueos.

#### Forma de trabajar

**No intentes programar el juego completo de una sola vez.**

Primero: 1. Inspecciona la estructura actual del repositorio. 2. Lee el
documento maestro. 3. Verifica el entorno de Visual Studio 2022. 4.
Comprueba la instalación y versión de SFML. 5. Identifica problemas de
configuración. 6. Presenta un plan corto para la siguiente tarea. 7.
Espera mi autorización si la tarea implica una decisión importante o una
modificación destructiva.

Después, desarrolla una fase a la vez.

Para cada tarea: 1. Explica el objetivo. 2. Indica los archivos que
crearás o modificarás. 3. Explica brevemente la responsabilidad de cada
archivo. 4. Implementa código completo y compilable. 5. Mantén
separación entre encabezados `.h` e implementaciones `.cpp` cuando
corresponda. 6. Compila el proyecto. 7. Ejecuta las pruebas disponibles.
8. Corrige los errores que introduzcan tus cambios. 9. Resume qué
funciona y qué queda pendiente. 10. Actualiza la documentación
necesaria.

#### Reglas de programación

-   Utiliza C++ moderno y legible.
-   Mantén responsabilidades pequeñas.
-   Prefiere composición sobre jerarquías de herencia profundas.
-   Evita clases gigantes.
-   Evita lógica de juego dentro de `main.cpp`.
-   Evita variables globales mutables.
-   Evita números mágicos.
-   Gestiona correctamente memoria y recursos.
-   Usa RAII y smart pointers cuando corresponda.
-   No uses punteros crudos para expresar propiedad.
-   Evita dependencias innecesarias.
-   No introduzcas patrones complejos sin una razón concreta.
-   No dupliques lógica.
-   No dejes funciones vacías o pseudocódigo en tareas terminadas.
-   No suprimas advertencias o errores sin entender su causa.
-   Mantén compatibilidad con la versión real de SFML instalada.
-   No mezcles APIs de SFML 2 y SFML 3.

#### Reglas para SFML

Antes de usar una clase o función: - Verifica que exista en la versión
instalada. - Comprueba firmas, namespaces y tipos. - Usa la
documentación correspondiente. - No inventes métodos. - Configura
correctamente CMake y Visual Studio. - Verifica dependencias de
ejecución, incluidas DLL cuando aplique.

#### Reglas para arquitectura

Mantén separados: - Núcleo del juego. - Entrada. - Entidades. -
Combate. - Mundo y mazmorras. - Colisiones. - IA. - Dificultad. -
Progresión. - Guardado. - Renderizado. - Interfaz. - Audio.

No crees todos los módulos de inmediato. Añádelos conforme se necesiten.

#### Reglas para jugabilidad

-   Movimiento y temporizadores independientes de FPS.
-   Movimiento diagonal normalizado.
-   Apuntado correcto con coordenadas de mundo.
-   Colisiones consistentes.
-   Ataques con hitboxes o proyectiles bien definidos.
-   Enemigos con estados de IA claros.
-   Generación procedural reproducible mediante seed.
-   Recompensas equilibradas.
-   Progresión permanente separada del progreso temporal.
-   Dificultad adaptativa limitada y justa.
-   Jefes con patrones legibles.

#### Reglas para calidad

Nunca digas que el proyecto compila si no has ejecutado la compilación.

Nunca digas que una prueba pasó si no se ejecutó.

Si no puedes ejecutar una prueba, explica cómo debo ejecutarla.

No borres ni reemplaces archivos sin revisar su contenido.

No cambies decisiones principales del diseño sin consultarme.

No conviertas el proyecto en una única implementación gigante.

#### Comunicación

Explícame los cambios en español, con lenguaje natural y directo.

El código, nombres de clases y funciones pueden estar en inglés para
mantener convenciones comunes de programación.

Cuando haya un error: 1. Identifica la causa. 2. Indica el archivo y la
línea cuando sea posible. 3. Explica la solución. 4. Aplica la
corrección. 5. Verifica nuevamente.

#### Primera tarea obligatoria

No empieces creando enemigos, armas o mazmorras.

Comienza por: 1. Inspeccionar el entorno. 2. Verificar Visual Studio
2022, compilador C++ y CMake. 3. Comprobar si SFML está instalada. 4.
Recomendar e instalar/configurar SFML 3.x si hace falta y si tengo
autorización para ello. 5. Crear una ventana mínima de prueba. 6.
Verificar compilación Debug y Release. 7. Documentar cómo ejecutar el
proyecto.

Cuando termines, espera mi confirmación antes de avanzar a la siguiente
fase.

Tu prioridad es construir una base sólida y funcional para Ecos del
Abismo, sin sacrificar la calidad por avanzar rápido.

------------------------------------------------------------------------

# PARTE VII. PRIMERA SESIÓN DE TRABAJO

## 28. Mensaje para iniciar el desarrollo

Después de guardar este documento en la raíz del repositorio, envía este
mensaje a la IA de Visual Studio:

> Vamos a comenzar Ecos del Abismo. Lee completamente
> `ECOS_DEL_ABISMO_MASTERPLAN.md`. Primero inspecciona el entorno y los
> archivos existentes. No escribas todavía todos los sistemas del juego.
> Quiero completar la Fase 0: verificar Visual Studio 2022, CMake,
> compilador y SFML; configurar el proyecto y crear una ventana mínima
> funcional. Explícame cada paso, no inventes instalaciones ni rutas y
> no sobrescribas archivos existentes sin revisarlos. Compila en Debug y
> Release cuando sea posible. Al terminar, presenta los resultados y
> espera mi autorización para comenzar la Fase 1.

## 29. Cómo utilizar este documento

1.  Crea una carpeta llamada `EcosDelAbismo`.
2.  Guarda este archivo en su raíz como `ECOS_DEL_ABISMO_MASTERPLAN.md`.
3.  Abre esa carpeta en Visual Studio 2022.
4.  Abre GitHub Copilot Chat o la IA que utilices.
5.  Adjunta o referencia el documento maestro.
6.  Copia el mensaje de inicio de la sección 28.
7.  Revisa los cambios y compila antes de aceptar cada etapa.

**Regla de oro:** primero una base estable, después sistemas de juego,
luego contenido y finalmente pulido.

------------------------------------------------------------------------

# VISIÓN FINAL

Ecos del Abismo debe ser un juego en el que cada expedición cuente una
historia distinta: una combinación inesperada de armas, una habitación
secreta, un jefe difícil, una habilidad que transforma la build o una
decisión arriesgada que termina cambiando la partida.

El objetivo no es acumular características por acumularlas. Es conseguir
que explorar, combatir, descubrir y progresar sea divertido, que cada
sistema se conecte con los demás y que el jugador siempre tenga una
razón para volver a entrar al Abismo.

**Fin del documento maestro --- Ecos del Abismo v1.0**
