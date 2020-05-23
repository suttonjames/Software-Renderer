#include "model.h"

#pragma warning(disable : 4996)

static char* ReadLine(FILE* file)
{
    char* line = NULL;

    while (1) {
        s32 c = fgetc(file);
        if (c == EOF)
            return NULL;
        sb_push(line, (char)c);
        if (c == '\n') {
            sb_push(line, '\0');
            break;
        }
    }
    return line;
}

Model *LoadModel(const char* file_name)
{
    Model *model;
    FILE *file;

    vec3 *positions = NULL;
    vec2 *texcoords = NULL;
    vec3 *normals = NULL;
    s32 *position_index = NULL;
    s32 *texcoord_index = NULL;
    s32 *normal_index = NULL;

    file = fopen(file_name, "rb");
    assert(file != NULL);
    while (1) {
        char* line = ReadLine(file);
        if (line == NULL)
            break;

        if (strncmp(line, "v ", 2) == 0) { // positions
            vec3 position;
            s32 items = sscanf(line + 2, "%f %f %f", &position.x, &position.y, &position.z);
            assert(items == 3);
            sb_push(positions, position);
        } else if (strncmp(line, "vt ", 3) == 0) { // texcoords
            vec2 texcoord;
            s32 items = sscanf(line + 3, "%f %f", &texcoord.x, &texcoord.y);
            assert(items == 2);
            sb_push(texcoords, texcoord);
        } else if (strncmp(line, "vn ", 3) == 0) { // normals
            vec3 normal;
            s32 items = sscanf(line + 3, "%f %f %f", &normal.x, &normal.y, &normal.z);
            assert(items == 3);
            sb_push(normals, normal);
        } else if (strncmp(line, "f ", 2) == 0) { // faces
            s32 position[3], texcoord[3], normal[3];
            s32 items = sscanf(line + 2, "%d/%d/%d %d/%d/%d %d/%d/%d",
                &position[0], &texcoord[0], &normal[0],
                &position[1], &texcoord[1], &normal[1],
                &position[2], &texcoord[2], &normal[2]
            );
            assert(items == 9);
            for (s32 i = 0; i < 3; i++) {
                sb_push(position_index, position[i] - 1);
                sb_push(texcoord_index, texcoord[i] - 1);
                sb_push(normal_index, normal[i] - 1);
            }
        }
        sb_free(line);
    }
    fclose(file);

    s32 num_indices = sb_count(position_index);

    model = (Model*)malloc(sizeof(model));
    model->positions = (vec3*)malloc(sizeof(vec3) * num_indices);
    model->texcoords = (vec2*)malloc(sizeof(vec2) * num_indices);
    model->normals = (vec3*)malloc(sizeof(vec3) * num_indices);
    model->num_faces = sb_count(position_index) / 3;

    for (s32 i = 0; i < num_indices; i++) {
        model->positions[i] = positions[position_index[i]];
        model->texcoords[i] = texcoords[texcoord_index[i]];
        model->normals[i] = normals[normal_index[i]];
    }
    sb_free(positions);
    sb_free(texcoords);
    sb_free(normals);
    sb_free(position_index);
    sb_free(texcoord_index);
    sb_free(normal_index);

    return model;
}

void FreeModel(Model* model)
{
    free(model->positions);
    free(model->texcoords);
    free(model->normals);
    free(model);
}