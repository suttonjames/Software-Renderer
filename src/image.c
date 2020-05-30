#include "image.h"

#pragma warning(disable : 4996)

Image* ReadFromTGA(const char* file_name)
{
	Image* image;
	FILE* file;
	u8 header[18];

	file = fopen(file_name, "rb");
	assert(file != NULL);
	fread((void*)header, 1, sizeof(header), file);

	s32 width = header[12] + (header[13] << 8);
	s32 height = header[14] + (header[15] << 8);
	s32 depth = header[16];
	s32 channels = depth >> 3;

	if (width <= 0 || height <= 0 || (channels != 1 && channels != 3 && channels != 4))
		return NULL;

	s32 buffer_size = width * height * channels;

	image = (Image*)malloc(sizeof(Image));
	image->width = width;
	image->height = height;
	image->channels = channels;
	image->buffer = (u8*)malloc(buffer_size);
	memset(image->buffer, 0, buffer_size);

	s32 image_type = header[2];
	if (image_type == 2 || image_type == 3) {
		fread(image->buffer, 1, sizeof(image->buffer), file);
	} else if (image_type == 10 || image_type == 11) {
		s32 buffer_count = 0;
		u8 pixel[4];
		while (buffer_count < buffer_size) {
			u8 header = (u8)fgetc(file);
			u32 is_rle_packet = header & 0x80;
			u32 pixel_count = (header & 0x7F) + 1;
			u32 expected_size = buffer_count + pixel_count * channels;
			if (is_rle_packet) { 
				for (s32 j = 0; j < channels; j++) {
					pixel[j] = (u8)fgetc(file);
				}
				for (s32 i = 0; i < pixel_count; i++) {
					for (s32 j = 0; j < channels; j++) {
						image->buffer[buffer_count++] = pixel[j];
					}
				}
			} else {
				for (s32 i = 0; i < pixel_count; i++) {
					for (s32 j = 0; j < channels; j++) {
						image->buffer[buffer_count++] = (u8)fgetc(file);
					}
				}
			}
		}
	} else {
		assert(0);
		return NULL;
	}
	fclose(file);

	return image;
}

vec3 GetColour(Image *image, int x, int y)
{
	s32 channels = image->channels;
	vec3 color;
	u8 *pixel;

	s32 index = x * image->width * image->channels + y * image->channels;
	pixel = &(image->buffer[index]);

	color.b = pixel[0];
	color.g = pixel[1];
	color.r = pixel[2];

	return color;
}