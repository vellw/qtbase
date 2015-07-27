/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <math.h>

#include "tabletcanvas.h"

//! [0]
TabletCanvas::TabletCanvas()
{
    resize(500, 500);
    myColor = Qt::red;
    myBrush = QBrush(myColor);
    myPen = QPen(myBrush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    initPixmap();
    setAutoFillBackground(true);
    deviceDown = false;
    alphaChannelType = AlphaTangentialPressure;
    colorSaturationType = NoSaturation;
    lineWidthType = LineWidthPressure;
}

void TabletCanvas::initPixmap()
{
    QPixmap newPixmap = QPixmap(width(), height());
    newPixmap.fill(Qt::white);
    QPainter painter(&newPixmap);
    if (!pixmap.isNull())
        painter.drawPixmap(0, 0, pixmap);
    painter.end();
    pixmap = newPixmap;
}
//! [0]

//! [1]
bool TabletCanvas::saveImage(const QString &file)
{
    return pixmap.save(file);
}
//! [1]

//! [2]
bool TabletCanvas::loadImage(const QString &file)
{
    bool success = pixmap.load(file);

    if (success) {
        update();
        return true;
    }
    return false;
}
//! [2]

//! [3]
void TabletCanvas::tabletEvent(QTabletEvent *event)
{

    switch (event->type()) {
        case QEvent::TabletPress:
            if (!deviceDown) {
                deviceDown = true;
                lastPoint.pos = event->posF();
                lastPoint.rotation = event->rotation();
            }
            break;
        case QEvent::TabletMove:
            if (event->device() == QTabletEvent::RotationStylus)
                updateCursor(event);
            if (deviceDown) {
                updateBrush(event);
                QPainter painter(&pixmap);
                paintPixmap(painter, event);
                lastPoint.pos = event->posF();
                lastPoint.rotation = event->rotation();
            }
            break;
        case QEvent::TabletRelease:
            if (deviceDown && event->buttons() == Qt::NoButton)
                deviceDown = false;
            break;
        default:
            break;
    }
    update();
}
//! [3]

//! [4]
void TabletCanvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
}
//! [4]

//! [5]
void TabletCanvas::paintPixmap(QPainter &painter, QTabletEvent *event)
{
    painter.setRenderHint(QPainter::Antialiasing);

    switch (event->device()) {
//! [6]
        case QTabletEvent::Airbrush:
            {
                painter.setPen(Qt::NoPen);
                QRadialGradient grad(lastPoint.pos, myPen.widthF() * 10.0);
                QColor color = myBrush.color();
                color.setAlphaF(color.alphaF() * 0.25);
                grad.setColorAt(0, myBrush.color());
                grad.setColorAt(0.5, Qt::transparent);
                painter.setBrush(grad);
                qreal radius = grad.radius();
                painter.drawEllipse(event->posF(), radius, radius);
            }
            break;
        case QTabletEvent::RotationStylus:
            {
                myBrush.setStyle(Qt::SolidPattern);
                painter.setPen(Qt::NoPen);
                painter.setBrush(myBrush);
                QPolygonF poly;
                qreal halfWidth = myPen.widthF();
                QPointF brushAdjust(qSin(qDegreesToRadians(lastPoint.rotation)) * halfWidth,
                                    qCos(qDegreesToRadians(lastPoint.rotation)) * halfWidth);
                poly << lastPoint.pos + brushAdjust;
                poly << lastPoint.pos - brushAdjust;
                brushAdjust = QPointF(qSin(qDegreesToRadians(event->rotation())) * halfWidth,
                                      qCos(qDegreesToRadians(event->rotation())) * halfWidth);
                poly << event->posF() - brushAdjust;
                poly << event->posF() + brushAdjust;
                painter.drawConvexPolygon(poly);
            }
            break;
//! [6]
        case QTabletEvent::Puck:
        case QTabletEvent::FourDMouse:
            {
                const QString error(tr("This input device is not supported by the example."));
#ifndef QT_NO_STATUSTIP
                QStatusTipEvent status(error);
                QApplication::sendEvent(this, &status);
#else
                qWarning() << error;
#endif
            }
            break;
        default:
            {
                const QString error(tr("Unknown tablet device - treating as stylus"));
#ifndef QT_NO_STATUSTIP
                QStatusTipEvent status(error);
                QApplication::sendEvent(this, &status);
#else
                qWarning() << error;
#endif
            }
            // FALL-THROUGH
        case QTabletEvent::Stylus:
            painter.setPen(myPen);
            painter.drawLine(lastPoint.pos, event->posF());
            break;
    }
}
//! [5]

