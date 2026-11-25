# Intel 8080 & Space Invaders Emulator

An Intel 8080 emulator that includes a separate setup for emulating the arcade game of Space Invaders and its hardware.

---

## Basic emulator

### Build
**Compile:**
`gcc src/emu_main.c src/emulator.c src/disassembler.c src/utils.c -o emu.bin`

**Arguments:**
`./emu.bin <binary> <binary load address (default 0)>`

**Example:**
`./emu.bin test.bin 0x0100`

---

## Space Invaders Setup

![Space Invaders running in the emulator](assets/demo.gif)

### Controls
Controls, window scaling and advanced configs are customizable at `src/custom_setups/space_invaders_main.c`


#### Default Control Scheme
- Insert Coin -> **C**

- Start With One Player -> **1**

- Start With Two Players -> **2**

- Player One Shoot -> **S**

- Player One Left -> **A**

- Player One Right -> **D**

- Player Two Shoot -> **K**

- Player Two Left -> **J**

- Player Two Right -> **L**


***For legal reasons ROM needs to be obtained by the user***. It usually comes in four parts, connect them in one before running with:
**Linux / MacOS**
```
cat invaders.h invaders.g invaders.f invaders.e > invaders.rom
```
**Windows (cmd)**
```
copy /b invaders.h+invaders.g+invaders.f+invaders.e invaders.rom
```

### Build
\***Requires raylib** for crossplatform window handling.

**Linux / MacOS**
```bash
gcc src/custom_setups/space_invaders_main.c src/emulator.c src/disassembler.c src/utils.c $(pkg-config --cflags --libs raylib) -o space_invaders
./space_invaders invaders.rom
```

**Windows (MinGW-w64, Git Bash)**
```bash
gcc src/custom_setups/space_invaders_main.c src/emulator.c src/disassembler.c src/utils.c $(pkg-config --cflags --libs raylib) -o space_invaders.exe
./space_invaders.exe invaders.rom
```
