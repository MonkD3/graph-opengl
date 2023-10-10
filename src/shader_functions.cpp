#include "headers/glad/gl.h"
#include <GLFW/glfw3.h>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

GLuint loadShaders(const char* fpath, GLuint shader_type){

    GLuint shaderId = glCreateShader(shader_type);

    std::string shaderCode;
    std::ifstream shaderCodeStream(fpath, std::ios::in);
    if (shaderCodeStream.is_open()){
        std::stringstream stream;
        stream << shaderCodeStream.rdbuf();
        shaderCode = stream.str();
        shaderCodeStream.close();
    } else {
        printf("File '%s' could not be opened\n", fpath);
        return 0;
    }

    GLint res = GL_FALSE;
    int log_len;

    // Compile shader
    char const* shaderStringPtr = shaderCode.c_str();
    glShaderSource(shaderId, 1, &shaderStringPtr, NULL);
    glCompileShader(shaderId);

    // Check Shader
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &res);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &log_len);
    if ( log_len > 0 ){
        std::vector<char> shaderErrorMsg(log_len+1);
        glGetShaderInfoLog(shaderId, log_len, NULL, &shaderErrorMsg[0]);
        printf("%s\n", &shaderErrorMsg[0]);
    }

    return shaderId;
}

GLuint loadProgram(GLuint vtxShaderId, GLuint fragShaderId){
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vtxShaderId);
    glAttachShader(programId, fragShaderId);
    glLinkProgram(programId);

    GLint res;
    int log_len;
	// Check the program
	glGetProgramiv(programId, GL_LINK_STATUS, &res);
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &log_len);
	if ( log_len > 0 ){
		std::vector<char> programErrorMsg(log_len+1);
		glGetProgramInfoLog(programId, log_len, NULL, &programErrorMsg[0]);
		printf("%s\n", &programErrorMsg[0]);
	}
	
    return programId;
}
