#include<stdexcept>
#include<vector>
#include<iostream>
#include<iomanip>

#include"mivm.hpp"

namespace MiVM {

void MiVM::load(const std::string& inputFile)
{
    throw std::runtime_error("not implemented");
}

void MiVM::load(const std::initializer_list<uint16_t>& program)
{
    memory.fill(0);
    instrPtr = 0x200;
    for (const auto instr : program) {
        memory[instrPtr++] = 0xFF & (instr >> 8);
        memory[instrPtr++] = 0xFF & (instr);
    }
}

void MiVM::run()
{
    instrPtr = 0x200;

    bool halt = false;

    while (!halt) {
        const OPCode opcode = OPCode(uint16_t(memory[instrPtr] << 0x8) |
                                     uint16_t(memory[instrPtr+1]));

        halt = execute(opcode);
    }
}

bool MiVM::execute(const OPCode opcode)
{
#define NIBBLE(position) ((opcode >> 4*position) & 0xF)
#define BYTE(position) ((opcode >> 8*position) & 0xFF)

    switch (opcode >> 12) {
        case 0x0:
            switch (opcode & 0xFFF) {
                case 0x0FD: return true;

                default:
                    throw std::logic_error("unknown opcode subtype: " + std::to_string(opcode));
            }
            break;

        case 0x8:
            switch (opcode & 0xF) {
                case 0x0: regV[NIBBLE(2)] = regV[NIBBLE(1)];
                          break;

                case 0x1: regV[NIBBLE(2)] = regV[NIBBLE(1)] | regV[NIBBLE(2)];
                          break;

                case 0x2: regV[NIBBLE(2)] = regV[NIBBLE(1)] & regV[NIBBLE(2)];
                          break;

                case 0x3: regV[NIBBLE(2)] = regV[NIBBLE(1)] ^ regV[NIBBLE(2)];
                          break;

                case 0x4: regV[0xF] = (regV[NIBBLE(1)] > 0xFF - regV[NIBBLE(2)]);
                          regV[NIBBLE(2)] = regV[NIBBLE(1)] + regV[NIBBLE(2)];
                          break;

                case 0x5: regV[0xF] = (regV[NIBBLE(2)] > regV[NIBBLE(1)]);
                          regV[NIBBLE(2)] = regV[NIBBLE(2)] - regV[NIBBLE(1)];
                          break;

                case 0x6: regV[0xF] = (regV[NIBBLE(2)] & 0x1);
                          regV[NIBBLE(2)] /= 2;
                          break;

                case 0x7: regV[0xF] = (regV[NIBBLE(1)] > regV[NIBBLE(2)]);
                          regV[NIBBLE(2)] = regV[NIBBLE(1)] - regV[NIBBLE(2)];
                          break;

                case 0xE: regV[0xF] = (regV[NIBBLE(2)] & 0x1);
                          regV[NIBBLE(2)] *= 2;
                          break;

                default:
                    throw std::logic_error("unknown opcode subtype: " + std::to_string(opcode));
            }
            instrPtr += 2;
            break;

        default:
            throw std::logic_error("unknown opcode: " + std::to_string(opcode));
    }

    return false;
#undef NIBBLE
#undef BYTE
}

#ifdef MIVM_DEBUG
void MiVM::dumpStack() const
{
    //std::cout << '[';
    //for (auto i = 0; i < stackPtr; ++i) {
        //std::cout << (int)stack[i] << ", ";
    //}
    //std::cout << "]" << std::endl;
}

void MiVM::dumpMemory() const
{
    std::cout << std::hex;

    for (size_t i = 0; i < memory.size(); ++i) {
        if (i % 0x10 == 0) {
            std::cout << std::setfill('0') << std::endl << "0x"
                      << std::setw(3) << i << "    ";
        }

        std::cout << std::setw(2) << (size_t)memory[i] << "  ";
    }

    std::cout << std::endl;
}
#endif

}
