#ifndef STAR_BG_H
#define STAR_BG_H

typedef struct star_render_context StarRenderContext;

// returns null if any error loading shaders
StarRenderContext* load_star_render_context(int n_stars);

void render_star_background(StarRenderContext *context, int program_time);
void free_star_render_context(StarRenderContext *context);

#endif