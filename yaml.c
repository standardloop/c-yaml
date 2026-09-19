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

    // YAMLLexer *lexer = YAMLLexerInit();

    // if (lexer == NULL)
    // {
    //     return NULL;
    // }
    // YAMLParser *parser = YAMLParserInit(lexer, YAMLParserInputFile, );
    // if (parser == NULL)
    // {
    //     YAMLLexerFree(lexer);
    //     return NULL;
    // }

    return NULL;
}

extern YAML *YAMLFromFile(FILE *file_ptr, size_t buffer_size)
{
    Log(TRACE, "entering YAMLFromFile");
    if (file_ptr == NULL)
    {
        Log(ERROR, "file is NULL");
        return NULL;
    }
    Log(TRACE, "file_ptr is not NULL");

    YAMLLexer *lexer = YAMLLexerInit();
    if (lexer == NULL)
    {
        return NULL;
    }

    YAMLParser *parser =
        YAMLParserInit(lexer, YAMLParserInputFile, file_ptr, buffer_size);

    YAML *yaml = YAMLParserParse(parser);

    // if (ferror(file_ptr))
    // {
    //     perror("Error reading file");
    // }
    // else if (feof(file_ptr))
    // {
    //     fclose(file_ptr);
    // }

    return yaml;
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
