# Creating your own script mods with the DC pseudo language - A complete guide

> ## Important: Alpha Version
>
> The dcpl-compiler (`dcc`) is still being actively developed. As this is a completely custom programming language,
> expect bugs and undefined behaviour, as well as breaking changes that might require you to update scripts as updates come in.

This is a complete tutorial on how you can create custom script mods, involving both modifying existing scripts and creating completely new ones to introduce completely new behaviour into the game. The possibilities with this type of modding are endless, and I hope that this guide will serve as a good starter.

The main part of this guide will be usage-based, but there are also technical explanations of how the games script files work. if this doesn't interest you feel free to skip these sections.

In order to make script mods, you will (currently) need to use the DCPL programming language. If you are completely unfamiliar with programming, this guide will have some basic concepts, but it is recommended that you perhaps try to seek out a tutorial for a simple scripting language like JavaScript or Python, as many of the concepts will be applicable to this language as well.

## Background & Motivation

For most of gaming's history, a game was a single program. Rendering, input, audio, physics, and the rules of play were written in the same systems language (most often C), compiled together, and linked into one executable that shipped on the cartridge or disk. When a title occupied only a few kilobytes or a few megabytes, this was entirely reasonable. The scope was small enough that a handful of engineers could hold the whole thing in their heads, and the cost of rebuilding the binary after a change was negligible.

That arrangement stopped scaling. As games grew — larger worlds, longer runtimes, hundreds of distinct mechanics, branching dialogue, scripted set pieces — the volume of code describing what the game does, as opposed to how it runs, exploded. Two problems followed. The first is iteration time: recompiling and relinking a multi-million-line C++ codebase to tweak a single encounter or adjust one timing value is prohibitively slow, and it drags a programmer into the loop for changes that are fundamentally design decisions. The second is authorship: the people defining behavior — designers, writers, scripters — are not necessarily the people who should be editing the engine's native source.

The response was to split the codebase into two layers with very different responsibilities. The lower layer is the native executable — in this case tlou-ii.exe — written in a compiled systems language and responsible for everything performance-critical and platform-specific: the rendering pipeline, memory management, asset streaming, the physics simulation, and the core runtime loop. This layer changes rarely and constitutes the true engine. The upper layer is game logic expressed in a dedicated scripting language, which the executable does not run directly. Instead, scripts are compiled to bytecode and executed by a virtual machine embedded in the engine, which exposes a controlled interface back into the native systems.

The scripting layer of TLOU is known as DC. It as a proprietary version of the programming language Racket. Designers at ND write the DC scripts and compile them into bytecode which ends up the in the .bin files shipped in bin.psarc. This is the target of this tool.

## Layout of the DC binary Files

All DC files follow a concrete layout.

# Making your first mod

First, you'll need the `dcc.exe` program. You can put this anywhere, but it's recommended that you put it in your Windows Path variable so you can use it from everywhere. `dcc.exe` is a command line only program with not visual interface. You can find all of the options by typing `dcc --about`.

There are two 'modes' for dcc: The `target` mode and the `standalone` mode. We will begin by describing the `target` mode.

## The `target` mode

The goal of `target` is to _edit_ an existing script file. Two types of entries inside of a .bin file can be edited: `script-lambda` (functions) and `state-script`-functions. If you instead want to edit static data like integers inside structs, you should use dconstruct.

In this example, we are going to modify the game's scripts to skip the 'rewards box' after an encounter in No Return, and instead just giving us the rewards instantly.

To start, create a fresh `.dcpl` file and name it something meaningful.

Every `.dcpl` file begins with what's known as precompiler directivs. You can think of these as "meta" instructions that describe how the file will be compiled using what settings.

The first precompiler directive in our example will be the `@target` directive: It tells the compiler what original .bin file we are going to edit. The `@target` directive is followed by a string path to your .bin file. In our example, we are going to edit the `ss-rogue/ss-hub-lockbox.bin` file. Determining what file you need to edit needs to be done by you before you can start editing.

So our first line is:

```py
@target "path/to/game/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/ss-rogue/ss-hub-lockbox.bin"
```

##
