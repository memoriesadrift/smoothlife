#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define WIDTH 100
#define HEIGHT 100

char level[] = " .-=coaA@#";
int level_count = (sizeof(level)/sizeof(level[0]) - 1);

float grid[HEIGHT][WIDTH] = {0.0f};
float grid_diff[HEIGHT][WIDTH] = {0.0f};

float ra = 11; // Outer radius
float alpha = 0.028;
// float alpha = 0.147;
float b1 = 0.278;
float b2 = 0.365;
float d1 = 0.267;
float d2 = 0.445;
float dt = 0.01f;

void clear_screen(void)
{
  printf("\e[1;1H\e[2J");
}

float rand_float(void)
{
  return (float)rand()/(float)RAND_MAX;
}

void clamp(float *x, float lo, float hi)
{
  if (*x < lo) *x = lo;
  if (*x > hi) *x = hi;
}

void print_grid(float grid[HEIGHT][WIDTH])
{
  for (size_t y = 0; y < HEIGHT; ++y) {
    for (size_t x = 0; x < WIDTH; ++x) {
      char display = level[(int)(grid[y][x] * (level_count - 1))];
      fputc(display, stdout);
    }
    fputc('\n', stdout);
  }
}

void random_grid(void)
{
  size_t w = WIDTH / 3;
  size_t h = HEIGHT / 3;

  for (size_t dy = 0; dy < h; ++dy) {
    for (size_t dx = 0; dx < w; ++dx) {
      size_t x = dx + WIDTH / 2 - w/2;
      size_t y = dy + HEIGHT / 2 - h/2;
      grid[y][x] = rand_float();
    }
  }
}

// wrapping modulo
int emod(int a, int b)
{
  return (a % b + b) % b;
}

// Sigmoid "black boxes" from paper

float sigma1(float x, float a)
{
  return 1.0f / (1.0f + expf(-(x - a) * 4 / alpha));
}

float sigma2(float x, float a, float b)
{
  return sigma1(x, a) * (1 - sigma1(x, b));
}

float sigmam(float x, float y, float m)
{
  return x * (1 - sigma1(m, 0.5f)) + y * sigma1(m, 0.5f);
}

float s(float n, float m)
{
  return sigma2(n, sigmam(b1, d1, m), sigmam(b2, d2, m));
}

void compute_grid_diff(void)
{
  int cx = 0;
  int cy = 0;

  for (int cy = 0; cy < HEIGHT; ++cy) {
    for (int cx = 0; cx < WIDTH; ++cx) {
      float m = 0, M = 0;
      float n = 0, N = 0;
      float ri = ra / 3; // inner radius

      for (int dy = -(ra - 1); dy <= (ra -1); ++dy) {
        for (int dx = -(ra - 1); dx <= (ra -1); ++dx) {
          int x = emod(cx + dx, WIDTH);
          int y = emod(cy + dy, HEIGHT);

          if (dx * dx + dy * dy <= ri * ri) {
            M++;
            m += grid[y][x];
          } else if (dx * dx + dy * dy <= ra * ra) {
            N++;
            n += grid [y][x];
          }

        }
      }

      // Normalise
      m /= M;
      n /= N;

      float q = s(n,m);
      grid_diff[cy][cx] = 2 * q - 1;
    }
  }
}

void apply_diff(void)
{
  for (size_t y = 0; y < HEIGHT; ++y) {
    for (size_t x = 0; x < WIDTH; ++x) {
      grid[y][x] += dt * grid_diff[y][x];
      clamp(&grid[y][x], 0, 1);
    }
  }
}

int main(void)
{
  srand(time(0));
  random_grid();

  for (;;) {
    compute_grid_diff();
    apply_diff();
    clear_screen();
    print_grid(grid);
  }
  return 0;
}
