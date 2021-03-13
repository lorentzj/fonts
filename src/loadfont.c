#include <GL/glew.h>
#include <SDL2/SDL_image.h>

GLuint compile_shader_from_path(const char* path, GLuint shader_type);

int load_font_shader_program(GLuint *program) {
    int ok;
    char shader_error[GL_INFO_LOG_LENGTH];

    GLuint vshader = compile_shader_from_path("shader/text.vert", GL_VERTEX_SHADER);
    GLuint fshader = compile_shader_from_path("shader/text.frag", GL_FRAGMENT_SHADER);
    if(vshader == -1 || fshader == -1) return -1;

    // Link Shader Program
    *program = glCreateProgram();

    glAttachShader(*program, vshader);
    glAttachShader(*program, fshader);

    glLinkProgram(*program);

    glDeleteShader(vshader);
    glDeleteShader(fshader);

    glGetProgramiv(*program, GL_LINK_STATUS, &ok);

    if(!ok) {
        glGetProgramInfoLog(*program, GL_INFO_LOG_LENGTH, NULL, shader_error);
        printf("Error Linking Shader Program:\n%s\n", shader_error);
    	glDeleteProgram(*program);
        return -1;
    }

    glUseProgram(*program);

    return 0;
}

int load_font_texture(const char* texture_path, GLuint *texture) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if(!IMG_Init(IMG_INIT_PNG)) {
        return -1;
    }

    SDL_Surface* image = IMG_Load(texture_path);
    if(image == NULL) {
        printf("Error loading Font Texture: \n%s\n", IMG_GetError());        
        return -1;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

    SDL_FreeSurface(image);

    return 0;
}

GLuint compile_shader_from_path(const char* path, GLuint shader_type) {
    int ok;
    int file_length;
    char shader_error[GL_INFO_LOG_LENGTH];

    // Load Shader Source
    FILE *shader_handle = fopen(path, "r");
    if(shader_handle == NULL) {
        return -1;
    }

    fseek(shader_handle, 0, SEEK_END);
    file_length = ftell(shader_handle);
    rewind(shader_handle);
    char *shader_source = malloc(file_length + 1);
    if(shader_source == NULL) {
        fclose(shader_handle);
        return -1;
    }

    fread(shader_source, file_length + 1, 1, shader_handle);
    shader_source[file_length] = '\0';
    fclose(shader_handle);
    
    // Compile Shader
    GLuint shader = glCreateShader(shader_type);

    const char *shader_source_const = (const char*)shader_source;
    glShaderSource(shader, 1, &shader_source_const, NULL);
    glCompileShader(shader);
    free(shader_source);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if(!ok) {
        glGetShaderInfoLog(shader, GL_INFO_LOG_LENGTH, NULL, shader_error);
        printf("Error Compiling Shader at %s:\n%s\n", path, shader_error);
        glDeleteShader(shader);
        return -1;
    }

    return shader;
}