#include<stdexcept>
#include<vector>
#include<iostream>
#include<iomanip>
#include<iterator>

#include"mivm.hpp"

namespace MiVM {

static const uint16_t font[] = { 0xF0, 0x90, 0x90, 0x90, 0xF0 // 0
                               , 0x20, 0x60, 0x20, 0x20, 0x70 // 1
                               , 0xF0, 0x10, 0xF0, 0x80, 0xF0 // 2
                               , 0xF0, 0x10, 0xF0, 0x10, 0xF0 // 3
                               , 0x90, 0x90, 0xF0, 0x10, 0x10 // 4
                               , 0xF0, 0x80, 0xF0, 0x10, 0xF0 // 5
                               , 0xF0, 0x80, 0xF0, 0x90, 0xF0 // 6
                               , 0xF0, 0x10, 0x20, 0x40, 0x40 // 7
                               , 0xF0, 0x90, 0xF0, 0x90, 0xF0 // 8
                               , 0xF0, 0x90, 0xF0, 0x10, 0xF0 // 9
                               , 0xF0, 0x90, 0xF0, 0x90, 0x90 // A
                               , 0xE0, 0x90, 0xE0, 0x90, 0xE0 // B
                               , 0xF0, 0x80, 0x80, 0x80, 0xF0 // C
                               , 0xE0, 0x90, 0x90, 0x90, 0xE0 // D
                               , 0xF0, 0x80, 0xF0, 0x80, 0xF0 // E
                               , 0xF0, 0x80, 0xF0, 0x80, 0x80 // F
                               };

void MiVM::load(const std::string& inputFile)
{
    reset(false);
    std::cout <<inputFile << std::endl;
    throw std::runtime_error("not implemented");
}

void MiVM::load(const std::initializer_list<uint16_t>& program)
{
    reset(false);

    std::copy(std::begin(font), std::end(font), std::begin(memory));

    uint16_t tmp = 0x200;
    for (const auto instr : program) {
        memory[tmp++] = 0xFF & (instr >> 8);
        memory[tmp++] = 0xFF & (instr);
    }

    state = State::Ready;
}

void MiVM::reset(const bool soft)
{
    if (!soft) {
        memory.fill(0);
    }
    regV.fill(0);
    regI = 0;
    stack = decltype(stack)();
    instrPtr = 0x200;

    if (state != State::Inactive) {
        state = State::Ready;
    }
}

State MiVM::run(const bool step)
{
    if (state != State::Ready && state != State::Running) {
        throw std::runtime_error("Not in Ready or Running state: " + std::to_string(state));
    }

    OPCode opcode;
    do {
        opcode = OPCode(uint16_t(memory[instrPtr] << 8) |
                        uint16_t(memory[instrPtr+1]));

#ifdef MIVM_DEBUG
        std::cout << " <- " << std::to_string(state) << std::endl
                  << "Addr: " << std::hex << std::setfill('0') << std::setw(3) << instrPtr
                  << " Instr: " << std::setw(4) << opcode << std::endl;
        dumpStack();
        dumpRegisters();
#endif
        state = execute(opcode);

#ifdef MIVM_DEBUG
        std::cout << " -> " << std::to_string(state) << std::endl;
#endif
    } while (state == State::Running && !step);

    return state;
}

State MiVM::getState() const
{
    return state;
}

State MiVM::setWaitingKeyboard(const uint16_t key)
{
    if (state != State::WaitingKeyboard) {
        throw std::runtime_error("Not in WaitingKeyboard state: " + std::to_string(state));
    }

    regV[tmpReg] = key;

    instrPtr += 2;
    state = State::Running;
    return state;
}

uint16_t MiVM::getGetKeyboard() const
{
    if (state != State::GetKeyboard) {
        throw std::runtime_error("Not in GetKeyboard state: " + std::to_string(state));
    }
    return tmpReg;
}

State MiVM::setGetKeyboard(const bool pressed)
{
    if (state != State::GetKeyboard) {
        throw std::runtime_error("Not in GetKeyboard state: " + std::to_string(state));
    }

    auto skip = pressed;
    if ((lastOpcode & 0xF0FF) == OPCode::SNKP) { // TODO: not really safe
        skip = !skip;
    }

    instrPtr += (skip ? 4 : 2);
    state = State::Running;
    return state;
}

State MiVM::execute(const OPCode opcode)
{
    lastOpcode = opcode;

    uint16_t tmp16;
#define NIBBLE(position) ((opcode >> 4*position) & 0xF)
#define BYTE(position) ((opcode >> 8*position) & 0xFF)
#define ADDR (opcode & 0xFFF)

    switch (opcode >> 12) {
        case 0x0:
            switch (opcode & 0xFFF) {
                case 0x0FD: return State::Finished;

                case 0x0EE: instrPtr = stack.top() + 2;
                            stack.pop();
                            break;

                case 0x0E0: //std::fill(videoMemory.begin(), videoMemory.end(), 0);
                            break;

                default:
                    throw std::logic_error("unknown opcode subtype: " + std::to_string(opcode));
            }
            break;

        case 0x1: instrPtr = ADDR;
                  break;

        case 0x2: stack.push(instrPtr);
                  instrPtr = ADDR;
                  break;

        case 0x3: instrPtr += (regV[NIBBLE(2)] == BYTE(0) ? 4 : 2);
                  break;

        case 0x4: instrPtr += (regV[NIBBLE(2)] != BYTE(0) ? 4 : 2);
                  break;

        case 0x5: instrPtr += (regV[NIBBLE(2)] == regV[NIBBLE(1)] ? 4 : 2);
                  break;

        case 0x6: regV[NIBBLE(2)] = BYTE(0);
                  instrPtr += 2;
                  break;

        case 0x7: regV[NIBBLE(2)] += BYTE(0);
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

        case 0x9: instrPtr += (regV[NIBBLE(2)] != regV[NIBBLE(1)] ? 4 : 2);
                  break;

        case 0xA: regI = ADDR;
                  instrPtr += 2;
                  break;

        case 0xB: instrPtr = ADDR + regV[0];
                  break;

        case 0xC: regV[NIBBLE(2)] = (rand() % 255) & BYTE(0);
                  instrPtr += 2;
                  break;

        case 0xE:
            switch (opcode & 0xFF) {
                case 0x9E:
                case 0xA1:
                    tmpReg = regV[NIBBLE(2)];
                    return State::GetKeyboard;

                default:
                    throw std::logic_error("unknown opcode subtype: " + std::to_string(opcode));
            }
            break;

        case 0xF:
            switch (opcode & 0xFF) {
                case 0x07: regV[NIBBLE(2)] = delayTimer;
                           break;

                case 0x0A: tmpReg = NIBBLE(2);
                           return State::WaitingKeyboard;

                case 0x15: delayTimer = regV[NIBBLE(2)];
                           break;

                case 0x18: soundTimer = regV[NIBBLE(2)];
                           break;

                case 0x1E: regI += regV[NIBBLE(2)];
                           break;

                case 0x29: regI = 5 * regV[NIBBLE(2)];
                           break;

                case 0x33: tmp16 = regV[NIBBLE(2)]; // TODO: is this correct ?

                           for (auto i = 0; i < 3; ++i) {
                               memory[regI + (2-i)] = tmp16 % 10;
                               tmp16 /= 10;
                           }
                           break;

                case 0x55: std::copy(std::begin(regV), std::begin(regV) + NIBBLE(2) + 1,
                                     std::begin(memory) + regI);
                           break;

                case 0x65: std::copy(std::begin(memory) + regI, std::begin(memory) + regI + NIBBLE(2) + 1,
                                     std::begin(regV));
                           break;

                default:
                    throw std::logic_error("unknown opcode subtype: " + std::to_string(opcode));
            }
            instrPtr += 2;
            break;

        default:
            throw std::logic_error("unknown opcode: " + std::to_string(opcode));
    }

    return State::Running;

#undef NIBBLE
#undef BYTE
#undef ADDR
}

#ifdef MIVM_DEBUG

template<typename T>
static void dumpContainer(const T& c, const bool addrPrefix = false)
{
    std::cout << std::hex;

    for (size_t i = 0; i < c.size(); ++i) {
        if (addrPrefix && i % 0x10 == 0) {
            std::cout << std::setfill('0') << std::endl << "0x"
                      << std::setw(3) << i << "    ";
        }

        std::cout << std::setw(2) << (size_t)c[i] << "  ";
    }

    std::cout << std::endl;
}

void MiVM::dumpStack() const
{
    std::cout << "S: ";

    if (stack.empty()) {
        std::cout << "<empty>" << std::endl;
        return;
    }

    std::vector<uint16_t> tmp(&stack.top() - stack.size() + 1, &stack.top() + 1);
    dumpContainer(tmp);
}

void MiVM::dumpRegisters() const {
    std::cout << "I: " << std::hex << regI << std::endl;

    std::cout << "V: ";
    dumpContainer(regV);
}

void MiVM::dumpMemory() const
{
    dumpContainer(memory, true);
}
#endif

}

namespace std { string to_string(const MiVM::State& state)
{
    switch (state) {
        //TODO: better solution?
        case MiVM::State::Inactive: return "Inactive";
        case MiVM::State::Ready:    return "Ready";
        case MiVM::State::Running:  return "Running";
        case MiVM::State::WaitingKeyboard: return "WaitingKeyboard";
        case MiVM::State::GetKeyboard:     return "GetKeyboard";
        case MiVM::State::Finished:        return "Finished";
    }
    return "";
}
}
