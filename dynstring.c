#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./yaml.h"

#define INIT_SIZE 20
#define RESIZE_MULTIPLE 2

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
            printf("%c", str->value[i]);
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

// void DynStringPrintInfo(DynString *str) {}

// int main(void)
// {
//     DynString *test = DynStringDefaultInit();
//     DynStringAddCharAt(test, 0, 'a');
//     DynStringAddCharAt(test, 1, 'b');
//     DynStringPrint(test);
// }
