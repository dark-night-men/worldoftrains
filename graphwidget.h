#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include <QSet>
#include <QHash>
#include <QPoint>
#include <QGraphicsItem>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

uint qHash(const QPoint& p);
class Continent : public QGraphicsItem
{
public:
    Continent();
    bool advance();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QRect rect_;
};

class Town : public QGraphicsItem
{
public:
    Town(const QPoint& place = QPoint(0,0), int id = -1, const QString& name = QString())
        :place_(place), id_(id), name_(name) {
            setPos(place);
        }

    QPoint place() {return place_;}
    QString name() {return name_;}
    int id() {return id_;}

    void setPlace(const QPoint& place) {place_ = place;}
    void setName(const QString& name) {name_ = name;}
    void setId(int id) {id_ = id;}

    bool advance();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


private:
    QPoint place_;
    int id_;
    QString name_;
};

class Grid;
class GraphWidget : public QGraphicsView
{
    Q_OBJECT
public:
    GraphWidget(QWidget *parent = 0);
    virtual ~GraphWidget();

    void itemMoved();
    void newEdge(int i, int j);

    static const int townsNumber ;

public slots:
    void shuffle();
    void zoomIn();
    void zoomOut();

protected:
    void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event);
#endif
    void drawBackground(QPainter *painter, const QRectF &rect);

    void scaleView(qreal scaleFactor);

protected:
    void genTowns(QGraphicsScene * scene);
    void genEdges();

private:
    int timerId;
    QHash<int,QSet<int> > edges_ ;
    QVector<Town*> towns_;    
    QSet<QPoint> places_;
    QSet<QString> names_;
    Grid * m_grid;
};

class Grid
{
public:
    Grid(QGraphicsScene * scene);
    
    void drawGrid(QPainter *painter, const QRectF &rect);

private:
    int m_xmax;
    int m_ymax;
    QGraphicsScene * m_scene;

    int m_xScale;
    int m_yScale;

};

#endif // GRAPHWIDGET_H
