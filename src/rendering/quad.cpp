#include <rendering/quad.h>

namespace rendering {

namespace {
struct vertex {
    GLfloat pos[3];
    GLfloat uv[2];
};

static vertex quad_vertices[] = { { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } },
                                  { { 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } },
                                  { { -0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f } },
                                  { { 0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f } } };
} // namespace

quad::quad()
{
    // Vertex buffer
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Vertex attributes
    glBindVertexBuffer(0, vbo, 0, sizeof(vertex));
    // - position
    glVertexAttribBinding(0, 0);
    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexAttribArray(0);
    // - uv
    glVertexAttribBinding(1, 0);
    glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, sizeof(quad_vertices[0].pos));
    glEnableVertexAttribArray(1);
}

quad::~quad()
{
    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void quad::draw()
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void quad::draw_patch()
{
    glBindVertexArray(vao);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArrays(GL_PATCHES, 0, 4);
}

void quad::draw_instanced(size_t num_instances)
{
    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, GLsizei(num_instances));
}

void quad::draw_patch_instanced(size_t num_instances)
{
    glBindVertexArray(vao);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArraysInstanced(GL_PATCHES, 0, 4, GLsizei(num_instances));
}

} // namespace rendering
