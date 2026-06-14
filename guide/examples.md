# Creating your own script mods with the DC pseudo language - A complete guide

> ## Important: Alpha Version
>
> The dcpl-compiler (`dcc`) is still being actively developed. As this is a completely custom programming language,
> expect bugs and undefined behaviour, as well as breaking changes that might require you to update scripts as updates come in.

This is a complete tutorial on how you can create custom script mods, involving both modifying existing scripts and creating completely new ones to introduce completely new behaviour into the game. The possibilities with this type of modding are endless, and I hope that this guide will serve as a good starter.

In order to make script mods, you will (currently) need to use the DCPL programming language. If you are completely unfamiliar with programming, this guide will have some basic concepts, but it is recommended that you perhaps try to seek out a tutorial for a simple scripting language like JavaScript or Python, as many of the concepts will be applicable to this language as well.

# Making your first mod

First, you'll need the `dcc.exe` program. You can put this anywhere, but it's recommended that you put it in your Windows Path variable so you can use it from everywhere. `dcc.exe` is a command line only program with not visual interface. You can find all of the options by typing `dcc --about`.

There are two 'modes' for dcc: The `target` mode and the `standalone` mode. We will begin by describing the `target` mode.

## Setup: Precompiler Directives

### The `target` mode

The goal of `target` is to _edit_ an existing script file. Two types of entries inside of a .bin file can be edited: `script-lambda` (functions) and `state-script`-functions. If you instead want to edit static data like integers inside structs, you should use dconstruct.

In this example mod, we are going to modify the game's scripts to skip the 'rewards box' after an encounter in No Return, and instead just giving us the rewards instantly.

To start, create a fresh `.dcpl` file and name it something meaningful.

Every `.dcpl` file begins with what's known as precompiler directives. You can think of these as "meta" instructions that describe how the file will be compiled using what settings.

The first precompiler directive in our example will be the `@game` directive: It sets the source path to your game. The program will use this path to put your mod in the correct location as well as setting up required files. This might look like:

```py
@game "C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II"
```

The next line in our program describes the script we are targeting. This is always done relative to the games `dc1` directory. If you don't have this, make sure to use ndarc to unpack the games bin.psarc folder first. This should create a `bin_unpacked` directory with a `dc1` directory inside of it. From there, find the path to your script. In our case we are editing `ss-rogue/ss-hub-lockbox.bin`, so this is what we are targeting:

```py
@target "ss-rogue/ss-hub-lockbox.bin"
```

Finally, you set your mod name: This will automatically create its directory in your mods folder, setup a fresh modules.bin and also pack it when we run the program using ndarc, which needs to be on your path. You can pick any name here:

```py
@mod "hub_box_skip"
```

## Main program


### The using directive

For reasons explained in the `background.md` file, `dcpl` requires you to tell the compiler what the function you'll be using look like. This is known as forward declaration. 

In order to do this, dcpl has the `using` statement. Fundamentally, it looks like this: 

```c
using #<function_name> as <near|far> <function_type>;
```

An example:

```
using #wait-for as far (symbol) -> u0;
```

I'll explain each part:

#### Function name

This is the name of the function prepended with a `#`. `dcpl` requires all SIDs (so stuff like global symbols) to be prepended with a `#`. This tells the compiler to not treat this name as a local variable, but a global game symbol.

#### `near`/`far`

This is the first hard part, and you'll have to 
