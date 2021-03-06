/***************************************************************************
                          audio_thread.cpp  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
***************************************************************************/
#include <stdio.h>
#include <math.h>
#include <QRect>
#include <QMutex>

#include "audio_thread.h"
#include "audio_stream.h"
#include "sound_file_stream.h"
#include "main.h"
#include "gdata.h"
#include "Filter.h"
#include "bspline.h"
#include "soundfile.h"
#include "channel.h"

//------------------------------------------------------------------------------
AudioThread::AudioThread()
: m_play_sound_file(nullptr)
, m_rec_sound_file(nullptr)
, m_stopping(false)
, m_fast_update_count(0)
, m_slow_update_count(0)
, m_frame_num(0)
, m_sleep_count(0)
{
    //printf("Warning - audio thread created with no sound file specified.\n");
}

//------------------------------------------------------------------------------
void AudioThread::start()
{
    fprintf(stderr, "Warning - audio thread created with no sound file specified.\n");
    m_play_sound_file = nullptr;
    m_rec_sound_file = nullptr;
    m_stopping = false;
    QThread::start(QThread::HighPriority);
}

//------------------------------------------------------------------------------
void AudioThread::start( SoundFile *p_s_play
                       , SoundFile *p_s_rec
                       )
{
    fprintf(stderr, "Audio thread created for\n");
    if(p_s_play)
    {
        fprintf(stderr, "Playing file %s\n", p_s_play->getFileName().c_str());
        fflush(stdout);
    }
    if(p_s_rec)
    {
        fprintf(stderr, "Recording file %s\n", p_s_rec->getFileName().c_str());
        fflush(stdout);
    }
    m_play_sound_file = p_s_play;
    m_rec_sound_file = p_s_rec;
    m_stopping = false;
    QThread::start(QThread::HighPriority/*TimeCriticalPriority*/);
}

//------------------------------------------------------------------------------
void AudioThread::stop()
{
    m_stopping = true;
}

//------------------------------------------------------------------------------
void AudioThread::stopAndWait()
{
    stop();
    wait();
}

//------------------------------------------------------------------------------
void AudioThread::run()
{
#ifndef WINDOWS
    //setup stuff for multi-threaded profiling
    g_profiler_ovalue.it_interval.tv_sec = 0;
    g_profiler_ovalue.it_interval.tv_usec = 0;
    g_profiler_ovalue.it_value.tv_sec = 0;
    g_profiler_ovalue.it_value.tv_usec = 0;
    setitimer(ITIMER_PROF, &g_profiler_value, &g_profiler_ovalue); //for running multi-threaded profiling
#endif // WINDOWS

    m_fast_update_count = 0;
    m_slow_update_count = 0;
    m_frame_num = 0;

    //read to the 1 chunk befor time 0
    if(GData::getUniqueInstance().isSoundModeRecording())
    {
        GData::getUniqueInstance().setDoingActiveAnalysis(true);
        myassert(m_rec_sound_file->isFirstTimeThrough() == true);
        m_rec_sound_file->recordChunk(m_rec_sound_file->offset());
    }
  
    QApplication::postEvent(g_main_window, new QEvent(static_cast<QEvent::Type>(m_sound_started_event_id)));
    GData::getUniqueInstance().setRunning(GData::RunningMode::STREAM_FORWARD);

    while(!m_stopping)
    {
        if(doStuff() == 0)
        {
            break;
        }
#ifdef WINDOWS
        Sleep(0); //make other threads do some stuff
#else // WINDOWS
        sleep(0); //make other threads do some stuff
#endif // WINDOWS
    }

    GData::getUniqueInstance().setRunning(GData::RunningMode::STREAM_STOP);

    if(GData::getUniqueInstance().isSoundModeRecording())
    {
        GData::getUniqueInstance().setDoingActiveAnalysis(false);
        m_rec_sound_file->setFirstTimeThrough(false);
        m_rec_sound_file->rec2play();
        GData::getUniqueInstance().setSoundMode(GData::SoundMode::SOUND_PLAY);
    }

    if(GData::getUniqueInstance().getAudioStream())
    {
        delete GData::getUniqueInstance().getAudioStream();
        GData::getUniqueInstance().setAudioStream(nullptr);
    }
    m_play_sound_file = nullptr;
    m_rec_sound_file = nullptr;
  
    QApplication::postEvent(g_main_window, new QEvent(static_cast<QEvent::Type>(m_sound_stopped_event_id)));
}

