#include<iostream>

#include<SFML/Graphics.hpp>

#include"mivm.hpp"

//#include<boost/program_options.hpp>



// TODO: not sure this is correct
static const sf::Keyboard::Key keys[] = { sf::Keyboard::Key::X
                                        , sf::Keyboard::Key::Num1
                                        , sf::Keyboard::Key::Num2
                                        , sf::Keyboard::Key::Num3
                                        , sf::Keyboard::Key::Q
                                        , sf::Keyboard::Key::W
                                        , sf::Keyboard::Key::E
                                        , sf::Keyboard::Key::A
                                        , sf::Keyboard::Key::S
                                        , sf::Keyboard::Key::D
                                        , sf::Keyboard::Key::Z
                                        , sf::Keyboard::Key::C
                                        , sf::Keyboard::Key::Num4
                                        , sf::Keyboard::Key::R
                                        , sf::Keyboard::Key::F
                                        , sf::Keyboard::Key::V
                                        };

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

    sf::RenderWindow window(sf::VideoMode(200, 200), "MiVM!");

    MiVM::MiVM vm;

    vm.load({ MiVM::OPCode::WKP, MiVM::OPCode::EXIT });

    auto state = vm.getState();

    while (state != MiVM::State::Finished && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed
                 &&  state == MiVM::State::WaitingKeyboard) {
                const auto it = std::find(std::begin(keys), std::end(keys), event.key.code);
                if (it != std::end(keys)) {
                    state = vm.setWaitingKeyboard(std::distance(std::begin(keys), it));
                }
            }
        }

        switch (state) {
            case MiVM::State::Ready:
            case MiVM::State::Running:
                state = vm.run(true);
                break;

            case MiVM::State::GetKeyboard:
                state = vm.setGetKeyboard(sf::Keyboard::isKeyPressed(keys[vm.getGetKeyboard()]));
                break;

            default: break;
        }

        window.clear();
        window.display();
    }
}
