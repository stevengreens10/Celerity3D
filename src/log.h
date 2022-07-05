#ifndef GUI_LOG_H
#define GUI_LOG_H

#include "GL/glew.h"
#include <windows.h>

void APIENTRY GLDebugMessageCallback(GLenum
                                     source,
                                     GLenum type, GLuint
                                     id,
                                     GLenum severity, GLsizei
                                     length,
                                     const GLchar *msg,
                                     const void *data
);

#endif //GUI_LOG_H
