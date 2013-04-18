#ifndef __QUADTREE__
#define __QUADTREE__

#include "src/entity.h"

struct XY {
  XY(float _x, float _y) : x(_x), y(_y) {}
  float x, y;
};

struct AABB {
  AABB(XY _center, XY _halfDimension) : center(_center), halfDimension(_halfDimension) {}

  bool containsPoint(Item* item) {
    XY p(item->x, item->y);
    return ((p.x > center.x - halfDimension.x && p.x <= center.x + halfDimension.x) &&
            (p.y > center.y - halfDimension.y && p.y <= center.y + halfDimension.y));
  }

  bool intersectsAABB(AABB other) {
    return (((other.center.x - other.halfDimension.x > center.x - halfDimension.x && other.center.x - other.halfDimension.x < center.x + halfDimension.x) ||
             (other.center.x + other.halfDimension.x > center.x - halfDimension.x && other.center.x + other.halfDimension.x < center.x + halfDimension.x)) &&
            ((other.center.y - other.halfDimension.y > center.y - halfDimension.y && other.center.y - other.halfDimension.y < center.y + halfDimension.y) ||
             (other.center.y + other.halfDimension.y > center.y - halfDimension.y && other.center.y + other.halfDimension.y < center.y + halfDimension.y)));
  }

  XY center;
  XY halfDimension;
};

class Quadtree {
 public:
  Quadtree(XY, XY);
  Quadtree(XY, XY, int);

  bool insert(Entity *);
  void subdivide();
  void queryRange(std::vector<Entity*> &, AABB);

  void clear();
  bool getCollision(std::vector<Entity*>);

  AABB boundary;
  int nodeCapacity;

  // leaves
  Quadtree * NW;
  Quadtree * NE;
  Quadtree * SW;
  Quadtree * SE;

  // data
  std::vector<Entity*> points;
};

#endif // __QUADTREE__
