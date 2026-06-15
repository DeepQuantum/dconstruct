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

`near` function are functions that are defined inside other scripts, as in, other bin files. You can find the definition to these functions by searching through the decompiled .dcpl files. `far` function on the other do not have a definition inside the script files and are instead functions from the games executable.
In .dcpl decompiled code, these functions have a `!` suffix after their call. Because the compiler needs to treat these functions differently internally, you need to tell it which type of function it is. Simple rule: If a function call in the decomp has a `!` after it, it's a `far` function, otherwise it's a `near` function.


### The function type

This is the most important part. It tells the compiler the type of the function, specifically the type of the arguments & the functions return type. You start with the argument types inside the parenthesis, and seperate them with a comma. If there aren't any function arguments you put empty parenthesis (`()`). This is followed by an arrow `->` and then the functions return type. Funtions that do not return a value (what you might know as `void` from other languages) are declared as returning a `u0` (0 byte value).

Going back from the example above, we now understand: 

```
using #wait-for as far (symbol) -> u0;
```

`wait-for` is defined as a function in the binary which takes a symbol as its only argument and returns nothing.

Every function that you use from the game (so excluding functions that you have defined yourself) must be declared this way.

### Overwriting a state script function

Now we can start actually changing the games code.

In target mode, the compiler will try to match the functions you've written to functions that exist in the original script file, and then change the file to use your functions instead, essentially overwriting them. This works with functions inside state scripts and standalone functions. For this mod, we are going to overwrite to functions inside state scripts.

The structure inside state scripts is layed out in detail inside the `background.md` file. In order to replace a state script lambda (function), you'll need to first target its exact name. The name of a script lambda is given as 

```
#<state>@<block>@<track>@<lambda_index>
```

You can read off the name for all of these values in the respective .dcpl decompiled file. You take this name and start the function definition as you would with any function, with its return type. As lambdas never return a value we simply write `u0`. Then we'll put the name as mentioned above. Finally we write the paramter list inside `()` which for lambas is always empty. This leaves us with a complete function header: 

```
u0 #spawn-lockbox@start@main@0()
```

Which equals the first lambda in track `main` in block `start` inside the state `spawn-lockbox`. This lambda will now be replaced by our code.

``` The code

This is of course the part where you'll have to do most of the work on your own. The programming language is described in great detail inside grammar.md. The syntax should be very familiar if you've ever written any C-based languages. Some of the unique features include:

#### `#` SID values

Any symbol that you use from the game (being that either global symbols or function names) must be prefixed with a `#` (similar to what we did with the function name). This tells the compiler to treat this symbol as a hash. As described above, functions must be declared, unlike global symbols (e.g. `#ellie`, `#pistol-baretta`, `#self`, etc...) which can just be used immediately.

#### Special operators

`dcpl` comes with special operators out of the box for some common functions used in the script.

* `>>`: The `display` operator. If you have a .asi file that hooks the games display function, this can be used for loggin. Very useful when debugging scripts. For example ´>> "inside start block";`
* `$`: The `dc:format` operator. Used to format strings with C-like format specifiers. Left side of the operator is a string and right side a list of comma-seperated expressions. For example: `string message = "current ammo count: %d" $ ammo_count;`. This can be of course be used in conjunction with the display opertor: `>> "current ammot count: %d" $ ammo_count;`
* `=>`: The `go` operator: Used to declare that we are transitioning to a new state. Replaces the games `go()` function. For example: `=> #spawn-empty-lockbox;`

Besides these, everything should feel like a normal programming language. Of course there aren't too many fancy features as this is a completely custom compiler and I don't have time to implement every single feature myself. But the foundation should be good enough for script modding.

Below is the full code for the mod to skip the No return rewards box. It should serve as a resource for you to see how mods can be written but of course is not the gold standard.

```
u0 #spawn-lockbox@start@main@0() {
    >> "reached spawn lockbox state";
    if (!#get-svar(#hub-opened-crate)) {
        >> "not opened lockbox yet, proceeding...";
        bool hasManual = #encounter--has-upgrade-manual?(#encounter--get-current-encounter-ddict());
        bool resumedRun = #rogue-get-is-resumed-run();
        bool waitingForCollect = #get-svar-as-boolean(#hub-reward-wait-collect);

        if (!(resumedRun && hasManual && !waitingForCollect)) {
            >> "giving out rewards.";
            #populate-hub-rewards();
            #inventory-item-add-quantity(#parts, #get-int32(#hud-parts-count, #self));
            #inventory-item-add-quantity(#player-upgrade-supplements, #get-int32(#hud-supplements-count, #self));
            #set-svar(#vendor-currency, #get-svar(#vendor-currency) + #get-int32(#hud-currency-count, #self));
            #send-event(
                #pick-up-vendor-currency,
                #canvas.hudRoot.healthAndAmmoV2.currency-count,
                #new-boxed-value(2, #get-int32(#hud-currency-count, #self))
            );
            #inventory-item-add-quantity(#vendor-currency, #get-int32(#hud-currency-count, #self));
            #broadcast-event(#vendor-changed);
        }

        #set-svar-as-boolean(#hub-reward-wait-collect, 0);

        if (hasManual) {
            >> "has a manual, giving... ";
            #give-upgrade-path(#encounter--get-upgrade-manual(#encounter--get-current-encounter-ddict()), 0);
        }

        >> "setting #hub-opened-crate to true";
        #set-svar(#hub-opened-crate, 1);
    }
}

u0 #spawn-lockbox@start@main@1() {
    >> "skipping lock box and spawning empty one...";
    => #spawn-empty-lockbox;
}
```

Note the liberal use of logging. I highly encourage this to quickly tell what's going on in your custom scripts. There might be some more advanced debugging tools in the future but for now print-debugging is a great way to do it.

After you've written your script, compile it using the `dcc.exe` compiler. If you've set everything up correctly, it should do everything automatically, including creating the final `your-mod.psarc` file in your games mod folder, which should leave you ready to immediately begin testing. From there it's an iterative approach: Compile the script, see what works and what doesn't, fix it, and recompile. Eventually you'll have a working mod!
