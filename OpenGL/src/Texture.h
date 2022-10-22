#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <GL/glew.h>

typedef struct _Texture
{
	unsigned int renderer_id;
	const char* file_path;
	unsigned char* local_buffer;
	int width, height, BPP; // Bytes per pixel
} Texture;


void init_texture(Texture* tex, const char* path)
{
	tex->renderer_id = 0;
	tex->file_path = path;
	tex->local_buffer = NULL;
	tex->width = 0;
	tex->height = 0;
	tex->BPP = 0;

	stbi_set_flip_vertically_on_load(1);
	tex->local_buffer = stbi_load(path, &tex->width, &tex->height, &tex->BPP, 4);

	glGenTextures(1, &(tex->renderer_id));
	glBindTexture(GL_TEXTURE_2D, tex->renderer_id);

	// Si no se establecen estos parametros, probablemnte salga una textura negra
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->local_buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (tex->local_buffer)
		stbi_image_free(tex->local_buffer);
}

void delete_texture(Texture* tex)
{
	glDeleteTextures(1, &tex->renderer_id);
}

void bind_texture(Texture* tex, unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, tex->renderer_id);
}

void unbind_texture(Texture* tex)
{
	glBindTexture(GL_TEXTURE_2D, 0);
}