#include<SFML/Audio.hpp>
#include<SFML/Graphics.hpp>
#include<iomanip>
#include"Snake.hpp"
int main()
{

    sf::RenderWindow window(sf::VideoMode(640,480),"Snek",sf::Style::Close | sf::Style::Titlebar);
    window.setFramerateLimit(60);
    window.setPosition({
        sf::VideoMode::getDesktopMode().width/2-window.getSize().x/2,
        sf::VideoMode::getDesktopMode().height/2-window.getSize().y/2
    });
    Snake snake(640.f,480.f,&window);
    sf::Event event;


    sf::SoundBuffer sound_buffer;
    sf::SoundBuffer sound_buffer_b;
    sound_buffer.loadFromFile("eat.wav");
    sound_buffer_b.loadFromFile("screen.wav");
    sf::Sound sound;
    sf::Sound sound_b;
    sound.setBuffer(sound_buffer);
    sound_b.setBuffer(sound_buffer_b);
    snake.loadSound(&sound,"eat.wav");
    snake.loadSound(&sound_b,"screen.wav");

    sf::Font f;
    f.loadFromFile("./fonts/joystix monospace.ttf");

    sf::Text t("",f,24);
    sf::Text t2("",f,24);

    while(window.isOpen()){
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
            
            if(event.type == sf::Event::KeyPressed)
                switch(event.key.code){
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::Up:
                        snake.changeHeadDirection(DIRECTION_UP);
                        break;
                    case sf::Keyboard::Right:
                        snake.changeHeadDirection(DIRECTION_RIGHT);
                        break;
                    case sf::Keyboard::Down:
                        snake.changeHeadDirection(DIRECTION_DOWN);
                        break;
                    case sf::Keyboard::Left:
                        snake.changeHeadDirection(DIRECTION_LEFT);
                        break;
                    case sf::Keyboard::R:
                        snake.restart();
                        break;
                }
        }
        
        window.clear(sf::Color::Black);

        snake.update();
        snake.draw(window);

        t.setString(
            snake.isGameOver()?
                "Game Over, Score "+std::to_string(snake.getScore()):
                "Score "+std::to_string(snake.getScore())
        );
        std::stringstream str;
        str << "Speed " << std::fixed << std::setprecision(2) << snake.getSpeed(); 
        t2.setString(
            str.str()
        );
        t2.setPosition(
            {window.getSize().x - t2.getGlobalBounds().width,0.f}
        );
        window.draw(t);
        window.draw(t2);

        window.display();
    }
    return 0;
}