//------------------------------------------------------------------------------
int AudioThread::doStuff()
{
    int l_force_update = false;
    if(GData::getUniqueInstance().getRunning() == GData::RunningMode::STREAM_PAUSE)
    {
        msleep(20);
        //paused
        return 1;
    }
    if(!m_play_sound_file && !m_rec_sound_file)
    {
        return 0;
    }

    ++m_frame_num;
	
    //update one frame before pausing again
    if(GData::getUniqueInstance().getRunning() == GData::RunningMode::STREAM_UPDATE)
    {
        //update then pause
        GData::getUniqueInstance().setRunning(GData::RunningMode::STREAM_PAUSE);
        l_force_update = true;
    }
    if(GData::getUniqueInstance().getRunning() != GData::RunningMode::STREAM_FORWARD)
    {
        return 0;
    }
  
    //This is the main block of code for reading or write the next chunk to the soundcard or file
    if(GData::getUniqueInstance().getSoundMode() == GData::SoundMode::SOUND_PLAY)
    {
        myassert(m_play_sound_file);
        if(!m_play_sound_file->playChunk())
        {
            //end of file
            QApplication::postEvent(g_main_window, new QEvent(static_cast<QEvent::Type>(m_update_slow_event_id)));
            return 0; //stop the audio thread playing
        }
        if(!GData::getUniqueInstance().getAudioStream())
        {
            if(++m_sleep_count % 4 == 0)
            {
                int l_sleepval = (1000 * m_play_sound_file->framesPerChunk()) / m_play_sound_file->rate();
                msleep(l_sleepval * 4);
            }
        }
    }
    else if(GData::getUniqueInstance().getSoundMode() == GData::SoundMode::SOUND_REC)
    {
        // SOUND_REC
        int l_buffer_chunks = GData::getUniqueInstance().getAudioStream()->inTotalBufferFrames() / m_rec_sound_file->framesPerChunk();
        if(m_frame_num > l_buffer_chunks)
        {
            m_rec_sound_file->recordChunk(m_rec_sound_file->framesPerChunk());
        }
    }
    else if(GData::getUniqueInstance().getSoundMode() == GData::SoundMode::SOUND_PLAY_REC)
    {
        int l_buffer_chunks = GData::getUniqueInstance().getAudioStream()->inTotalBufferFrames() / m_rec_sound_file->framesPerChunk();
        if(m_frame_num > l_buffer_chunks)
        {
            if(!SoundFile::playRecordChunk(m_play_sound_file, m_rec_sound_file))
            {
                //end of file
                QApplication::postEvent(g_main_window, new QEvent(static_cast<QEvent::Type>(m_update_slow_event_id)));
                return 0; //stop the audio thread playing
            }
        }
    }

    //Set some flags to cause an update of views, every now and then
    m_fast_update_count++;
    m_slow_update_count++;

    int l_slow_update_after = toInt(double(GData::getUniqueInstance().slowUpdateSpeed()) / 1000.0 / curSoundFile()->timePerChunk());
    int l_fast_update_after = toInt(double(GData::getUniqueInstance().fastUpdateSpeed()) / 1000.0 / curSoundFile()->timePerChunk());
    if(!GData::getUniqueInstance().needUpdate())
    {
        if((m_slow_update_count >= l_slow_update_after) || l_force_update)
        {
            GData::getUniqueInstance().setNeedUpdate(true);
            m_fast_update_count = 0;
            m_slow_update_count = 0;
            QApplication::postEvent(g_main_window, new QEvent(static_cast<QEvent::Type>(m_update_slow_event_id)));
        }
        else if(m_fast_update_count >= l_fast_update_after)
        {
            GData::getUniqueInstance().setNeedUpdate(true);
            m_fast_update_count = 0;
            QApplication::postEvent(g_main_window, new QEvent(static_cast<QEvent::Type>(m_update_fast_event_id)));
        }
    }
    GData::getUniqueInstance().doChunkUpdate();

    return 1;
}

//EOF
