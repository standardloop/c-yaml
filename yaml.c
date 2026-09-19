#include <errno.h>
#include <standardloop/collections.h>
#include <standardloop/logger.h>
#include <standardloop/util.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "./yaml.h"

extern YAML *YAMLInit()
{
    YAML *yaml = malloc(sizeof(YAML));
    if (yaml == NULL)
    {
        return NULL;
    }
    yaml->documents = ListInit(1, 2);
    if (yaml->documents == NULL)
    {
        YAMLFree(yaml);
        return NULL;
    }
    return yaml;
}

extern YAML *StringToYAML(char *yaml_string)
{
    if (yaml_string == NULL)
    {
        return NULL;
    }

    YAMLLexer *lexer = YAMLLexerInit();

    if (lexer == NULL)
    {
        return NULL;
    }
    YAMLParser *parser = YAMLParserInit(lexer);
    if (parser == NULL)
    {
        YAMLLexerFree(lexer);
        return NULL;
    }

    return NULL;
}

extern YAML *YAMLFromFile(FILE *file_ptr, size_t buffer_size)
{
    if (file_ptr == NULL)
    {
        Log(ERROR, "file is NULL");
        return NULL;
    }

    char *raw_yaml_buffer_a = calloc(buffer_size + 1, sizeof(char));
    if (raw_yaml_buffer_a == NULL)
    {
        // fclose(file_ptr);
        Log(ERROR, "no mem for raw_yaml_buffer_a");
        return NULL;
    }
    char *raw_yaml_buffer_b = calloc(buffer_size + 1, sizeof(char));
    if (raw_yaml_buffer_b == NULL)
    {
        // fclose(file_ptr);
        Log(ERROR, "no mem for raw_yaml_buffer_b");
        return NULL;
    }

    char *current_buffer = raw_yaml_buffer_a;
    char *next_buffer = raw_yaml_buffer_b;

    size_t current_bytes =
        fread(current_buffer, sizeof(char), buffer_size, file_ptr);
    current_buffer[buffer_size] = NULL_CHAR;

    if (current_bytes == 0)
    {

        Log(ERROR, "the file is empty");
        // fclose(file_ptr);
        return NULL; // fixme, maybe we can still return an initialized yaml
                     // struct
    }

    YAMLLexer *lexer = YAMLLexerInit();

    if (lexer == NULL)
    {
        return NULL;
    }

    bool done = false;
    while (ALWAYS)
    {
        size_t next_bytes =
            fread(next_buffer, sizeof(char), buffer_size, file_ptr);
        current_buffer[buffer_size] = NULL_CHAR;
        next_buffer[buffer_size] = NULL_CHAR;

        // Log(ERROR, "%s", current_buffer);
        // Log(ERROR, "%d", (int)next_bytes);
        YAMLLexerReload(lexer, current_buffer, current_bytes, next_bytes == 0);
        while (!IsLexerHungry(lexer))
        {
            YAMLToken *token = YAMLLex(lexer);
            if (token != NULL)
            {
                YAMLTokenPrint(token);
                if (token->type == YAMLTokenEOF)
                {
                    done = true;
                    break;
                }
            }
            // sleep(1);
        }
        // if (next_bytes == 0)
        // {
        //     break;
        // }

        if (done)
        {
            break;
        }

        // loop
        char *temp = current_buffer;
        current_buffer = next_buffer;
        next_buffer = temp;

        current_bytes = next_bytes;
    }

    // if (ferror(file_ptr))
    // {
    //     perror("Error reading file");
    // }
    // else if (feof(file_ptr))
    // {
    //     fclose(file_ptr);
    // }

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
    if (yaml != NULL)
    {
        if (yaml->documents != NULL)
        {
            ListFree(yaml->documents);
        }
        free(yaml);
    }
}

extern void YAMLPrint(YAML *yaml)
{
    if (yaml != NULL)
    {
        ListPrint(yaml->documents);
    }
}

extern void TestYaml(void) {}
