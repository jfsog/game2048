#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#define N 3
typedef struct {
  int tab[N * N];
  bool end;
} game_t;
int digitAmount(int num) {
  int c = 0;
  do
    c++, num /= 10;
  while (num > 0);
  return c;
}
int greaterNum(game_t *game) {
  int max = game->tab[0];
  for (int i = 1; i < N * N; i++) {
    if (game->tab[i] > max)
      max = game->tab[i];
  }
  return max;
}
int countEmpty(game_t *game) {
  int c = 0;
  for (int i = 0; i < N * N; i++)
    c += (game->tab[i] == 0);
  return c;
}
void putrand(game_t *game, int count) {
  int disponivel = countEmpty(game);
  if (disponivel == 0) {
    game->end = true;
    return;
  }
  while (count > 0) {
    int rd;
    do
      rd = rand() % (N * N);
    while (game->tab[rd] != 0);
    game->tab[rd] = 2;
    count--;
  }
}
void simpleMov(game_t *game, int start, int step) {
  static int temp[N];
  int idx = 0;
  for (int i = start, c = 0; c < N; i += step, c++) {
    if (game->tab[i] != 0) {
      if (idx > 0 && temp[idx - 1] == game->tab[i])
        temp[idx - 1] += game->tab[i];
      else
        temp[idx++] = game->tab[i];
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
    simpleMov(game, i, -1);
}
void gameRight(game_t *game) {
  for (int i = N - 1; i < N * N; i += N)
    simpleMov(game, i, -1);
}
void printGame(game_t *game) {
  int max = greaterNum(game);
  int d = digitAmount(max) + 1;
  for (int i = 0; i < N * N; i++)
    printf("[%*d ]%c", d, game->tab[i], (i + 1) % N == 0 ? '\n' : ' ');
  printf("\n");
}
int main(void) {
  game_t *game = calloc(sizeof(game_t), 1);
  putrand(game, 6);
  printGame(game);
  gameRight(game);
  printGame(game);
  return 0;
}
