#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

in vec3[] g_color;
in float[] g_shimmer_offset;

out vec3 f_color;
out float f_shimmer_offset;

uniform float u_aspect;

void main() {
    f_color = g_color[0];
    f_shimmer_offset = g_shimmer_offset[0];

    const float INNER_STAR_SIZE = 0.002;
    const float OUTER_STAR_SIZE = 0.008;

    const float TWO_PI = 6.28318;
    int n_points = 4;

    for(int i = 0; i <= n_points; ++i) {
        float theta = TWO_PI*i/n_points;

        vec2 outer_offset = vec2(
            cos(theta)*OUTER_STAR_SIZE*u_aspect, 
            sin(theta)*OUTER_STAR_SIZE
        );

        vec2 inner_offset = vec2(
            cos(theta/2)*INNER_STAR_SIZE*u_aspect, 
            sin(theta/2)*INNER_STAR_SIZE
        );

        gl_Position = gl_in[0].gl_Position + vec4(outer_offset, 0.0, 0.0); 
        EmitVertex();

        gl_Position = gl_in[0].gl_Position + vec4(inner_offset, 0.0, 0.0); 
        EmitVertex();

        gl_Position = gl_in[0].gl_Position + vec4(-inner_offset, 0.0, 0.0); 
        EmitVertex();

        EndPrimitive();

        gl_Position = gl_in[0].gl_Position + vec4(-outer_offset, 0.0, 0.0); 
        EmitVertex();

        gl_Position = gl_in[0].gl_Position + vec4(-inner_offset, 0.0, 0.0); 
        EmitVertex();

        gl_Position = gl_in[0].gl_Position + vec4(inner_offset, 0.0, 0.0); 
        EmitVertex();

        EndPrimitive();
    }
}