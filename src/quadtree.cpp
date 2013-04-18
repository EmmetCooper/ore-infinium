#include "quadtree.h"

#include <assert.h>

Quadtree::Quadtree(XY _center, XY _halfDimension)
  : boundary(_center, _halfDimension), nodeCapacity(4)
{
  NW = NE = SW = SE = NULL;
  points.reserve(nodeCapacity);
}

Quadtree::Quadtree(XY _center, XY _halfDimension, int _nodeCapacity)
  : boundary(_center, _halfDimension), nodeCapacity(_nodeCapacity)
{
  NW = NE = SW = SE = NULL;
  points.reserve(nodeCapacity);
}

bool Quadtree::insert(Item * a) {
  if (!boundary.containsPoint(a))
    return false;

  if (points.size() < nodeCapacity) {
    points.push_back(a);
    return true;
  }

  if (NW == NULL) subdivide();

  if (NW->insert(a)) return true;
  if (NE->insert(a)) return true;
  if (SW->insert(a)) return true;
  if (SE->insert(a)) return true;

  assert(false);
  return false; // should never happen
}

void Quadtree::subdivide() {
  XY center = boundary.center;
  XY newDim(boundary.halfDimension.x / 2, boundary.halfDimension.y / 2);

  NW = new Quadtree(XY(center.x - newDim.x, center.y - newDim.y), newDim);
  NE = new Quadtree(XY(center.x + newDim.x, center.y - newDim.y), newDim);
  SW = new Quadtree(XY(center.x - newDim.x, center.y + newDim.y), newDim);
  SE = new Quadtree(XY(center.x + newDim.x, center.y + newDim.y), newDim);
}

void Quadtree::queryRange(std::vector<Item*> & list, AABB range) {
  if (!boundary.intersectsAABB(range)) return ; // list is empty

  for (int i = 0; i < points.size(); ++i)
    if (range.containsPoint(points[i]))
      list.push_back(points[i]);

  if (NW == NULL) return ;
  NW->queryRange(list, range);
  NE->queryRange(list, range);
  SW->queryRange(list, range);
  SE->queryRange(list, range);
}

// scan the tree and remove all node/Item*
void Quadtree::clear() {
  if (this == NULL) return ;
  points.clear();

  NW->clear();
  delete NW;
  NW = NULL;
  NE->clear();
  delete NE;
  NE = NULL;
  SW->clear();
  delete SW;
  SW = NULL;
  SE->clear();
  delete SE;
  SE = NULL;
}
