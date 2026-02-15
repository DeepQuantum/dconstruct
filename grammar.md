# DConstruct Language Grammar

## 1) High-Level Structure

```ebnf
program             ::= global_declaration* EOF

global_declaration  ::= struct_definition
                      | enum_definition
                      | using_declaration
                      | function_definition
                      | state script definition
```

---

## 2) Declarations

### 2.1 Structs and Enums

```ebnf
struct_definition   ::= "struct" IDENTIFIER "{" variable_declaration* "}"

enum_definition     ::= "enum" IDENTIFIER "{" IDENTIFIER ("," IDENTIFIER)* "}"
```

Structs work like you'd expect in most languages. Enums are roughly equivalent to C++'s `enum class`.

### 2.2 Using Declarations

```ebnf
using_declaration   ::= "using" SID "as" far_near_spec? type alias_name? ";"

far_near_spec       ::= "far" | "near"
alias_name          ::= IDENTIFIER | SID
```

The `using` declaration in the primary way that you include functions and symbols from the game in your program. The SID is either a name (e.g. "#alloc-array") or an 8-Byte hash. The smallest using declarations just define a type for the SID, with no alias. Aliases follow the same rules as normal identfiers. If no alias is specified, the given SID is used as the identifier.

The `far/near` specification is required for function types. This is needed because functions can either be script functions or binary functions, which require different instructions in the backend.

### 2.3 Function Definitions

```ebnf
function_definition ::= type IDENTIFIER "(" parameter_list? ")" block

parameter_list      ::= parameter ("," parameter)*
parameter           ::= type IDENTIFIER
```

### 2.4 Variable Declarations

```ebnf
variable_declaration ::= type IDENTIFIER ("=" expression)? ";"
```

---

## 3) Types

```ebnf
type                ::= named_type pointer_suffix*
                      | function_type

named_type          ::= IDENTIFIER

pointer_suffix      ::= "*"

function_type       ::= "(" function_type_params? ")" "->" type
function_type_params::= type ("," type)*
```

Built-in known type names include:

- `u8`, `u16`, `u32`, `u64`
- `i8`, `i16`, `i32`, `i64`
- `f32`, `f64`
- `char`, `bool`, `string`, `sid`, `u0`

---

## 4) Statements

```ebnf
statement           ::= if_statement
                      | while_statement
                      | for_statement
                      | foreach_statement
                      | return_statement
                      | breakpoint_statement
                      | block
                      | expression_statement

block               ::= "{" declaration_or_statement* "}"

declaration_or_statement
                    ::= variable_declaration
                      | statement

if_statement        ::= "if" "(" expression ")" statement ("else" statement)?

while_statement     ::= "while" "(" expression ")" statement

for_statement       ::= "for" "(" for_initializer for_condition ";" for_increment ")" statement

for_initializer     ::= ";"
                      | variable_declaration
                      | expression_statement

for_condition       ::= expression
for_increment       ::= expression

foreach_statement   ::= "foreach" "(" type IDENTIFIER ":" expression ")" statement

return_statement    ::= "return" (expression)? ";"

breakpoint_statement::= "breakpoint" ";"

expression_statement::= expression ";"
```

> [!IMPORTANT]
> Current parser behavior requires both `for_condition` and `for_increment` to be present (not optional).

> [!NOTE]
> Placeholder: Explain intended use of `breakpoint;` in dev/runtime tooling.

---

## 5) Expressions

## 5.1 Precedence and Associativity (lowest -> highest)

1. Assignment (`=`) — right-associative
2. Logical OR (`or`, `||`) — left-associative
3. Logical AND (`and`, `&&`) — left-associative
4. Equality (`==`, `!=`) — left-associative
5. Comparison (`<`, `<=`, `>`, `>=`) — left-associative
6. Additive (`+`, `-`) — left-associative
7. Multiplicative (`*`, `/`) — left-associative
8. Unary (`!`, unary `+`, unary `-`, `~`, `++`, `--`, `*`, `&`) — right-associative parse
9. Postfix / call chain (`()`, `[]`, postfix `++`, postfix `--`)
10. Primary literals/identifiers/grouping/cast/match/sizeof

