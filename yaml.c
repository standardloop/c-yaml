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

extern YAML *StringToYAML(char *input_str, size_t input_str_len)
{
    FILE *file_ptr = fmemopen((void *)input_str,
                              input_str_len == 0 ? 1 : input_str_len, "rb");
    YAMLParser *parser = YAMLParserInit(file_ptr);
    if (parser == NULL)
    {
        return NULL;
    }
    YAML *yaml = YAMLParse(parser);
    fclose(file_ptr);
    return yaml;
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
    // Log(TRACE, "file_ptr is not NULL");

    YAMLParser *parser = YAMLParserInit(file_ptr);
    if (parser == NULL)
    {
        return NULL;
    }

    YAML *yaml = YAMLParse(parser);
    fclose(file_ptr);

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
        for (u_int32_t i = 0; i < yaml->size; i++)
        {
            printf("---\n");
            ItemPrint(yaml->items[i]);
            printf("\n...\n");
        }
        return;
    }
}
