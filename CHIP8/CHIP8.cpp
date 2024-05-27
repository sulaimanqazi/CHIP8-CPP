#include <bits/stdc++.h>
#include "CHIP8.hpp"
#include "Platform.hpp"

const unsigned int FONTSET_SIZE = 80; //16 characters,5 bytes each, 80 total
const unsigned int FONTSET_START_ADDRESS = 0x50; //80
const unsigned int START_ADDRESS = 0x200;//512
using namespace std;

uint8_t fontset[FONTSET_SIZE]=
        {
                0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                0x20, 0x60, 0x20, 0x20, 0x70, // 1
                0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2aqqdw
                0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                0xF0, 0x80, 0xF0, 0x80, 0x80  // F

        };
//Seed random number generator with the current time so its a different number each time
Chip8::Chip8() : randGen(chrono::system_clock::now().time_since_epoch().count()){

    //initialize the program counter to the starting address of 512
    pc = START_ADDRESS;

    //Load fonts into memory by iterating through memory at the starting address of 80/0x50 and setting the next 80 bytes as the fontset
    for(unsigned int i = 0; i <FONTSET_SIZE; ++i){ //repeat 80 times
        memory[FONTSET_START_ADDRESS + i]  = fontset[i];
    }

    //initialize RNG of randByte
    randByte = uniform_int_distribution<uint8_t>(0, 255U);

    //Function pointer table
    //Entire opcode is unique
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    for (size_t i = 0; i <= 0xE; i++)
    {
        //In case invalid opcode is called, dummy null function used to do nothing
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }


    //First three digits are $00E, fourth is unique
    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    //First digit repeats, last digit unique
    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    //This and lower ones: First digit repeats, last 2 digits unique
    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;

    //Table F last 2 digits go up to 0x65 so they need their own for loop
    for (size_t i = 0; i <= 0x65; i++)
    {
        //In case invalid opcode is called, dummy null function used to do nothing
        tableF[i] = &Chip8::OP_NULL;
    }

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;


}

int main(int argc,char** argv){
    if(argc != 4) {
        cerr << "Usage: " << argv[0] << " <111111111weeee> <Delay> <ROM>\n";
        exit(EXIT_FAILURE);
    }

    int videoScale = stoi(argv[1]);
    int cycleDelay = stoi(argv[2]);
    char const* romFilename = argv[3];

    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);

    Chip8 chip8;
    chip8.LoadROM(romFilename);

    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

    auto lastCycleTime = chrono::high_resolution_clock::now();
    bool quit = false;
    while(!quit){
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = chrono::high_resolution_clock ::now();
        float dt = chrono::duration<float, chrono::milliseconds::period>(currentTime-lastCycleTime).count();

        if(dt>cycleDelay){
            lastCycleTime = currentTime;

            chip8.Cycle();

            platform.Update(chip8.video,videoPitch);
        }
    }
    return 0;


}

void Chip8::LoadROM(char const* filename){
    //Load the rom
    //Open file as binary (ios::binary) ,move file pointer to the end (ios::ate)
    ifstream file(filename, ios::binary | ios:: ate);

    //Proceeds if the file is open
    if(file.is_open()){
        //Current position of file pointer (which is at end) is retrieved and stored to size
        streampos size = file.tellg();
        //Buffer allocated with the file size to hold contents
        char* buffer = new char[size];

        //Go to beggining of file (ios::beg) and fill the buffer with the file size
        file.seekg(0, ios::beg);
        file.read(buffer, size);

        file.close();
        //Load rom content into chip* memory starting at 0x200 (512, the starting address)
        for(long i = 0; i< size; ++i){
            memory[START_ADDRESS + i] = buffer[i];
        }

        //Free the buffer, We already loaded rom so there's no need for it
        delete[] buffer;
    }




}


void Chip8::Cycle(){
    //Fetch
    opcode = (memory[pc] << 8u) | memory[pc+1];

    //Increment program counter before executing anything
    pc+=2;

    //Decode and execute
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();


    //If the delay/sound timers are set, make them go down by decrementing for each cycle
    if(delayTimer > 0){
        --delayTimer;
    }
    if(soundTimer > 0){
        --soundTimer;
    }
}





