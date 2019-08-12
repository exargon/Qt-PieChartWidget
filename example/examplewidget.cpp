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

#include "examplewidget.h"

#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>

ExampleWidget::ExampleWidget(QWidget *parent) : QWidget(parent)
{
    /* Initilization constants */
    const int number_of_dividers = 5;
    const int initial_total = 100;
    const int max_total = 1000000;


    pie_chart = new PieChartSlider(number_of_dividers, initial_total);
    pie_chart->setFixedSize(300,300);

    /* Make spinboxes for dividers and connect input */
    for (int i = 0; i < pie_chart->numberOfDividers(); ++i){
        divider_inputs.push_back(new QSpinBox);
        divider_inputs[i]->setRange(0, max_total);
        divider_inputs[i]->setValue(pie_chart->dividerValue(i));
        connect(divider_inputs[i], qOverload<int>(&QSpinBox::valueChanged),
                pie_chart, [&, i](int value){pie_chart->setDividerValue(i, value);});
    }

    /* Make spinboxes for sectors and connect input */
    for (int i = 0; i < pie_chart->numberOfSectors(); ++i){
        sector_inputs.push_back(new QSpinBox);
        sector_inputs[i]->setRange(0, max_total);
        sector_inputs[i]->setValue(pie_chart->sectorValue(i));
        connect(sector_inputs.back(), qOverload<int>(&QSpinBox::valueChanged),
                pie_chart, [&, i](int value){pie_chart->setSectorValue(i, value);});
    }

    /* Connect output */
    connect(pie_chart, &PieChartSlider::dividerValueChanged,
            this, [this](int index, int value){divider_inputs[index]->setValue(value);});
    connect(pie_chart, &PieChartSlider::sectorValueChanged,
            this, [this](int index, int value){sector_inputs[index]->setValue(value);});


    QHBoxLayout *top_layout = new QHBoxLayout(this);
    top_layout->addWidget(pie_chart);

    /* Add labels for sector spinboxes */
    QGridLayout *values_display = new QGridLayout;
    for (int i = 0; i < sector_inputs.size(); ++i){
        values_display->addWidget(sector_inputs[i],i*2,1);
        QLabel *label = new QLabel;
        label->setStyleSheet( "* { background-color: " + pie_chart->sectorColor(i).name() + " }");
        values_display->addWidget(label, i*2,0);
    }
    for (int i = 0; i < divider_inputs.size(); ++i){
        values_display->addWidget(divider_inputs[i],i*2 + 1,0);
    }

    top_layout->addLayout(values_display);

    /* Add spinbox for total */
    total = new QSpinBox;
    total->setButtonSymbols(QAbstractSpinBox::NoButtons);
    total->setKeyboardTracking(false);
    total->setRange(0, max_total);
    total->setValue(pie_chart->total());
    connect(total, qOverload<int>(&QSpinBox::valueChanged), pie_chart, &PieChartSlider::setTotal);
    connect(pie_chart, &PieChartSlider::totalChanged, total, &QSpinBox::setValue);

    QVBoxLayout* display_total = new QVBoxLayout;
    display_total->addSpacerItem(new QSpacerItem(0,150));
    display_total->addWidget(new QLabel("Total"), 0, Qt::AlignCenter);
    display_total->addWidget(total, 0, Qt::AlignCenter);
    display_total->addSpacerItem(new QSpacerItem(0,150));
    top_layout->addLayout(display_total);
}
