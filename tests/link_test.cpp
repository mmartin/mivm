#include<stdexcept>
#include<string>

#include<mivm.hpp>

int main() {
    MiVM::MiVM vm(0, 0);
    vm.load({ MiVM::OPCode::EXIT });
    vm.run();
}
