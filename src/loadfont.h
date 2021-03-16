#ifndef LOAD_FONT_H
#define LOAD_FONT_H

#define STELLQ_WINDOW_WIDTH 1500
#define STELLQ_WINDOW_HEIGHT 500
#define STELLQ_WINDOW_ASPECT ((float)STELLQ_WINDOW_HEIGHT/STELLQ_WINDOW_WIDTH)
#define STELLQ_CHAR_HEIGHT 128
#define STELLQ_CHAR_WIDTH (STELLQ_CHAR_HEIGHT/2)

typedef struct text_render_context TextRenderContext;

// returns null if any error loading shaders & texture
TextRenderContext* load_text_render_context(char *font_path);
void load_text_to_context(TextRenderContext *context, char *text);
void render_text_from_context(TextRenderContext *context);
void free_render_context(TextRenderContext *context);

#endif