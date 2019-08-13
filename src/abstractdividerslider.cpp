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

#include "abstractdividerslider.h"

#include <QtMath>

AbstractDividerSlider::AbstractDividerSlider(int number_of_dividers, int total, QWidget *parent)
    : QWidget(parent), total_value(total), number_of_dividers(number_of_dividers)
{
    /* Round for more equal dividing at low total_value */
    int divider_spacing = static_cast<int>(round(static_cast<qreal>(total_value) / numberOfSectors()));

    for (int i = 1; i <= number_of_dividers; ++i){
        divider_values.push_back(std::min(i*divider_spacing, total_value));
    }

    sectors_collapsed.fill(false,numberOfSectors());
}

void AbstractDividerSlider::setTotal(int total){
    if (total == total_value || total < 1) return;

    qreal ratio = static_cast<qreal>(total)/total_value;
    total_value = total;

    for (int index = 0; index < number_of_dividers; ++index){
        divider_values[index] = static_cast<int>(std::round(divider_values[index]*ratio));
        emit dividerValueChanged(index, divider_values[index]);
        moveHandles(index, divider_values[index]);
    }
    for (int index = 0; index < numberOfSectors(); ++index){
        emit sectorValueChanged(index, sectorValue(index));
    }

    emit totalChanged(total);
    update();
}

void AbstractDividerSlider::setDividerValue(int index, int value){
    if (value == dividerValue(index)) return;

    int min = index;
    for (; min > 0 && sectors_collapsed[min]; --min);
    int max = index;
    for (; max < numberOfDividers() - 1 && sectors_collapsed[max + 1]; ++max);

    setDividersInRange(min, max, value);
    update();
}

void AbstractDividerSlider::setDividersInRange(int first, int last, int value){
    int minimum = dividerMinimum(first);
    int maximum = dividerMaximum(last);

    if (value < minimum){
        value = minimum;

    } else if (value > maximum){
        value = maximum;
    }

    for (int i = first; i <= last; ++i){
        divider_values[i] = value;
    }

    for (int i = first; i <= last; ++i){
        moveHandles(i, value);
        emit dividerValueChanged(i, value);
        emit sectorValueChanged(i, sectorValue(i));
    }
    emit sectorValueChanged(last + 1, maximum - value);
}

int AbstractDividerSlider::dividerMinimum(int index) const{
    for (; index > 0 && sectors_collapsed[index]; --index);
    return (index == 0) ? 0 : divider_values[index - 1];
}

int AbstractDividerSlider::dividerMaximum(int index) const{
    for (; index < numberOfDividers() - 1 && sectors_collapsed[index + 1]; ++index);
    return (index == numberOfDividers() - 1) ? total_value : divider_values[index + 1];
}

void AbstractDividerSlider::setSectorValue(int index, int value){
    if (value == sectorValue(index)) return;
    sectors_collapsed[index] = false;

    if (index == number_of_dividers){
        setDividerValue(index - 1, total_value - value);
    } else if (index == 0){
        setDividerValue(index, value);
    } else {     
        setDividerValue(index, divider_values[index - 1] + value);
    }
}

int AbstractDividerSlider::sectorValue(int index) const{
    if (index == 0){
        return divider_values[index];
    } else if (index == number_of_dividers){
        return total_value - divider_values[index - 1];
    } else {
        return divider_values[index] - divider_values[index - 1];
    }
}

void AbstractDividerSlider::setSectorCollapsed(int index, bool is_collapsed){
    sectors_collapsed[index] = is_collapsed;
}
