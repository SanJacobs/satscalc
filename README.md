![satscalc logotype vector](https://user-images.githubusercontent.com/21090839/164118463-ca2ab578-3347-4764-bc74-dae629646a89.svg)

# satscalc
A calculator to help with invoicing for Norwegian film crew members.

At first the intention was to make a simple Python script that does what I needed it to do.
However, it turns out that I don't like python that much anymore, so here we go with making it in C++.

It's basically in a usable state now, the first proper alpha release is coming up.

Currently, the only ruleset it follows is the one for advertisement and short-term work.
Others may be added in the future.

## Building from source

### 🐧 Linux and (probably) MacOS 🍎

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

### 🪟 Windows / cross-compilation

You'll need `zig` (Tested with 0.9.1) and `make`.

Using zig because MinGW/GCC can't supply .pdb files needed for debugging with RemedyBG, and clang on Windows is a pain. MSVC is even more of a pain.

```
git clone https://github.com/SanJacobs/satscalc
cd satscalc
make windows
```
Or `make windows32` if you need a 32-bit binary.

## Disclaimer

This is free software with ABSOLUTELY NO WARRENTY.\
This software does NOT give financial advice.
