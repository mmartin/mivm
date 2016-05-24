#include<iostream>
#include"mivm.hpp"

//#include<boost/program_options.hpp>

//int main(int argc, const char* argv[])
int main()
{

    /*namespace po = boost::program_options;

    po::positional_options_description p;
    p.add("input-file", -1);

    po::options_description desc("Program options");
    desc.add_options()
        ("help", "help message")
        ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
            options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }*/

    std::cout << sizeof(MiVM::MiVM) << std::endl;

    MiVM::MiVM vm;
    vm.run();
}
