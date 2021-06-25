#ifndef LOAD_FONT_H
#define LOAD_FONT_H

typedef struct text_render_context TextRenderContext;

// returns null if any error loading shaders & texture
TextRenderContext* load_text_render_context(char *font_path);
// accepts text height in pixels
void load_text_to_context(TextRenderContext *context, char *text, int text_height);
void render_text_from_context(TextRenderContext *context);
void free_render_context(TextRenderContext *context);

#endif