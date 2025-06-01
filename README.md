3D Büntetőfoci – OpenGL Játék
Készítette: Dobozi Botond
Neptun kód: HYS4P5

📝 Projekt leírása
Ez a projekt egy egyszerű, de látványos 3D-s büntetőrúgó focijáték, amely OpenGL-lel, saját shaderekkel és interaktív kamerával készült. A játékos egy stadionban próbál minél több gólt lőni egy mozgó kapusnak.
A játék tartalmaz:

Mozgatható kamerát

Dinamikus fényerőszabályzást

Animált füst-effekteket

Gól/miss/győzelem/vesztés overlay-eket

Segéd (help) képernyőt

⚽ Játék menete
Célozz a labdával a képernyőn látható nyíl segítségével.

Rúgj kapura – a kapus jobbra-balra mozog, próbál védeni.

5 gól esetén nyersz, ha a kapus hárít 5-öt, veszítesz.

Gól esetén füstpartikula-effekt jelenik meg.

🚀 Főbb funkciók
Funkció	Leírás
3D modellbetöltés	Labda .obj modellből, saját textúrával
Textúrázás	Fehér-fekete focilabda, stadion háttér
Shader-ek	Minden fő objektumhoz saját shader
Dinamikus fényerő	+ és - gombbal állítható
Részecske rendszer	Egyszerű füst-effekt partikularendszerrel
HUD & overlay	Konzolos/overlayes visszajelzések, help képernyő
Animált kapus	Automatikusan mozog a gólvonalon

🎮 Irányítás
Billentyű	Funkció
WASD	Kamera mozgatása (előre/hátra/jobbra/balra)
Egér	Kamera gyors nézelődés
J / K	Kamera elforgatása balra/jobbra
Nyilak	Labda célzása (felfelé/lefelé/jobbra/balra)
SPACE	Rúgás
+ / -	Fényerő növelése/csökkentése
F1	Súgó megjelenítése
Esc	Kilépés a játékból
F11	Teljes képernyő mód váltás
X	Játék újraindítása

🛠️ Főbb technikai részletek
OpenGL 3.3 Core – Saját shader loader

cglm – Vektor- és mátrixműveletek

GLFW – Ablakkezelés és input

GLEW – OpenGL extension kezelés

stb_image – Textúra betöltés PNG-ből

Textúrák
Fájl	Leírás
assets/ball_diffuse.png	Focilabda textúra
assets/stadium.png	Stadion háttér
assets/help.png	Súgókép, billentyűk
assets/keeper.png	Kapus sprite
assets/goal.png	Gól overlay
assets/miss.png	Hibázott lövés overlay
assets/win.png	Győzelem overlay
assets/lose.png	Veszteség overlay

Objektumok
Labda (OBJ modell)

Kapufa, háló (vertex buffer)

Kapus (sprite quad)

Füst (pont primitívek)

UI (overlay quads)

Shader-ek
sceneSh – 3D objektumok (labda, kapufa, stb.)

groundSh – Fű (talaj)

lineSh – Vonalak, hálók

uiSh – HUD, overlayek

smokeSh – Füst-effekt

spriteSh – Kapus sprite

⚙️ Fordítás & futtatás
Szükséges:
C fordító (pl. gcc)

OpenGL 3.3+ támogatás

GLFW, GLEW fejlécek és könyvtárak

cglm könyvtár

Fordítás parancs (példa):

gcc -Wall -O2 -std=c11 -I./src -o soccer src/main.c src/camera.c src/shader.c src/model.c src/fog.c src/texture.c -lGL -lglfw -lGLEW -lm
Futtatás:

./app

💡 Megjegyzések
Az összes szükséges textúra az assets mappában található.

Ha bármelyik textúra hiányzik, a program azt kiírja a konzolra.
