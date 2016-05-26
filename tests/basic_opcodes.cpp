#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE BASIC_OPCODES
#include<boost/test/unit_test.hpp>

#include<vector>

#include<mivm.hpp>

using namespace MiVM;

struct MiVMFixture {
    MiVM::MiVM vm;

    MiVMFixture()
        : vm(0, 0)
    {}

    void checkStack(const std::initializer_list<uint16_t>& s2) const {
        const std::vector<uint16_t> s1(&vm.stack.top() - vm.stack.size() + 1, &vm.stack.top() + 1);

        BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(s1), std::end(s1),
                                      std::begin(s2), std::end(s2));
    }
};

#define BOOST_CHECK_EQUAL_U(a, b) BOOST_CHECK_EQUAL((int)a, (int)b)

BOOST_FIXTURE_TEST_CASE(test_jump, MiVMFixture)
{
    vm.load({ OPCode::JP | (0x200 + 4)
            , OPCode::LD | 0x006
            , OPCode::LD | 0x013
            , OPCode::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x13);
    checkStack({});

    vm.load({ OPCode::LD | 0x006
            , OPCode::JPD | 0x200
            , OPCode::LD | 0x006
            , OPCode::LD | 0x013
            , OPCode::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x13);
    checkStack({});
}

BOOST_FIXTURE_TEST_CASE(test_subr, MiVMFixture)
{
    vm.load({ OPCode::CALL | (0x200 + 4)
            , OPCode::LD | 0x006
            , OPCode::LD | 0x013
            , OPCode::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x13);
    checkStack({ 0x200 });

    vm.load({ OPCode::CALL | (0x200 + 6)
            , OPCode::LD | 0x106
            , OPCode::EXIT
            , OPCode::LD | 0x013
            , OPCode::RET
            });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL(vm.regV[1], 0x6);
    checkStack({});
}

BOOST_FIXTURE_TEST_CASE(test_skip, MiVMFixture)
{
    vm.load({ OPCode::LD | 0x013
            , OPCode::SE | 0x013
            , OPCode::LD | 0x106
            , OPCode::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL(vm.regV[1], 0x0);
    checkStack({});

    vm.load({ OPCode::LD | 0x012
            , OPCode::SE | 0x013
            , OPCode::LD | 0x106
            , OPCode::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x12);
    BOOST_CHECK_EQUAL(vm.regV[1], 0x6);
    checkStack({});

    vm.load({ OPCode::LD | 0x013
            , OPCode::SNE | 0x013
            , OPCode::LD | 0x106
            , OPCode::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL(vm.regV[1], 0x6);
    checkStack({});

    vm.load({ OPCode::LD | 0x012
            , OPCode::SNE | 0x013
            , OPCode::LD | 0x106
            , OPCode::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x12);
    BOOST_CHECK_EQUAL(vm.regV[1], 0x0);
    checkStack({});

    vm.load({ OPCode::LD | 0x013
            , OPCode::LD | 0x113
            , OPCode::SER | 0x010
            , OPCode::LD | 0x206
            , OPCode::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL(vm.regV[1], 0x13);
    BOOST_CHECK_EQUAL(vm.regV[2], 0x0);
    checkStack({});

    vm.load({ OPCode::LD | 0x012
            , OPCode::LD | 0x113
            , OPCode::SER | 0x010
            , OPCode::LD | 0x206
            , OPCode::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x12);
    BOOST_CHECK_EQUAL(vm.regV[1], 0x13);
    BOOST_CHECK_EQUAL(vm.regV[2], 0x6);
    checkStack({});

    vm.load({ OPCode::LD | 0x013
            , OPCode::LD | 0x113
            , OPCode::SNER | 0x010
            , OPCode::LD | 0x206
            , OPCode::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL(vm.regV[1], 0x13);
    BOOST_CHECK_EQUAL(vm.regV[2], 0x6);
    checkStack({});

    vm.load({ OPCode::LD | 0x012
            , OPCode::LD | 0x113
            , OPCode::SNER | 0x010
            , OPCode::LD | 0x206
            , OPCode::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x12);
    BOOST_CHECK_EQUAL(vm.regV[1], 0x13);
    BOOST_CHECK_EQUAL(vm.regV[2], 0x0);
    checkStack({});
}

BOOST_FIXTURE_TEST_CASE(test_load, MiVMFixture)
{
    vm.load({ OPCode::LD | 0x013
            , OPCode::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x13);
    checkStack({});

    vm.load({ OPCode::LD | 0x113
            , OPCode::LDR | 0x010
            , OPCode::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL(vm.regV[1], 0x13);
    checkStack({});

    vm.load({ OPCode::LDI | 0x123
            , OPCode::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regI, 0x123);
    checkStack({});
}

BOOST_FIXTURE_TEST_CASE(test_math, MiVMFixture)
{
    vm.load({ OPCode::LD | 0x01F
            , OPCode::LD | 0x1F0
            , OPCode::OR | 0x010
            , OPCode::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0xFF);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0xF0);
    checkStack({});

    vm.load({ OPCode::LD | 0x01F
            , OPCode::LD | 0x1F0
            , OPCode::AND | 0x010
            , OPCode::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x10);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0xF0);
    checkStack({});

    vm.load({ OPCode::LD | 0x01F
            , OPCode::LD | 0x1F0
            , OPCode::XOR | 0x010
            , OPCode::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0xEF);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0xF0);
    checkStack({});

    vm.load({ OPCode::LD | 0x01F
            , OPCode::SHL | 0x000
            , OPCode::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x3E);
    checkStack({});

    vm.load({ OPCode::LD | 0x01F
            , OPCode::SHR | 0x000
            , OPCode::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0xF);
    checkStack({});

    vm.load({ OPCode::LD | 0x010
            , OPCode::LD | 0x103
            , OPCode::ADD | 0x010
            , OPCode::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x13);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0x3);
    BOOST_CHECK_EQUAL_U(vm.regV[15], 0);
    checkStack({});

    vm.load({ OPCode::LD | 0x0F7
            , OPCode::LD | 0x10F
            , OPCode::ADD | 0x010
            , OPCode::EXIT
            });
    vm.run();
    BOOST_CHECK_EQUAL_U(vm.regV[0], 0x6);
    BOOST_CHECK_EQUAL_U(vm.regV[1], 0xF);
    BOOST_CHECK_EQUAL_U(vm.regV[15], 1);
    checkStack({});

    vm.load({ OPCode::LD | 0x013
            , OPCode::INC | 0x013
            , OPCode::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0x26);
    checkStack({});


/*
    TODO: find out correct truncation for negative values
    SUB  = 0x8005 // xy5 Vx, Vy
    SUBN = 0x8007 // xy7 Vx, Vy
*/
}

BOOST_FIXTURE_TEST_CASE(test_misc, MiVMFixture)
{
    srand(666); // rand() == 0xBF // TODO: better solution?
    vm.load({ OPCode::RND | 0x0FB
            , OPCode::EXIT });
    vm.run();
    BOOST_CHECK_EQUAL(vm.regV[0], 0xBB);
    checkStack({});
}
