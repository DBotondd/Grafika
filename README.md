3D Büntetőfoci – OpenGL játék
Készítette:
Név: Dobozi Botond
Neptun kód: HYS4P5


Projekt leírása
Ez a projekt egy egyszerű 3D-s büntetőrúgó focijáték OpenGL-lel, saját shaderekkel és interaktív kamerával.
A játékos egy stadionban találja magát, ahol a cél minél több gólt rúgni a kapusnak.
A játék tartalmaz mozgatható kamerát, dinamikus fényerőt, animált füst-effekteket, gól/miss/győzelem/vesztés overlay-eket és segéd (help) képernyőt.

Játék menete
A labdát a játékos egy képernyőn megjelenő nyíllal célozza, majd rúgja kapura.

A kapuban egy jobbra-balra mozgó kapus található, aki próbálja hárítani a lövéseket.

Ha 5 gólt rúgsz, megnyered a játékot, ha 5-öt véd a kapus, veszítesz.

Füstpartikula-effekt jelenik meg gól esetén.

Főbb funkciók
3D modellbetöltés: A labda .obj fájlból, saját textúrával töltődik be.

Textúrázás: Saját generált fehér-fekete „focilabda” textúra (ball_diffuse.png), illetve stadion háttér (stadium.png).

Shader-ek: Minden fő objektumhoz külön vertex/fragment shader.

Fényerő: A jelenet fényereje dinamikusan állítható a + és - gombokkal.

Részecske rendszer: Egyszerű füst-effekt partikularendszerrel, amikor gól születik.

HUD: Konzolos és overlayes visszajelzés az aktuális állásról, segítség overlay.

Animált kapus: A kapus automatikusan mozog a gólvonalon.

Irányítás


WASD           - Kamera mozgatása (előre/hátra/jobbra/balra)
Egér           - Kamera gyorsabb mozgatása (nézelődés)
J / K          - Kamera elforgatása balra/jobbra
Nyilak         - Labda célzása (felfelé/lefelé/yaw)
SPACE          - Rúgás
+ / -          - Fényerő növelése/csökkentése
F1             - Súgó megjelenítése
Esc            - Kilépés a játékból
F11            - Teljes képernyő mód váltás
X              - Játék újraindítása
Főbb technikai részletek
OpenGL 3.3 Core – Saját shader loader.

cglm – Vektor- és mátrixműveletekhez.

GLFW – Ablakkezelés és input.

GLEW – OpenGL extension kezelés.

stb_image – Textúra betöltés PNG-ből.

Textúrák:

assets/ball_diffuse.png – Focilabda textúra (saját generált)

assets/stadium.png – Stadion háttér (egyszerű png)

assets/help.png – Súgókép, billentyűk listája

assets/keeper.png – Kapus sprite

assets/goal.png, miss.png, win.png, lose.png – Overlay képek

Objektumok:

Labda (OBJ modell)

Kapufa, háló (vertex bufferrel rajzolva)

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

Fordítás / Futtatás
Fordításhoz szükséges:

C fordító (pl. gcc)

OpenGL 3.3+ támogatás

GLFW, GLEW, stb. fejlécek és könyvtárak

cglm

Fordítás parancs (tipikus):

gcc -Wall -O2 -std=c11 -I./src -o soccer src/main.c src/camera.c src/shader.c src/model.c src/fog.c src/texture.c -lGL -lglfw -lGLEW -lm
Futtatás:

./app
Megjegyzések
Az összes szükséges textúra az assets mappában található
Link: https://unimiskolchu-my.sharepoint.com/:f:/r/personal/botond_dobozi_student_uni-miskolc_hu/Documents/assets?csf=1&web=1&e=VZm42h

Ha bármelyik textúra hiányzik, a program azt kiírja a konzolra