void Chip8::OP_00E0()
{
    //Clear display by setting video buffer to all zeroes
    memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE(){
    //RET Return from a subroutine
    // decrease stack pointer by one, set program counter to stack at the location of the pointer
    --sp;
    pc = stack[sp];
}

void Chip8::OP_2nnn(){
    // CALL address. Call subroutine at nnn
    uint16_t address = opcode & 0x0FFFu; //address = nnn, the AND is to zero out the 1 since we only need nnn
    stack[sp] = pc; //current program counter gets put on top of the stack
    ++sp; //increase stack pointer by one
    pc = address; //set program counter to nnn
    // pc+=2 in cycle so current pc holds instruction after this call, avoiding loop of calls and returns
}

void Chip8::OP_1nnn(){
    //Same as CALL, just no interaction with stack
    uint16_t address = opcode & 0x0FFFu; //address = nnn, the AND is to zero out the 1 since we only need nnn
    pc = address; //set program counter to nnn
}

void Chip8::OP_3xkk(){
    //Skip next instruction if Vx = kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode (zeroes out the rest), shifts the values by 8 to the right edge so it can be compared with kk
    uint8_t byte = opcode & 0x00FFu; // Only takes the last 2 nibbles of the opcode

    //If the vx register is equal to kk, skip by incrementing pc by 2 again
    if(registers[Vx] == byte){
        pc +=2;
    }
}

void Chip8::OP_4xkk(){
    //Skip next instruction if vx is not equal to kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge so it can be compared with kk
    uint8_t byte = opcode & 0x00FFu; // Only takes the last 2 nibbles of the opcode

    //If the vx register is  not equal to kk, skip by incrementing pc by 2 again
    if(registers[Vx] != byte){
        pc +=2;
    }
}

void Chip8::OP_5xy0(){
    //Skip next instruction if Vx = Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge so it can be compared with vy
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; //Only takes the third nibble of the opcode, shifts the values by 4 to the right edge so it can be compared with Vx

    //If the vx register is equal to the vy register, skip by incrementing pc by 2 again
    if (registers[Vx] == registers[Vy])
    {
        pc += 2;
    }
}

void Chip8:: OP_6xkk(){
    //Set Vx to kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Takes only the second nibble, shifts it to the right edge like byte
    uint8_t byte = opcode & 0x00FFu;//Takes only the last two nibbles (kk)
    //Set vx register to byte
    registers[Vx] = byte;
}

void Chip8::OP_7xkk(){
    //Set Vx = Vx + kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Takes only the second nibble, shifts it to the right edge like byte
    uint8_t byte = opcode & 0x00FFu;//Takes only the last two nibbles (kk)
    //Vx register is added with byte
    registers[Vx] += byte;

}

void Chip8::OP_8xy0(){
    //Set Vx to vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; //Only takes the third nibble of the opcode, shifts the values by 4 to the right edge

    //Vx register is set to vy register
    registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1(){
    //Set Vx = Vx OR Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; //Only takes the third nibble of the opcode, shifts the values by 4 to the right edge

    //OR the values of Vx and Vy and set it to VX
    registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2(){
    //Set Vx = Vx AND Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; //Only takes the third nibble of the opcode, shifts the values by 4 to the right edge

    //AND the values of Vx and Vy and set it to VX
    registers[Vx] &= registers[Vy];

}

void Chip8::OP_8xy3(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; //Only takes the third nibble of the opcode, shifts the values by 4 to the right edge

    registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4(){
    //Vx = Vx + Vy, VF = Carry
    //Vx and Vy are added, if the result is over 8 bits (>255) then Vf is set to 1 indicating a carry over in the addition
    //Only the lowest 8 bits are stored in vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; //Only takes the third nibble of the opcode, shifts the values by 4 to the right edge

    //Add register vx and vy
    uint16_t sum = registers[Vx] + registers[Vy];

    //IF we need to carry, make the changes to the Vf register
    if(sum>255u){
        registers[0xF] = 1;
    }else{
        registers[0xF] = 0;
    }

    //Set vx to the lowest 8 bits of the sum
    registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5(){
    //Set Vx = Vx - Vy, Vf = NOT borrow

    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; //Only takes the third nibble of the opcode, shifts the values by 4 to the right edge

    //if vx is greater than vy we won't need to borrow (set it to 1) otherwise we will so set it to 0
    if(registers[Vx] > registers[Vy]){
        registers[0xF] = 1;
    }else{
        registers[0xF] = 0;
    }
    //Vx = vx - vy
    registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6(){
    //Set Vx - Vx SHR 1
    // make Vf =1 if the least significant bit (first one on the right) of Vx is 1, otherwise its 0, then Vx is divided by 2
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge

    //Set Vf to LSB of vx
    registers[0xF] = (registers[Vx] & 0x1u);

    //Right shift of register vx to divide it by 2;
    registers[Vx] >>=1;

}

void Chip8::OP_8xy7(){
    //Set Vx = Vy - Vx, set VF = NOT borrow
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; //Only takes the third nibble of the opcode, shifts the values by 4 to the right edge

    //if vy>vx set vf to 1 because we won't need to borrow, otherwise its 0.
    if(registers[Vy] > registers[Vx]){
        registers[0xF] = 1;
    }else{
        registers[0xF] = 0;
    }

    //subtract vy-vx and store it in Vx
    registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE(){
    //If the most significant bit (leftmost bit) of Vx is 1 set Vf to 1, otherwise set it 0. Then Multiply Vx by 2
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge

    //Set Vf to the Most significant bit
    registers[0xF] = (registers[Vx] * 0x80F) >> 7u;

    //Multiply vx by 2 by leftshifting by 1
    registers[Vx] <<= 1;

}

void Chip8::OP_9xy0(){
    //Skip next instruction if Vx is not equal to vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; //Only takes the third nibble of the opcode, shifts the values by 4 to the right edge

    if(registers[Vx] != registers[Vy]){
        pc+=2; //Skip by incrementing program counter by 2 again
    }
}

void Chip8::OP_Annn(){
    //set i = nnn
    uint16_t address = opcode & 0x0FFFu; //set address to nnn
    index = address;
}

void Chip8::OP_Bnnn() {
    //make program counter jump to nnn + register v0
    uint16_t address = opcode & 0x0FFFu; //set address to nnn
    pc = registers[0] + address;
}
void Chip8::OP_Cxkk() {
    //Set Vx to a random byte AND kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t byte = opcode & 0x00FFu; //Only takes last 2 nibbles of opcode (kk)

    //randbyte/randgen comes from what I imported in the hpp file
    registers[Vx] = randByte(randGen) & byte;
}
void Chip8::OP_Dxyn(){

    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; //Only takes the third nibble of the opcode, shifts the values by 4 to the right edge
    uint8_t height = opcode & 0x000Fu; //Takes only the 4th nibble

    //Wrap if going beyond screen boundaries
    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row){
        uint8_t spriteByte = memory[index +row];
        for ( unsigned  int col = 0; col < 8; ++col){
            uint8_t spritePixel = spriteByte &(0x80u >> col);
            uint32_t* screenPixel = &video[(yPos+row)*VIDEO_WIDTH+(xPos+col)];

            if(spritePixel){
                if(*screenPixel ==  0xFFFFFFFF){
                    registers[0xF] = 1;
                }

                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }

}

void Chip8::OP_Ex9E(){
    //Skip next instruction if key with the value of vx is pressed
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t key = registers[Vx];

    if(keypad[key]){
        pc+=2; //increment program counter by 2 again to skip
    }
}

void Chip8:: OP_ExA1(){
    //Skip next instruction if key with the value of vx is not pressed
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t key = registers[Vx];

    if(!keypad[key]){
        pc+=2; //increment program counter by 2 again to skip
    }
}

void Chip8::OP_Fx07(){
    //Set Vx = delay timer value

    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge

    registers[Vx] = delayTimer;

}

void Chip8::OP_Fx0A(){
//Wait for a key press, store the value of the key in vx
uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
if(keypad[0]){
    registers[Vx] = 0;
}else if (keypad[1]){
    registers[Vx] = 1;
}else if (keypad[2]){
    registers[Vx] = 2;
}else if (keypad[3]){
    registers[Vx] = 3;
}else if (keypad[4]){
    registers[Vx] = 4;
}else if (keypad[5]){
    registers[Vx] = 5;
}else if (keypad[6]){
    registers[Vx] = 6;
}else if (keypad[7]){
    registers[Vx] = 7;
}else if (keypad[8]){
    registers[Vx] = 8;
}else if (keypad[9]){
    registers[Vx] = 9;
}else if (keypad[10]){
    registers[Vx] = 10;
}else if (keypad[11]){
    registers[Vx] = 11;
}else if (keypad[12]){
    registers[Vx] = 12;
}else if (keypad[13]){
    registers[Vx] = 13;
}else if (keypad[14]){
    registers[Vx] = 14;
}else if (keypad[15]){
    registers[Vx] = 15;
}else{
    pc -= 2; // Makes PC "Wait" by reversing each time keypad not pressed
}

}

void Chip8::OP_Fx15(){
    //Set delay time = Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    delayTimer = registers[Vx];
}

void Chip8::OP_Fx18() {
    //Set sound timer = vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E() {
    //Set I = I + Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    index+= registers[Vx];
}

void Chip8::OP_Fx29(){
    //Set I = location of sprite for digit vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t digit = registers[Vx]; // VX register gives the digit

    //Any index can be selected by starting at starting address 0x50, and adding the digit multiplied by 5 since each font is 5 bytes
    index = FONTSET_START_ADDRESS + (5*digit);
}

void Chip8::OP_Fx33() {
    //Store Binary coded decimal representation of Vx in memory locations I, I+1, I+2
    //Takes decimal value of vx, places hundreds digit in memory at location I, tens in i+1, and ones in I+2
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge
    uint8_t value = registers[Vx];
    //Modulus operator used to get right most digit of number
    //divide by ten to remove the last digit to move on to next place value

    //Ones place / i+2
    memory[index+2] = value %10;
    value /= 10;
    //tens place / i+1
    memory[index+1] = value %10;
    value /= 10;
    //Hundreds place / i
    memory[index] = value %10;
}

void Chip8::OP_Fx55(){
    //Store registers V0 to Vx in memory starting at location i
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge

    for(uint8_t i = 0; i <= Vx; ++i){
        memory[index+i] = registers[i];
    }
}

void Chip8::OP_Fx65() {
    //Read registers V0 to Vx from memory starting at location I
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Only takes the second nibble of the opcode, shifts the values by 8 to the right edge

    for(uint8_t i = 0; i <= Vx; ++i){
        registers[i] = memory[index+i];
    }
}

void Chip8::Table0()
{
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8()
{
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{
    ((*this).*(tableF[opcode & 0x00FFu]))();
}

void Chip8::OP_NULL()
{}


