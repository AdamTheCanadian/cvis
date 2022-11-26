#ifndef CVIS_INCLUDE_CVIS_SHADER_H_
#define CVIS_INCLUDE_CVIS_SHADER_H_

#include <stdint.h>

typedef uint32_t visShader;

visShader visShader_LoadShaderFromFiles(const char *vertexSourceFile,
                                        const char *fragmentShaderFile);



#endif