/*
 * Copyright (C) 2019  Exargon
 *
 * This is part of a widget to provide a slider having multiple
 * parts with a constant sum.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include "piechartslider.h"

#include <QtMath>

#include <QPainter>
#include <QStyleOption>

#include <QMouseEvent>
#include <QWheelEvent>

qreal angleBetweenVectors(const QPointF& vec1, const QPointF& vec2);
qreal distance(const QPointF& first, const QPointF& second);

PieChartSlider::PieChartSlider(int number_of_dividers, int total, QWidget* parent)
    : AbstractDividerSlider(number_of_dividers, total, parent)
{
    setMinimumSize(100,100);

    //Color Palette by Sasha Trubetskoy, https://sashat.me/2017/01/11/list-of-20-simple-distinct-colors/
    piechart_palette = {
        "#e6194B", "#3cb44b", "#ffe119", "#4363d8", "#f58231",
        "#911eb4", "#42d4f4", "#f032e6", "#bfef45", "#fabebe",
        "#469990", "#e6beff", "#9A6324", "#fffac8", "#800000",
        "#aaffc3", "#808000", "#ffd8b1", "#000075", "#a9a9a9"};

    sector_handles.fill(SectorHandle(),numberOfSectors());
    sector_handles.back().angle = ANGLE_TICKS_IN_CIRCLE;

    for (int i = 0; i < numberOfDividers(); ++i){
        divider_handles.push_back(DividerHandle());
        moveHandles(i, dividerValue(i));
    }

    connect(this, &PieChartSlider::sectorValueChanged,
            this, &PieChartSlider::updateSectorHandels);
}


void PieChartSlider::moveHandles(int index, int value){
    divider_handles[index].angle = valueToAngle(value);
    sector_handles[index].angle = divider_handles[index].angle;
}

void PieChartSlider::updateSectorHandels(int index, int value){
    sector_handles[index].visible = (value == 0)? true : false;

    //To make sure max angle handles are updated when a handle at zero has been moved
    if (sectorValue(index) == dividerValue(index) && index != numberOfSectors() - 1){
        updateSectorHandels(numberOfSectors() - 1, sectorValue(numberOfSectors() - 1));
    }

    std::function<bool(int)> sameAngle = [&](int i)
        {return sector_handles[i].angle == sector_handles[index].angle;};

    if (sector_handles[index].angle == 0 || sector_handles[index].angle == ANGLE_TICKS_IN_CIRCLE){
        sameAngle = [this](int i)
            {return sector_handles[i].angle == 0 || sector_handles[i].angle == ANGLE_TICKS_IN_CIRCLE;};
    }

    int current_level = 0;
    for (int i = 0; i < numberOfSectors(); ++i){
        if (sector_handles[i].visible && sameAngle(i)){
            sector_handles[i].collapse_level = current_level++;
        }
    }
}

int PieChartSlider::radius() const{
    return std::min(width(),height())/2 - DividerHandle::SIZE/2;
}

int PieChartSlider::valueToAngle(int value) const{
    return (ANGLE_TICKS_IN_CIRCLE*value) / total();
}

int PieChartSlider::angleToValue(int angle) const{
    /*Round to get better handling when the anglestep between values is large*/
    return static_cast<int>(round(static_cast<double>(angle*total()) / ANGLE_TICKS_IN_CIRCLE));
}

int PieChartSlider::positionToAngle(QPoint pos) const{
    QPointF vec1 = pos - pieCentre();
    QPointF vec2 = angleToPosition(0, radius()) - pieCentre();
    qreal real_angle = angleBetweenVectors(vec1, vec2);
    return static_cast<int>(round(real_angle*ANGLE_TICKS_IN_CIRCLE/(2*M_PI)));
}

QPointF PieChartSlider::angleToPosition(int angle, qreal radius) const{
    qreal real_angle = -(zero_angle + angle) * 2*M_PI/ANGLE_TICKS_IN_CIRCLE;
    return pieCentre() + QPointF(round(radius*cos(real_angle)), round(radius*sin(real_angle)));
}


