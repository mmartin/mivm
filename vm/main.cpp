#include<iostream>
#include<cmath>
#include<boost/program_options.hpp>

#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>

#include"mivm.hpp"

// TODO: not sure this is correct/maybe make this configurable?
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

int main(int argc, const char* argv[])
{
    int pixelSize;
    size_t sync; // TODO: maybe run VM in separate thread

    namespace po = boost::program_options;

    po::positional_options_description p;
    p.add("input-file", 1);

    po::options_description desc("Program options");
    desc.add_options()
        ("input-file", po::value<std::string>(), "input file")
        ("pixel-size", po::value<int>(&pixelSize)->default_value(10), "size of a pixel in pixels")
        ("sync", po::value<size_t>(&sync)->default_value(60), "synchronize after `n` VM cycles")
        ("help", "help message")
        ;

    po::variables_map pvm;
    try {
        po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), pvm);
        po::notify(pvm);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl << desc << std::endl;
        return -1;
    }

    if (pvm.count("help") || !pvm.count("input-file")) {
        std::cerr << desc << std::endl;
        return 1;
    }

    MiVM::MiVM vm;

    try {
        vm.load(pvm["input-file"].as<std::string>());
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    sf::RenderWindow window(sf::VideoMode(64*pixelSize, 32*pixelSize), "MiVM!");
    window.setFramerateLimit(120);

    const unsigned int sampleRate = 44100, soundFreq = 666;
    sf::SoundBuffer soundBuffer;
    std::vector<sf::Int16> soundSamples(sampleRate);
    std::generate(std::begin(soundSamples), std::end(soundSamples),
                  [sampleRate, soundFreq]{
                      static unsigned int n = 0;
                      return 32000 * std::sin((double(n++) * soundFreq / sampleRate) * M_PI * 2);
                  });
    soundBuffer.loadFromSamples(&soundSamples[0], soundSamples.size(), 1, sampleRate);
    sf::Sound sound;
    sound.setBuffer(soundBuffer);
    sound.setLoop(true);

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
                state = vm.run(sync);
                break;

            case MiVM::State::DrawRequest:
                state = vm.setDrawRequest();
                break;

            case MiVM::State::SoundStart:
                sound.play();
                state = vm.setSoundStart();
                break;

            case MiVM::State::SoundStop:
                sound.stop();
                state = vm.setSoundStop();
                break;

            case MiVM::State::GetKeyboard:
                state = vm.setGetKeyboard(sf::Keyboard::isKeyPressed(keys[vm.getGetKeyboard()]));
                break;

            default: break;
        }

        window.clear(sf::Color::Black);

        const auto& videoMemory = vm.getVideoMemory();
        for (size_t y = 0; y < 32; ++y) {
            for (size_t x = 0; x < 64; ++x) {
                if (!videoMemory[y][x]) {
                    continue;
                }
                sf::VertexArray p(sf::Quads, 4);
                p[0].position = sf::Vector2f(x*pixelSize, y*pixelSize);
                p[1].position = sf::Vector2f((x+1)*pixelSize, y*pixelSize);
                p[2].position = sf::Vector2f((x+1)*pixelSize, (y+1)*pixelSize);
                p[3].position = sf::Vector2f(x*pixelSize, (y+1)*pixelSize);
                window.draw(p);
            }
        }

        window.display();
    }
}
