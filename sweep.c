#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <raylib.h>

// Window/Rendering
#define WIN_W 800
#define WIN_H 600
#define FPS 30

// Game
#define HCELLS 6
#define VCELLS 5
#define BOMB_COUNT 5

// cell bits:
// 00011111
//      ^-^ -> number of bombs around cell  (&7)
//     ^    -> wether cell has been clicked (&1<<3)
//    ^     -> wether cell is a bomb        (&1<<4)
// rest is unused

#define NUM   7
#define KNOWN 1<<3 // clicked
#define BOMB  1<<4

char cells[VCELLS][HCELLS];

void Process() {
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		int xid = GetMouseX() / (WIN_W/HCELLS);
		int yid = GetMouseY() / (WIN_H/VCELLS);

		if (xid < HCELLS && xid >= 0 && yid < VCELLS && yid >= 0) {
			if (cells[yid][xid] & BOMB) {
				printf("BOOM!\nlost :c - better luck next time!\n");
				exit(EXIT_SUCCESS);
			} else {
				cells[yid][xid] |= KNOWN;
			}
		}
	}
}

void Draw() {
	BeginDrawing();

	for (int i = 0; i < HCELLS*VCELLS; ++i) {
		int xid = i%HCELLS;
		int yid = i/HCELLS;

		Color fill = WHITE;
		if (cells[yid][xid] & KNOWN)     fill = GRAY;
		else if (cells[yid][xid] & BOMB) fill = RED;

		DrawRectangle(xid*WIN_W/HCELLS, yid*WIN_H/VCELLS, WIN_W/HCELLS, WIN_H/VCELLS, fill);
		DrawRectangleLines(xid*WIN_W/HCELLS, yid*WIN_H/VCELLS, WIN_W/HCELLS, WIN_H/VCELLS, BLACK);
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

	// Getting bomb indices
	int bombs[BOMB_COUNT];

	srand(time(NULL));
	for (int i = 0; i < BOMB_COUNT; ++i) {
		int r = -1;
		while (r < 0 || cells[r/HCELLS][r%HCELLS] & BOMB) {
			r = rand() % (HCELLS*VCELLS);
		}

		cells[r/HCELLS][r%HCELLS] = BOMB;
	}
	
	// Initializing cells
	for (int i = 0; i < HCELLS*VCELLS; ++i) {
		cells[i/HCELLS][i%HCELLS] = 0;
	}
	for (int i = 0; i < BOMB_COUNT; ++i) {
		cells[bombs[i]/HCELLS][bombs[i]%HCELLS] = BOMB;
	}

	// Main loop
	while (!WindowShouldClose()) {
		Process();
		Draw();
	}
	
	exit(EXIT_SUCCESS);
}