qreal angleBetweenVectors(const QPointF& vec1, const QPointF& vec2){
    /* atan2 has range from -M_PI to M_PI, so to compensate vec2 is reversed (subtracts M_PI)
    * and M_PI is added so the range becomes 0 to 2*M_PI */

    return std::atan2(vec1.y()*vec2.x() - vec1.x()*vec2.y(), -vec1.x()*vec2.x() - vec1.y()*vec2.y()) + M_PI;
}

void PieChartSlider::wheelEvent(QWheelEvent *event){
    int sector_index = 0;
    while(sector_index < numberOfDividers() && dividerValue(sector_index) < angleToValue(positionToAngle(event->pos()))){
          ++sector_index;
    }
    int delta = angleToValue(event->angleDelta().y()/5);
    if (delta == 0){
        delta = (event->angleDelta().y() < 0)? -1 : 1;
    }
    setSectorValue(sector_index, sectorValue(sector_index) + delta);
}

void PieChartSlider::mousePressEvent(QMouseEvent *event){
    if (event->button() != Qt::LeftButton) return;
    setEmptySectorsCollapsed();

    for (DividerHandle& handle : divider_handles){
        if (onHandle(handle, event->pos())){
            handle.is_pressed = true;
            repaint(boundingRect(handle).toAlignedRect());
            return;
        }
    }
    for (SectorHandle& handle : sector_handles){
        if (handle.visible && onHandle(handle, event->pos())){
            handle.is_pressed = true;
            handle_start_angle = handle.angle;
            repaint(boundingRect(handle).toAlignedRect());
            return;
        }
    }
}

bool PieChartSlider::onHandle(const Handle& handle, QPoint pos) const{
    QPointF centre = angleToPosition(handle.angle, radius() + handle.radiusOffset());

    return (distance(centre, pos) < handle.size() / 2);
}

qreal distance(const QPointF& first, const QPointF& second){
    qreal dx = second.x() - first.x();
    qreal dy = second.y() - first.y();
    return sqrt(dx*dx + dy*dy);
}

void PieChartSlider::mouseMoveEvent(QMouseEvent *event){

    for (int index = 0; index < numberOfSectors(); ++index){
        if (sector_handles[index].is_pressed){
            processSectorMouseInput(index, event->pos());
        }
    }
    for (int index = 0; index < numberOfDividers(); ++index){
        if (divider_handles[index].is_pressed){
            setDividerValueFromMouse(index, event->pos());
        }
    }
}

void PieChartSlider::processSectorMouseInput(int index, QPoint mouse_pos){
    int angle = angleFromMouse(index, mouse_pos);
    if (angle == handle_start_angle) return;

    setSectorCollapsed(index, false);

    if (angle < handle_start_angle && index > 0){
        divider_handles[index - 1].is_pressed = true;

        if (index < numberOfDividers() && divider_handles[index].is_pressed){
            divider_handles[index].is_pressed = false;
            setDividerValue(index, angleToValue(handle_start_angle));
        }
    }else if (angle > handle_start_angle){
        divider_handles[index].is_pressed = true;

        if (index > 0 && divider_handles[index - 1].is_pressed){
            divider_handles[index - 1].is_pressed = false;
            setDividerValue(index - 1, angleToValue(handle_start_angle));
        }
    }
}

int PieChartSlider::angleFromMouse(int index, QPoint mouse_pos) const{
    int value = angleToValue(positionToAngle(mouse_pos));

    int min_index = (index == 0)? index : index - 1;
    int max_index = (index < numberOfDividers())? index : index - 1;

    return valueToAngle(stabilizedValue(min_index, max_index, value));
}

void PieChartSlider::setDividerValueFromMouse(int index, QPoint mouse_pos){
    int value = angleToValue(positionToAngle(mouse_pos));

    setDividerValue(index, stabilizedValue(index,index,value));
}

