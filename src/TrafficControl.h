//
// Created by kuco on 08.12.16.
//

#ifndef ZPR_TRAFFICCONTROL_H
#define ZPR_TRAFFICCONTROL_H


#include "Movable.h"
#include "Cross.h"
#include <algorithm>
#include <stack>
#include <list>
#include "Point.h"
#include "CrossFactory.h"

class TrafficControl {

private:
    std::vector<Cross*> crosses;
    std::list<Movable*> movables;

    int findCrossByPoint(Point*);
    void prepareFinding();
    void findRoute(Point*, Point*, std::vector<Point*>&);


public:
    bool createNewMovable(Point*, Point*, const int&);
    void createRoute(Point*, Point*);

    TrafficControl();

    ~TrafficControl();


};


#endif //ZPR_TRAFFICCONTROL_H
