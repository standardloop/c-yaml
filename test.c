#include <standardloop/testing.h>
#include <stdlib.h>

#include "./yaml.h"
#include <standardloop/collections.h>
#include <standardloop/logger.h>

// void josh()
// {
//     Item *foo = ItemInit(strdup("foo"), &SL_COLLECTIONS_TYPE_STRING,
//                            &ItemValueStringOperations);
//     Item *bar = ItemInit(strdup("bar"), &SL_COLLECTIONS_TYPE_STRING,
//                            &ItemValueStringOperations);
//
//     Item *bazz = ItemInit(ListInitDefault(), &SL_COLLECTIONS_TYPE_LIST,
//                             &ItemValueListOperations);
//
//     ListAddFirst(bazz->value, bar);
//
//     printf("%s\n", ITEM_TYPES_MATCH(foo, bar) == 1 ? "same" : "different");
//     printf("%s\n", ITEM_TYPES_MATCH(foo, bazz) == 1 ? "same" : "different");
//     printf("%s\n", ITEM_TYPES_MATCH(((List *)bazz->value)->items[0], bar) ==
//     1
//                        ? "same"
//                        : "different");
//     exit(0);
// }

int main(void)
{
    // josh();
    // YAMLFree(yaml);
    TestingInit();
    // TestLexer();
    // TestParser();
    TestYaml();
    TestingTearDown();

    return EXIT_SUCCESS;
}
