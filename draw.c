#include "draw.h"
#include <stdio.h>

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

static int SIZE_OBJECTS = 0;
static struct DrawObject **objects = NULL;
static unsigned int object_index = 1; //1 so that 0 can be error

//DON'T USE OBJECT_INDEX DIRECTLY USE THIS FUNC
int increment_object_index() {
  //make sure we have enough memory before initializing
  if(object_index >= SIZE_OBJECTS) {
    SIZE_OBJECTS = object_index * 2;
    objects = realloc(objects, sizeof(struct DrawObject*)*SIZE_OBJECTS);
    printf("New size: %d\n", SIZE_OBJECTS);
  }
  objects[object_index] = malloc(sizeof(struct DrawObject));
  objects[object_index]->vertices = NULL;
  objects[object_index]->texcoords = NULL;
  objects[object_index]->normals = NULL;
  objects[object_index]->size = 0;
  objects[object_index]->capacity = 0;
  objects[object_index]->indices = NULL;
  objects[object_index]->size_i = 0;
  objects[object_index]->capacity_i = 0;
  objects[object_index]->texture = NULL;
  return object_index++;
}

void obj_add_vertex(struct DrawObject *obj, float x, float y, float z) {
  obj->size+=3;
  if(obj->size > obj->capacity) {
    obj->capacity = obj->size * 2;
    obj->vertices = realloc(obj->vertices, sizeof(float)*obj->capacity);
  }
  obj->vertices[obj->size-3] = x;
  obj->vertices[obj->size-2] = y;
  obj->vertices[obj->size-1] = z;
}
void obj_add_face(struct DrawObject *obj, int a, int b, int c) {
  obj->size_i+=3;
  if(obj->size_i > obj->capacity_i) {
    obj->capacity_i = obj->size_i * 2;
    obj->indices = realloc(obj->indices, sizeof(int)*obj->capacity_i);
  }
  obj->indices[obj->size_i-3] = a;
  obj->indices[obj->size_i-2] = b;
  obj->indices[obj->size_i-1] = c;
}

//loads an object and returns a positive index, or, a negative error
unsigned int loadObjectFile(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if(fp == NULL) {
    printf("Warning: could not open file: %s\n", filename);
    return 0;
  }
  char line[2048];
  int obj = increment_object_index();
  while(fgets(line, 2048, fp)) {
    switch(line[0]) {
    case '#':
      //comment
      break;
    case 'v':
      //printf("loading vertex...\n");
      //vertex
      ;
      float x, y, z;
      sscanf(line, "v %f %f %f", &x, &y, &z);
      //printf("v %f %f %f\n", x, y, z);
      obj_add_vertex(objects[obj], x, y, z);
      break;
    case 'f':
      //face (indices)
      ;
      //printf("loading face...\n");
      int a, b, c;
      sscanf(line, "f %d %d %d", &a, &b, &c);
      //printf("f %d %d %d\n", a, b, c);
      obj_add_face(objects[obj], a, b, c);
      break;
    }
  }
  /*for(int i = 0; i < objects[obj]->size_i; ++i) {
    printf("%d ", objects[obj]->indices[i]);
    }
    for(int i = 0; i < objects[obj]->size; ++i) {
    printf("%f ", objects[obj]->vertices[i]);
    }*/
  return obj;
}

PyObject *draw_loadObject(PyObject *self, PyObject *args) {
  //prepare the return object
  PyObject *ret = Py_BuildValue("I", increment_object_index());
  //vertices indices texcoords texture
  PyObject *vertices;
  PyObject *indices;
  PyObject *texcoords;
  const char *surfaceName;
  if(!PyArg_ParseTuple(args, "O!O!O!s", &PyList_Type, &vertices, &PyList_Type, &indices, &PyList_Type, &texcoords, &surfaceName)) {
    return NULL;
  }
  return ret;
}


typedef struct {float x; float y; float z;} point3;
typedef struct {float x; float y;} point2;
typedef point3 vec3;
typedef point2 vec2;

/*float dot(point3 a, point3 b) {
  return a.x*b.x + a.y*b.y + a.z*b.z;
  }
  vec3 cross(vec3 a, vec3 b) {
  vec3 c = {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.y*b.x-a.x*b.y};
  return c;
  }*/
float edge(point2 a, point2 b, point2 c) {
  return (c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x);
}

