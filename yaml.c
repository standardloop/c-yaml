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
    YAML *yaml = ListInit(1, 2);
    if (yaml == NULL)
    {
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

extern YAML *YAMLFromFile(char *file_name)
{
    FILE *file_ptr = fopen(file_name, "rb");
    if (file_ptr == NULL)
    {
        Log(FATAL, "file_ptr is NULL in %s", __FUNCTION__);
    }
    // Log(TRACE, "%s", __FUNCTION__);
    if (file_ptr == NULL)
    {
        Log(ERROR, "file is NULL");
        return NULL;
    }
    Log(TRACE, "file_ptr is not NULL");

    YAMLParser *parser = YAMLParserInit(file_ptr);
    if (parser == NULL)
    {
        return NULL;
    }

    YAML *yaml = YAMLParse(parser);
    fclose(file_ptr);

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
        ListFree(yaml);
    }
}

extern void YAMLPrint(YAML *yaml)
{
    if (yaml != NULL)
    {
        // FIXME, custom print needed here.
        ListPrint(yaml);
        return;
    }
}
