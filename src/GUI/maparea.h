#ifndef MAPAREA_H
#define MAPAREA_H

#include <QWidget>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QMouseEvent>
#include <iostream>
#include "drawable.h"
#include "cargui.h"
#include "pplgui.h"
#include "gridgui.h"
#include "roadgui.h"
#include "../Point.h"
#include <memory>
#include "eventinterpreter.h"
#include "../Map.h"


/*!
 * \brief The MapArea class. Object responsible for drawing objects on screen.
 * \details Object represents main widget in application. It is sandbox that
 * intercepts user actions and draw everything that has graphical representation.
 * It is mainly operated with mouse. Due to that all mouse movements and clicks
 * are intercepted by object. Apart from that also keys 'escape', and arrows
 * left and right are intercepted. Rest of keyboard events are sent to base class
 * QWidget.
 */
class MapArea : public QWidget
{
    Q_OBJECT
public:

    explicit MapArea(QWidget *parent = 0);
    ~MapArea();

    void setCar(const unsigned int id, const unsigned int x, const unsigned int y, const bool fast);
    void setPpl(const unsigned int id, const unsigned int x, const unsigned int y);
    void removeObject(const unsigned int id);
    void createRoad(Point end1, Point end2);


    void snapToGrid(Point &point);
    void setCurrentOption(EventInterpreter::Option option);

    void setMap(std::shared_ptr<Map> map);

    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

signals:

public slots:
    void toggleGrid();
    void registerRoad(RoadGUI*);
    void registerDrawable(Drawable*);
    void registerCamera(CameraGUI*);

private:
    int roadID;
    std::map<unsigned int, Drawable*> movableMap;
    std::map<unsigned int, RoadGUI*> roadMap;
    std::vector<Drawable*> objectMap;
    std::vector<CameraGUI*> cameraMap;
    bool displayGrid;
    EventInterpreter eventInterpreter;
    std::shared_ptr<Drawable> ghost;



};

#endif // MAPAREA_H
