/***************************************************************************
                          sound_stream.h  -  description
                             -------------------
    begin                : Sat Nov 27 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef SOUND_STREAM_H
#define SOUND_STREAM_H

//class FBuffer;
class QStringList;

class SoundStream
{
  public:

    typedef enum class OpenMode { F_NONE, F_READ, F_WRITE, F_RDWR} t_open_mode;

    inline SoundStream();
    virtual inline ~SoundStream();

    inline int sample_size() const;
    inline int frame_size() const;

    virtual long read_bytes( void * p_data
                           , long p_length
                           ) = 0;

    virtual long read_frames( void * p_data
                            , long p_length
                            ) = 0;

    virtual long write_bytes( void * p_data
                            , long p_length
                            ) = 0;

    virtual long write_frames(void * p_data
                             , long p_length
                             ) = 0;

    inline virtual long wait_bytes(long /*length*/);

    inline virtual long wait_frames(long /*length*/);


    virtual int writeFloats( float ** p_channel_data
                           , int p_length
                           , int p_ch
                           );

    virtual int readFloats( float ** p_channel_data
                          , int p_length
                          , int p_ch
                          );

    virtual int writeReadFloats( float ** p_out_channel_data
                               , int p_out_ch
                               , float ** p_in_channel_data
                               , int p_in_ch
                               , int p_length
                               );

    inline int get_frequency() const;
    inline int get_channels() const;
    inline int get_bits() const;

    inline bool is_write_mode() const;

  protected:

    inline void set_frequency(int p_frequency);
    inline void set_channels(int p_channels);
    inline void set_bits(int p_bits);
    inline void set_mode(t_open_mode p_mode);
    inline t_open_mode get_mode() const;
    inline bool is_read_mode() const;

  private:

    int m_freq;
    int m_channels;
    int m_bits;
    t_open_mode m_mode;
};

#include "sound_stream.hpp"

#endif // SOUND_STREAM_H
// EOF