#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <standardloop/util.h>

#include "./yaml.h"

extern YAML *YAMLInit()
{
}

extern YAML *StringToYAML(char *yaml_string)
{
    if (yaml_string == NULL)
    {
        return NULL;
    }
    return NULL;
}

extern YAML *YAMLFromFile(char *filename)
{
    FILE *file_ptr = fopen(filename, "rb");
    if (file_ptr == NULL)
    {
        return NULL;
    }

    fseek(file_ptr, 0, SEEK_END);
    u_int64_t length = ftell(file_ptr);
    fseek(file_ptr, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    if (buffer == NULL)
    {
        fclose(file_ptr);
        errno = ENOMEM;
        return NULL;
    }

    fread(buffer, 1, length, file_ptr);
    fclose(file_ptr);
    buffer[length] = NULL_CHAR;

    return NULL;
}

extern char *YAMLToString(YAML *yaml)
{
    if (yaml == NULL)
    {
        return NULL;
    }
    return NULL;
}

extern void FreeYAML(YAML *yaml)
{
    if (yaml == NULL)
    {
        return;
    }
}

extern void PrintYAML(YAML *yaml)
{
    if (yaml == NULL)
    {
        return;
    }
}
