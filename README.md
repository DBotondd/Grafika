# Grafika
# 3D Focipálya Szimuláció

Egy egyszerű, interaktív 3D-s demonstráció, ahol egy focilabdát rúghatsz kapuba, teljes pályaelemekkel (büntetőterület, kapu, háló), füst-animációval gólnál, és stadion‐skybox háttérrel.

## Főbb funkciók

- 🏟️ **Stadion skybox** – valósághű környezeti kocka a pálya körül  
- ⚽ **Mozgó labda** – fizikai alapú ütközés a talajjal, hálóval, kerettel  
- 🎯 **Büntetőpontról rúgható** – közelítés után SPACE-szel rúghatod a labdát  
- 🎉 **Gól-animáció** – füst‐effekt és „GOAL!!!” visszajelzés, majd automatikus újrajátszás  
- 🌱 **Fű-sík** – a pálya zöld fűvel van borítva, sík felület a `y=0` síkban  
- 🎮 **Kamera-vezérlés** – WASD: előre/hátra/balra/jobra, nyilakkal a forgatás kényelmes finomítása  

## Eszközök

- C11-kompatibilis fordító (pl. `gcc` ≥ 9)  
- [GLFW](https://www.glfw.org/), [GLEW](http://glew.sourceforge.net/), [OpenGL 3.3+](https://www.opengl.org/)  
- [cglm](https://github.com/recp/cglm) (vagy más GLM-kompatibilis lineáris algebra könyvtár)  
- [Assimp](https://github.com/assimp/assimp) – 3D modellek betöltéséhez  
- [stb_image](https://github.com/nothings/stb) – textúrák betöltéséhez  
- `make` (vagy más build-automatikus eszköz)  

## Telepítés és fordítás

1. **Könyvtárak telepítése** (például Ubuntu/Debian):
   ```bash
   sudo apt-get update
   sudo apt-get install build-essential libglfw3-dev libglew-dev libglm-dev libassimp-dev
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
│   ├── camera.c / .h
│   ├── shader.c / .h
│   ├── model.c / .h
│   ├── fog.c / .h
│   ├── texture.c / .h
│   └── collision.c / .h
├── Makefile
└── README.md
