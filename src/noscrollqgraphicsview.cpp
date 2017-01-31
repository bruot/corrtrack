/*
 * This file is part of the particle tracking software CorrTrack.
 *
 * Copyright 2016, 2017 Nicolas Bruot
 *
 *
 * CorrTrack is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CorrTrack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CorrTrack.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <QGraphicsSceneWheelEvent>
#include <QApplication>
#include <QAbstractScrollArea>
#include "noscrollqgraphicsview.h"


NoScrollQGraphicsView::NoScrollQGraphicsView(QWidget *parent)
    : QGraphicsView(parent) {}


NoScrollQGraphicsView::NoScrollQGraphicsView(QGraphicsScene *scene,
                                             QWidget *parent)
    : QGraphicsView(scene, parent) {}


void NoScrollQGraphicsView::wheelEvent(QWheelEvent *event)
{
    // Override wheelEvent in order to do nothing rather than scroll.

    // This is copied from widgets/graphicsview/qgraphicsview.cpp:
    QGraphicsSceneWheelEvent wheelEvent(QEvent::GraphicsSceneWheel);
    wheelEvent.setWidget(viewport());
    wheelEvent.setScenePos(mapToScene(event->pos()));
    wheelEvent.setScreenPos(event->globalPos());
    wheelEvent.setButtons(event->buttons());
    wheelEvent.setModifiers(event->modifiers());
    wheelEvent.setDelta(event->delta());
    wheelEvent.setOrientation(event->orientation());
    wheelEvent.setAccepted(false);
    QApplication::sendEvent(QGraphicsView::scene(), &wheelEvent);
    event->setAccepted(wheelEvent.isAccepted());
}
