# API Reference

## Classes

| Name                                                | Description |
| --------------------------------------------------- | ----------- |
| [`DynString`](#dynstring)                           |             |
| [`YAMLLexer`](#yamllexer)                           |             |
| [`YAMLToken`](#yamltoken)                           |             |
| [`YAMLParser`](#yamlparser)                         |             |
| [`block_scalar_options_s`](#block_scalar_options_s) |             |

## Macros

---

### CHUNK_SIZE

```cpp
#define CHUNK_SIZE 4096
```

---

### BUFFER_SIZE

```cpp
#define BUFFER_SIZE (CHUNK_SIZE * 2)
```

## Enumerations

---

### YAMLTokenType

```cpp
enum YAMLTokenType
```

| Value                          | Description |
| ------------------------------ | ----------- |
| `YAMLTokenStartStream`         |             |
| `YAMLTokenStartOfDocument`     |             |
| `YAMLTokenEndOfDocument`       |             |
| `YAMLTokenDirective`           |             |
| `YAMLTokenIndent`              |             |
| `YAMLTokenDedent`              |             |
| `YAMLTokenNewline`             |             |
| `YAMLTokenScalar`              |             |
| `YAMLTokenValueIndicator`      |             |
| `YAMLTokenFlowMappingStart`    |             |
| `YAMLTokenFlowMappingEnd`      |             |
| `YAMLTokenFlowSequenceStart`   |             |
| `YAMLTokenFlowSequenceEnd`     |             |
| `YAMLTokenListDash`            |             |
| `YAMLTokenFlowEntry`           |             |
| `YAMLTokenAlias`               |             |
| `YAMLTokenAnchor`              |             |
| `YAMLTokenComplexKeyIndicator` |             |
| `YAMLTokenTag`                 |             |
| `YAMLTokenAT`                  |             |
| `YAMLTokenBacktick`            |             |
| `YAMLTokenEndStream`           |             |
| `YAMLTokenIllegal`             |             |

---

### YAMLLexerState

```cpp
enum YAMLLexerState
```

| Value                                            | Description |
| ------------------------------------------------ | ----------- |
| `YAMLLexerStartStream`                           |             |
| `YAMLLexerStateNormal`                           |             |
| `YAMLLexerStateJustGotNewline`                   |             |
| `YAMLLexerStatePopDedent`                        |             |
| `YAMLLexerStateFoundEOFNeedToPopRemainingDedent` |             |
| `YAMLLexerStateCurlyFlow`                        |             |
| `YAMLLexerStateSequenceFlow`                     |             |

---

### BlockScalarStyle

```cpp
enum BlockScalarStyle
```

| Value                     | Description |
| ------------------------- | ----------- |
| `BlockScalarStyleLiteral` |             |
| `BlockScalarStyleFolded`  |             |

---

### BlockScalarChomping

```cpp
enum BlockScalarChomping
```

| Value                   | Description |
| ----------------------- | ----------- |
| `BlockScalarStyleClip`  |             |
| `BlockScalarStyleStrip` |             |
| `BlockScalarStyleKeep`  |             |

---

### YAMLParserInputMode

```cpp
enum YAMLParserInputMode
```

| Value                 | Description |
| --------------------- | ----------- |
| `YAMLParserInputFile` |             |

## Typedefs

---

### YAMLValue

```cpp
using YAMLValue = Item
```

---

### YAML

```cpp
using YAML = List
```

## Functions

---

### DynStringToCString

```cpp
char * DynStringToCString(DynString * str)
```

---

### DynStringFree

```cpp
void DynStringFree(DynString * str)
```

---

### DynStringPrint

```cpp
void DynStringPrint(DynString * str)
```

---

### DynStringAddCharAt

```cpp
void DynStringAddCharAt(DynString * str, size_t idx, char c)
```

---

### DynStringTrimEnd

```cpp
void DynStringTrimEnd(DynString * str)
```

---

### DynStringShiftLeft

```cpp
void DynStringShiftLeft(DynString * str, size_t index)
```

---

### DynStringDefaultInit

```cpp
DynString * DynStringDefaultInit()
```

---

### YAMLTokenInit

```cpp
YAMLToken * YAMLTokenInit(enum YAMLTokenType, u_int32_t, u_int32_t, u_int32_t, char *)
```

---

### YAMLTokenTypeToString

```cpp
char * YAMLTokenTypeToString(enum YAMLTokenType)
```

---

### YAMLTokenPrint

```cpp
void YAMLTokenPrint(YAMLToken *)
```

---

### YAMLLex

```cpp
YAMLToken * YAMLLex(YAMLLexer * lexer)
```

---

### YAMLTokenFree

```cpp
void YAMLTokenFree(YAMLToken * token)
```

---

### YAMLLexerInit

```cpp
YAMLLexer * YAMLLexerInit(FILE * file_ptr)
```

---

### YAMLLexerFree

```cpp
void YAMLLexerFree(YAMLLexer * lexer)
```

---

### YAMLParserInit

```cpp
YAMLParser * YAMLParserInit(FILE * file_ptr)
```

---

### YAMLParserFree

```cpp
void YAMLParserFree(YAMLParser * parser)
```

---

### YAMLInit

```cpp
YAML * YAMLInit()
```

---

### StringToYAML

```cpp
YAML * StringToYAML(char *)
```

---

### YAMLFromFile

```cpp
YAML * YAMLFromFile(char * file_name)
```

---

### YAMLFromSTDIN

```cpp
YAML * YAMLFromSTDIN(size_t)
```

---

### YAMLToString

```cpp
char * YAMLToString(YAML *)
```

---

### YAMLFree

```cpp
void YAMLFree(YAML *)
```

---

### YAMLPrint

```cpp
void YAMLPrint(YAML *)
```

---

### YAMLParse

```cpp
YAML * YAMLParse(YAMLParser * parser)
```

## DynString

```cpp
struct DynString
```

### Public Attributes

| Return   | Name              | Description |
| -------- | ----------------- | ----------- |
| `char *` | [`value`](#value) |             |
| `size_t` | [`size`](#size)   |             |

---

#### value

```cpp
char * value
```

---

#### size

```cpp
size_t size
```

## YAMLLexer

```cpp
struct YAMLLexer
```

### Public Attributes

| Return                                     | Name                                                  | Description |
| ------------------------------------------ | ----------------------------------------------------- | ----------- |
| `FILE *`                                   | [`file_ptr`](#file_ptr)                               |             |
| `char`                                     | [`buffer`](#buffer)                                   |             |
| `size_t`                                   | [`cursor`](#cursor)                                   |             |
| `size_t`                                   | [`bytes_in_buffer`](#bytes_in_buffer)                 |             |
| `u_int32_t`                                | [`line`](#line)                                       |             |
| `char`                                     | [`current_char`](#current_char)                       |             |
| `bool`                                     | [`eof_reached`](#eof_reached)                         |             |
| `enum YAMLLexerState`                      | [`state`](#state)                                     |             |
| `List *`                                   | [`flow_stack`](#flow_stack)                           |             |
| `List *`                                   | [`indent_stack`](#indent_stack)                       |             |
| `u_int8_t`                                 | [`this_indent_space_count`](#this_indent_space_count) |             |
| `int`                                      | [`space_count`](#space_count)                         |             |
| `struct YAMLLexer::block_scalar_options_s` | [`block_scalar_options`](#block_scalar_options)       |             |

---

#### file_ptr

```cpp
FILE * file_ptr
```

---

#### buffer

```cpp
char buffer[BUFFER_SIZE]
```

---

#### cursor

```cpp
size_t cursor
```

---

#### bytes_in_buffer

```cpp
size_t bytes_in_buffer
```

---

#### line

```cpp
u_int32_t line
```

---

#### current_char

```cpp
char current_char
```

---

#### eof_reached

```cpp
bool eof_reached
```

---

#### state

```cpp
enum YAMLLexerState state
```

---

#### flow_stack

```cpp
List * flow_stack
```

---

#### indent_stack

```cpp
List * indent_stack
```

---

#### this_indent_space_count

```cpp
u_int8_t this_indent_space_count
```

---

#### space_count

```cpp
int space_count
```

---

#### block_scalar_options

```cpp
struct YAMLLexer::block_scalar_options_s block_scalar_options
```

## block_scalar_options_s

```cpp
struct block_scalar_options_s
```

### Public Attributes

| Return                     | Name                                  | Description |
| -------------------------- | ------------------------------------- | ----------- |
| `bool`                     | [`enabled`](#enabled)                 |             |
| `enum BlockScalarStyle`    | [`style`](#style)                     |             |
| `enum BlockScalarChomping` | [`chomping`](#chomping)               |             |
| `uint8_t`                  | [`explicit_indent`](#explicit_indent) |             |

---

#### enabled

```cpp
bool enabled
```

---

#### style

```cpp
enum BlockScalarStyle style
```

---

#### chomping

```cpp
enum BlockScalarChomping chomping
```

---

#### explicit_indent

```cpp
uint8_t explicit_indent
```

## YAMLToken

```cpp
struct YAMLToken
```

### Public Attributes

| Return               | Name                  | Description |
| -------------------- | --------------------- | ----------- |
| `enum YAMLTokenType` | [`type`](#type)       |             |
| `u_int32_t`          | [`start`](#start)     |             |
| `u_int32_t`          | [`end`](#end)         |             |
| `u_int32_t`          | [`line`](#line-1)     |             |
| `char *`             | [`literal`](#literal) |             |

---

#### type

```cpp
enum YAMLTokenType type
```

---

#### start

```cpp
u_int32_t start
```

---

#### end

```cpp
u_int32_t end
```

---

#### line

```cpp
u_int32_t line
```

---

#### literal

```cpp
char * literal
```

## YAMLParser

```cpp
struct YAMLParser
```

### Public Attributes

| Return        | Name                              | Description |
| ------------- | --------------------------------- | ----------- |
| `YAMLLexer *` | [`lexer`](#lexer)                 |             |
| `YAMLToken *` | [`current_token`](#current_token) |             |
| `YAMLToken *` | [`peek_token`](#peek_token)       |             |
| `char *`      | [`error_message`](#error_message) |             |

---

#### lexer

```cpp
YAMLLexer * lexer
```

---

#### current_token

```cpp
YAMLToken * current_token
```

---

#### peek_token

```cpp
YAMLToken * peek_token
```

---

#### error_message

```cpp
char * error_message
```

## block_scalar_options_s

```cpp
struct block_scalar_options_s
```

### Public Attributes

| Return                     | Name                                  | Description |
| -------------------------- | ------------------------------------- | ----------- |
| `bool`                     | [`enabled`](#enabled)                 |             |
| `enum BlockScalarStyle`    | [`style`](#style)                     |             |
| `enum BlockScalarChomping` | [`chomping`](#chomping)               |             |
| `uint8_t`                  | [`explicit_indent`](#explicit_indent) |             |

---

#### enabled

```cpp
bool enabled
```

---

#### style

```cpp
enum BlockScalarStyle style
```

---

#### chomping

```cpp
enum BlockScalarChomping chomping
```

---

#### explicit_indent

```cpp
uint8_t explicit_indent
```
