#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE BASIC_OPCODES
#include<boost/test/unit_test.hpp>

#include<mivm.hpp>

using namespace MiVM;

struct MiVMFixture {
    MiVM::MiVM vm;

    void checkStack(const std::vector<arch_t>& s2) const {
        const auto s1 = vm.getStack();
        BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(s1), std::end(s1),
                                      std::begin(s2), std::end(s2));
    }
};

BOOST_FIXTURE_TEST_CASE(run, MiVMFixture)
{
    BOOST_CHECK_THROW(vm.run(), std::runtime_error);
}

BOOST_FIXTURE_TEST_CASE(opc_halt, MiVMFixture)
{
    vm.load({ OPCode::PUSH, 0, OPCode::HALT });
    BOOST_CHECK_EQUAL(vm.run(), 0);
    checkStack({});

    vm.load({ OPCode::HALT });
    BOOST_CHECK_THROW(vm.run(), std::underflow_error);
}

BOOST_FIXTURE_TEST_CASE(opc_push, MiVMFixture)
{
    vm.load({ OPCode::PUSH, 13, OPCode::HALT });
    BOOST_CHECK_EQUAL(vm.run(), 13);
    checkStack({});

    vm.load({OPCode::PUSH, 1,  OPCode::PUSH, 2, OPCode::HALT });
    BOOST_CHECK_EQUAL(vm.run(), 2);
    checkStack({ 1 });
}

BOOST_FIXTURE_TEST_CASE(opc_inc_dev, MiVMFixture)
{
    vm.load({ OPCode::PUSH, 13, OPCode::INC, OPCode::HALT });
    BOOST_CHECK_EQUAL(vm.run(), 14);
    checkStack({});

    vm.load({ OPCode::PUSH, 13, OPCode::DEC, OPCode::HALT });
    BOOST_CHECK_EQUAL(vm.run(), 12);
    checkStack({});
}

BOOST_FIXTURE_TEST_CASE(opc_jmp, MiVMFixture)
{
    vm.load({ OPCode::JMP, 3, -13, OPCode::PUSH, 1, OPCode::HALT });
    BOOST_CHECK_EQUAL(vm.run(), 1);
    checkStack({});

    vm.load({ OPCode::PUSH, 0, OPCode::JMPZ, 6, OPCode::PUSH, 1, OPCode::PUSH, 2, OPCode::HALT });
    BOOST_CHECK_EQUAL(vm.run(), 2);
    checkStack({});

    vm.load({ OPCode::PUSH, 1, OPCode::JMPZ, 6, OPCode::PUSH, 1, OPCode::PUSH, 2, OPCode::HALT });
    BOOST_CHECK_EQUAL(vm.run(), 2);
    checkStack({ 1 });

    vm.load({ OPCode::PUSH, 1, OPCode::JMPNZ, 6, OPCode::PUSH, 1, OPCode::PUSH, 2, OPCode::HALT });
    BOOST_CHECK_EQUAL(vm.run(), 2);
    checkStack({});

    vm.load({ OPCode::PUSH, 0, OPCode::JMPNZ, 6, OPCode::PUSH, 1, OPCode::PUSH, 2, OPCode::HALT });
    BOOST_CHECK_EQUAL(vm.run(), 2);
    checkStack({ 1 });
}

BOOST_FIXTURE_TEST_CASE(opc_load_store, MiVMFixture)
{
    vm.load({ OPCode::PUSH, 1, OPCode::PUSH, 2
            , OPCode::LOAD, 0, OPCode::STORE, 1
            , OPCode::HALT
            });
    BOOST_CHECK_EQUAL(vm.run(), 1);
    checkStack({ 1 });
}

BOOST_FIXTURE_TEST_CASE(opc_call_ret, MiVMFixture)
{
    vm.load({ OPCode::CALL, 3, OPCode::HALT
            , OPCode::PUSH, 1, OPCode::PUSH, 2, OPCode::ADD, OPCode::RET, 0
            });
    BOOST_CHECK_EQUAL(vm.run(), 3);
    checkStack({});

    vm.load({ OPCode::PUSH, 1, OPCode::CALL, 5, OPCode::HALT
            , OPCode::LOAD, -3, OPCode::PUSH, 2, OPCode::ADD, OPCode::RET, 1
            });
    BOOST_CHECK_EQUAL(vm.run(), 3);
    checkStack({});

    vm.load({ OPCode::PUSH, 13, OPCode::PUSH, 3, OPCode::CALL, 7, OPCode::HALT
            , OPCode::LOAD, -3, OPCode::LOAD, -3
            , OPCode::JMPZ, 16, OPCode::DEC, OPCode::CALL, 7, OPCode::RET, 1
            });
    BOOST_CHECK_EQUAL(vm.run(), 0);
    checkStack({13});
}

BOOST_FIXTURE_TEST_CASE(stack_overflow, MiVMFixture)
{
    /*if (sizeof(arch_t) <= 2) {
        vm.load({ OPCode::PUSH, 0, OPCode::JMP, 0 });
        BOOST_CHECK_THROW(vm.run(), std::overflow_error);
    }*/
}

BOOST_FIXTURE_TEST_CASE(simple_loop, MiVMFixture)
{
    vm.load({ OPCode::PUSH, 0, OPCode::PUSH, 0
            , OPCode::PUSH, 10, OPCode::LOAD, 1, OPCode::LT, OPCode::JMPZ, 25
            , OPCode::LOAD, 0, OPCode::LOAD, 1, OPCode::ADD, OPCode::STORE, 0
            , OPCode::LOAD, 1, OPCode::INC, OPCode::STORE, 1, OPCode::JMP, 4
            , OPCode::POP, OPCode::HALT
            });
    BOOST_CHECK_EQUAL(vm.run(), 45);
}
