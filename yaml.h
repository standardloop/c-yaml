#ifndef STANDARDLOOP_YAML_H
#define STANDARDLOOP_YAML_H

#define STANDARDLOOP_YAML_H_MAJOR_VERSION 0
#define STANDARDLOOP_YAML_H_MINOR_VERSION 0
#define STANDARDLOOP_YAML_H_PATCH_VERSION 0
#define STANDARDLOOP_YAML_H_VERSION "0.0.0"

#include <standardloop/collections.h>

#define LEXER_BUFFER_SIZE 4

// ————————— LEXER START —————————
enum YAMLTokenType
{
    YAMLTokenStartOfDocument, // ---
    YAMLTokenEndOfDocument,   // ...

    YAMLTokenDirective, // %
    // %YAML
    // %TAG

    // spaces
    YAMLTokenIndent,
    YAMLTokenDedent,
    YAMLTokenSpace, //

    // general
    YAMLTokenKey,               // part before the colon
    YAMLTokenValueIndicator,    // :
    YAMLTokenFlowMappingStart,  // {
    YAMLTokenFlowMappingEnd,    // }
    YAMLTokenFlowSequenceStart, // [
    YAMLTokenFlowSequenceEnd,   // ]
    YAMLTokenListDash,          // -
    YAMLTokenComma,             // ,

    YAMLTokenValue,  // do we want general value? or the below
    YAMLTokenBool,   // TRUE, true, FALSE, false
    YAMLTokenNumber, // 1, 3.14, -10
    YAMLTokenString, // hello
    YAMLTokenNULL,   // null

    YAMLTokenSingleQuotes, // "
    YAMLTokenDoubleQuotes, // "

    YAMLTokenLiteralBlockStart, // |
    YAMLTokenFoldedBlockStart,  // >
    YAMLTokenListChompingDash,  // -
    YAMLTokenListKeepChomping,  // +
    YAMLTokenChompingNumber,    // 1-9

    YAMLTokenAlias,        // *
    YAMLTokenAnchor,       // &
    YAMLTokenKeyIndicator, // ? // TODO: understand this one more

    YAMLTokenTag, // !

    YAMLTokenComment, // #

    // reserved
    YAMLTokenAT,       // @
    YAMLTokenBacktick, // `
    // yaml 1.1
    YAMLTokenMerge, // <<

    YAMLTokenHungry, // we need more tokens to know.
    YAMLTokenEOF,
    YAMLTokenIllegal
};

typedef struct
{
    enum YAMLTokenType type;
    u_int32_t start;
    u_int32_t end;
    u_int32_t line;
    char *literal;
} YAMLToken;

typedef struct
{
    char *input;
    u_int32_t input_len;
    char current_char;
    u_int32_t position;
    u_int32_t read_position;
    u_int32_t line;
    char *error;
} YAMLLexer;

extern YAMLToken *YAMLLex(YAMLLexer *);
extern YAMLLexer *YAMLLexerInit();
extern void YAMLLexerFree(YAMLLexer *);

// ————————— LEXER END —————————

typedef struct
{
    float version;
    HashMap *values;
} YAMLDocument;

typedef struct
{
    YAMLDocument **documents;
} YAML;

extern YAML *YAMLInit();
extern YAML *StringToYAML(char *);
extern YAML *YAMLFromFile(char *);
extern char *YAMLToString(YAML *);

extern void YAMLFree(YAML *);
extern void YAMLPrint(YAML *);

#endif
