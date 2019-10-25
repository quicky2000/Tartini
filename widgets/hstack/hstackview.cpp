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

#include <qlayout.h>
#include <qpushbutton.h>
#include <qsizegrip.h>
#include <qsplitter.h>
#include <qtooltip.h>
#include <q3grid.h>
#include <qwt_wheel.h>
#include <qcursor.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <Q3Frame>

//------------------------------------------------------------------------------
HStackView::HStackView( int p_view_id
                      , QWidget * p_parent
                      )
: ViewWidget( p_view_id, p_parent)
{
    setCaption("Harmonic Stack");

    Q3BoxLayout * l_main_layout = new Q3HBoxLayout(this);

    Q3BoxLayout * l_left_layout = new Q3VBoxLayout(l_main_layout);
    Q3BoxLayout * l_right_layout = new Q3VBoxLayout(l_main_layout);

    Q3Grid * l_wave_frame = new Q3Grid(1, this);
    l_wave_frame->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
    l_left_layout->addWidget(l_wave_frame);
    m_h_stack_widget = new HStackWidget(l_wave_frame);

    Q3BoxLayout * l_bottom_layout = new Q3HBoxLayout(l_left_layout);

    QwtWheel * l_db_range_wheel = new QwtWheel(this);
    l_db_range_wheel->setOrientation(Qt::Vertical);
    l_db_range_wheel->setWheelWidth(14);
#if QWT_VERSION >= 0x060000
    l_db_range_wheel->setRange(5, 160.0);
    l_db_range_wheel->setSingleStep(0.1);
    // Multiplicator value is 1000 = 100 * 0.1
    l_db_range_wheel->setPageStepCount(1000);
#else
    l_db_range_wheel->setRange(5, 160.0, 0.1, 100);
#endif // QWT_VERSION >= 0x060000
    l_db_range_wheel->setValue(100);
    QToolTip::add(l_db_range_wheel, "Zoom dB range vertically");
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
#else
    l_window_size_wheel->setRange(32, 1024, 2, 1);
#endif // QWT_VERSION >= 0x060000
    l_window_size_wheel->setValue(128);
    QToolTip::add(l_window_size_wheel, "Zoom windowsize horizontally");
    l_bottom_layout->addWidget(l_window_size_wheel, 0);
    l_bottom_layout->addStretch(2);

    connect(l_db_range_wheel, SIGNAL(valueChanged(double)), m_h_stack_widget, SLOT(setDBRange(double)));
    connect(l_db_range_wheel, SIGNAL(valueChanged(double)), m_h_stack_widget, SLOT(update()));

    connect(l_window_size_wheel, SIGNAL(valueChanged(double)), m_h_stack_widget, SLOT(setWindowSize(double)));
    connect(l_window_size_wheel, SIGNAL(valueChanged(double)), m_h_stack_widget, SLOT(update()));
}

//------------------------------------------------------------------------------
HStackView::~HStackView(void)
{
    delete m_h_stack_widget;
}

//------------------------------------------------------------------------------
QSize HStackView::sizeHint(void) const
{
    return QSize(300, 200);
}

// EOF
