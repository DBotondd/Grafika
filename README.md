# 3D Focipálya Szimuláció

Egy interaktív, OpenGL alapú demonstráció, amelyben egy büntetőpontról rúgható focilabda szerepel, valósághű pályaelemekkel (büntetőterület, kapu, háló), füst-effekttel gólnál, és stadion–skybox háttérrel.

---

## 📋 Tartalom

1. [Áttekintés](#áttekintés)  
2. [Főbb funkciók](#főbb-funkciók)  
3. [Megvalósítás részletei](#megvalósítás-részletei)  
   - Kamera- és inputkezelés  
   - Labdafizika és ütközés  
   - Füst részecskerendszer  
   - Pályaelemek és skybox  
   - Shaderek  
4. [Projekt struktúra](#projekt-struktúra)  
5. [Fordítás & futtatás](#fordítás--futtatás)  
6. [Vezérlés](#vezérlés)  
7. [Licenc](#licenc)  

---

## Áttekintés

A program C11 és OpenGL 3.3 Core Profile alatt fut. A felhasználó a WASD gombokkal mozgatja a kamerát, SPACE-szel rúghatja a labdát, a nyilakkal finomhangolható a nézőpont. A pálya talaját egy zöld sík (fű), a környezetet stadion‐skybox adja, míg a kapu és háló lineáris primitívekből épül fel. A labda fizikailag viselkedik gravitáció és fékezés hatására, gól esetén füstfelhő és automatikus újraindítás következik.

---

## Főbb funkciók

- **Stadion skybox** – környezeti kocka 6 oldallal, stadion textúrákkal  
- **Zöld talaj** – a `y=0` síkban kirajzolt zöld quad (fű)  
- **Mozgó labda** – gravitáció, ütközés a talajjal, hálóval, kerettel; rugóerő SPACE-szel  
- **Kamera-vezérlés** – WASD (mozgás), nyilak (forgatás), scroll (zoom)  
- **Füst-effekt** – egyszerű részecskerendszer gólnál  
- **Gól-animáció** – 200 részecske, “GOAL!!!” üzenet, 2 mp után reset  

---

## Megvalósítás részletei

### Kamera- és inputkezelés

- **`camera.c/.h`**: klasszikus FPS-kamera  
  - Pozíció (vec3), irányvektorok (Front, Right, Up)  
  - `Camera_ProcessKeyboard()` mozgatja előre/hátra/balra/jobra a `CAMERA_MV_*` enum szerint  
  - `Camera_ProcessMouseMovement()` állítja a yaw/pitch értékeket (egérmozgás vagy nyilak)  
  - `Camera_ProcessMouseScroll()` a zoom (FOV) kezelésére  

- **input**: a `processInput()` minden frame-ben ellenőrzi a GLFW billentyű-állapotokat, és hívja a kamera- vagy rúgásfunkciókat.

### Labdafizika és ütközés

- **Állapot**: `vec3 ballPos`, `ballVel`  
- **Rúgás**: ha a labda és kamera távolsága <2 egység és SPACE lenyomott, akkor a labda sebessége a kamera Front irányába kap egy impulzust:  
  ```c
  vec3 d = camera.Front; d.y = 0.2f; normalize(d);
  ballVel += d * KICK_FORCE;
Gravitáció: minden frame-ben ballVel.y -= GRAVITY * dt

Franciás fékezés: talajra csapódva a vel.y és vízszintes komponensek is csökkennek * -0.5, * FRICTION

Ütközés a hálóval: ha a labda átlépi a net síkját (netZ) a kapu szélességén belül, visszapattan

Füst részecskerendszer
Egyszerű tároló: statikus tömb Particle particles[MAX_PARTICLES]

Életciklus: minden részecske kap kezdeti pozíciót, sebességet, és 1 másodperc életet

Frissítés: updateSmoke(dt) csökkenti az életet, szaporítja a pozíciót, és eltávolítja a leperegteket

Kirajzolás: pontméret és átlátszó textúra shader (smoke_vertex/smoke_fragment)

Pályaelemek és skybox
Talaj: egy egyszerű quad (GL_TRIANGLE_FAN) a y=0 síkban, zöld fragment shaderrel

Vonalak: büntetőterület és kapuvonal GL_LINES primitívek

Kapu és crossbar: kis kockák (36 vertex / VAO) transzformálva posztokká és rúdá

Skybox: (opcionális) cubemap textúra 6 oldallal; külön VAO/VBO és skybox shader

Shaderek
vertex.glsl / fragment.glsl: Phong-szerű egyszerű textúra–világítás; useTexture uniform

line_vert.glsl / line_frag.glsl: egyszínű vonal shader

ground_vert.glsl / ground_frag.glsl: zöld föld sík

smoke_vertex.glsl / smoke_fragment.glsl: pontalapú részecskerendszer, élettartam szerinti átlátszóság

skybox_vert.glsl / skybox_frag.glsl: cubemap sampling

Projekt struktúra
css

.
├── assets/
│   ├── ball_uv.obj
│   ├── ball_diffuse.png
│   └── skybox/
│       ├── right.jpg
│       ├── left.jpg
│       ├── top.jpg
│       ├── bottom.jpg
│       ├── front.jpg
│       └── back.jpg
├── shaders/
│   ├── vertex.glsl
│   ├── fragment.glsl
│   ├── line_vert.glsl
│   ├── line_frag.glsl
│   ├── ground_vert.glsl
│   ├── ground_frag.glsl
│   ├── smoke_vertex.glsl
│   ├── smoke_fragment.glsl
│   ├── skybox_vert.glsl
│   └── skybox_frag.glsl
├── src/
│   ├── main.c
│   ├── camera.c / camera.h
│   ├── shader.c / shader.h
│   ├── model.c / model.h
│   ├── fog.c / fog.h
│   ├── texture.c / texture.h
│   └── collision.c / collision.h
├── Makefile
└── README.md
Fordítás & futtatás
Követelmények:

GLFW, GLEW, Assimp, stb (sudo apt install libglfw3-dev libglew-dev libassimp-dev)

stb_image (be van ágyazva a texture.c-ben)

Fordítás:

bash
make
Futtatás:

bash
./app
Vezérlés
W/S/A/D – kamera előre/hátra/balra/jobra

←/→/↑/↓ – kamera forgatása (finomhangolás)

SPACE – labda rúgása (ha közel vagy hozzá)

ESC – kilépés

