

## Background & Motivation

For most of gaming's history, a game was a single program. Rendering, input, audio, physics, and the rules of play were written in the same systems language (most often C), compiled together, and linked into one executable that shipped on the cartridge or disk. When a title occupied only a few kilobytes or a few megabytes, this was entirely reasonable. The scope was small enough that a handful of engineers could hold the whole thing in their heads, and the cost of rebuilding the binary after a change was negligible.

That arrangement stopped scaling. As games grew — larger worlds, longer runtimes, hundreds of distinct mechanics, branching dialogue, scripted set pieces — the volume of code describing what the game does, as opposed to how it runs, exploded. Two problems followed. The first is iteration time: recompiling and relinking a multi-million-line C++ codebase to tweak a single encounter or adjust one timing value is prohibitively slow, and it drags a programmer into the loop for changes that are fundamentally design decisions. The second is authorship: the people defining behavior — designers, writers, scripters — are not necessarily the people who should be editing the engine's native source.

The response was to split the codebase into two layers with very different responsibilities. The lower layer is the native executable — in this case tlou-ii.exe — written in a compiled systems language and responsible for everything performance-critical and platform-specific: the rendering pipeline, memory management, asset streaming, the physics simulation, and the core runtime loop. This layer changes rarely and constitutes the true engine. The upper layer is game logic expressed in a dedicated scripting language, which the executable does not run directly. Instead, scripts are compiled to bytecode and executed by a virtual machine embedded in the engine, which exposes a controlled interface back into the native systems.

The scripting layer of TLOU is known as DC. It as a proprietary version of the programming language Racket. Designers at ND write the DC scripts and compile them into bytecode which ends up the in the .bin files shipped in bin.psarc. This is the target of this tool.

## Layout of the DC binary Files

All DC files follow a concrete layout.

The Last of Us Part II's fundamentally has two types of functions, `near` and `far`. (Note: these are not there official designations but they're the ones I've adopted as I think they illustrate their different usages).
