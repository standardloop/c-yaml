#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <standardloop/util.h>
#include <standardloop/logger.h>

#include "./yaml.h"

extern YAML *YAMLInit()
{
    return NULL;
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
    FILE *file_ptr = fopen(filename, "r");
    if (file_ptr == NULL)
    {
        return NULL;
    }

    char *raw_yaml_buffer = calloc(LEXER_BUFFER_SIZE + 1, sizeof(char));
    if (raw_yaml_buffer == NULL)
    {
        Log(ERROR, "no mem for raw_yaml_buffer");
        return NULL;
    }
    size_t bytes_read = 0;
    YAMLLexer *lexer = YAMLLexerInit();

    if (lexer == NULL)
    {
        return NULL;
    }

    bool is_last_chunk = false;
    while ((bytes_read = fread(raw_yaml_buffer, 1, LEXER_BUFFER_SIZE, file_ptr)) > 0)
    {
        // for (size_t i = 0; i < bytes_read; i++)
        // {
        // printf("%c", raw_yaml_buffer[i]);
        // }
        is_last_chunk = bytes_read < LEXER_BUFFER_SIZE;

        Log(DEBUG, "%d", (int)bytes_read);
        LexerReload(lexer, raw_yaml_buffer, bytes_read);
        while (!IsLexerHungry(lexer))
        {
            YAMLToken *token = YAMLLex(lexer);
            if (token != NULL)
            {
                YAMLTokenPrint(token);
            }
        }
    }

    if (ferror(file_ptr))
    {
        perror("Error reading file");
    }
    else if (feof(file_ptr))
    {
        fclose(file_ptr);
    }

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

extern void YAMLFree(YAML *yaml)
{
    if (yaml == NULL)
    {
        return;
    }
}

extern void YAMLPrint(YAML *yaml)
{
    if (yaml == NULL)
    {
        return;
    }
}
