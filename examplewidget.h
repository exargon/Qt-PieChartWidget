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

#pragma once

#include "piechartslider.h"
#include <QWidget>

class QSpinBox;

class ExampleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExampleWidget(QWidget *parent = nullptr);

private:
    PieChartSlider *pie_chart;

    QSpinBox* total;
    QVector<QSpinBox*> sector_inputs;
    QVector<QSpinBox*> divider_inputs;
};
