#include "SDL2/SDL.h"
#include <python2.7/Python.h>

#ifndef DRAW_H
#define DRAW_H

/* TODO: implement drawing routines (just color for now)
 * then add SDL image texturing capabilities
 * then add python support
 * then RELEASE
 */


//basic list type for lists
/*typedef struct List List;
struct List {
  void *pdata;
  List *next;
};
List *list_append(List *head, void *item); //returns newly appended item
List *list_index(List *head, int i);
void list_delete(List *head);*/

//structure to represent all 3d objects (OpenGL compatible)
struct DrawObject {
  float *vertices; //point3
  float *texcoords; //point2
  float *normals;
  //only 1 size/capacity because each vertex has 1 normal and 1 texcoord
  size_t size;
  size_t capacity;

  //indices have separate memory
  int *indices;
  size_t size_i;
  size_t capacity_i;

  SDL_Surface *texture;
};

unsigned int loadObjectFile(const char *filename);
PyObject *draw_loadObject(PyObject *self, PyObject *args);
static PyMethodDef DrawMethods[] = {
  {"load_object", draw_loadObject, METH_VARARGS,
   "load a 3d object into memory for rendering"},
  {NULL, NULL, 0, NULL}
};

//global-ish image registry so that SDL doesn't have to load the same image multiple times
//List *texture_names; 
//will load an image if not loaded, or simply reference it if it is loaded
SDL_Surface *loadImage(const char *filname);


//the main draw routine!!!
void drawObject(unsigned int object, Uint32 *pixels, float *depth, int width, int height);

#endif