int PieChartSlider::stabilizedValue(int min_index, int max_index, int value) const{
    int minimum = dividerMinimum(min_index);
    int maximum = dividerMaximum(max_index);

    /* Avoid value jumping around. First branch detects jumps larger than legal range
     * and second branch decides if this is a minimum or maximum by which is closest */
    if(value - dividerValue(min_index) > maximum - minimum) {
        value = (total() - value + minimum < value - maximum)? minimum : maximum;

    }else if (dividerValue(max_index) - value > maximum - minimum){
        value = (total() + value - maximum < minimum - value)? maximum : minimum;
    }

    return value;
}

void PieChartSlider::mouseReleaseEvent(QMouseEvent *event){
    if (event->button() != Qt::LeftButton) return;
    setEmptySectorsCollapsed();

    for (int index = 0; index < numberOfDividers(); ++index){
        if(divider_handles[index].is_pressed){
            divider_handles[index].is_pressed = false;
            repaint(boundingRect(divider_handles[index]).toAlignedRect());
            break;
        }
    }

    for (SectorHandle& handle : sector_handles){
        if (handle.is_pressed){
            handle.is_pressed = false;
            repaint(boundingRect(handle).toAlignedRect());
            break;
        }
    }
}

void PieChartSlider::setEmptySectorsCollapsed(){
    for (int i = 0; i < numberOfSectors(); ++i){
        if(sectorValue(i) == 0){
            setSectorCollapsed(i, true);
        }
    }
}

QRectF PieChartSlider::boundingRect(const Handle& handle) const{
    QPointF centre = angleToPosition(handle.angle, radius() + handle.radiusOffset());

    return QRectF(centre - QPointF(handle.size()/2, handle.size()/2),
                  centre + QPointF(handle.size()/2, handle.size()/2));
}

void PieChartSlider::paintEvent(QPaintEvent *event){
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    painter.setClipRegion(event->region());

    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    painter.setRenderHint(QPainter::Antialiasing);

    /* Draw the pies. Without outlines to avoid ugly double lines */
    QRect pie_envelope(pieCentre() - QPoint(radius(),radius()), QSize(2*radius(), 2*radius()));
    painter.setPen(Qt::NoPen);

    painter.setBrush(sectorColor(0));
    painter.drawPie(pie_envelope, zero_angle, divider_handles[0].angle);

    for (int index = 0; index < numberOfDividers(); ++index){
        int sector = index + 1;
        painter.setBrush(sectorColor(sector));
        painter.drawPie(pie_envelope, divider_handles[index].angle + zero_angle, valueToAngle(sectorValue(sector)));
    }

    /* Draw dividing lines and circle outline */
    painter.setPen(QPen(Qt::black, 0, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(Qt::NoBrush);

    for (DividerHandle handle : divider_handles){
        painter.drawLine(pieCentre(), angleToPosition(handle.angle, radius()));
    }
    painter.drawLine(pieCentre(), angleToPosition(0, radius()));

    painter.drawEllipse(pie_envelope);

    /* Paint divider handles. Reverse so the top handle for the mouse is painted last */
    for (auto handle = divider_handles.crbegin(); handle != divider_handles.crend(); ++handle){
        if (handle->is_pressed){
            painter.setBrush(palette().mid());
        } else {
            painter.setBrush(palette().button());
        }
        painter.setPen(QPen(palette().shadow(), 0, Qt::SolidLine));

        painter.drawEllipse(boundingRect(*handle));
    }

    /* Paint the visible sector handles. */
    for(int index = 0; index < numberOfSectors(); ++index){
        if (sector_handles[index].visible){
            if (sector_handles[index].is_pressed){
                painter.setBrush(sectorColor(index).darker(125));
            } else{
                painter.setBrush(sectorColor(index));
            }

            painter.setPen(QPen(Qt::black, 0, Qt::SolidLine));
            painter.drawEllipse(boundingRect(sector_handles[index]));
        }
    }
}

