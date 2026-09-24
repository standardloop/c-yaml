# API Reference

## Classes

| Name                        | Description |
| --------------------------- | ----------- |
| [`YAML`](#yaml)             |             |
| [`DynString`](#dynstring)   |             |
| [`YAMLLexer`](#yamllexer)   |             |
| [`YAMLToken`](#yamltoken)   |             |
| [`YAMLValue`](#yamlvalue)   |             |
| [`YAMLParser`](#yamlparser) |             |

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

| Value                        | Description |
| ---------------------------- | ----------- |
| `YAMLTokenStartOfDocument`   |             |
| `YAMLTokenEndOfDocument`     |             |
| `YAMLTokenDirective`         |             |
| `YAMLTokenIndent`            |             |
| `YAMLTokenDedent`            |             |
| `YAMLTokenSpace`             |             |
| `YAMLTokenNewline`           |             |
| `YAMLTokenScalar`            |             |
| `YAMLTokenValueIndicator`    |             |
| `YAMLTokenFlowMappingStart`  |             |
| `YAMLTokenFlowMappingEnd`    |             |
| `YAMLTokenFlowSequenceStart` |             |
| `YAMLTokenFlowSequenceEnd`   |             |
| `YAMLTokenListDash`          |             |
| `YAMLTokenComma`             |             |
| `YAMLTokenSingleQuotes`      |             |
| `YAMLTokenDoubleQuotes`      |             |
| `YAMLTokenLiteralBlockStart` |             |
| `YAMLTokenFoldedBlockStart`  |             |
| `YAMLTokenListChompingDash`  |             |
| `YAMLTokenListKeepChomping`  |             |
| `YAMLTokenChompingNumber`    |             |
| `YAMLTokenAlias`             |             |
| `YAMLTokenAnchor`            |             |
| `YAMLTokenKeyIndicator`      |             |
| `YAMLTokenTag`               |             |
| `YAMLTokenComment`           |             |
| `YAMLTokenAT`                |             |
| `YAMLTokenBacktick`          |             |
| `YAMLTokenMerge`             |             |
| `YAMLTokenEOF`               |             |
| `YAMLTokenIllegal`           |             |

---

### YAMLLexerState

```cpp
enum YAMLLexerState
```

| Value                                            | Description |
| ------------------------------------------------ | ----------- |
| `YAMLLexerStateNormal`                           |             |
| `YAMLLexerStateIncomplete`                       |             |
| `YAMLLexerStateEatingComment`                    |             |
| `YAMLLexerStateInSingleQuotes`                   |             |
| `YAMLLexerStateInDoubleQuotes`                   |             |
| `YAMLLexerStateWaitingForChompingDashOrPlus`     |             |
| `YAMLLexerStateWaitSpaceAfterDash`               |             |
| `YAMLLexerStateJustGotNewline`                   |             |
| `YAMLLexerStatePopDedent`                        |             |
| `YAMLLexerStateFoundEOFNeedToPopRemainingDedent` |             |
| `YAMLLexerStateWaitingForChompingNumber`         |             |
| `YAMLLexerStateCurlyFlow`                        |             |
| `YAMLLexerStateSequenceFlow`                     |             |

---

### YAMLParserInputMode

```cpp
enum YAMLParserInputMode
```

| Value                 | Description |
| --------------------- | ----------- |
| `YAMLParserInputFile` |             |

---

### YAMLValueType

```cpp
enum YAMLValueType
```

| Value                 | Description |
| --------------------- | ----------- |
| `YAMLOBJ_t`           |             |
| `YAMLNUMBER_INT_t`    |             |
| `YAMLNUMBER_DOUBLE_t` |             |
| `YAMLSTRING_t`        |             |
| `YAMLBOOL_t`          |             |
| `YAMLNULL_t`          |             |
| `YAMLLIST_t`          |             |

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

### YAMLLexerDebugTest

```cpp
void YAMLLexerDebugTest(char * file_name)
```

---

### YAMLParserInit

```cpp
YAMLParser * YAMLParserInit()
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
YAML * YAMLFromFile(FILE *, size_t)
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

### YAMLParserParse

```cpp
YAML * YAMLParserParse(YAMLParser * parser)
```

---

### YAMLParseFile

```cpp
YAML * YAMLParseFile(YAMLParser * parser, FILE * file_ptr, size_t buffer_size)
```

## YAML

```cpp
struct YAML
```

### Public Attributes

| Return        | Name            | Description |
| ------------- | --------------- | ----------- |
| `YAMLValue *` | [`root`](#root) |             |

---

#### root

```cpp
YAMLValue * root
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

| Return                | Name                                    | Description |
| --------------------- | --------------------------------------- | ----------- |
| `FILE *`              | [`file_ptr`](#file_ptr)                 |             |
| `char`                | [`buffer`](#buffer)                     |             |
| `size_t`              | [`cursor`](#cursor)                     |             |
| `size_t`              | [`bytes_in_buffer`](#bytes_in_buffer)   |             |
| `u_int32_t`           | [`line`](#line)                         |             |
| `char`                | [`current_char`](#current_char)         |             |
| `bool`                | [`eof_reached`](#eof_reached)           |             |
| `enum YAMLLexerState` | [`state`](#state)                       |             |
| `List *`              | [`indent_stack`](#indent_stack)         |             |
| `int`                 | [`space_count`](#space_count)           |             |
| `bool`                | [`just_got_newline`](#just_got_newline) |             |

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

#### indent_stack

```cpp
List * indent_stack
```

---

#### space_count

```cpp
int space_count
```

---

#### just_got_newline

```cpp
bool just_got_newline
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

## YAMLValue

```cpp
struct YAMLValue
```

### Public Attributes

| Return                                                               | Name                                                                                                     | Description |
| -------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------- | ----------- |
| `enum YAMLValueType`                                                 | [`value_type`](#value_type)                                                                              |             |
| `union YAMLValue::@027336223146000335222027022202053122010174050143` | [`@056073055070301067077232157254230012151024272147`](#056073055070301067077232157254230012151024272147) |             |

---

#### value_type

```cpp
enum YAMLValueType value_type
```

---

#### @056073055070301067077232157254230012151024272147

```cpp
union YAMLValue::@027336223146000335222027022202053122010174050143 @056073055070301067077232157254230012151024272147
```

## [union].**unnamed0**

```cpp
union [union].__unnamed0__
```

### Public Attributes

| Return             | Name                        | Description |
| ------------------ | --------------------------- | ----------- |
| `List *`           | [`list`](#list)             |             |
| `ComplexHashMap *` | [`map`](#map)               |             |
| `int64_t *`        | [`num_int`](#num_int)       |             |
| `double *`         | [`num_double`](#num_double) |             |
| `char *`           | [`str`](#str)               |             |
| `bool *`           | [`boolean`](#boolean)       |             |

---

#### list

```cpp
List * list
```

---

#### map

```cpp
ComplexHashMap * map
```

---

#### num_int

```cpp
int64_t * num_int
```

---

#### num_double

```cpp
double * num_double
```

---

#### str

```cpp
char * str
```

---

#### boolean

```cpp
bool * boolean
```

## YAMLParser

```cpp
struct YAMLParser
```

### Public Attributes

| Return                                                                | Name                                                                                                     | Description |
| --------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------- | ----------- |
| `enum YAMLParserInputMode`                                            | [`input_mode`](#input_mode)                                                                              |             |
| `union YAMLParser::@164241133151322264244351313220306251143335010005` | [`@306142324131216233076101367267331363154064025165`](#306142324131216233076101367267331363154064025165) |             |
| `size_t`                                                              | [`buffer_size`](#buffer_size-1)                                                                          |             |
| `YAMLLexer *`                                                         | [`lexer`](#lexer)                                                                                        |             |
| `YAMLToken *`                                                         | [`current_token`](#current_token)                                                                        |             |
| `YAMLToken *`                                                         | [`peek_token`](#peek_token)                                                                              |             |
| `char *`                                                              | [`error_message`](#error_message)                                                                        |             |
| `char *`                                                              | [`current_buffer`](#current_buffer)                                                                      |             |
| `char *`                                                              | [`next_buffer`](#next_buffer)                                                                            |             |
| `size_t`                                                              | [`current_bytes`](#current_bytes)                                                                        |             |

---

#### input_mode

```cpp
enum YAMLParserInputMode input_mode
```

---

#### @306142324131216233076101367267331363154064025165

```cpp
union YAMLParser::@164241133151322264244351313220306251143335010005 @306142324131216233076101367267331363154064025165
```

---

#### buffer_size

```cpp
size_t buffer_size
```

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

---

#### current_buffer

```cpp
char * current_buffer
```

---

#### next_buffer

```cpp
char * next_buffer
```

---

#### current_bytes

```cpp
size_t current_bytes
```

## [union].**unnamed0**

```cpp
union [union].__unnamed0__
```

### Public Attributes

| Return   | Name                      | Description |
| -------- | ------------------------- | ----------- |
| `FILE *` | [`file_ptr`](#file_ptr-1) |             |

---

#### file_ptr

```cpp
FILE * file_ptr
```

## [union].**unnamed0**

```cpp
union [union].__unnamed0__
```

### Public Attributes

| Return             | Name                        | Description |
| ------------------ | --------------------------- | ----------- |
| `List *`           | [`list`](#list)             |             |
| `ComplexHashMap *` | [`map`](#map)               |             |
| `int64_t *`        | [`num_int`](#num_int)       |             |
| `double *`         | [`num_double`](#num_double) |             |
| `char *`           | [`str`](#str)               |             |
| `bool *`           | [`boolean`](#boolean)       |             |

---

#### list

```cpp
List * list
```

---

#### map

```cpp
ComplexHashMap * map
```

---

#### num_int

```cpp
int64_t * num_int
```

---

#### num_double

```cpp
double * num_double
```

---

#### str

```cpp
char * str
```

---

#### boolean

```cpp
bool * boolean
```

## [union].**unnamed0**

```cpp
union [union].__unnamed0__
```

### Public Attributes

| Return   | Name                      | Description |
| -------- | ------------------------- | ----------- |
| `FILE *` | [`file_ptr`](#file_ptr-1) |             |

---

#### file_ptr

```cpp
FILE * file_ptr
```
