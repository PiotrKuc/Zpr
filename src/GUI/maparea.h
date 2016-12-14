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

class MapArea : public QWidget
{
    Q_OBJECT
public:

    explicit MapArea(QWidget *parent = 0);
    ~MapArea();

    void setCar(int id, int x, int y);
    void setPpl(int id, int x, int y);
    void createRoad(Point end1, Point end2);


    void snapToGrid(Point &point);
    void setCurrentOption(EventInterpreter::Option option);

    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:

public slots:
    void toggleGrid();
    void registerRoad(RoadGUI*);
    void registerDrawable(Drawable*);

private:
    int roadID;
    std::map<int, Drawable*> objectMap;
    std::map<int, RoadGUI*> roadMap;
    bool displayGrid;
    EventInterpreter eventInterpreter;
    std::shared_ptr<Drawable> ghost;



};

#endif // MAPAREA_H