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
    FILE *file_ptr = fopen(filename, "rb");
    if (file_ptr == NULL)
    {
        return NULL;
    }

    char *raw_yaml_buffer_a = calloc(LEXER_BUFFER_SIZE + 1, sizeof(char));
    if (raw_yaml_buffer_a == NULL)
    {
        Log(ERROR, "no mem for raw_yaml_buffer_a");
        return NULL;
    }
    char *raw_yaml_buffer_b = calloc(LEXER_BUFFER_SIZE + 1, sizeof(char));
    if (raw_yaml_buffer_b == NULL)
    {
        Log(ERROR, "no mem for raw_yaml_buffer_a");
        return NULL;
    }

    char *current_buffer = raw_yaml_buffer_a;
    char *next_buffer = raw_yaml_buffer_b;

    size_t current_bytes = fread(current_buffer, sizeof(char), LEXER_BUFFER_SIZE, file_ptr);

    if (current_bytes == 0)
    {
        Log(ERROR, "the file is empty");
        fclose(file_ptr);
        return NULL;
    }

    YAMLLexer *lexer = YAMLLexerInit();

    if (lexer == NULL)
    {
        return NULL;
    }

    while (ALWAYS)
    {
        size_t next_bytes = fread(next_buffer, sizeof(char), LEXER_BUFFER_SIZE, file_ptr);

        if (next_bytes == 0)
        {
            LexerReload(lexer, current_buffer, current_bytes, true);
            YAMLToken *token = YAMLLex(lexer);
            if (token != NULL)
            {
                YAMLTokenPrint(token);
            }
            break;
        }
        // Log(DEBUG, "%d", (int)bytes_read);
        LexerReload(lexer, current_buffer, current_bytes, false);
        while (!IsLexerHungry(lexer))
        {
            YAMLToken *token = YAMLLex(lexer);
            if (token != NULL)
            {
                YAMLTokenPrint(token);
            }
        }

        // loop
        char *temp = current_buffer;
        current_buffer = next_buffer;
        next_buffer = temp;

        current_bytes = next_bytes;
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
