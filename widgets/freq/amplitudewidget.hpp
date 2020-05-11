/*    This file is part of Tartini
      Copyright (C) 2016  Julien Thevenon ( julien_thevenon at yahoo.fr )
      Copyright (C) 2004 by Philip McLeod ( pmcleod@cs.otago.ac.nz )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

//------------------------------------------------------------------------------
QSize AmplitudeWidget::sizeHint()const
{
    return QSize(400, 100);
}

//------------------------------------------------------------------------------
double AmplitudeWidget::leftTime()const
{
    return GData::getUniqueInstance().getView().viewLeft();
}

//------------------------------------------------------------------------------
double AmplitudeWidget::rightTime()const
{
    return GData::getUniqueInstance().getView().viewRight();
}

//------------------------------------------------------------------------------
double AmplitudeWidget::timeWidth()const
{
    return GData::getUniqueInstance().getView().viewTotalTime();
}

//------------------------------------------------------------------------------
const double & AmplitudeWidget::range()const
{
    return m_range;
}

//------------------------------------------------------------------------------
double AmplitudeWidget::maxOffset()const
{
    return 1.0 - m_range;
}

//------------------------------------------------------------------------------
const double & AmplitudeWidget::offset()const
{
    return m_offset;
}

//------------------------------------------------------------------------------
const double & AmplitudeWidget::offsetInv()const
{
    return m_offset_inv;
}

//EOF
