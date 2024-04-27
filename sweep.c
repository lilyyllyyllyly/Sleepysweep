#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <raylib.h>

// Window/Rendering
#define WIN_W 800
#define WIN_H 600
#define FPS 30

#define TEXT_PAD   3
#define FONT_SIZE 32

// Game
#define HCELLS 6
#define VCELLS 5
#define BOMB_COUNT 5

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

void Process() {
	int leftBtn  = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	int rightBtn = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
	if (!leftBtn && !rightBtn) return;

	int xid = GetMouseX() / (WIN_W/HCELLS);
	int yid = GetMouseY() / (WIN_H/VCELLS);

	if (xid < HCELLS && xid >= 0 && yid < VCELLS && yid >= 0) {
		if (leftBtn) {
			if (cells[yid][xid] & BOMB) {
				printf("BOOM!\nlost :c - better luck next time!\n");
				exit(EXIT_SUCCESS);
			} else if (++known >= HCELLS*VCELLS - BOMB_COUNT) {
				printf("VICTORY!\ngood job!\n");
				exit(EXIT_SUCCESS);
			}
			
			cells[yid][xid] &= ~FLAG; // unflagging
			cells[yid][xid] |= KNOWN; // revealing
		} else {
			cells[yid][xid] ^= FLAG;
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
		} else if (cells[yid][xid] & FLAG) {
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

int main() {
	if (BOMB_COUNT > HCELLS*VCELLS) {
		fprintf(stderr, "[%s:%d (%s)] ERROR: BOMB_COUNT exceeds total cell count (HCELLS*VCELLS).\n", __FILE__, __LINE__, __func__);
		exit(EXIT_FAILURE);
	}

	// Initializing window
	SetTraceLogLevel(LOG_WARNING); /* getting rid of annoying init info */
	InitWindow(WIN_W, WIN_H, "sweeper :3");
	SetTargetFPS(FPS);
	atexit(CloseWindow);

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
		cells[i/HCELLS][i%HCELLS] = 0;

		// Counting bombs
		for (int y = i/HCELLS - 1; y <= i/HCELLS + 1; ++y) {
			if (y < 0) continue;
			if (y >= VCELLS) break;
			for (int x = i%HCELLS - 1; x <= i%HCELLS + 1; ++x) {
				if (x < 0) continue;
				if (x >= HCELLS) break;

				if (cells[y][x] & BOMB) {
					++cells[i/HCELLS][i%HCELLS];
				}
			}
		}
	}

	// Main loop
	while (!WindowShouldClose()) {
		Process();
		Draw();
	}
	
	exit(EXIT_SUCCESS);
}

