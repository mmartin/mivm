#include<stdexcept>
#include<string>

#include<mivm.hpp>

int main() {
    MiVM::MiVM vm;
    vm.load({ MiVM::OPCode::EXIT });
    vm.run();
}
