# qntm's cpp standard

## style

### naming things

- variable names should be as descriptive as possible. do not abbreviate, unless the abbreviation is a common acronym (http, ftp, etc..) or the abbreviation
  is unmistakable, for example, "init" instead of "initilisation" is fine.

- member variables are always prepended with "m\_".

- most names are in snake case, including local variables, function names, structs, template types and namespaces. a notable exception are classes, which are PascalCase, and member variables, which are "m_camelCase". interfaces are prepended with "I", e.g. "Iprintable".

- function names should always be verbs.

- structs/classes should generally not be verb-nouns, e.g. "thing_doer". that's a sign that this type probably doesn't need to exist to begin with, and
  you should write a function instead.

## code

- don't use exceptions. prefer returning clear, invalid data instead, for example "nullptr" for std::unique_ptr, or "std::optional" for other types.

- use const references whenever possible in function parameters.

- completely avoid manual new/delete. the only exception is passing a "new" pointer into a std::unique_ptr immediately.

- use ++i instead of i++ everywhere.
