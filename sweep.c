#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <raylib.h>

#ifdef PLATFORM_WEB
    #include <emscripten/emscripten.h>
#endif

// Window/Rendering
#define WIN_W 800
#define WIN_H 600
#define FPS 30

#define TEXT_PAD   3
#define FONT_SIZE 32

// Game
#define HCELLS 10
#define VCELLS 10
#define BOMB_COUNT 25

// cell bits:
// 01111111
//     ^--^ -> number of bombs around cell  (&0xF)
//    ^     -> wether cell has been clicked (&1<<4)
//   ^      -> wether cell is a bomb        (&1<<5)
//  ^       -> wether cell is flagged       (&1<<6)
// rest is unused

#define NUM   0xF
#define KNOWN (1<<4) // clicked
#define BOMB  (1<<5)
#define FLAG  (1<<6)

char cells[VCELLS][HCELLS];
int known = 0;
int ended = 0;

void Start() {
	known = 0;
	ended = 0;

	memset(cells, 0, sizeof(cells));

	// Placing bombs
	srand(time(NULL));
	for (int i = 0; i < BOMB_COUNT; ++i) {
		int r = rand() % (HCELLS*VCELLS);
		while (cells[r/HCELLS][r%HCELLS] & BOMB) {
			r = (r + 1) % (HCELLS*VCELLS);
		}

		cells[r/HCELLS][r%HCELLS] = BOMB;
	}

	// Initializing other cells
	for (int i = 0; i < HCELLS*VCELLS; ++i) {
		if (cells[i/HCELLS][i%HCELLS] & BOMB) continue;

		// Counting bombs
		for (int y = i/HCELLS - 1; y <= i/HCELLS + 1; ++y) {
			if (y < 0) continue;
			if (y >= VCELLS) break;
			for (int x = i%HCELLS - 1; x <= i%HCELLS + 1; ++x) {
				if (x < 0 || (x == i%HCELLS && y == i/HCELLS)) continue;
				if (x >= HCELLS) break;

				if (cells[y][x] & BOMB) {
					++cells[i/HCELLS][i%HCELLS];
				}
			}
		}
	}
}

void Reveal(int yid, int xid) {
	if (cells[yid][xid] & KNOWN) return;

	cells[yid][xid] &= ~FLAG; // unflagging
	cells[yid][xid] |= KNOWN; // revealing

	if (++known >= HCELLS*VCELLS - BOMB_COUNT){
		printf("VICTORY!\ngood job!\n");
		ended = true;
	}

	if ((cells[yid][xid] & NUM) == 0) {
		for (int newy = yid - 1; newy <= yid + 1; ++newy) {
			if (newy < 0) continue;
			if (newy >= VCELLS) break;
			for (int newx = xid - 1; newx <= xid + 1; ++newx) {
				if (newx < 0 || (newx == xid && newy == yid)) continue;
				if (newx >= HCELLS) break;
				
				Reveal(newy, newx);
			}
		}
	}
}

void Process() {
	int leftBtn  = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	int rightBtn = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
	if (!leftBtn && !rightBtn) return;

	if (ended) {
		Start();
		return;
	}

	int xid = GetMouseX() / (WIN_W/HCELLS);
	int yid = GetMouseY() / (WIN_H/VCELLS);

	if (xid < HCELLS && xid >= 0 && yid < VCELLS && yid >= 0) {
		if (leftBtn && !(cells[yid][xid] & FLAG)) {
			if (cells[yid][xid] & BOMB) {
				printf("BOOM!\nlost :c - better luck next time!\n");
				ended = true;
			} else {
				Reveal(yid, xid);
			}
		} else if (rightBtn) {
			if (!(cells[yid][xid] & KNOWN)) cells[yid][xid] ^= FLAG;
		}
	}
}

void Draw() {
	BeginDrawing();

	for (int i = 0; i < HCELLS*VCELLS; ++i) {
		int xid = i%HCELLS;
		int yid = i/HCELLS;

		int xpos = xid*WIN_W/HCELLS;
		int ypos = yid*WIN_H/VCELLS;
		int w = WIN_W/HCELLS;
		int h = WIN_H/VCELLS;

		Color fill = WHITE;
		int draw_num = 0;
		if (cells[yid][xid] & KNOWN) {
			fill = GRAY;
			draw_num = 1;
		} else if ((!ended && cells[yid][xid] & FLAG) || (ended && cells[yid][xid] & BOMB)) {
			fill = RED;
		}

		DrawRectangle(xpos, ypos, w, h, fill);
		DrawRectangleLines(xpos, ypos, w, h, BLACK);

		// Drawing number
		if (!draw_num) continue;
		int num = cells[yid][xid] & NUM;

		char* msg = calloc(2, sizeof(char));
		msg[0] = '0' + num;
		msg[1] = 0;

		DrawText(msg, xpos + TEXT_PAD, ypos + TEXT_PAD, FONT_SIZE, BLACK);

		free(msg);
	}

	EndDrawing();
}

void MainLoop() {
	Process();
	Draw();
}

int main() {
	if (BOMB_COUNT > HCELLS*VCELLS) {
		fprintf(stderr, "[%s:%d (%s)] ERROR: BOMB_COUNT exceeds total cell count (HCELLS*VCELLS).\n", __FILE__, __LINE__, __func__);
		exit(EXIT_FAILURE);
	}

	// Initializing window
	SetTraceLogLevel(LOG_WARNING); /* getting rid of annoying init info */
	InitWindow(WIN_W, WIN_H, "sweeper :3");
	atexit(CloseWindow);

	Start();

	// Main loop
#ifndef PLATFORM_WEB
	SetTargetFPS(FPS);
	while (!WindowShouldClose()) {
		MainLoop();
	}
#else
	emscripten_set_main_loop(MainLoop, FPS, 1);
#endif
	
	exit(EXIT_SUCCESS);
}

