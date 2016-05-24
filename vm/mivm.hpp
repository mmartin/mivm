#ifndef MIVM_H
#define MIVM_H

#include<string>
#include<array>
#include<stack>
#include<cstdint>

namespace MiVM {

// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
enum OPCode { CLS  = 0x00E0 // 0E0
            //, RET  = 0x00EE // 0EE
            //, JP   = 0x1000 // nnn addr
            //, CALL = 0x2000 // nnn addr
            //, SE   = 0x3000 // xkk Vx, byte
            //, SNE  = 0x4000 // xkk Vx, byte
            //, SER  = 0x5000 // xy0 Vx, Vy
            //, LD   = 0x6000 // xkk Vx, byte
            //, ADD  = 0x7000 // xkk Vx, byte
            //, LDR  = 0x8000 // xy0 Vx, Vy
            //, OR   = 0x8001 // xy1 Vx, Vy
            //, AND  = 0x8002 // xy2 Vx, Vy
            //, XOR  = 0x8003 // xy3 Vx, Vy
            //, ADD  = 0x8004 // xy4 Vx, Vy
            //, SUB  = 0x8005 // xy5 Vx, Vy
            //, SHR  = 0x8006 // xy6 Vx {, Vy}
            //, SUBN = 0x8007 // xy7 Vx, Vy
            //, SHL  = 0x800E // xyE Vx {, Vy}
            //, SNER = 0x9000 // xy0 Vx, Vy
            //, LDI  = 0xA000 // nnn I, addr
            //, JPD  = 0xB000 // nnn V0, addr
            //, RND  = 0xC000 // xkk Vx, byte
            //, DRW  = 0xD000 // xyn Vx, Vy, nibble
            //, SKP  = 0xE09E // x9E Vx
            //, SKNP = 0xE0A1 // xA1 Vx
            //, LD   = 0xF007 // x07 Vx, DT
            //, LD   = 0xF00A // x0A Vx, K
            //, LD   = 0xF015 // x15 DT, Vx
            //, LD   = 0xF018 // x18 ST, Vx
            //, ADD  = 0xF01E // x1E I, Vx
            //, LD   = 0xF029 // x29 F, Vx
            //, LD   = 0xF033 // x33 B, Vx
            //, LD   = 0xF055 // x55 [I], Vx
            //, LD   = 0xF065 // x65 Vx, [I]
            , EXIT = 0x00FD
            };

class MiVM {
public:
    void load(const std::string& inputFile);
    void load(const std::initializer_list<uint16_t>& program);
    void run();

#ifdef BOOST_TEST_MODULE
    const std::stack<uint16_t>& getStack() const {
        return stack;
    }
#endif

private:
    bool execute(const OPCode opcode);

#ifdef MIVM_DEBUG
    void dumpStack() const;
    void dumpMemory() const;
#endif

    std::array<uint8_t, 1024> memory;
    std::array<int8_t, 16> regV;
    uint16_t regI, instrPtr;

    std::stack<uint16_t> stack;
};

}
#endif
