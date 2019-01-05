#include "SDL2/SDL.h"
#include <Python.h>

typedef float point3[3];
typedef float point2[2];
typedef point3 vec3;
typedef point2 vec2;

inline float dot(point3 a, point3 b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}
inline vec3 cross(vec3 a, vec3 b) {
    vec3 c = {a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[1]*b[0]-a[0]*b[1]};
    return c;
}
inline float edge(point2 a, point2 b, point2 c) {
    return (c[0]-a[0])*(b[1]-a[1])-(c[1]-a[1])*(b[0]-a[0]);
}


//basic list type for lists
typedef struct List List;
struct List {
  void *pdata;
  List *next;
};
List *append(List *head, void *item); //returns newly appended item
List *index(List *head, int i);
void delete(List *head);

//structure to represent all 3d objects (OpenGL compatible)
struct 3dObj {
  point3 *vertices;
  int *indices;
  point2 *texcoords;
  SDL_Surface *texture;
};

static PyObject *draw_loadObject(PyObject *self, PyObject *args);

//global-ish image registry so that SDL doesn't have to load the same image multiple times
List *texture_names; 
//will load an image if not loaded, or simply reference it if it is loaded
SDL_Surface *loadImage(const char *filename);
