#include <stdio.h>
typedef struct {int x; int y;} point2;

point2 transform2(point2 in) {
  point2 s = in;
  s.x *= 2;
  s.y *= 2;
  return s;
};

int main(int argc, char **argv) {
  point2 p = {2, 2};
  p = transform2(p);
  printf("should be 8 8: %d %d\n", transform2(p).x, transform2(p).y);
  return 0;
}
