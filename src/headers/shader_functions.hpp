#include "glad/gl.h"
#include <GLFW/glfw3.h>

GLuint loadShaders(const char* fpath, GLuint shader_type);
GLuint loadProgram(GLuint vtxShaderId, GLuint fragShaderId);
