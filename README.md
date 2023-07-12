# ⚠ THIS REPO IS OUT OF DATE, GO TO [GIT.SPARKBURST.NET](https://git.sparkburst.net/satscalc/about/) FOR THE LATEST VERSION

![satscalc logotype vector](https://user-images.githubusercontent.com/21090839/164118463-ca2ab578-3347-4764-bc74-dae629646a89.svg)

# satscalc
A calculator to help with invoicing for Norwegian film crew members.

You put in the time you worked, and it produces a nice receipt that you can copy-paste.

## Building from source

### 🐧 Linux and MacOS 🍎

You'll need `gcc` and `make`.

```
git clone https://github.com/SanJacobs/satscalc
cd satscalc
make
```

And if you want to install it, do:

```
sudo make install
```

### 🪟 Windows / cross-compilation ⚡

You'll need `zig` (Tested with 0.9.1) and `make`.

Using zig because MinGW/GCC can't supply .pdb files needed for debugging with RemedyBG, and clang on Windows is a pain. MSVC is even more of a pain.

```
git clone https://github.com/SanJacobs/satscalc
cd satscalc
make windows
```
Or `make windows32` if you need a 32-bit binary.

## State of development

It's basically in a usable state now.

Currently, the only ruleset implemented is the one for advertisement and short-term work.
Others may be added in the future.

Code is not yet law, so the [source of this ruleset](https://filmforbundet.no/wp-content/uploads/2022/04/Reklameoverenskomsten.pdf) is flawed, because it has never had to compile.
It is full of would-be unlit code paths, undefined cases and strangenesses.
On top of this, the interpreter is me, a flawed human being, and so there is no guarantee of correctness.

However, in my flawed human opinion, I think it is pretty much on the money.

## Known issues

Any known issues should be solved in a way which would not anger producers nor crew.
Which is an impossibility, but I try.

Descriptions here are a simplifications and does not fully encapsulate the solutions as implemented in the code.
Before you complain about any solution, read the code.

The main known issue is that in practice, advertisement and short-term jobs don't usually have a defined "normal" workday, because they're by definition only a few days, often all with different crew call times.
Yet, the ruleset bases multiple rules off of this variable (which, as mentioned, oft goes undefined).
This means there is no implementation of "overtime" before the "normal" workday has started, because there is no normal.
Overtime is assumed to come beyond 8 hours after the call time.

There is a separate host of rules for work where you're staying and sleeping on-location.
These rules are not covered.
Aside from the rule about pay on holidays, which strangely appears in this section, despite seeming to apply outside of cases where you sleep on-location.

## License

Copyright (C) 2022 Sander Skjegstad

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

This software does NOT give financial advice.

