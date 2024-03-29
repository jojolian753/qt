/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEPINCHAREA_P_H
#define QDECLARATIVEPINCHAREA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qdatetime.h>
#include <qbasictimer.h>
#include <qevent.h>
#include <qgraphicssceneevent.h>
#include "qdeclarativeitem_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativePinch;
class QDeclarativePinchAreaPrivate : public QDeclarativeItemPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativePinchArea)
public:
    QDeclarativePinchAreaPrivate()
      : absorb(true), stealMouse(false), inPinch(false)
      , pinchRejected(false), pinch(0), pinchStartDist(0)
    {
    }

    ~QDeclarativePinchAreaPrivate();

    void init()
    {
        Q_Q(QDeclarativePinchArea);
        q->setAcceptedMouseButtons(Qt::LeftButton);
        q->setAcceptTouchEvents(true);
        q->setFiltersChildEvents(true);
    }

    bool absorb : 1;
    bool stealMouse : 1;
    bool inPinch : 1;
    bool pinchRejected : 1;
    QDeclarativePinch *pinch;
    QPointF sceneStartPoint1;
    QPointF sceneStartPoint2;
    QPointF lastPoint1;
    QPointF lastPoint2;
    qreal pinchStartDist;
    qreal pinchStartScale;
    qreal pinchLastScale;
    qreal pinchStartRotation;
    qreal pinchStartAngle;
    qreal pinchLastAngle;
    QPointF sceneStartCenter;
    QPointF pinchStartCenter;
    QPointF sceneLastCenter;
    QPointF pinchStartPos;
    QList<QTouchEvent::TouchPoint> touchPoints;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEPINCHAREA_P_H
