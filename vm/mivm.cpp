#include<stdexcept>
#include<vector>
#include<iostream>

#include"mivm.hpp"

namespace MiVM {

/*void MiVM::load(const std::string& inputFile)
{
    throw std::runtime_error("not implemented");
}*/

void MiVM::load(const std::initializer_list<arch_t>& programData)
{
    program = programData;
}

arch_t MiVM::run()
{
    if (program.empty()) {
        throw std::runtime_error("program not loaded");
    }

    framePtr = 0;
    uarch_t instructionPtr = 0;

    while (true) {
        //TODO: this looks stupid

#define OPC_ARG(i) program[instructionPtr+i]

#ifdef MIVM_DEBUG
#define OPC(work, ip) \
        case OPC_CODE: \
            std::cout << std::endl << "proc@" << (int)instructionPtr << ": " << OPC_NAME; \
            for (uarch_t i = 1; i <= OPC_ARGC; ++i) { std::cout << " " << (int)OPC_ARG(i); } \
            std::cout << std::endl; \
            work; instructionPtr = ip; \
            dumpStack(); break;
#else
#define OPC(work, ip) \
        case OPC_CODE: work; instructionPtr = ip; break;
#endif

//TODO: maybe move this to opcodes.csv, too?
#define OPC_PUSH OPC(push(OPC_ARG(1)), OPC_IP_INC)
#define OPC_POP  OPC(pop(),            OPC_IP_INC)

#define OPC_STORE OPC(store(OPC_ARG(1)), OPC_IP_INC)
#define OPC_LOAD  OPC(load(OPC_ARG(1)),  OPC_IP_INC)

#define OPC_ADD OPC(push(pop() + pop()), OPC_IP_INC)
#define OPC_MUL OPC(push(pop() * pop()), OPC_IP_INC)
#define OPC_INC OPC(push(pop() + 1),     OPC_IP_INC)
#define OPC_DEC OPC(push(pop() - 1),     OPC_IP_INC)

#define OPC_JMP   OPC(, OPC_ARG(1))
#define OPC_JMPZ  OPC(, pop() == 0 ? OPC_ARG(1) : OPC_IP_INC)
#define OPC_JMPNZ OPC(, pop() != 0 ? OPC_ARG(1) : OPC_IP_INC)

#define OPC_EQ   OPC(push(pop() == pop()), OPC_IP_INC)
#define OPC_LT   OPC(push(pop()  < pop()), OPC_IP_INC)
#define OPC_HALT OPC(return pop(), 0)

        switch (program[instructionPtr]) {
#include "mivm.cpp.inc.tmp"

            default:
                throw std::logic_error("unknown opcode: " +
                                       std::to_string(program[instructionPtr]));
        }
    }
}

void MiVM::push(const arch_t value)
{
#ifndef MIVM_NO_OVERFLOW_CHECK
    if (stack.size() == MAX_MEM) {
        throw std::overflow_error("stack overflow");
    }
#endif

#ifdef MIVM_DEBUG
    std::cout << "  push@" << stack.size() << ": " << (int)value << std::endl;
#endif
    stack.push_back(value);
}

arch_t MiVM::pop()
{
#ifndef MIVM_NO_UNDERFLOW_CHECK
    if (stack.size() == 0) {
        throw std::underflow_error("stack underflow");
    }
#endif

#ifdef MIVM_DEBUG
    std::cout << "  pop@" << stack.size()-1 << ": " << (int)stack.back() << std::endl;
#endif
    const auto r = stack.back();
    stack.pop_back();
    return r;
}

void MiVM::store(const uarch_t delta)
{
    const auto value = pop();
    stack[framePtr+delta] = value;
}

void MiVM::load(const uarch_t delta)
{
    const auto value = stack[framePtr+delta];
    push(value);
}

const std::vector<arch_t>& MiVM::getStack() const
{
    return stack;
}

#ifdef MIVM_DEBUG
void MiVM::dumpStack() const
{
    std::cout << '[';
    for (const auto& e : stack) {
        std::cout << (int)e << ", ";
    }
    std::cout << "]" << std::endl;
}
#endif

}
