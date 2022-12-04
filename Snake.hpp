#pragma once
#include"Node.hpp"
#include<memory>
#include<iostream>
#include<cmath>
#include<random>

#define _SEGMENT_SIZE_ 24.f
#define _DEBUG_DRAW_ false

enum Direction{
    DIRECTION_UP,
    DIRECTION_RIGHT,
    DIRECTION_DOWN,
    DIRECTION_LEFT
};

struct Pivot{
    Pivot(sf::Vector2f vec, Direction dir){
        position = vec;
        nextDirection = dir;
    }

    Pivot* next = nullptr;
    sf::Vector2f position;
    Direction nextDirection;
};
class Pivot_List{
    public:
        Pivot_List(){
            head = nullptr;
            tail = nullptr;
        }
        void addNode(sf::Vector2f vec, Direction dir){
            Pivot* temp = new Pivot(vec,dir);

            if(head==nullptr){
                head = temp;
                tail = temp;
            }
            else{
                tail->next = temp;
                tail = temp;
            }
        }
        Pivot* getTail() const{
            return tail;
        }
    private:
        Pivot* head;
        Pivot* tail;    
};

class Segment : public Node{
    public:
        Segment(float x, float y, float vel = 2.0f, Pivot *piv = nullptr, Direction dir = DIRECTION_RIGHT){
            rs.setSize({_SEGMENT_SIZE_,_SEGMENT_SIZE_});
            rs.setFillColor(sf::Color::White);
            rs.setOutlineColor(sf::Color::Black);
            rs.setOutlineThickness(4.0f);
            rs.setPosition({x,y});
            
            velocity = vel;
            direction = dir;
            nextPivot = piv;
        }

        float getVelocity() const{
            return velocity;
        }
        Direction getDirection() const{
            return direction;
        }
        sf::Vector2f getPosition() const{
            return rs.getPosition();
        }
        sf::Vector2f getCenterPosition() const{
            sf::Vector2f pos = rs.getPosition();
            pos.x += rs.getSize().x/2.f;
            pos.y += rs.getSize().y/2.f;
            return pos;
        }
        Pivot* getNextPivot() const{
            return nextPivot;
        }

        void setColor(sf::Color c){
            rs.setFillColor(c);
        }
        void setVelocity(float v){
            velocity = v;
        }
    private:
        float velocity;
        Pivot* nextPivot = nullptr;

        Direction direction;
        float distance = 0.f;

        sf::RectangleShape rs;
        std::shared_ptr<Segment> prev_segment = nullptr;

        void onDraw(sf::RenderTarget& target) const override{
            target.draw(rs);

            if(_DEBUG_DRAW_){
                sf::CircleShape c;
                sf::Vector2f pos = getCenterPosition();
                c.setRadius(_SEGMENT_SIZE_/2.5f);
                c.setFillColor(sf::Color::Blue);

                pos.x -= c.getRadius();
                pos.y -= c.getRadius();
                c.setPosition(pos);
                target.draw(c);
            }
        }
        void onUpdate() override{
            float moveAmount = velocity;
            if(nextPivot->next == nullptr){
                move(moveAmount,direction);
            }else{
                while(moveAmount>0.f){
                    if(nextPivot->next == nullptr){
                        move(moveAmount,direction);
                        break;
                    }
                    float distance = distanceToPivot(nextPivot->next);
                    if(distance <= moveAmount){
                        rs.setPosition(nextPivot->next->position);
                        direction = nextPivot->next->nextDirection;
                        nextPivot = nextPivot->next;
                        moveAmount-=distance;
                    }
                    else{
                        move(moveAmount,direction);
                        break;
                    }
                }
            }
        }
        void move(float distance, Direction dir){
            switch (dir)
            {
            case DIRECTION_UP:
                rs.move({0.f,-distance});
                break;
            case DIRECTION_RIGHT:
                rs.move({distance,0.f});
                break;
            case DIRECTION_DOWN:
                rs.move({0.f,distance});
                break;
            case DIRECTION_LEFT:
                rs.move({-distance,0.f});
                break;
            }
        }
        float distanceToPivot(Pivot* p){
            switch(direction){
            case DIRECTION_UP:
                return rs.getPosition().y - p->position.y;
            case DIRECTION_RIGHT:
                return p->position.x - rs.getPosition().x;
            case DIRECTION_DOWN:
                return p->position.y - rs.getPosition().y;
            case DIRECTION_LEFT:
                return rs.getPosition().x - p->position.x;
            default:
                return 0.f;
            }
        }
};
class Cherry : public Node{
    public:
       Cherry(float x, float y, sf::Color c):Cherry(x,y){
           rs.setFillColor(c);
        }
        Cherry(float x, float y){
            rs.setFillColor(sf::Color::Red);
            rs.setOutlineColor(sf::Color::Yellow);
            rs.setOutlineThickness(2.f);
            rs.setSize({_SEGMENT_SIZE_,_SEGMENT_SIZE_});
            rs.setPosition({x,y});
        }
    
        sf::Vector2f getPosition(){
            return rs.getPosition();
        }
        sf::Vector2f getCenterPosition() const{
            sf::Vector2f pos = rs.getPosition();
            pos.x += rs.getSize().x/2.f;
            pos.y += rs.getSize().y/2.f;
            return pos;
        }
    private:
        sf::RectangleShape rs;

        void onDraw(sf::RenderTarget& target) const{
            target.draw(rs);
        }
        void onUpdate(){}
};

