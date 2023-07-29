#include <limits.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define N 4
typedef struct {
  uint tab[N * N];
  uint empty[N * N];
  uint points;
  bool end;
} game_t;
const int BLOCKSIZE = 200;
const int DESL = 100;
const int WIDTH = N * BLOCKSIZE;
const int HEIGHT = WIDTH + DESL;
const int TEXTSIZE = BLOCKSIZE / 10;
Color BACKGROUNDCOLOR = CLITERAL(Color){205, 193, 180, 200};
static char buff[1024];

int digitAmount(uint num) {
  int c = 0;
  do
    c++, num /= 10;
  while (num > 0);
  return c;
}
uint greaterNum(game_t *game) {
  uint max = game->tab[0];
  for (int i = 1; i < N * N; i++) {
    if (game->tab[i] > max)
      max = game->tab[i];
  }
  return max;
}
int countEmpty(game_t *game) {
  int c = 0;
  for (int i = 0; i < N * N; i++) {
    if (game->tab[i] == 0)
      game->empty[c++] = i;
  }
  return c;
}
void swap(uint *arr, int i1, int i2) {
  uint t = arr[i1];
  arr[i1] = arr[i2];
  arr[i2] = t;
}
bool checkGameOver(game_t *game) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N - 1; j++) {
      int id1 = i * N + j;
      int id2 = j * N + i;
      if (game->tab[id1] == game->tab[id1 + 1] ||
          game->tab[id2] == game->tab[id2 + N]) {
        return false;
      }
    }
  }
  return true;
}
void putrand(game_t *game, int count, int disp) {
  if (disp == 0)
    return;
  while (count > 0) {
    int rd = rand() % disp;
    game->tab[game->empty[rd]] = (rand() % 10) <= 1 ? 4 : 2;
    swap(game->empty, rd, --disp);
    count--;
  }
}
void simpleMov(game_t *game, int start, int step) {
  static uint temp[N];
  int idx = 0;
  bool sum = false;
  for (int i = start, c = 0; c < N; i += step, c++) {
    if (game->tab[i] != 0) {
      if (sum && idx > 0 && temp[idx - 1] == game->tab[i]) {
        game->points += temp[idx - 1] += game->tab[i];
        sum = false;
      } else {
        temp[idx++] = game->tab[i];
        sum = true;
      }
      game->tab[i] = 0;
    }
  }
  for (int i = start, ii = 0; ii < idx; i += step, ii++)
    game->tab[i] = temp[ii];
}
void gameUp(game_t *game) {
  for (int i = 0; i < N; i++)
    simpleMov(game, i, N);
}
void gameDown(game_t *game) {
  for (int i = N * N - N; i < N * N; i++)
    simpleMov(game, i, -N);
}
void gameLeft(game_t *game) {
  for (int i = 0; i < N * N; i += N)
    simpleMov(game, i, 1);
}
void gameRight(game_t *game) {
  for (int i = N - 1; i < N * N; i += N)
    simpleMov(game, i, -1);
}
void printGame(game_t *game) {
  int max = greaterNum(game);
  int d = digitAmount(max) + 1;
  for (int i = 0; i < N * N; i++)
    printf("[%*u ]%c", d, game->tab[i], (i + 1) % N == 0 ? '\n' : ' ');
  printf("\n");
}
void drawInfo(game_t *game) {
  sprintf(buff, "Pontuação: %d", game->points);
  DrawText(buff, 0, 0, TEXTSIZE * 2, GREEN);
  if (game->end) {
    sprintf(buff, "Fim de Jogo!\nPressione espaco para continuar!");
    DrawText(buff, 0, TEXTSIZE * 2, TEXTSIZE, RED);
  }
}
Color colorForCell(uint p) {
  static Color cores[] = {
      CLITERAL(Color){238, 228, 218, 255}, CLITERAL(Color){237, 224, 200, 255},
      CLITERAL(Color){242, 177, 121, 255}, CLITERAL(Color){245, 149, 99, 255},
      CLITERAL(Color){245, 149, 99, 255},  CLITERAL(Color){237, 207, 114, 255},
      CLITERAL(Color){237, 207, 114, 255}, CLITERAL(Color){237, 204, 97, 255},
      CLITERAL(Color){237, 200, 80, 255},  CLITERAL(Color){255, 150, 0, 255}};
  int count = -2;
  while (p > 0)
    p >>= 1, count++;
  return count > 10 ? BLACK : cores[count];
}
void drawCell(game_t *game, int i) {
  if (game->tab[i] != 0) {
    int x = (i % N) * BLOCKSIZE;
    int y = (i / N * BLOCKSIZE) + DESL;
    // ColorBrightness(ORANGE, (float)1 / (N * N) * i);

    Color c = colorForCell(game->tab[i]);
    Rectangle rec = {.x = x + 1,
                     .y = y + 1,
                     .height = BLOCKSIZE - 2,
                     .width = BLOCKSIZE - 2};
    DrawRectangleRounded(rec, 0.1f, 5, c);
    int d = digitAmount(game->tab[i]);
    sprintf(buff, "%u", game->tab[i]);
    DrawText(buff, x + BLOCKSIZE / 4, y + TEXTSIZE * d, BLOCKSIZE / d, WHITE);
  }
}
void drawGame(game_t *game) {
  drawInfo(game);
  DrawRectangle(0, DESL, WIDTH, HEIGHT - DESL, BACKGROUNDCOLOR);
  DrawLine(0, DESL, WIDTH, DESL, GREEN);

  for (int i = 0; i < N * N; i++)
    drawCell(game, i);
  for (int i = 1; i < N; i++) {
    int nht = i * BLOCKSIZE;
    DrawLine(0, nht + DESL, WIDTH, nht + DESL, GREEN);
    DrawLine(nht, DESL, nht, HEIGHT, GREEN);
  }
}
void (*direcoes[])(game_t *) = {gameRight, gameLeft, gameDown, gameUp};
bool compareGameStates(game_t *game, uint *prev) {
  for (int i = 0; i < N * N; i++) {
    if (game->tab[i] != prev[i])
      return false;
  }
  return true;
}
int main(void) {
  srand(time(0));
  InitWindow(WIDTH, HEIGHT, "Jogo 2048");
  SetTargetFPS(10);
  game_t game = {.tab = {0}, .empty = {0}, .end = false, .points = 0};
  putrand(&game, 2, countEmpty(&game));
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    int key = 0;
    if ((key = GetKeyPressed()) != 0 && (key >= 262 && key <= 265)) {
      static uint p[N * N];
      memcpy(p, game.tab, sizeof(uint) * N * N);
      direcoes[key - 262](&game);
      int disp = countEmpty(&game);
      if (disp == 0)
        game.end = checkGameOver(&game);
      if (!compareGameStates(&game, p)) {
        putrand(&game, 1, disp);
      }
    } else if (key == 32 && game.end) {
      memset(&game, 0, sizeof(game));
    }
    drawGame(&game);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
