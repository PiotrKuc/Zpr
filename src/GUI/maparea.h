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

private:
    int roadID;
    std::map<unsigned int, Drawable*> movableMap;
    std::map<unsigned int, RoadGUI*> roadMap;
    std::vector<Drawable*> objectMap;
    bool displayGrid;
    EventInterpreter eventInterpreter;
    std::shared_ptr<Drawable> ghost;



};

#endif // MAPAREA_H
