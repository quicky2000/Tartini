/***************************************************************************
                          pianoview.h  -  description
                             -------------------
    begin                : 24 Mar 2005
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
#ifndef PIANOVIEW_H
#define PIANOVIEW_H

#include "viewwidget.h"
#include <QResizeEvent>

class PianoWidget;

class PianoView : public ViewWidget
{
  Q_OBJECT

  public:

    PianoView( int p_view_id
             , QWidget *p_parent = nullptr
             );
    virtual ~PianoView();

    void resizeEvent(QResizeEvent *);

    QSize sizeHint() const;

  public slots:
    void changeKey();

  private:
    PianoWidget * m_piano_widget;

};


#endif // PIANOVIEW_H
// EOF

