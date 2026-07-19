#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <standardloop/util.h>

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

    char raw_yaml_buffer[LEXER_BUFFER_SIZE] = {NULL_CHAR};
    size_t bytes_read;

    YAMLLexer *lexer = YAMLLexerInit();
    if (lexer == NULL)
    {
        return NULL;
    }
    // Loop until the end of the file is reached
    while ((bytes_read = fread(raw_yaml_buffer, 1, sizeof(raw_yaml_buffer), file_ptr)) > 0)
    {

        lexer->input = raw_yaml_buffer;
        lexer->input_len = bytes_read;
        YAMLLex(lexer);
        // printf("Successfully read a chunk of %zu bytes.\n", bytes_read);
    }

    // Check if the loop terminated due to an error or EOF
    if (ferror(file_ptr))
    {
        perror("Error reading file");
    }
    else if (feof(file_ptr))
    {
        fclose(file_ptr);
    }
    exit(1);
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
