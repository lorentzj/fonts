#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>

#include "compile_shader_from_path.h"

GLuint compile_shader_from_path(const char* path, GLuint shader_type) {
    int ok;
    int file_length;
    char shader_error[GL_INFO_LOG_LENGTH];

    // Load Shader Source
    FILE* shader_handle = fopen(path, "r");
    if(shader_handle == NULL) {
        printf("Failed to open %s\n", path);
        return -1;
    }

    fseek(shader_handle, 0, SEEK_END);
    file_length = ftell(shader_handle);
    rewind(shader_handle);
    char* shader_source = malloc(file_length + 1);
    if(shader_source == NULL) {
        fclose(shader_handle);
        printf("Failed to allocate space for contents of %s\n", path);
        return -1;
    }

    fread(shader_source, file_length + 1, 1, shader_handle);
    shader_source[file_length] = '\0';
    fclose(shader_handle);
    
    // Compile Shader
    GLuint shader = glCreateShader(shader_type);

    const char* shader_source_const = (const char*)shader_source;
    glShaderSource(shader, 1, &shader_source_const, NULL);
    glCompileShader(shader);
    free(shader_source);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if(!ok) {
        glGetShaderInfoLog(shader, GL_INFO_LOG_LENGTH, NULL, shader_error);
        glDeleteShader(shader);
        printf("Error Compiling Shader at %s:\n%s\n", path, shader_error);
        return -1;
    }

    return shader;
}