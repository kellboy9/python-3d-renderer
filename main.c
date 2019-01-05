#include <stdio.h>
#include <stdlib.h>
#include <python2.7/Python.h>
#include "SDL2/SDL.h"
#include "draw.h"

static PyObject *main_printHewwo(PyObject *self, PyObject *args) {
  int n = 1;
  const char *msg;
  if(!PyArg_ParseTuple(args, "s|i", &msg, &n)) {
    //returning NULL indicates error, interpreter will print it
    return NULL; 
  }
  int i;
  for(i = 0; i < n; i++) {
    printf("Hewwo, %s! ^w^\n", msg);
  }
  Py_RETURN_NONE;
}
static PyMethodDef MainMethods[] = {
  {"printHewwo", main_printHewwo, METH_VARARGS,
   "prints out a cute message"},
  {NULL, NULL, 0, NULL}
};

int main(int argc, char **argv) {
  //initialize python
  Py_NoSiteFlag = 1;
  Py_SetProgramName(argv[0]);
  Py_SetPythonHome("./python27/");
  Py_Initialize();

  //initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("Error: Failed to initialize SDL: %s\n", SDL_GetError()); 
    return EXIT_FAILURE;
  }

  //initialize video renderer
  SDL_Renderer *renderer = NULL;
  SDL_Window *window = NULL;
  const int SCREEN_WIDTH = 640;
  const int SCREEN_HEIGHT = 400;

  if(SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) != 0) {
    printf("Error: Failed to create window and/or renderer: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }
  SDL_ShowCursor(SDL_DISABLE);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
  if(SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT) != 0) {
    printf("Error: Failed to create renderer of size %dx%d: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
    return EXIT_FAILURE;
  }

  //create buffers
  SDL_Texture *screen_buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
  if(screen_buffer == NULL) {
    printf("Error: Failed to create screen buffer: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }
  Uint32 *pixels = calloc(SCREEN_WIDTH*SCREEN_HEIGHT, sizeof(Uint32));
  float *depth = calloc(SCREEN_WIDTH*SCREEN_HEIGHT, sizeof(float));

  //load textures
  SDL_Surface *crate = SDL_LoadBMP("tex_crate.bmp");
  if(crate == NULL) {
    printf("Warning: Failed to load tex_crate.bmp: %s\n", SDL_GetError());
  }
  SDL_ConvertSurfaceFormat(crate, SDL_PIXELFORMAT_ARGB8888, 0); //make sure it's in argb

  //load/execute python modules
  Py_InitModule("main", MainMethods);
  Py_InitModule("draw", DrawMethods);
  PyRun_SimpleString("from modules import *");

  printf("Ready!\n");

  //load the test teapot
  unsigned int cube = loadObjectFile("cube.obj");

  //draw screen
  drawObject(cube, pixels, depth, SCREEN_WIDTH, SCREEN_HEIGHT);
  //pixels[100 * SCREEN_WIDTH + 100] = 0xffffffff;
  SDL_RenderClear(renderer);
  SDL_UpdateTexture(screen_buffer, NULL, pixels, SCREEN_WIDTH*sizeof(Uint32));
  SDL_RenderCopy(renderer, screen_buffer, NULL, NULL);
  SDL_RenderPresent(renderer);

  SDL_Delay(5000);

  //clean up and quit
  free(pixels);
  free(depth);
  SDL_DestroyTexture(screen_buffer);
  SDL_FreeSurface(crate);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  Py_Finalize();
  return EXIT_SUCCESS;
}
