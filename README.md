[![Version](https://img.shields.io/badge/version-%s-blue.svg)](https://github.com/yourusername/dconstruct/releases)
[![License: CC BY-NC-ND 4.0](https://img.shields.io/badge/License-CC--BY--NC--ND%204.0-lightgrey.svg)](https://creativecommons.org/licenses/by-nc-nd/4.0/)

# What is this?

dconstruct disassembles (interprets the binary data) of the DC-Script files used in The Last of Us Part II.

It outputs large text files containing structured data from the original DC-file. It’s optimized for data accuracy and speed.

You can also make edits to files via the command line, including replacing entire structures with little effort.

# How to use

Use --help to see all available options.

    Note:
    This is a beta version, so there might be bugs, incorrect data, or unexpected crashes.
    If you encounter any of these, please let me know and include the command you were running.
    Many more features are planned for future updates!

# Editing

Editing DC Files Using the -e Flag

You can use the -e flag to apply edits to DC files. These edits are saved into a new copy of the original file, leaving the original untouched. Multiple -e flags can be specified in one command to make several edits at once.
Syntax for Edits

Each edit follows this syntax:

<address>[<member_offset>]=<value>

    <address>: The memory address of the structure you want to edit (in hexadecimal).

    <member_offset>: The index of the member variable inside the structure.

    <value>: The new value to assign to that member.

Example

Suppose you have a structure like this:

[4] firearm-gameplay-def [0x11C28] {
[0] float 1.0
...
}

To replace the first member variable (index 0) with the float value 0.5, the edit command would be:

0x11C28[0]=0.5

Types of Member Variables

Structures can have different types of member variables:

    float — Specify decimal values with a period (e.g., 0.5).

    int — Specify integer values without a period (e.g., 42).

    sid (string identifier) — Can be replaced in two ways:

        By name lookup:

0xABC[5]=ellie

This looks for the value "ellie" in the current sidbase. If it doesn't exist, a warning is issued and no edit is applied.

By direct hash override:

        0xABC[5]=#XXXXXXXXXXXXXXXX

        The # indicates a raw hash value, which will be applied directly without a lookup.

    string — Replace with the desired string value (not covered in detail here).

    structure — Entire member structures can be replaced by referencing another structure’s address.

Replacing Entire Member Structures

If a structure contains another structure as a member, you can replace the entire member structure by assigning it the address of another structure.

For example, suppose you have the following:

```
    [4] weapon-gameplay-def [0x0C523] {
    ...
    [7] firearm-gameplay-def [0x11C28] {
    ...
    }
```

To replace the firearm-gameplay-def inside the weapon-gameplay-def with a different firearm-gameplay-def located at address 0x0ABC, the edit would be:

0x11C28=0x0ABC

# Special Thanks

icemesh – for providing the underlying structures for the DC-files (GitHub repo) and his disassembler, which largely served as an inspiration.

Specilizer – for his DC-Tool, also an inspiration for this program.

uxh – for scripting knowledge.

The entire modding Discord community – for being friendly & helpful.

## Support

All my tools and mods will always be 100% free, but programs like this take a lot of work.

If you'd like to support me, you can visit my Ko-fi:

[![Ko-fi](https://img.shields.io/badge/Donate-Ko--fi-ff5f5f.svg)](https://ko-fi.com/deepquantum)

## License

This software is licensed under the
[Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License](https://creativecommons.org/licenses/by-nc-nd/4.0/).

You may redistribute it, but you may not use it commercially or modify it.
