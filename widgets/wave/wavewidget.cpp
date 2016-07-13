/***************************************************************************
                          wavewidget.cpp  -  description
                             -------------------
    begin                : Mon Mar 14 2005
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
#include <qpixmap.h>
#include <qpainter.h>
#include <q3memarray.h>
#include <QPaintEvent>

#include "wavewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

//------------------------------------------------------------------------------
WaveWidget::WaveWidget(QWidget *parent):
  DrawWidget(parent)
{
  setZoomY(1.0);
}

//------------------------------------------------------------------------------
WaveWidget::~WaveWidget(void)
{
}

//------------------------------------------------------------------------------
void WaveWidget::setZoomY(const double & zoomY_)
{
  if(_zoomY != zoomY_)
    {
      _zoomY = zoomY_;
      emit zoomYChanged(_zoomY);
    }
}

//------------------------------------------------------------------------------
void WaveWidget::paintEvent(QPaintEvent *)
{
  Channel * active = gdata->getActiveChannel();

  beginDrawing(false);

  if(active)
    {
      active->lock();
      AnalysisData * data = active->dataAtCurrentChunk();
      int centerX = width() / 2;
      if(data)
	{
	  double freq = data->getFundamentalFreq();
	  double period = double(active->rate()) / freq;
	  double numPeriods = double(active->size()) / period;
	  //pixels per period
	  double scaleX = period * double(width()) / double(active->size());
      
	  //draw altinating background color indicating period
	  if(gdata->getView().backgroundShading() && period > 4.0 && period < double(active->get_nsdf_data().size()))
	    {
	      int n = int(ceil(double(centerX) / scaleX));
	      int j, x;
	      p.setPen(Qt::NoPen);
	      QColor color1 = colorBetween(gdata->backgroundColor(), gdata->shading1Color(), data->getCorrelation());
	      QColor color2 = colorBetween(gdata->backgroundColor(), gdata->shading2Color(), data->getCorrelation());
	      for(j = -n; j < n; j++)
		{
		  x = centerX + toInt(scaleX * double(j));
		  p.setBrush((j % 2) ? color1 : color2);
		  p.drawRect(x, 0, toInt(scaleX * double(j + 1)) - toInt(scaleX * double(j)), height());
		}
	      p.setPen(colorBetween(gdata->backgroundColor(), Qt::black, 0.3 * data->getCorrelation()));
	      for(j = -n; j < n; j++)
		{
		  x = centerX + toInt(scaleX * j);
		  p.drawLine(x, 0, x, height());
		}
	    }
	  else
	    {
	      clearBackground();
	    }
	  QString numPeriodsText;
	  numPeriodsText.sprintf("# Periods = %lf", numPeriods);
	  p.setPen(Qt::black);
	  p.drawText(5, 15, numPeriodsText);
	}
      else
	{
	  clearBackground();
	}
    }
  else
    {
      clearBackground();
    }

  double dh2 = double(height() - 1) / 2.0;
    
  //draw the center line
  p.setPen(QPen(colorBetween(colorGroup().background(), Qt::black, 0.3), 0));
  p.drawLine(0, toInt(dh2), width(), toInt(dh2));

    
  if(active)
    {
      //draw the waveform
      //only do every second pixel (for speed)
      int w = width() / 2;
      if(int(pointArray.size()) != w)
	{
	  pointArray.resize(w);
	}
      double scaleY = dh2 * zoomY();

      //Use Bresenham's algorithm in 1d to choose the points to draw
      const Array1d<float> &filteredData = active->get_filtered_input();

      int intStep = int(filteredData.size() / w);
      int remainderStep = filteredData.size() - (intStep * w);
      int pos = 0;
      int remainder = 0;
      for(int j = 0; j < w; j++, pos += intStep, remainder += remainderStep)
	{
	  if(remainder >= w)
	    {
	      pos++;
	      remainder -= w;
	    }
	  myassert(pos < filteredData.size());
	  if(pos >= filteredData.size())
	    {
	      printf("pos = %d, filteredData.size()=%d\n", pos, filteredData.size());
	    }
	  pointArray.setPoint(j, j*2, toInt(dh2 - (filteredData.at(pos))*scaleY));
	}
      p.setPen(QPen(active->get_color(), 0));
      p.drawPolyline(pointArray);
      active->unlock();
    }
  endDrawing();
}

//------------------------------------------------------------------------------
QSize WaveWidget::sizeHint(void) const
{
  return QSize(500, 128);
}

//------------------------------------------------------------------------------
const double & WaveWidget::zoomY(void) const
{
  return _zoomY;
}

// EOF
