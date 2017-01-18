//
// Created by kuco on 12.01.17.
//

#include "Map.h"
#include <GUI/mainwindow.h>


void Map::createCar(PtrToConstPoint startingPoint, PtrToConstPoint endingPoint, int speed) {
    criticalSection.lock();
    movableFactory.createCar(startingPoint, endingPoint, speed, crossFactory.getCrosses());
    criticalSection.unlock();
}

bool Map::createRoad(PtrToConstPoint begin, PtrToConstPoint end) {
    criticalSection.lock();
    LineSegment lineSegment(*begin, *end);
    for (const PtrBuilding &building : facilities.getBuildings()) {
        if(building->hasIntersection(lineSegment)){
            criticalSection.unlock();
            return false;
        }
    }
    crossFactory.createRoad(begin, end);
    criticalSection.unlock();
    return true;
}

void Map::setRunningMovablePermission(bool permission) {
    if (runningMovablePermission == permission) return;
    runningMovablePermission = permission;
    if (runningMovablePermission)
        runningMovables = std::thread(&Map::runRunningMovables, this);
    else
        runningMovables.join();
}

void Map::setCameraScanningPermission(bool permission) {
    if (cameraScanningPermission == permission) return;
    cameraScanningPermission = permission;
    if (cameraScanningPermission)
        camerasScanning = std::thread(&Map::runCamerasScanning, this);
    else
        camerasScanning.join();
}

bool Map::createBuilding(const Point &upperLeft, const Point &lowerRight) {
    criticalSection.lock();
    Building building(upperLeft, lowerRight);
    for (PtrCross &cross : crossFactory.getCrosses()) {
        PtrToConstPoint startPoint = cross->getPosition();
        PtrCross south = cross->getSouthNeighbour();
        PtrCross east = cross->getEastNeighbour();
        if (east != nullptr && building.hasIntersection(LineSegment(*startPoint, *(east->getPosition()))) ||
            south != nullptr && building.hasIntersection(LineSegment(*startPoint, *(south->getPosition())))){
            criticalSection.unlock();
            return false;
        }
    }
    facilities.addBuilding(upperLeft, lowerRight);
    criticalSection.unlock();
    return true;
}

void Map::runRunningMovables(){
    while(runningMovablePermission){
        criticalSection.lock();
        std::list<PtrCar> &cars = movableFactory.getCars();
        std::list<PtrCar>::iterator cars_iter = cars.begin();
        while(cars_iter!=cars.end()){
            if(!(*cars_iter)->move()){
                MainWindow::getInstance().removeObject((*cars_iter)->getId());
                cars_iter= cars.erase(cars_iter);
            }

            else {
                MainWindow::getInstance().setCar(
                        (*cars_iter)->getId(),
                        static_cast<unsigned int>((*cars_iter)->getActualPoint().getX()),
                        static_cast<unsigned int>((*cars_iter)->getActualPoint().getY()),
                        (*cars_iter)->isFast());
                ++cars_iter;
            }
        }

        std::list<PtrHuman> &humans= movableFactory.getHumans();
        std::list<PtrHuman>::iterator humans_iter = humans.begin();
        while(humans_iter!=humans.end()){
            if(!(*humans_iter)->move()){
                MainWindow::getInstance().removeObject((*humans_iter)->getId());
                humans_iter = humans.erase(humans_iter);
            }

            else {
                MainWindow::getInstance().setPpl(
                        (*humans_iter)->getId(),
                        static_cast<unsigned int>((*humans_iter)->getActualPoint().getX()),
                        static_cast<unsigned int>((*humans_iter)->getActualPoint().getY()));
                ++humans_iter;
            }
        }
        criticalSection.unlock();
        MainWindow::getInstance().refresh();
        std::this_thread::sleep_for (MainWindow::REFRESH_TIME);
    }
}

void Map::createHuman(PtrToConstPoint src, PtrToConstPoint dst, int speed){
    criticalSection.lock();
    movableFactory.createHuman(src, dst, speed, crossFactory.getCrosses());
    criticalSection.unlock();
}

void Map::runCamerasScanning() {
    while (cameraScanningPermission) {
        criticalSection.lock();
        std::vector<PtrConstCar> cars(movableFactory.getCars().begin(), movableFactory.getCars().end());
        std::vector<PtrConstHuman> humans(movableFactory.getHumans().begin(), movableFactory.getHumans().end());
        facilities.scan(cars, humans);
        criticalSection.unlock();
        for (const PtrCamera &camera : facilities.getCameras()) {
            for(const PtrConstCar &car : camera->getSeenCars())
                std::cout << "I see car: " << car->getActualPoint() << std::endl;
            for(const PtrConstHuman &human : camera->getSeenHumans())
                std::cout << "I see human: " << human->getActualPoint() << std::endl;
        }
        std::this_thread::sleep_for (MainWindow::CAMERA_SCAN_FREQ);
    }
}

void Map::createCamera(const Point &startPoint, const Point &endPoint, double angle) {
    criticalSection.lock();
    facilities.addCamera(startPoint, endPoint, angle, 1);
    criticalSection.unlock();
}
