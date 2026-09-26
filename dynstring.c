#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./yaml.h"
#include <standardloop/util.h>

#define INIT_SIZE 20
#define RESIZE_MULTIPLE 2
// #define NULL_CHAR '\0'
// #define SPACE_CHAR ' '
//
// typedef struct
// {
//     char *value;
//     size_t size;
//     // size_t chars;
//     // size_t cur_idx;
// } DynString;

extern DynString *DynStringDefaultInit()
{
    DynString *this = malloc(sizeof(DynString));

    this->value = calloc(INIT_SIZE, sizeof(char));
    this->size = INIT_SIZE;
    // this->chars = 0;
    // this->cur_idx = 0;
    return this;
}

static void dynStringResize(DynString *str, size_t resize_multiple)
{
    char *temp = realloc(str->value, str->size * resize_multiple);
    if (temp != NULL)
    {
        memset(temp + str->size, 0, str->size * resize_multiple - str->size);
        str->value = temp;
        str->size *= resize_multiple;
    }
}

extern void DynStringAddCharAt(DynString *str, size_t idx, char c)
{
    if (str == NULL || str->value == NULL)
    {
        return;
    }
    if (idx >= str->size)
    {
        size_t resize_multiple = RESIZE_MULTIPLE;
        if (idx >= str->size * resize_multiple)
        {
            resize_multiple = (idx / str->size * resize_multiple); // TODO
        }
        dynStringResize(str, resize_multiple);
    }
    str->value[idx] = c;
}

extern void DynStringPrint(DynString *str)
{
    if (str != NULL)
    {
        for (size_t i = 0; i < str->size; i++)
        {
            printf("\'%c\'", str->value[i]);
        }
    }
}

extern void DynStringFree(DynString *str)
{
    if (str != NULL)
    {
        if (str->value != NULL)
        {
            free(str->value);
        }
        free(str);
    }
}

extern char *DynStringToCString(DynString *str)
{
    return str->value;
}

extern void DynStringTrimEnd(DynString *str)
{
    if (str != NULL)
    {
        if (str->size != 0)
        {
            for (size_t i = str->size - 1; i > 0; i--)
            {
                if (str->value[i] == SPACE_CHAR)
                {
                    str->value[i] = NULL_CHAR;
                }
                else if (str->value[i] == NULL_CHAR)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
        }
    }
}

// void DynStringPrintInfo(DynString *str) {}

// int main(void)
// {
//     DynString *test = DynStringDefaultInit();
//     DynStringAddCharAt(test, 0, 'a');
//     DynStringAddCharAt(test, 1, 'b');
//     DynStringAddCharAt(test, 2, 'b');
//     DynStringAddCharAt(test, 3, 'b');
//     DynStringAddCharAt(test, 4, 'b');
//     DynStringAddCharAt(test, 5, 'b');
//     DynStringAddCharAt(test, 6, 'b');
//
//     DynStringAddCharAt(test, 7, ' ');
//     DynStringPrint(test);
//     printf("\n");
//     DynStringTrimEnd(test);
//     DynStringPrint(test);
//     printf("\n");
// }
