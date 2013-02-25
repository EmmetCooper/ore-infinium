#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include <vector>

using namespace std;

class QuadTree;
class Entity;

class QuadTree
{
public:
    QuadTree(float x, float y, float width, float height, int level);

    void addEntity(Entity* object);
    vector<Entity*> entitiesAt(float x, float y);
    void clear();
    void split();

private:
    const int MAX_OBJECTS = 10;
    const int MAX_LEVELS = 5;

private:
    float m_x;
    float m_y;
    float m_width;
    float m_height;
    int m_level;
    vector<Entity*> m_entities;

//    QuadTree* parent;
    QuadTree* nodes[4];

//    bool contains(Quadtree* child, Entity* object);
    int getIndex(Entity* entity);
};

#endif
