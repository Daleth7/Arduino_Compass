#include "Serial_Port/Serial_Port.h"

#include "SFML/Graphics.hpp"

#include <memory>
#include <sstream>

#ifndef NDEBUG
    #include <iostream>
#endif

int main(){
    using namespace sf;

        // Port communication
    std::unique_ptr<RS_232::Serial_Port> port_ptr(RS_232::open_serial_port());

    auto connect = [&port_ptr](){
        port_ptr->close();
        for(unsigned int i(0); i < 5; ++i){
            if(port_ptr->open(i+1u, RS_232::Serial_Port::br_4800, 1u)){
#ifndef NDEBUG
                std::cout << "Successful connection to port #" << (i+1u) << '\n';
#endif
                break;
            }
#ifndef NDEBUG
            std::cerr << "Unsuccessful connection to port #" << (i+1u) << '\n';
#endif
        }
    };

        // Compass sprites
    Texture compassTexBg, compassTexNeedle;
    if(!compassTexBg.loadFromFile("Sprites/CompassBG.png"))
        return 0xBAD001;
    if(!compassTexNeedle.loadFromFile("Sprites/CompassNeedle.png"))
        return 0xBAD002;
    const Sprite compassBg(compassTexBg);
    Sprite compassNeedle(compassTexNeedle);
        compassNeedle.setOrigin(
            compassTexNeedle.getSize().x/2u,
            compassTexNeedle.getSize().y/2u
            );

        // Window
    RenderWindow screen(
        {compassTexBg.getSize().x, compassTexBg.getSize().y},
        "Compass Tracker"
        );
        screen.setFramerateLimit(30);

    compassNeedle.move(screen.getSize().x/2u, screen.getSize().y/2u);

        // Status text
    Font statusFont;
    if(!statusFont.loadFromFile("Fonts/arial.ttf"))
        return 0xBAD003;
    Text statusText("Loading...", statusFont, 15);
        statusText.setColor(Color::Black);
    Text angleText("0.0", statusFont, 15);
        angleText.move(0, compassTexNeedle.getSize().y-20);
        angleText.setColor(Color::Black);

    connect();

    bool focused(true);

    while(screen.isOpen()){
        Event event;
        while(screen.pollEvent(event)){
            switch(event.type){
                case Event::Closed:
                    screen.close();
                    break;
                case Event::LostFocus:
                    focused = false;
                    break;
                case Event::GainedFocus:
                    focused = true;
                    break;
                default:    break;
            }
        }

        if(!focused){
#ifndef NDEBUG
            std::cout << "Window not focused.\n";
#endif
            port_ptr->flush_input();
            continue;
        }

        if(Keyboard::isKeyPressed(Keyboard::Key::Space)){
            connect();
            port_ptr->flush_input();
        }
        if(Keyboard::isKeyPressed(Keyboard::Key::BackSpace)){
            port_ptr->close();
            port_ptr->flush_input();
        }

        if(port_ptr->is_connected())    port_ptr->clear_error();
        statusText.setString(
            port_ptr->fail()
                ? port_ptr->error().what()
                : port_ptr->is_connected()
                    ? "Connected"
                    : "Disconnected"
            );
        if(port_ptr->is_connected()){
            RS_232::Serial_Port::byte_type angle_bin;
            short s_angle_bin(angle_bin);
            if(s_angle_bin < 0) s_angle_bin += 256;
            const unsigned short us_angle_bin(s_angle_bin);
            const float new_ang(us_angle_bin*360.0f/256.0f);
            port_ptr->read(angle_bin);
            compassNeedle.setRotation(new_ang);
            std::stringstream ss;
                ss << new_ang;
            angleText.setString(ss.str());
#ifndef NDEBUG
            std::cout << "\tAngle: " << new_ang << '\n';
#endif
        }

        screen.clear(Color::White);
        screen.draw(compassBg);
        screen.draw(compassNeedle);
        screen.draw(statusText);
        screen.draw(angleText);
        screen.display();
    }

    port_ptr->flush_input(true);
    port_ptr->flush_output(true);
    port_ptr->close();

    return 0;
}