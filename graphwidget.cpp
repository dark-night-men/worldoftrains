#include "graphwidget.h"
#include <math.h>
#include <QKeyEvent>
#include <QTextStream>
#include <QDebug>
#include <assert.h>

uint qHash(const QPoint& p)
{
    return p.x() * 17 ^ p.y();
}


QStringList loadCapitals()
{
    QFile file("./capitals");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QStringList();

    QTextStream in(&file);
    return in.readAll().split("\n");
}


bool Town::advance()
{
//    if (newPos == pos())
//        return false;
//
//    setPos(newPos);
//    return true;
    return true;
}

QRectF Town::boundingRect() const
{
    qreal adjust = 2;
    return QRectF( -10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

QPainterPath Town::shape() const
{
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

void Town::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-7, -7, 20, 20);

    QRadialGradient gradient(-3, -3, 10);
    if (option->state & QStyle::State_Sunken) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, QColor(Qt::yellow).light(120));
        gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
    } else {
        gradient.setColorAt(0, Qt::yellow);
        gradient.setColorAt(1, Qt::darkYellow);
    }
    painter->setBrush(gradient);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);


    //QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
    //                sceneRect.width() - 4, sceneRect.height() - 4);
    QRectF textRect(4, 4, 200, 24);

    QString message(name_ + QString(" >%1,%2< ")
            .arg(pos().x()).arg(pos().y()));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(11);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);

}

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), timerId(0)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-200, -200, 400, 400);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setWindowTitle(tr("Elastic Nodes"));

    /*
    Node *node1 = new Node(this);
    scene->addItem(node1);
    scene->addItem(new Edge(node1, node2));
    node1->setPos(-50, -50);
    */
    genTowns(scene);

    genEdges();
}

const int GraphWidget::townsNumber = 5;
//n*(n-1)/2 All possible connection
void GraphWidget::genEdges()
{
    int maxEdges = townsNumber*(townsNumber-1)/2 ;
    int edgeCount = qrand() % maxEdges;
    
    for (int i = 0; i < edgeCount; ++i){

        int t1;
        int t2;
        do {
            t1 = qrand() % townsNumber;
            t2 = qrand() % townsNumber;

        } while (edges_.contains(t1) and edges_[t1].contains(t2));
        newEdge(t1,t2);
    }
}

void GraphWidget::newEdge(int i, int j)
{
    qDebug() << i << " --- " << j ;
    edges_[i].insert(j);
    edges_[j].insert(i);
}


void GraphWidget::genTowns(QGraphicsScene * scene)
{
    QRect sceneRect(scene->sceneRect().toRect());
    const int towns = townsNumber;

    const QStringList names( loadCapitals() );
    for (int i = 0; i < towns; ++i){
        QPoint place;

        QString name;
        do {
            name =  names.at(qrand() % names.size());
        } while(names_.contains(name));

        int iter = 0;
        Town * t = 0;
        do {
            qDebug(QString("genTowns town %1 iter %2").arg(i).arg(++iter)
                    .toLatin1().constData());
            if (t) {
                scene->removeItem(t);
                delete t;
                //delete t; check valgrind
            }
            place = QPoint((qrand() % (sceneRect.width()/10))*10 + sceneRect.left()
                    ,(qrand() % (sceneRect.height()/10)*10) + sceneRect.top());
            t = new Town(place, i, name);
            scene->addItem(t);
            t->setPos(t->place());

        } while(!t->collidingItems().isEmpty());

        towns_.append(t); 
        places_ << place;
        names_ << name;
    }

}

void GraphWidget::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}

void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        //centerNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        //centerNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        //centerNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        //centerNode->moveBy(20, 0);
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        shuffle();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
/*
    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    foreach (Node *node, nodes)
        node->calculateForces();

    bool itemsMoved = false;
    foreach (Node *node, nodes) {
        if (node->advance())
            itemsMoved = true;
    }

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
*/
}

#ifndef QT_NO_WHEELEVENT
void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
#endif

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
	painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
	painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    // Grid
    painter->drawLines(QVector<QLine>() 
            << QLine(QPoint(0,sceneRect.top()), QPoint(0,sceneRect.bottom()))
            << QLine(QPoint(sceneRect.top(), 0), QPoint(sceneRect.bottom(), 0)));

    //qDebug(QString("sceneRect top %1 bottom %2").arg(sceneRect.top()).arg(sceneRect.bottom()).toLatin1().constData());
    QVector<QLine> dashes;
    for ( int y=0; y<=sceneRect.bottom(); y+=100 )
        dashes << QLine(QPoint(-10,y), QPoint(10,y)) << QLine(QPoint(-10,-y), QPoint(10,-y))
               << QLine(QPoint(y,-10), QPoint(y,10)) << QLine(QPoint(-y,-10), QPoint(-y,10));
    painter->drawLines(dashes);

    // Text
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
                    sceneRect.width() - 4, sceneRect.height() - 4);
    QString message(tr("Click and drag the nodes around, and zoom with the mouse "
                       "wheel or the '+' and '-' keys"));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);
}

void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void GraphWidget::shuffle()
{
    /*
    foreach (QGraphicsItem *item, scene()->items()) {
        if (qgraphicsitem_cast<Node *>(item))
            item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
    }
    */
}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}
