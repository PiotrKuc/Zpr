#include "maparea.h"

MapArea::MapArea(QWidget *parent) : QWidget(parent), roadID(0), displayGrid(false) {

    connect(&eventInterpreter, &EventInterpreter::drawableCreated, this, &MapArea::registerDrawable);
    connect(&eventInterpreter, &EventInterpreter::roadCreated, this, &MapArea::registerRoad);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::gray);
    setAutoFillBackground(true);
    setPalette(pal);

    setMouseTracking(true);
}

MapArea::~MapArea() {
    for (const auto &drawable : objectMap)
        delete drawable.second;

    for (const auto &road : roadMap)
        delete road.second;
}

void MapArea::setCar(const unsigned int id, const unsigned int x, const unsigned int y) {
    if (objectMap[id] == nullptr) {
        objectMap[id] = new CarGUI(2, x, y);
    } else {
        objectMap[id]->setTo(x, y);
    }
    update();
}

void MapArea::setPpl(const unsigned int id, const unsigned int x, const unsigned int y) {
    if (objectMap[id] == nullptr) {
        objectMap[id] = new PplGUI(2, x, y);
    } else {
        objectMap[id]->setTo(x, y);
    }
    update();
}

void MapArea::removeObject(const unsigned int id) {
    if (objectMap[id] != nullptr) {
        delete objectMap[id];
        objectMap.erase(id);
    }
}

void MapArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    for (const auto &road : roadMap) {
        road.second->drawSidewalk(painter);
    }

    for (const auto &drawable : roadMap) {
            drawable.second->draw(painter);
    }
    for (const auto &drawable : objectMap) {
            drawable.second->draw(painter);
    }

    if(ghost != nullptr)
        ghost->draw(painter);

    if(displayGrid)
        GridGUI(1, width(), height()).draw(painter);

  //  QWidget::paintEvent(event);
}

void MapArea::snapToGrid(Point &point) {
    int x = point.getX() / GridGUI::SIZE;
    int y = point.getY() / GridGUI::SIZE;
    point.setX(x * GridGUI::SIZE + GridGUI::SIZE/2);
    point.setY(y * GridGUI::SIZE + GridGUI::SIZE/2);
}



void MapArea::mouseReleaseEvent(QMouseEvent *event) {
    eventInterpreter.mouseClicked(event->x(), event->y());
    event->accept();
}

void MapArea::mouseMoveEvent(QMouseEvent *event) {
    eventInterpreter.mouseMoved(event->x(), event->y());
    event->accept();
    update();
}

void MapArea::createRoad(Point end1, Point end2) {
    roadMap[++roadID] = new RoadGUI(3, end1, end2);
    update();
}


void MapArea::toggleGrid() {
    displayGrid = !displayGrid;
    update();
}

void MapArea::setCurrentOption(EventInterpreter::Option option) {
    ghost = eventInterpreter.setCurrentOption(option);
}

void MapArea::registerRoad(RoadGUI *road) {
    roadMap[++roadID] = road;
    update();
}

void MapArea::registerDrawable(Drawable *drawable) {
    int id = 80; //TEMPRARILY
    objectMap[id] = drawable;
    update();
}

void MapArea::setTrafficControl(std::shared_ptr<TrafficControl> tc) {
    eventInterpreter.setTrafficControl(tc);
}
