/***************************************************************************
                          hstackview.cpp  -  description
                             -------------------
    begin                : Mon Jan 10 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "hstackview.h"
#include "hstackwidget.h"
#include "channel.h" 
#include "analysisdata.h"
#include "useful.h" 
#include "myscrollbar.h"

#include <QLayout>
#include <QPushButton>
#include <QSizeGrip>
#include <QSplitter>
#include <QToolTip>
#include <qwt_wheel.h>
#include <QCursor>
#include <QComboBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

//------------------------------------------------------------------------------
HStackView::HStackView( int p_view_id
                      , QWidget * p_parent
                      )
: ViewWidget( p_view_id, p_parent)
{
    setWindowTitle("Harmonic Stack");

    QHBoxLayout * l_main_layout = new QHBoxLayout(this);

    QVBoxLayout * l_left_layout = new QVBoxLayout();
    l_main_layout->addLayout(l_left_layout);
    QVBoxLayout * l_right_layout = new QVBoxLayout();
    l_main_layout->addLayout(l_right_layout);

    QFrame * l_wave_frame = new QFrame(this);
    l_wave_frame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    l_left_layout->addWidget(l_wave_frame);
    m_h_stack_widget = new HStackWidget(l_wave_frame);

    QHBoxLayout * l_bottom_layout = new QHBoxLayout();
    l_left_layout->addLayout(l_bottom_layout);

    QwtWheel * l_db_range_wheel = new QwtWheel(this);
    l_db_range_wheel->setOrientation(Qt::Vertical);
    l_db_range_wheel->setWheelWidth(14);
#if QWT_VERSION >= 0x060000
    l_db_range_wheel->setRange(5, 160.0);
    l_db_range_wheel->setSingleStep(0.1);
    // Multiplicator value is 1000 = 100 * 0.1
    l_db_range_wheel->setPageStepCount(1000);
    l_db_range_wheel->setMass(0.1);
#else
    l_db_range_wheel->setRange(5, 160.0, 0.1, 100);
#endif // QWT_VERSION >= 0x060000
    l_db_range_wheel->setValue(100);
    l_db_range_wheel->setToolTip("Zoom dB range vertically");
    l_right_layout->addWidget(l_db_range_wheel, 0);
    l_right_layout->addStretch(2);

    QwtWheel * l_window_size_wheel = new QwtWheel(this);
    l_window_size_wheel->setOrientation(Qt::Horizontal);
    l_window_size_wheel->setWheelWidth(14);
#if QWT_VERSION >= 0x060000
    l_window_size_wheel->setRange(32, 1024);
    l_window_size_wheel->setSingleStep(2);
    // Sould be 0.5 but put to 1 because 0 zero value disable page stepping
    l_window_size_wheel->setPageStepCount(1);
    l_window_size_wheel->setMass(0.1);
#else
    l_window_size_wheel->setRange(32, 1024, 2, 1);
#endif // QWT_VERSION >= 0x060000
    l_window_size_wheel->setValue(128);
    l_window_size_wheel->setToolTip("Zoom windowsize horizontally");
    l_bottom_layout->addWidget(l_window_size_wheel, 0);
    l_bottom_layout->addStretch(2);

    connect(l_db_range_wheel, SIGNAL(valueChanged(double)), m_h_stack_widget, SLOT(setDBRange(double)));
    connect(l_db_range_wheel, SIGNAL(valueChanged(double)), m_h_stack_widget, SLOT(update()));

    connect(l_window_size_wheel, SIGNAL(valueChanged(double)), m_h_stack_widget, SLOT(setWindowSize(double)));
    connect(l_window_size_wheel, SIGNAL(valueChanged(double)), m_h_stack_widget, SLOT(update()));
}

//------------------------------------------------------------------------------
HStackView::~HStackView()
{
    delete m_h_stack_widget;
}

//------------------------------------------------------------------------------
QSize HStackView::sizeHint() const
{
    return QSize(300, 200);
}

//------------------------------------------------------------------------------
void HStackView::resizeEvent(QResizeEvent *)
{
  m_h_stack_widget->resize(size());
}
// EOF