//! [7]
void TabletCanvas::updateBrush(const QTabletEvent *event)
{
    int hue, saturation, value, alpha;
    myColor.getHsv(&hue, &saturation, &value, &alpha);

    int vValue = int(((event->yTilt() + 60.0) / 120.0) * 255);
    int hValue = int(((event->xTilt() + 60.0) / 120.0) * 255);
//! [7] //! [8]

    switch (alphaChannelType) {
        case AlphaPressure:
            myColor.setAlphaF(event->pressure());
            break;
        case AlphaTangentialPressure:
            if (event->device() == QTabletEvent::Airbrush)
                myColor.setAlphaF(qMax(0.01, (event->tangentialPressure() + 1.0) / 2.0));
            else
                myColor.setAlpha(255);
            break;
        case AlphaTilt:
            myColor.setAlpha(maximum(abs(vValue - 127), abs(hValue - 127)));
            break;
        default:
            myColor.setAlpha(255);
    }

//! [8] //! [9]
    switch (colorSaturationType) {
        case SaturationVTilt:
            myColor.setHsv(hue, vValue, value, alpha);
            break;
        case SaturationHTilt:
            myColor.setHsv(hue, hValue, value, alpha);
            break;
        case SaturationPressure:
            myColor.setHsv(hue, int(event->pressure() * 255.0), value, alpha);
            break;
        default:
            ;
    }

//! [9] //! [10]
    switch (lineWidthType) {
        case LineWidthPressure:
            myPen.setWidthF(event->pressure() * 10 + 1);
            break;
        case LineWidthTilt:
            myPen.setWidthF(maximum(abs(vValue - 127), abs(hValue - 127)) / 12);
            break;
        default:
            myPen.setWidthF(1);
    }

//! [10] //! [11]
    if (event->pointerType() == QTabletEvent::Eraser) {
        myBrush.setColor(Qt::white);
        myPen.setColor(Qt::white);
        myPen.setWidthF(event->pressure() * 10 + 1);
    } else {
        myBrush.setColor(myColor);
        myPen.setColor(myColor);
    }
}
//! [11]

void TabletCanvas::updateCursor(const QTabletEvent *event)
{
    QCursor cursor;
    if (event->type() != QEvent::TabletLeaveProximity) {
        if (event->pointerType() == QTabletEvent::Eraser) {
            cursor = QCursor(QPixmap(":/images/cursor-eraser.png"), 3, 28);
        } else {
            switch (event->device()) {
            case QTabletEvent::Stylus:
                cursor = QCursor(QPixmap(":/images/cursor-pencil.png"), 0, 0);
                break;
            case QTabletEvent::Airbrush:
                cursor = QCursor(QPixmap(":/images/cursor-airbrush.png"), 3, 4);
                break;
            case QTabletEvent::RotationStylus: {
                QImage origImg(QLatin1String(":/images/cursor-felt-marker.png"));
                QImage img(32, 32, QImage::Format_ARGB32);
                QColor solid = myColor;
                solid.setAlpha(255);
                img.fill(solid);
                QPainter painter(&img);
                QTransform transform = painter.transform();
                transform.translate(16, 16);
                transform.rotate(-event->rotation());
                painter.setTransform(transform);
                painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                painter.drawImage(-24, -24, origImg);
                painter.setCompositionMode(QPainter::CompositionMode_HardLight);
                painter.drawImage(-24, -24, origImg);
                painter.end();
                cursor = QCursor(QPixmap::fromImage(img), 16, 16);
            } break;
            default:
                break;
            }
        }
    }
    setCursor(cursor);
}

void TabletCanvas::resizeEvent(QResizeEvent *)
{
    initPixmap();
}
