#ifndef MIVM_H
#define MIVM_H

#include<chrono>
#include<string>
#include<array>
#include<stack>
#include<vector>
#include<cstdint>

namespace MiVM {

// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
enum OPCode { CLS  = 0x00E0 // 0E0
            , JP   = 0x1000 // nnn addr
            , JPD  = 0xB000 // nnn V0, addr

            , RET  = 0x00EE // 0EE
            , CALL = 0x2000 // nnn addr

            , SE   = 0x3000 // xkk Vx, byte
            , SNE  = 0x4000 // xkk Vx, byte
            , SER  = 0x5000 // xy0 Vx, Vy
            , SNER = 0x9000 // xy0 Vx, Vy

            , LD   = 0x6000 // xkk Vx, byte
            , LDR  = 0x8000 // xy0 Vx, Vy
            , LDI  = 0xA000 // nnn I, addr
            , INCI = 0xF01E // x1E I, Vx

            , OR   = 0x8001 // xy1 Vx, Vy
            , AND  = 0x8002 // xy2 Vx, Vy
            , XOR  = 0x8003 // xy3 Vx, Vy
            , SHR  = 0x8006 // xy6 Vx
            , SHL  = 0x800E // xyE Vx
            , ADD  = 0x8004 // xy4 Vx, Vy
            , SUB  = 0x8005 // xy5 Vx, Vy
            , SUBN = 0x8007 // xy7 Vx, Vy
            , INC  = 0x7000 // xkk Vx, byte

            , SKP  = 0xE09E // x9E Vx
            , SNKP = 0xE0A1 // xA1 Vx
            , WKP  = 0xF00A // x0A Vx, K

            , GDT  = 0xF007 // x07 Vx, DT
            , SDT  = 0xF015 // x15 DT, Vx
            , SST  = 0xF018 // x18 ST, Vx

            , CHR  = 0xF029 // x29 F, Vx
            , RND  = 0xC000 // xkk Vx, byte
            , DRW  = 0xD000 // xyn Vx, Vy, nibble

            , LDID = 0xF033 // x33 B, Vx
            , LDIS = 0xF055 // x55 [I], Vx
            , LDIR = 0xF065 // x65 Vx, [I]
            , EXIT = 0x00FD
            };

enum class State { Inactive
                 , Ready
                 , Running
                 , DrawRequest
                 , WaitingKeyboard
                 , GetKeyboard
                 , Finished
                 };

template<typename T>
class Timer {
public:
    Timer (const uint16_t freq_ = 60)
        : value(0)
        , freq(freq_)
    {}

    void set(const uint16_t value_) {
        baseValue = value = value_;
        base = T::now();
    }

    uint16_t get() const {
        return value;
    }

    bool update() {
        if (value == 0) {
            return false;
        }

        const auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(T::now() - base).count() * freq / 1000;
        if (delta >= baseValue) {
            value = 0;
            return true;
        }
        else {
            value = baseValue - delta;
            return false;
        }
    }

private:
    uint16_t value, baseValue;
    const uint16_t freq;
    typename T::time_point base;
};

class MiVM {
public:
    MiVM() : state(State::Inactive) {}

    void load(const std::string& inputFile);
    void load(const std::initializer_list<uint16_t>& program);
    void reset(const bool soft = true);

    State run(const size_t cycles = 0);
    State setWaitingKeyboard(const uint16_t key);
    uint16_t getGetKeyboard() const;
    State setGetKeyboard(const bool pressed);
    State setDrawRequest();

#ifdef MIVM_DEBUG
    void dumpStack() const;
    void dumpRegisters() const;
    void dumpMemory() const;
    void dumpVideoMemory() const;
#endif

private:
    State execute(const OPCode opcode);


#ifdef BOOST_TEST_MODULE
public:
#endif
    std::array<uint8_t, 4096> memory;
    std::array<std::array<bool, 64>, 32> videoMemory;
    std::array<uint8_t, 16> regV;
    uint16_t regI, instrPtr;

    std::stack<uint16_t> stack;

    Timer<std::chrono::steady_clock> delayTimer, soundTimer;

    State state;
    OPCode lastOpcode;
    uint16_t tmpReg; // used between states

public:
    auto getState() const -> decltype(state);
    auto getVideoMemory() const -> const decltype(videoMemory)&;
    auto getSoundTimer() const -> decltype(soundTimer);

    bool getSound() const;
};

}

namespace std { string to_string(const MiVM::State& state); }
#endif
