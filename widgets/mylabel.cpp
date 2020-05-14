/***************************************************************************
                          mylabel.cpp  -  description
                             -------------------
    begin                : 29/6/2005
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

#include "mylabel.h"
#include <QPixmap>
#include <QPaintEvent>

//------------------------------------------------------------------------------
MyLabel::MyLabel(const std::string & p_text
                ,QWidget * p_parent
                ,const std::string & p_name
                )
: DrawWidget(p_parent, p_name)
, m_text(p_text)
, m_font_height(0)
, m_text_width(0)
, m_size_computed(false)
{
   setObjectName(QString::fromStdString(p_name));
   this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
   compute_text_width();
}

//------------------------------------------------------------------------------
void MyLabel::paintEvent( QPaintEvent * )
{
    beginDrawing(false);
    if(!m_size_computed)
    {
        compute_text_width();
    }
    QPalette l_palette;
    fillBackground(l_palette.color(QPalette::Window));
    get_painter().drawText(4, m_font_height - 2, QString::fromStdString(m_text));
    endDrawing();
}

// EOF
