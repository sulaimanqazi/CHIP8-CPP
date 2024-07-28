# CHIP8-CPP
### A Chip 8 Interpeter I wrote with C++ and SDL2


![unknown_2024 05 27-18 59_1_1-ezgif com-crop](https://github.com/Sulaimanqazi/CHIP8-CPP/assets/109396075/591cf720-122f-4473-b59d-588a1975f53f)


The Chip 8 is a virtual machine developed in the mid-1970s used to play 8 bit games on computers. Although theres no underlying hardware, it serves great as introduction to programming emulators. I later implemented the skills learned in this project into building a [Game Boy Emulator](https://github.com/Sulaimanqazi/Game-Boy-Emulator).

The Chip 8 consists of the following components, which I implemented and manipulated to get the program functioning. 

* 16 8Bit Registers `uint8_t registers[16]{};`

* 4K Bytes of Memory `uint8_t memory[4096]{}`

* 16bit Index Register `uint16_t index{};`

* 16bit Program Counter `uint16_t pc{};`

* 16 Level Stack uint16_t `stack[16]{};`

* 8bit Stack Pointer `uint8_t sp{};`

* 8bit Delay Timer `uint8_t delayTimer{};`

* 8bit Sound Timer `uint8_t soundTimer{};`

* 16 Input Keys `uint8_t keypad[16]{};`

* 64x32 Monochrome Display Memory `uint32_t video[64*32]{};`


A large bulk of the code is spent on programming the CPU by implementing each opcode. The Chip 8 uses 35 opcodes, which are all two bytes long. Each opcode is an instruction the CPU may be asked to perform from the rom file.

For example, opcode 00E0 (CLS) instructs the CPU to clear the display, this is implemented by setting the video buffer to all zeroes. `memset(video, 0, sizeof(video));`

The SDL portion is used to process input from the user's keyboard and send it to the Chip 8. It is also used to render and display the output coming from the Chip 8.

After executing this program and loading in a rom from online such as tetris, the result is a playable game!

