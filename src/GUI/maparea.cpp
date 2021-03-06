#include "maparea.h"

/*!
 * \brief MapArea::MapArea. Constructor. Sets all variables.
 * \param parent - pointer to parent object.
 * \details Constructor initiates variables and connects signals from EventInterpreter
 * to slots. Its parameter is pointer to parent object which is connected with
 * Qt's way of coordinating objects life span. This object will be deleted by
 * parent's destructor.
 */
MapArea::MapArea(QWidget *parent) : QWidget(parent), roadID(0), displayGrid(false) {

    connect(&eventInterpreter, &EventInterpreter::drawableCreated, this, &MapArea::registerDrawable);
    connect(&eventInterpreter, &EventInterpreter::roadCreated, this, &MapArea::registerRoad);
    connect(&eventInterpreter, &EventInterpreter::cameraCreated, this, &MapArea::registerCamera);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(196, 195, 208));
    setAutoFillBackground(true);
    setPalette(pal);

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

/*!
 * \brief MapArea::~MapArea.
 * \details Destructor clears vectors of objects that are graphic representation
 * of objects in app.
 */
MapArea::~MapArea() {
    for (const auto &drawable : movableMap)
        delete drawable.second;

    for (const auto &road : roadMap)
        delete road.second;

    for (const auto &object : objectMap)
        delete object;

    for (const auto &camera : cameraMap)
        delete camera;
}

/*!
 * \brief MapArea::setCar.
 * \param id - car ID.
 * \param x - position X.
 * \param y - position Y.
 * \param fast - flag tells if car is fast car or not.
 * \details Method used to set graphic representation of car object to
 * position corresponding with app's model state. If car with given ID
 * doesn't exist it creates such.
 */
void MapArea::setCar(const unsigned int id, const unsigned int x, const unsigned int y,
                     const bool fast) {
    if (movableMap[id] == nullptr) {
        movableMap[id] = new CarGUI(x, y, fast);
    } else {
        movableMap[id]->setTo(x, y);
    }
}

/*!
 * \brief MapArea::setPpl.
 * \param id - person ID.
 * \param x - position X.
 * \param y - position Y.
 * \details Method used to set graphic representation of human object to
 * position corresponding with app's model state. If car with given ID
 * doesn't exist it creates such.
 */
void MapArea::setPpl(const unsigned int id, const unsigned int x, const unsigned int y) {
    if (movableMap[id] == nullptr) {
        movableMap[id] = new PplGUI(x, y);
    } else {
        movableMap[id]->setTo(x, y);
    }
}

/*!
 * \brief MapArea::removeObject.
 * \param id - object ID.
 * \details Method removes movable object with given ID from list of redrawn objects.
 * Movable object is either Human or Car. If object with given ID does not exist,
 * function does nothing.
 */
void MapArea::removeObject(const unsigned int id) {
    if (movableMap[id] != nullptr) {
        delete movableMap[id];
        movableMap.erase(id);
    }
}

/*!
 * \brief MapArea::paintEvent.
 * \param event - pointer to object with info about event that called method.
 * \details Method inherited from base class. It creates painter and
 * iterates through object's lists of drawable objects and calls theirs
 * draw method.
 */
void MapArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    for (const auto &road : roadMap) {
        road.second->drawSidewalk(painter);
    }

    for (const auto &drawable : roadMap) {
            drawable.second->draw(painter);
    }
    for (const auto &drawable : movableMap) {
            drawable.second->draw(painter);
    }
    for (const auto &drawable : objectMap) {
        drawable->draw(painter);
    }
    for (const auto &drawable : cameraMap) {
        drawable->draw(painter);
    }

    if(ghost != nullptr)
        ghost->draw(painter);

    if(displayGrid)
        GridGUI(width(), height()).draw(painter);
}

/*!
 * \brief MapArea::mouseReleaseEvent.
 * \param event - pointer to object with info about event that called method.
 * \details Method inherited from base class. It intercepts all
 * events of releasing left mouse button and send that info
 * to appropriate class to interpret that. If clicked button isn't
 * RMB then event is sent up to base class to be interpreted.
 */
void MapArea::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        eventInterpreter.mouseClicked(event->x(), event->y());
        event->accept();
    } else {
        QWidget::mouseReleaseEvent(event);
    }
}

/*!
 * \brief MapArea::mouseMoveEvent.
 * \param event - pointer to object with info about event that called method.
 * \details Method inherited from base class. It intercepts all
 * events of moving mouse and send that info to appropriate class
 * to interpret that. Before exit map area is being redrawn.
 */
void MapArea::mouseMoveEvent(QMouseEvent *event) {
    eventInterpreter.mouseMoved(event->x(), event->y());
    event->accept();
    update();
}

/*!
 * \brief MapArea::keyPressEvent.
 * \param event - pointer to object with info about event that called method.
 * \details Method inherited from base class. It intercepts key press events.
 * If pressed key is left/right arrow or escape appropriate interpeter oprions
 * are called. Otherwise event is passed to base object. Before exit map area
 * is being redrawn.
 */
void MapArea::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
    if (key == Qt::Key_Escape)
        setCurrentOption(EventInterpreter::Option::doNothing);
    else if (key == Qt::Key_Left || key == Qt::Key_Right)
        eventInterpreter.steerCamera(key);
    else
        QWidget::keyPressEvent(event);
	update();
    event->accept();

}

/*!
 * \brief MapArea::registerCamera.
 * \param cam - pointer to CameraGUI object.
 * \details Function adds new camera to the list of cameras to be drawn.
 * Pointer to camera is passed as argument. Before exit map area is
 * redrawn. Function is a slot, so it is called by signal in EventInterpreter
 * object.
 */
void MapArea::registerCamera(CameraGUI *cam) {
    cameraMap.push_back(cam);
    update();
}

/*!
 * \brief MapArea::toggleGrid.
 * Toggles flag, whether grid is supposed to be drawn.
 * After that map area is being redrawn.
 */
void MapArea::toggleGrid() {
    displayGrid = !displayGrid;
    update();
}

/*!
 * \brief MapArea::setCurrentOption.
 * \param option - Option to be set.
 * \details Passes current tool that user want to use to EventInterpreter
 * object, and sets appropriate ghost object that will be drawn.
 */
void MapArea::setCurrentOption(EventInterpreter::Option option) {
    ghost = eventInterpreter.setCurrentOption(option);
}

/*!
 * \brief MapArea::registerRoad.
 * \param road - pointer to RoadGUI object.
 * \details Function adds new road to the list of roads to be drawn.
 * Pointer to RoadGUI object is passed as argument. Before exiting map
 * area is redrawn. Function is a slot, so it is called by signal in
 * EventInterpreter object.
 */
void MapArea::registerRoad(RoadGUI *road) {
    roadMap[++roadID] = road;
    update();
}

/*!
 * \brief MapArea::registerDrawable.
 * \param drawable - pointer to Drawable object.
 * \details Function adds new drawable object to list of objects
 * to be drawn. Pointer to Drawable object is being passed as argument.
 * Before exiting map area is being redrawn. Function is a slot, so it
 * is called by signal in EventInterpreter object.
 */
void MapArea::registerDrawable(Drawable *drawable) {
    objectMap.push_back(drawable);
    update();
}

/*!
 * \brief MapArea::setMap.
 * \param map - Shared pointer to map object.
 * \details Function sets map object in EventInterpreter object
 * \sa EventInterpreter::setMap.
 */
void MapArea::setMap(std::shared_ptr<Map> map) {
    eventInterpreter.setMap(map);
}