void drawObject(unsigned int object, Uint32 *pixels, float *depth, int width, int height) {
  //clear out pixels
  for(int i = 0; i < width*height; i++) {
    pixels[i] = 0xff000000;
    depth[i] = 1000.0f;
  }

  struct DrawObject *obj = objects[object];
  int cur = 0;
  while(cur < obj->size_i-2) {
    //get them triangles
    point3 p1 = {
      obj->vertices[obj->indices[cur+0]*3+0],
      obj->vertices[obj->indices[cur+0]*3+1],
      obj->vertices[obj->indices[cur+0]*3+2],
    };
    point3 p2 = {
      obj->vertices[obj->indices[cur+1]*3+0],
      obj->vertices[obj->indices[cur+1]*3+1],
      obj->vertices[obj->indices[cur+1]*3+2],
    };
    point3 p3 = {
      obj->vertices[obj->indices[cur+2]*3+0],
      obj->vertices[obj->indices[cur+2]*3+1],
      obj->vertices[obj->indices[cur+2]*3+2],
    };
    //get them texcoords
    //point2 t1 = cube_texcoords[cube_indices[cur]];
    //point2 t2 = cube_texcoords[cube_indices[cur+1]];
    //point2 t3 = cube_texcoords[cube_indices[cur+2]];
    p1.z += 8.0f;
    p2.z += 8.0f;
    p3.z += 8.0f;

    //convert to screenspace points via projecting over square screen height and mapping to the screen resolution
    p1.x = p1.x * ((float)height/p1.z) + (float)width/2.0f;
    p1.y = p1.y * ((float)height/p1.z) + (float)height/2.0f;
    p2.x = p2.x * ((float)height/p2.z) + (float)width/2.0f;
    p2.y = p2.y * ((float)height/p2.z) + (float)height/2.0f;
    p3.x = p3.x * ((float)height/p3.z) + (float)width/2.0f;
    p3.y = p3.y * ((float)height/p3.z) + (float)height/2.0f;

    //convert to 2D and find area*2
    point2 a = {p1.x, p1.y};
    point2 b = {p2.x, p2.y};
    point2 c = {p3.x, p3.y};
    float area = edge(a, b, c);

    float minx = fmin(fmin(p1.x, p2.x), p3.x);
    float miny = fmin(fmin(p1.y, p2.y), p3.y);
    float maxx = fmax(fmax(p1.x, p2.x), p3.x);
    float maxy = fmax(fmax(p1.y, p2.y), p3.y);
    for(int x = minx; x < maxx; x++) {
      for(int y = miny; y < maxy; y++) {
        //clip to screen boundaries
        if(x < 0 || y < 0 | x > width || y > height) {
          continue;
        }
        point2 p = {(float)x + 0.5f, (float)y + 0.5f};
        float u = edge(b, c, p); //a : b c
        float v = edge(c, a, p); //b : c a
        float w = edge(a, b, p); //c : a b
        u /= area;
        v /= area;
        w /= area;

        if(u >= 0 && v >= 0 && w >= 0) {
          float z = 1.0f/((1.0f/p1.z)*u + (1.0f/p2.z)*v + (1.0f/p3.z)*w);
          int pp = y * width + x;
          if(z < depth[pp]) {
            depth[pp] = z;
#ifdef DEPTH
            pixels[pp] = 0xff<<24;
            pixels[pp] |= (Uint32)(z*(float)0xff)<<16;
            pixels[pp] |= (Uint32)(z*(float)0xff)<<8;
            pixels[pp] |= (Uint32)(z*(float)0xff);
#elif COLORS
            pixels[pp] = 0xff<<24;
            pixels[pp] |= (int)((float)0xff*u)<<16; //r
            pixels[pp] |= (int)((float)0xff*v)<<8;  //g
            pixels[pp] |= (int)((float)0xff*w);     //b
#elif POINTS
            
            pixels[(int)p1.y * width + (int)p1.x] = 0xffffffff;
            pixels[(int)p2.y * width + (int)p2.x] = 0xffffffff;
            pixels[(int)p3.y * width + (int)p3.x] = 0xffffffff;

#else
            //double s = t1[0]*u + t2[0]*v + t3[0]*w;
            //double t = t1[1]*u + t2[1]*v + t3[1]*w;
            //int offset = (int)(t*crate->h) * crate->pitch + (int)(s*crate->w) * crate->format->BytesPerPixel;
            //pixels[pp] = *(Uint32*)((Uint8*)(crate->pixels) + offset);

#endif
          }
        }
      } 
    }

    cur+=3;
  }
}
