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

#ifndef ABSTRACTDIVIDERSLIDER_H
#define ABSTRACTDIVIDERSLIDER_H

#include <QWidget>

class AbstractDividerSlider : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractDividerSlider(int number_of_dividers = 1, int total = 100, QWidget *parent = nullptr);

    int numberOfDividers() const {return number_of_dividers;}
    int numberOfSectors() const  {return number_of_dividers + 1;}

    int total() const                 {return total_value;}
    int dividerValue(int index) const {return divider_values[index];}
    int sectorValue(int index) const;

    int dividerMaximum(int index) const;
    int dividerMinimum(int index) const;

signals:
    void totalChanged(int value);
    void sectorValueChanged(int index, int value);
    void dividerValueChanged(int index, int value);

public slots:
    void setTotal(int value);
    void setSectorValue(int index, int value);
    void setDividerValue(int index, int value);

protected:
    virtual void moveHandles(int index, int value) = 0;

    void setDividersInRange(int first, int last, int value);
    void setDividersBoundness(int first, bool boundness);

private:
    int total_value;
    int number_of_dividers;

    QVector<int> divider_values;
    QVector<bool> dividers_bound;
};

#endif // ABSTRACTDIVIDERSLIDER_H
