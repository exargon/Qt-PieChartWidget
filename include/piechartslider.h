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

#ifndef PIECHARTSLIDER_H
#define PIECHARTSLIDER_H

#include "abstractdividerslider.h"

class PieChartSlider : public AbstractDividerSlider
{
    Q_OBJECT

public:
    explicit PieChartSlider(int number_of_dividers = 1, int total = 100, QWidget *parent = nullptr);

    QColor sectorColor(int index) const
        {return piechart_palette[index%piechart_palette.size()];}

    void setPiechartPalette(QVector<QColor> palette)
        {piechart_palette = palette;}

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *even) override;

    void paintEvent(QPaintEvent *event) override;

private:
    /* Internal types */
    struct Handle{
        virtual ~Handle() = default;

        static const int SIZE = 16;
        virtual int size() const = 0;
        virtual qreal radiusOffset() const
            {return 0;}

        bool is_pressed = false;
        int angle = 0;
    };

    struct DividerHandle : public Handle{
        int size() const override {return DividerHandle::SIZE;}
    };

    struct SectorHandle : public Handle{
        int size() const override {return SectorHandle::SIZE;}
        qreal radiusOffset() const override
            {return -DividerHandle::SIZE/2 - (collapse_level + 0.5)*SectorHandle::SIZE;}

        int collapse_level = 0;
        bool visible = false;
    };

    /* Handle handling */
    void moveHandles(int index, int value) override;
    void updateSectorHandels(int index, int value);

    /* Properties */
    QPoint pieCentre() const {return QPoint(width()/2, height()/2);}
    int radius() const;

    /* Convertion functions */
    int valueToAngle (int value) const;
    int angleToValue (int angle) const;

    QPointF angleToPosition(int angle, qreal radius) const;
    int positionToAngle(QPoint pos) const;

    /* Mouse input processing */
    void processSectorMouseInput(int index, QPoint mouse_pos);
    int angleFromMouse(int index, QPoint mouse_pos) const;

    void setDividerValueFromMouse(int index, QPoint mouse_pos);
    int stabilizedValue(int min_index, int max_index, int value) const;

    bool onHandle(const Handle& handle, QPoint mouse_pos) const;

    void setEmptySectorsCollapsed();

    /* Painting helpers */
    QRectF boundingRect(const Handle& handle) const;

    /*from Qt definition of 16 ticks per degree*/
    static const int ANGLE_TICKS_IN_CIRCLE = 360*16;

    //Left here to enable future use of the zero/total divider
    int zero_angle = 0;

    QVector<QColor> piechart_palette;

    QVector<DividerHandle> divider_handles;
    QVector<SectorHandle> sector_handles;

    int handle_start_angle;
};

#endif // PIECHARTSLIDER_H