class Snake : public Node{
    public:
        Snake(float w, float h, sf::RenderWindow* rw):width(w),initial_width(w),initial_height(h),height(h),window(rw){
            updateWindow();
            setup();
        }
        int getScore(){
            return body.size()-2;
        }
        float getSpeed(){
            return velocity;
        }
        bool isGameOver(){
            return game_over;
        }

        void changeHeadDirection(Direction dir){
            sf::Vector2f vec = body[0].get()->getPosition();
            if(game_over == false)
            pivots.addNode(vec,dir);
        }
        void loadSound(sf::Sound* s,std::string str){
            sounds.insert({str,s});
        }
        void restart(){
            removeAll();
            body.clear();
            cherries.clear();

            velocity = initial_velocity;
            width = initial_width;
            height = initial_height;
            game_over = false;

            updateWindow();
            setup();
        }

    private:
        std::vector<std::shared_ptr<Segment>> body;
        std::vector<std::shared_ptr<Cherry>> cherries;
        Pivot_List pivots;

        std::map<std::string,sf::Sound*> sounds;
        sf::RenderWindow* window;
        
        float velocity = 1.0f;
        float initial_velocity = 1.0f;
        bool game_over = false;

        float width;
        float height;
        float initial_width;
        float initial_height;

        void addSegment(){
            Segment* tail = body.back().get();

            sf::Vector2f pos = tail->getPosition();
            Direction dir = tail->getDirection();
            float vel = tail->getVelocity();
            Pivot* piv = tail->getNextPivot();

            switch(dir){
                case DIRECTION_UP:pos.y+=_SEGMENT_SIZE_;break;
                case DIRECTION_RIGHT:pos.x-=_SEGMENT_SIZE_;break;
                case DIRECTION_DOWN:pos.y-=_SEGMENT_SIZE_;break;
                case DIRECTION_LEFT:pos.x+=_SEGMENT_SIZE_;break;
            }

            body.push_back(
                std::shared_ptr<Segment>(new Segment(pos.x,pos.y,vel,piv,dir))
            );
            addChildren(
                body.back().get()
            );
        }
        void generateCherry(int count = 1){
            float x,y;
            bool collision = false;
            for(int i=0; i<count; i++){
                do{
                    int temp_w = width;
                    int temp_h = height;
                x = (rand()%temp_w)*0.7f+(0.1f*width);
                y = (rand()%temp_h)*0.7f+(0.1f*height);

                collision = false;
                for(std::size_t i=0; i<body.size(); i++)
                    if(checkCollision({x+_SEGMENT_SIZE_/2.f,y+_SEGMENT_SIZE_/2.f},body.at(i).get()->getCenterPosition(),_SEGMENT_SIZE_))
                        collision = true;
                }while(collision);
                cherries.push_back(
                    std::shared_ptr<Cherry>(new Cherry(x,y))
                );
                addChildren(
                    cherries.back().get()
                );
            }
        }
        void speedUp(){
            float d = ((float)(rand()%200+50)/1000.f);
            velocity+=d;
            for(std::size_t i=0; i<body.size(); i++)
                body.at(i).get()->setVelocity(velocity);
        }
        void enlarge(){
            if(width+50.f > sf::VideoMode::getDesktopMode().width || height+30.f > sf::VideoMode::getDesktopMode().height)
                return;

            width+=24.f;
            height+=18.f;
            updateWindow();
        }
        void updateWindow(){
            window->setSize({width,height});
            window->setView(sf::View(sf::FloatRect(0.f,0.f,width,height)));
            window->setPosition({
                sf::VideoMode::getDesktopMode().width/2-window->getSize().x/2,
                sf::VideoMode::getDesktopMode().height/2-window->getSize().y/2
            });
        }
        void setup(){
            pivots.addNode({width/2,height/2},DIRECTION_RIGHT);
            body.push_back(
                std::shared_ptr<Segment>(new Segment(width/2,height/2,velocity,pivots.getTail()))
            );
            addChildren(
                body.back().get()
            );
            addSegment();
            srand(std::time(NULL));
            generateCherry(4);
        }
        void onDraw(sf::RenderTarget& target) const override{
        }
        void onUpdate() override{
            Segment* head = body.at(0).get();
            
            for(std::size_t i=1; i<body.size(); i++)
                if(checkCollision(head->getCenterPosition(),body.at(i).get()->getCenterPosition(),_SEGMENT_SIZE_/1.5f)){
                    gameOver();
                }else{
                    //
                }
            for(std::size_t i=0; i<cherries.size(); i++)
                if(checkCollision(head->getCenterPosition(),cherries.at(i).get()->getCenterPosition(),_SEGMENT_SIZE_/1.f)){
                    addSegment();
                    speedUp();
                    removeChildren(cherries.at(i).get());
                    cherries.erase(cherries.begin()+i);

                    if(cherries.size() == 0){
                        sounds["screen.wav"]->play();
                        generateCherry(rand()%3+2);
                        if(rand()%2)
                            enlarge();
                    }else{
                        sounds["eat.wav"]->play();
                    }
                    break;
                }

            sf::Vector2f pos = head->getPosition();
            if(pos.x < 0.f || pos.x > width || pos.y < 0.f || pos.y > height){
                gameOver();
            }
        }
        bool checkCollision(sf::Vector2f c1, sf::Vector2f c2, float radius){
            return distance(c1,c2)<radius;
        }
        float distance(sf::Vector2f v1,sf::Vector2f v2){
            return std::sqrt(
                std::pow(v1.x-v2.x,2.f)+std::pow(v1.y-v2.y,2.f)
            );
        }
        void gameOver(){
            game_over = true;
            //
        };
};

#undef _SEGMENT_SIZE