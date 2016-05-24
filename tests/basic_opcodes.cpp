#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE BASIC_OPCODES
#include<boost/test/unit_test.hpp>

#include<vector>

#include<mivm.hpp>


using namespace MiVM;

struct MiVMFixture {
    MiVM::MiVM vm;

    void checkStack(const std::vector<uint16_t>& s2) const {
        std::vector<uint16_t> s1;
        auto tmp = vm.getStack();
        while (!tmp.empty()) {
            s1.insert(s1.begin(), tmp.top());
            tmp.pop();
        }
        BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(s1), std::end(s1),
                                      std::begin(s2), std::end(s2));
    }
};

BOOST_FIXTURE_TEST_CASE(test_0, MiVMFixture)
{
    vm.load({});
}
