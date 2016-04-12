#include<stdexcept>
#include<string>

#include<mivm.hpp>

int main() {
    MiVM::MiVM vm;
    vm.load({ MiVM::OPCode::PUSH, 13, MiVM::OPCode::HALT });
    const auto r = vm.run();
    if (r != 13) {
        throw std::logic_error("test fail: " + std::to_string(r));
    }
}
