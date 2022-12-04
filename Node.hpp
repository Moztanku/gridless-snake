#pragma once
#include<SFML/Graphics.hpp>
#include<algorithm>

class Node{
    public:
        void draw(sf::RenderTarget& target) const{
            onDraw(target);

            for(std::size_t i=0; i<m_children.size(); i++)
                m_children[i]->draw(target);
        }

        void update(){
            onUpdate();

            for(std::size_t i=0; i<m_children.size(); i++)
                m_children[i]->onUpdate();
        }

        void addChildren(Node* n){
            m_children.push_back(n);
        }
        void removeChildren(Node* n){
            m_children.erase(
                std::remove(
                    m_children.begin(),m_children.end(),n
                ),m_children.end()
            );
        }
        void removeAll(){
            m_children.clear();
        }
    private:
        virtual void onDraw(sf::RenderTarget& target) const = 0;
        virtual void onUpdate() = 0;
        std::vector<Node *> m_children;
};