/***************************************************************************
                          audio_stream.h  -  linux audio routines
                             -------------------
    begin                : 2003
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
***************************************************************************/
#ifndef AUDIO_STREAM_H
#define AUDIO_STREAM_H

#include <stdio.h>
#include "RtAudio.h"
#include "sound_stream.h"
#include "RingBuffer.h"

class AudioStream: public SoundStream
{
  public:

    AudioStream();
    virtual ~AudioStream();

    int bufferSize() { return m_buffer_size; }
    int numBuffers() { return m_num_buffers; }
    int totalBufferFrames() { return m_buffer_size * m_num_buffers; }
    int inTotalBufferFrames() { return totalBufferFrames(); }
    int outTotalBufferFrames() { return totalBufferFrames(); }

    void close();

    int open( t_open_mode p_mode
            , int p_freq = 44100
            , int p_channels = 2
            , int p_bits = 16
            , int p_buffer_size = 2048
            );

    //note: use the writeFloats or readFloats instead
    long read_bytes( void * /*data*/
                   , long /*length*/
                   )
    {
        return 0;
    } //not implemented

    long read_frames( void * /*data*/
                    , long /*length*/
                    )
    {
        return 0;
    } //not implemented

    long write_bytes( void * /*data*/
                    , long /*length*/
                    )
    {
        return 0;
    } //not implemented

    long write_frames( void * /*data*/
                     , long /*length*/
                     )
    {
        return 0;
    } //not implemented

    int writeFloats( float ** p_channel_data
                   , int p_length
                   , int p_ch
                   );
    int readFloats( float ** p_channel_data
                  , int p_length
                  , int p_ch
                  );


    /**
       This requires that inChannelData and outChannelData have the same number of channels
       and the same length, and this length is the same size as the AudioStream buffer
       @param p_out_channel_data The data to write out to the sound card
       @param p_in_channel_data The data read back from the sound card
       @param p_length The amount of data per channel. This has to be the same for in and out data
    */
    int writeReadFloats( float ** p_out_channel_data
                       , int p_out_channel
                       , float ** p_in_channel_data
                       , int p_in_channel
                       , int p_length
                       );

    static QStringList getInputDeviceNames();
    static QStringList getOutputDeviceNames();

    /**
       @param p_device_name The name of a device as given from get*DeviceNames
       @return The device number that matches the name, or -1 if the device name is not found
    */
    static int getDeviceNumber( const char * p_device_name
                              , bool p_input
                              );

  private:

    /**
     * This static function is called when data is availble or required.  The p_user_data is a pointer to the AudioStream object.
     */
    static int callback( void * p_output_buffer
                       , void * p_input_buffer
                       , unsigned int p_n_buffer_frames
                       , double p_stream_time
                       , RtAudioStreamStatus p_status
                       , void * p_user_data
                       );

    /**
     * This instance method is called when data is available or required.
     * It copies data between the audio devices and the input and output RingBuffers.
     */
    int callback( void * p_output_buffer
                , void * p_input_buffer
                , unsigned int p_n_buffer_frames
                , double p_stream_time
                , RtAudioStreamStatus p_status
                );
    
    int m_buffer_size; //in frames
    int m_num_buffers; //ignored
    RtAudio * m_audio;
    QMutex m_in_mutex;
    QMutex m_out_mutex;
    RingBuffer<float> m_in_buffer;
    RingBuffer<float> m_out_buffer;

};
#endif // AUDIO_STREAM_H
// EOF
