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

    framePtr = stackPtr = 0;
    stack.resize(MAX_MEM);

    uarch_t instructionPtr = 0, argPtr = 0;
    //arch_t tmp = 0;

    while (true) {
        //TODO: this looks stupid

#define OPC_ARG(i) program[argPtr+i]

#ifdef MIVM_DEBUG
#define OPC(work, ip) \
        case OPC_CODE: \
            std::cout << std::endl << "proc (i=" << (int)instructionPtr \
                                   << ", f="     << (int)framePtr \
                                   << ", s="     << (int)stackPtr \
                                   << "): "      << OPC_NAME; \
            for (uarch_t i = 1; i <= OPC_ARGC; ++i) { std::cout << " " << (int)OPC_ARG(i); } \
            std::cout << std::endl; \
            work; instructionPtr = ip; \
            dumpStack(); break;
#else
#define OPC(work, ip) \
        case OPC_CODE: work; instructionPtr = ip; break;
#endif

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

#define OPC_CALL OPC(push(instructionPtr+2); push(framePtr); framePtr = stackPtr, OPC_ARG(1))
#define OPC_RET  OPC(stack[framePtr] = pop(); stackPtr = framePtr; framePtr = pop(), \
                     pop(); stackPtr -= OPC_ARG(1); push(stack[stackPtr + 2 + OPC_ARG(1)]))

#define OPC_HALT OPC(return pop(), 0)

        argPtr = instructionPtr;

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
#ifdef MIVM_DEBUG
    std::cout << "  push@" << (int)stackPtr << ": " << (int)value << std::endl;
#endif

#ifndef MIVM_NO_OVERFLOW_CHECK
    if (stackPtr == MAX_MEM) {
        throw std::overflow_error("stack overflow");
    }
#endif

    stack[stackPtr] = value;

    ++stackPtr;
}

arch_t MiVM::pop()
{
#ifdef MIVM_DEBUG
    std::cout << "  pop@" << (int)stackPtr-1 << ": ";
#endif

#ifndef MIVM_NO_UNDERFLOW_CHECK
    if (stackPtr == 0) {
        throw std::underflow_error("stack underflow");
    }
#endif

    --stackPtr;

#ifdef MIVM_DEBUG
    std::cout << (int)stack[stackPtr] << std::endl;
#endif

    return stack[stackPtr];
}

void MiVM::store(const arch_t delta)
{
    const auto value = pop();
    stack[framePtr+delta] = value;
}

void MiVM::load(const arch_t delta)
{
    const auto value = stack[framePtr+delta];
    push(value);
}

#ifdef MIVM_DEBUG
void MiVM::dumpStack() const
{
    std::cout << '[';
    for (auto i = 0; i < stackPtr; ++i) {
        std::cout << (int)stack[i] << ", ";
    }
    std::cout << "]" << std::endl;
}
#endif

}
