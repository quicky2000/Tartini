/***************************************************************************
                          volumemeterview.h  -  description
                             -------------------
    begin                : Tue Dec 21 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef VOLUMEMETERVIEW_H
#define VOLUMEMETERVIEW_H

#include "viewwidget.h"
#include <QResizeEvent>
class VolumeMeterWidget;

class VolumeMeterView: public ViewWidget
{
  Q_OBJECT

 public:
  VolumeMeterView(int viewID_, QWidget * parent = nullptr);
  ~VolumeMeterView() override;

  void resizeEvent(QResizeEvent *) override;

  QSize sizeHint() const override;
    
 private:
  VolumeMeterWidget *m_volume_meter_widget;
};
#endif // VOLUMEMETERVIEW_H
// EOF
