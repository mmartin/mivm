#ifndef MIVM_H
#define MIVM_H

#include<string>
#include<vector>
#include<cstdint>

namespace MiVM {

//FIXME: this should not be compile time
typedef int8_t arch_t;
typedef uint8_t uarch_t;
const auto MAX_MEM = UINT8_MAX;

enum OPCode : arch_t {
#include "mivm.hpp.inc.tmp"
};

class MiVM {
public:
    //void load(const std::string& inputFile);
    void load(const std::initializer_list<arch_t>& programData);
    arch_t run();

    const std::vector<arch_t>& getStack() const;

private:
    void push(const arch_t value);
    arch_t pop();

    void store(const uarch_t delta);
    void load(const uarch_t delta);

#ifdef MIVM_DEBUG
    void dumpStack() const;
#endif

    std::vector<arch_t> stack, program;
    uarch_t framePtr, stackPtr;
};

}
#endif
