#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE BASIC_OPCODES
#include<boost/test/unit_test.hpp>

#include<vector>

#include<mivm.hpp>

using OPC = MiVM::OPCode;

struct MiVMFixture {
    MiVM::MiVM vm;

    void checkResult(const std::initializer_list<uint16_t>& stack,
                     const MiVM::State state = MiVM::State::Finished) const
    {
        BOOST_CHECK_EQUAL((int)vm.getState(), (int)state);
        const std::vector<uint16_t> s1(&vm.stack.top() - vm.stack.size() + 1, &vm.stack.top() + 1);

        BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(s1), std::end(s1),
                                      std::begin(stack), std::end(stack));
    }
};

#define BOOST_CHECK_EQUAL_U(a, b) BOOST_CHECK_EQUAL((int)a, (int)b)

BOOST_FIXTURE_TEST_CASE(test_jump, MiVMFixture)
{
    vm.load({ OPC::JP | (0x200 + 4)
            , OPC::LD | 0x006
            , OPC::LD | 0x013
            , OPC::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    checkResult({});

    vm.load({ OPC::LD | 0x006
            , OPC::JPD | 0x200
            , OPC::LD | 0x006
            , OPC::LD | 0x013
            , OPC::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    checkResult({});
}

BOOST_FIXTURE_TEST_CASE(test_subr, MiVMFixture)
{
    vm.load({ OPC::CALL | (0x200 + 4)
            , OPC::LD | 0x006
            , OPC::LD | 0x013
            , OPC::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    checkResult({ 0x200 });

    vm.load({ OPC::CALL | (0x200 + 6)
            , OPC::LD | 0x106
            , OPC::EXIT
            , OPC::LD | 0x013
            , OPC::RET
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x6);
    checkResult({});
}

BOOST_FIXTURE_TEST_CASE(test_skip, MiVMFixture)
{
    vm.load({ OPC::LD | 0x013
            , OPC::SE | 0x013
            , OPC::LD | 0x106
            , OPC::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x0);
    checkResult({});

    vm.load({ OPC::LD | 0x012
            , OPC::SE | 0x013
            , OPC::LD | 0x106
            , OPC::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x12);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x6);
    checkResult({});

    vm.load({ OPC::LD | 0x013
            , OPC::SNE | 0x013
            , OPC::LD | 0x106
            , OPC::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x6);
    checkResult({});

    vm.load({ OPC::LD | 0x012
            , OPC::SNE | 0x013
            , OPC::LD | 0x106
            , OPC::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x12);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x0);
    checkResult({});

    vm.load({ OPC::LD | 0x013
            , OPC::LD | 0x113
            , OPC::SER | 0x010
            , OPC::LD | 0x206
            , OPC::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[2], 0x0);
    checkResult({});

    vm.load({ OPC::LD | 0x012
            , OPC::LD | 0x113
            , OPC::SER | 0x010
            , OPC::LD | 0x206
            , OPC::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x12);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[2], 0x6);
    checkResult({});

    vm.load({ OPC::LD | 0x013
            , OPC::LD | 0x113
            , OPC::SNER | 0x010
            , OPC::LD | 0x206
            , OPC::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[2], 0x6);
    checkResult({});

    vm.load({ OPC::LD | 0x012
            , OPC::LD | 0x113
            , OPC::SNER | 0x010
            , OPC::LD | 0x206
            , OPC::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x12);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[2], 0x0);
    checkResult({});
}

BOOST_FIXTURE_TEST_CASE(test_load, MiVMFixture)
{
    vm.load({ OPC::LD | 0x013
            , OPC::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    checkResult({});

    vm.load({ OPC::LD | 0x113
            , OPC::LDR | 0x010
            , OPC::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x13);
    checkResult({});

    vm.load({ OPC::LDI | 0x300
            , OPC::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regI, 0x300);
    checkResult({});

    vm.load({ OPC::LDI | 0x300
            , OPC::LD | 0x013
            , OPC::INCI
            , OPC::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regI, 0x313);
    checkResult({});



    const uint8_t mem[16] = { 0x13, 0x0, 0x1, 0x0
                            , 0x0,  0x0, 0x0, 0x0
                            , 0x0,  0x0, 0x0, 0x0
                            , 0x0,  0x0, 0x0, 0xF
                            };

    vm.load({ OPC::LD | 0x013
            , OPC::LD | 0x201
            , OPC::LD | 0xF0F
            , OPC::LDI | 0x300
            , OPC::LDIS | 0xF00
            , OPC::EXIT
            });
    vm.run();
    checkResult({});

    BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(vm.regV), std::end(vm.regV),
                                  std::begin(mem), std::end(mem));

    BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(vm.memory) + 0x300 , std::begin(vm.memory) + 0x30F + 1,
                                  std::begin(mem), std::end(mem));

    BOOST_CHECK_EQUAL_U(vm.regI, 0x300);

    vm.load({ OPC::LDI | 0x300
            , OPC::LDIR | 0xF00
            , OPC::EXIT
            });
    std::copy(std::begin(mem), std::end(mem), std::begin(vm.memory) + 0x300);
    vm.run(); // TODO this will break, if run() clears memory!
    checkResult({});

    BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(vm.regV), std::end(vm.regV),
                                  std::begin(mem), std::end(mem));

    BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(vm.memory) + 0x300 , std::begin(vm.memory) + 0x30F + 1,
                                  std::begin(mem), std::end(mem));

    BOOST_CHECK_EQUAL_U(vm.regI, 0x300);

    vm.load({ OPC::LD | 0x0FE
            , OPC::LDI | 0x300
            , OPC::LDID
            , OPC::EXIT
            });
    vm.run();

    BOOST_CHECK_EQUAL_U(vm.memory[0x300], 0x2);
    BOOST_CHECK_EQUAL_U(vm.memory[0x301], 0x5);
    BOOST_CHECK_EQUAL_U(vm.memory[0x302], 0x4);
    checkResult({});
}

BOOST_FIXTURE_TEST_CASE(test_math, MiVMFixture)
{
    vm.load({ OPC::LD | 0x01F
            , OPC::LD | 0x1F0
            , OPC::OR | 0x010
            , OPC::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0xFF);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0xF0);
    checkResult({});

    vm.load({ OPC::LD | 0x01F
            , OPC::LD | 0x1F0
            , OPC::AND | 0x010
            , OPC::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x10);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0xF0);
    checkResult({});

    vm.load({ OPC::LD | 0x01F
            , OPC::LD | 0x1F0
            , OPC::XOR | 0x010
            , OPC::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0xEF);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0xF0);
    checkResult({});

    vm.load({ OPC::LD | 0x01F
            , OPC::SHL | 0x000
            , OPC::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x3E);
    checkResult({});

    vm.load({ OPC::LD | 0x01F
            , OPC::SHR | 0x000
            , OPC::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0xF);
    checkResult({});

    vm.load({ OPC::LD | 0x010
            , OPC::LD | 0x103
            , OPC::ADD | 0x010
            , OPC::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x3);
    BOOST_CHECK_EQUAL_U(vm.regV[15], 0);
    checkResult({});

    vm.load({ OPC::LD | 0x0F7
            , OPC::LD | 0x10F
            , OPC::ADD | 0x010
            , OPC::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x6);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0xF);
    BOOST_CHECK_EQUAL_U(vm.regV[15], 1);
    checkResult({});

    vm.load({ OPC::LD | 0x013
            , OPC::INC | 0x013
            , OPC::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x26);
    checkResult({});


/*
    TODO: find out correct truncation for negative values
    SUB  = 0x8005 // xy5 Vx, Vy
    SUBN = 0x8007 // xy7 Vx, Vy
*/
}

BOOST_FIXTURE_TEST_CASE(test_misc, MiVMFixture)
{
    srand(666); // rand() == 0xBF // TODO: better solution?
    vm.load({ OPC::RND | 0x0FB
            , OPC::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0xBB);
    checkResult({});
}

BOOST_FIXTURE_TEST_CASE(test_keyboard, MiVMFixture)
{
    vm.load({ OPC::WKP
            , OPC::EXIT
            });

    vm.run();
    BOOST_REQUIRE_EQUAL((int)vm.getState(), (int)MiVM::State::WaitingKeyboard);
    vm.setWaitingKeyboard(0x4);
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x4);
    checkResult({});


    vm.load({ OPC::LD | 0x013
            , OPC::LD | 0x104
            , OPC::SKP | 0x100
            , OPC::LD | 0x006
            , OPC::EXIT
            });
    vm.run();
    BOOST_REQUIRE_EQUAL((int)vm.getState(), (int)MiVM::State::GetKeyboard);
    BOOST_REQUIRE_EQUAL((int)vm.getGetKeyboard(), 0x4);
    vm.setGetKeyboard(true);
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x4);
    checkResult({});

    vm.reset();
    vm.run();
    BOOST_REQUIRE_EQUAL((int)vm.getState(), (int)MiVM::State::GetKeyboard);
    BOOST_REQUIRE_EQUAL((int)vm.getGetKeyboard(), 0x4);
    vm.setGetKeyboard(false);
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x6);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x4);
    checkResult({});

    vm.load({ OPC::LD | 0x013
            , OPC::LD | 0x104
            , OPC::SNKP | 0x100
            , OPC::LD | 0x006
            , OPC::EXIT
            });
    vm.run();
    BOOST_REQUIRE_EQUAL((int)vm.getState(), (int)MiVM::State::GetKeyboard);
    BOOST_REQUIRE_EQUAL((int)vm.getGetKeyboard(), 0x4);
    vm.setGetKeyboard(false);
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x4);
    checkResult({});

    vm.reset();
    vm.run();
    BOOST_REQUIRE_EQUAL((int)vm.getState(), (int)MiVM::State::GetKeyboard);
    BOOST_REQUIRE_EQUAL((int)vm.getGetKeyboard(), 0x4);
    vm.setGetKeyboard(true);
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x6);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x4);
    checkResult({});
}

BOOST_FIXTURE_TEST_CASE(test_timers, MiVMFixture)
{
    vm.load({ OPC::LD | 0x013
            , OPC::SDT
            , OPC::GDT | 0x100
            , OPC::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x13);
    BOOST_CHECK_EQUAL_U(vm.delayTimer.get(), 0x13);
    checkResult({});

    vm.load({ OPC::LD | 0x013
            , OPC::SST
            , OPC::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL_U(vm.soundTimer.get(), 0x13);
    checkResult({});

    // TODO: tests for timers itself
}

BOOST_FIXTURE_TEST_CASE(test_video, MiVMFixture)
{
    //vm.load({ OPC::LD | 0x03F
            //, OPC::LD | 0x11F
            //, OPC::DRW | 0x015
            //, OPC::EXIT
            //});
    //vm.run();

    vm.load({ OPC::DRW | 0x5
            , OPC::DRW | 0x5
            , OPC::EXIT
            });
    vm.run();
    checkResult({}, MiVM::State::DrawRequest);
    vm.setDrawRequest();
    vm.run();
    checkResult({}, MiVM::State::DrawRequest);
    vm.setDrawRequest();
    vm.run();
    checkResult({});
    BOOST_CHECK_EQUAL_U(vm.regV[0xF], 1);

    // TODO: write real tests
}
