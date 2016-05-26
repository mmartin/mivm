#include<stdexcept>
#include<vector>
#include<iostream>
#include<iomanip>
#include<iterator>

#include"mivm.hpp"

namespace MiVM {

MiVM::MiVM(const int width, const int height)
{
    if (width == 0 || height == 0) {
        return;
    }

    videoMemory.resize(width * height);
}

void MiVM::load(const std::string& inputFile)
{
    throw std::runtime_error("not implemented");
}

void MiVM::load(const std::initializer_list<uint16_t>& program)
{
    memory.fill(0);
    regV.fill(0);
    regI = 0;
    instrPtr = 0x200;

    for (const auto instr : program) {
        memory[instrPtr++] = 0xFF & (instr >> 8);
        memory[instrPtr++] = 0xFF & (instr);
    }
}

void MiVM::run()
{
    std::fill(videoMemory.begin(), videoMemory.end(), 0);
    stack = decltype(stack)();
    instrPtr = 0x200;

    bool halt = false;

    while (!halt) {
        const OPCode opcode = OPCode(uint16_t(memory[instrPtr] << 8) |
                                     uint16_t(memory[instrPtr+1]));

#ifdef MIVM_DEBUG
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(3) << instrPtr
                  << " @ 0x" << std::setw(4) << opcode << std::endl;
#endif

        halt = execute(opcode);
    }
}

bool MiVM::execute(const OPCode opcode)
{
#define NIBBLE(position) ((opcode >> 4*position) & 0xF)
#define BYTE(position) ((opcode >> 8*position) & 0xFF)
#define ADDR (opcode & 0xFFF)

    switch (opcode >> 12) {
        case 0x0:
            switch (opcode & 0xFFF) {
                case 0x0FD:
                    return true;

                case 0x0EE:
                    instrPtr = stack.top() + 2;
                    stack.pop();
                    break;

                case 0x0E0:
                    std::fill(videoMemory.begin(), videoMemory.end(), 0);
                    break;

                default:
                    throw std::logic_error("unknown opcode subtype: " + std::to_string(opcode));
            }
            break;

        case 0x1:
            instrPtr = ADDR;
            break;

        case 0x2:
            stack.push(instrPtr);
            instrPtr = ADDR;
            break;

        case 0x3:
            instrPtr += (regV[NIBBLE(2)] == BYTE(0) ? 4 : 2);
            break;

        case 0x4:
            instrPtr += (regV[NIBBLE(2)] != BYTE(0) ? 4 : 2);
            break;

        case 0x5:
            instrPtr += (regV[NIBBLE(2)] == regV[NIBBLE(1)] ? 4 : 2);
            break;

        case 0x6:
            regV[NIBBLE(2)] = BYTE(0);
            instrPtr += 2;
            break;

        case 0x7:
            regV[NIBBLE(2)] += BYTE(0);
            instrPtr += 2;
            break;

        case 0x8:
            switch (opcode & 0xF) {
                case 0x0: regV[NIBBLE(2)] = regV[NIBBLE(1)];
                          break;

                case 0x1: regV[NIBBLE(2)] |= regV[NIBBLE(1)];
                          break;

                case 0x2: regV[NIBBLE(2)] &= regV[NIBBLE(1)];
                          break;

                case 0x3: regV[NIBBLE(2)] ^= regV[NIBBLE(1)];
                          break;

                case 0x4: regV[0xF] = (regV[NIBBLE(1)] > 0xFF - regV[NIBBLE(2)]);
                          regV[NIBBLE(2)] += regV[NIBBLE(1)];
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

        case 0x9:
            instrPtr += (regV[NIBBLE(2)] != regV[NIBBLE(1)] ? 4 : 2);
            break;

        case 0xA:
            regI = ADDR;
            instrPtr += 2;
            break;

        case 0xB:
            instrPtr = ADDR + regV[0];
            break;

        case 0xC:
            regV[NIBBLE(2)] = (rand() % 255) & BYTE(0);
            instrPtr += 2;
            break;

        default:
            throw std::logic_error("unknown opcode: " + std::to_string(opcode));
    }

    return false;

#undef NIBBLE
#undef BYTE
#undef ADDR
}

#ifdef MIVM_DEBUG
void MiVM::dumpStack() const
{
    if (stack.empty()) {
        std::cout << "[ ]" << std::endl;
        return;
    }

    std::vector<uint16_t> tmp(&stack.top() - stack.size() + 1, &stack.top() + 1);

    std::cout << std::hex << "[ 0x";
    std::copy (tmp.begin(), tmp.end(), std::ostream_iterator<uint16_t>(std::cout, ", 0x"));
    std::cout << "\b\b\b\b ]  " << std::endl;
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