### 5.2 Expression Grammar

```ebnf
expression          ::= assignment

assignment          ::= or_expr ("=" assignment)?

or_expr             ::= and_expr (("or" | "||") and_expr)*

and_expr            ::= equality (("and" | "&&") equality)*

equality            ::= comparison (("==" | "!=") comparison)*

comparison          ::= term (("<" | "<=" | ">" | ">=") term)*

term                ::= factor (("+" | "-") factor)*

factor              ::= unary (("*" | "/") unary)*

unary               ::= ("!" | "+" | "-" | "~" | "++" | "--" | "*" | "&") unary
                      | call

call                ::= primary (call_suffix)*

call_suffix         ::= "(" argument_list? ")"
                      | "[" expression "]"
                      | "++"
                      | "--"

argument_list       ::= expression ("," expression)*
```

---

## 6) Primary Forms

```ebnf
primary             ::= literal
                      | IDENTIFIER
                      | match_expression
                      | sizeof_expression
                      | cast_or_grouping

cast_or_grouping    ::= "(" type ")" expression
                      | "(" expression ")"

sizeof_expression   ::= "sizeof" "(" (type | expression) ")"
```

### 6.1 Match Expression

```ebnf
match_expression    ::= "match" "(" match_conditions ")" "{" match_arms "}"

match_conditions    ::= expression (";" expression)*

match_arms          ::= non_default_arm+ default_arm

non_default_arm     ::= pattern_list "->" expression ","

default_arm         ::= "else" "->" expression

pattern_list        ::= literal ("," literal)*
```

> [!NOTE]
> Placeholder: Explain match semantics (multi-condition matching, pattern arity expectations, type compatibility, fallthrough/ordering behavior).

### 6.2 Literals

```ebnf
literal             ::= "true"
                      | "false"
                      | "null"
                      | INT
                      | DOUBLE
                      | STRING
                      | SID
```

Additional lexical notes:

- `SID` tokens are written with `#...` (e.g. symbolic or hex-like SID forms).
- Hex numeric input (`0x...`) is lexed as numeric `INT`.

> [!NOTE]
> Placeholder: Document full SID literal format and recommended style conventions.

---

## 7) Lexical Notes (Simplified)

```ebnf
IDENTIFIER          ::= [A-Za-z_][A-Za-z0-9_]*
STRING              ::= '"' ... '"'
INT                 ::= decimal_integer | hex_integer
DOUBLE              ::= decimal_with_fraction
SID                 ::= '#' sid_chars
```

Comments and whitespace:

- Line comments: `// ...`
- Whitespace/newlines are ignored except as token separators.

---

## 8) AST Surface Beyond Main Parser (Placeholder)

The AST also includes a **state script** domain (`state_script`, `state`, `block`, `track`, `lambda`) that appears to represent a separate syntax layer/workflow.

> [!NOTE]
> Placeholder: Add dedicated state-script grammar once its parser/frontend entry points are finalized.

Suggested section outline for later:

1. State script file structure
2. Options block grammar
3. Declarations block grammar
4. State/block/track/lambda nesting rules
5. Runtime semantics and execution model

---

## 9) Future/Reserved Areas (Placeholder)

Several AST/token elements exist that are not fully wired in the primary parser grammar yet.

> [!NOTE]
> Placeholder: Track and document rollout status for constructs like `break`, `continue`, ternary forms, modulo/shift operators, and any additional expression forms.

---

## 10) Example Snippets

### 10.1 Using alias

```dcpl
using #my_original_sid as far (u64, string) -> bool MyAlias;
```

### 10.2 Match expression

```dcpl
u64 v = match (x; y) {
    1, 2 -> 10,
    3, 4 -> 20,
    else -> 0
};
```

### 10.3 Foreach

```dcpl
foreach (u64 item : items) {
    breakpoint;
}
```
