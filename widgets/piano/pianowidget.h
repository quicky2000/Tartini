/***************************************************************************
                          paino.h  -  description
                             -------------------
    begin                : 17 Mar 2005
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

#ifndef PIANO_H
#define PIANO_H

#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>
#include "drawwidget.h"

class PianoWidget:
public DrawWidget
{
  public:

    PianoWidget(QWidget * p_parent);
    ~PianoWidget() override;

    void paintEvent(QPaintEvent *) override;
  

    inline QSize sizeHint() const override;
    inline int currentNote();
    void setCurrentNote( int p_n
                       , float p_amount_pressed = 1.0
                       );
    inline void setNoNote();
    inline bool isNote();
    inline float amountPressed();
    inline void setAmountPressed(float p_amount_pressed);

    //-1=no note, 0=C, 1=C#, 2=D ... 11=B
    int m_current_note;

    float m_amount_pressed;
};

#include "pianowidget.hpp"

#endif // PIANO_H
// EOF
