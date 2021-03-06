/***************************************************************************
                          gdata.h  -  
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
#ifndef GDATA_H
#define GDATA_H

#define TARTINI_NAME_STR      "Tartini1.2.1"
#define TARTINI_VERSION_STR   "1.2.1"
//#define TARTINI_VERSION       1.2

#include <Qt>
#include <QApplication>
#include <QObject>

#include <QMutex>
#include <QPixmap>

#include <vector>
//#include "sound_file_stream.h"
//#include "audio_stream.h"
#include "audio_thread.h"
//#include "chirp_xform.h"
//#include "settings.h"
#include <QSettings>

#ifndef WINDOWS
#include <sys/time.h>
#endif
#include <QColor>
#include "array2d.h"
#include "useful.h"
#include "view.h"
#include "analysisdata.h"
#include "music_scale.h"

#ifndef WINDOWS
//for multi-threaded profiling
extern struct itimerval g_profiler_value;
extern struct itimerval g_profiler_ovalue;
#endif

typedef enum class AnalysisModes { MPM, AUTOCORRELATION, MPM_MODIFIED_CEPSTRUM } t_analysis_modes;

class FBuffer;
class SoundStream;
class SoundFileStream;
class AudioStream;
class FWinFunc;
class Filter;
class SoundFile;
class Channel;

class GData : public QObject
{
  Q_OBJECT

  public:
    
    enum class RunningMode
    { STREAM_STOP
    , STREAM_FORWARD
    , STREAM_PAUSE
    , STREAM_UPDATE
    };

    enum class SoundMode
    { SOUND_PLAY
    , SOUND_REC
    , SOUND_PLAY_REC
    };
    
    typedef enum class SavingModes
    { ALWAYS_ASK
    , NEVER_SAVE
    , ALWAYS_SAVE
    } t_saving_modes;

    inline static
    GData & getUniqueInstance();

    inline static
    void createUniqueInstance();

    inline static
    void deleteUniqueInstance();

    inline
    void set_play_or_record(bool p_value);

    /**
       Empty methods just defined to avoid unused parameters warning
       assuming that will be removed at ocmpile time during optimisation
       phase
    */
    inline void do_nothing()const;
    inline bool settingsContains(const std::string & p_key)const;
    inline int getSettingsValue( const std::string & p_key
                               , const int & p_default_value
                               )const;
    inline bool getSettingsValue( const std::string & p_key
                                , const bool & p_default_value
                                )const;
  inline std::string getSettingsValue( const std::string & p_key
                                     , const std::string & p_default_value
                                     )const;
  inline QPoint getSettingsValue( const std::string & p_key
                                , const QPoint & p_default_value
                                )const;
  inline QSize getSettingsValue( const std::string & p_key
                               , const QSize & p_default_value
                               )const;
  inline void setSettingsValue( const std::string & p_key
                              , const std::string & p_value
                              );
  inline void setSettingsValue( const std::string & p_key
                              , const int & p_value
                              );
  inline void setSettingsValue( const std::string & p_key
                              , const QPoint & p_value
                              );
  inline void setSettingsValue( const std::string & p_key
                              , const QSize & p_value
                              );
  inline void clearSettings();
  inline void syncSettings();
  inline int getSettingsIntValue(const std::string & p_key)const;
  inline bool getSettingsBoolValue(const std::string & p_key)const;
  inline std::string getSettingsStringValue(const std::string & p_key)const;

  inline bool isSoundModeRecording()const;
  inline SoundMode getSoundMode()const;
  inline void setSoundMode(const SoundMode & p_mode);

  inline AudioStream * getAudioStream()const;
  inline void setAudioStream(AudioStream * p_audio_stream);

  inline bool needUpdate()const;
  inline void setNeedUpdate(bool p_need_update);

  inline const AudioThread & getAudioThread()const;
  inline void stopAndWaitAudioThread();

  inline void setRunning(RunningMode p_running);
  inline RunningMode getRunning()const;

  inline size_t getChannelsSize()const;
  inline Channel * getChannelAt(size_t p_index)const;

  inline const View & getView()const;
  inline View & getView();

  void setActiveChannel(Channel * p_to_active);
  inline Channel * getActiveChannel();
  SoundFile * getActiveSoundFile();

  inline QPixmap * drawingBuffer();

  /**
     Allows you to specify the leftmost time a file starts
   */
  void setLeftTime(double p_x);

  /**
     Allows you to specify the rightmost time a file starts
   */
  void setRightTime(double p_x);

  /**
     Returns the leftmost time a file starts
  */
  inline const double & leftTime()const;

  /**
     Returns the rightmost time a file stops
   */
  inline const double & rightTime()const;

  /**
     Returns the total number of seconds the files take up
  */
  inline double totalTime()const;

  /**
     Returns the top note pitch the programme allows
  */
  inline const double & topPitch()const;

  /**
     Allows you to specify the top note pitch the programme should allow
  */
  void setTopPitch(double p_y);

  inline void setDoingActiveAnalysis(bool p_x);
  inline void setDoingActiveFFT(bool p_x);
  inline void setDoingActiveCepstrum(bool p_x);
  inline void setDoingDetailedPitch(bool p_x);

  QColor getNextColor();

  int getAnalysisBufferSize(int p_rate);
  int getAnalysisStepSize(int p_rate);

  inline bool doingHarmonicAnalysis()const;
  inline bool doingAutoNoiseFloor()const;
  inline bool doingEqualLoudness()const;
  inline bool doingFreqAnalysis()const;
  inline bool doingActiveAnalysis()const;
  inline bool doingActiveFFT()const;
  inline bool doingActiveCepstrum()const;
  inline bool doingDetailedPitch()const;
  inline bool doingActive()const;
  inline bool vibratoSineStyle()const;
  inline t_amplitude_modes amplitudeMode()const;
  inline int pitchContourMode()const;
  inline int fastUpdateSpeed()const;
  inline int slowUpdateSpeed()const;
  inline bool mouseWheelZooms()const;

  void setAmpThreshold( t_amplitude_modes p_mode
                      , int p_index
                      , double p_value
                      );
  double ampThreshold( t_amplitude_modes p_mode
                     , int p_index
                     );
  void setAmpWeight( int p_mode
                   , double p_value
                   );
  double ampWeight(int p_mode);

  inline t_analysis_modes analysisType()const;
  inline bool polish()const;
  inline bool showMeanVarianceBars()const;
  inline t_saving_modes savingMode()const;

  inline const QColor & backgroundColor()const;
  inline const QColor & shading1Color()const;
  inline const QColor & shading2Color()const;

  QString getFilenameString();
  void addFileToList(SoundFile * p_sound_file);
  void removeFileFromList(SoundFile * p_sound_file);
  void clearFreqLookup();
  void clearAmplitudeLookup();
  void recalcScoreThresholds();
  int getActiveIntThreshold();

  inline const double & dBFloor()const;
  inline void setDBFloor(double p_dB_floor);
  inline const double & rmsFloor()const;
  inline const double & rmsCeiling()const;
  inline void set_rms_floor(const double &);
  inline void set_rms_ceiling(const double &);

  inline int musicKey()const;
  inline MusicScale::ScaleType musicScale()const;
  inline MusicTemperament::TemperamentType musicTemperament()const;
  inline const double & freqA()const;
  inline const double & semitoneOffset()const;

signals:
  void activeChannelChanged(Channel * p_active_channel);
  void activeIntThresholdChanged(int p_threshold_percentage);
  void leftTimeChanged(double p_x);
  void rightTimeChanged(double p_x);
  void timeRangeChanged( double p_left_time
                       , double p_right_time
                       );
  void channelsChanged();
  void onChunkUpdate();

  void musicKeyChanged(int p_music_key);
  void musicScaleChanged(int p_music_scale);
  void musicTemperamentChanged(int p_music_temperament);
  void freqAChanged(double p_freq);

public slots:

    /**
     * Signal emitted by Qt combox provide int argument
     * this slot will convert it to t_amplitude_modes
     */
  inline
  void setAmplitudeModeInt(int p_amplitude_mode);
  void setPitchContourMode(int p_pitch_contour_mode);

  inline void setMusicKey(int p_music_key);
  inline void setMusicScale(int p_music_scale);
  inline void setMusicScale(MusicScale::ScaleType p_music_scale);
  void setMusicTemperament(int p_music_temperament);
  void setMusicTemperament(MusicTemperament::TemperamentType p_music_temperament);
  void setFreqA(double p_freq);
  inline void setFreqA(int p_freq);

  void pauseSound();
  bool openPlayRecord( SoundFile * p_sound_file_rec
                     , SoundFile * p_soundfile_play
                     );
  bool playSound(SoundFile * p_sound_file);
  void updateViewLeftRightTimes();
  void updateActiveChunkTime(double p_time);
  void updateQuickRefSettings();
  
  void beginning();
  void rewind();
  bool play();
  void stop();
  void end();
  void fastforward();

  bool closeAllFiles();
  void saveActiveFile();
  void closeActiveFile();
  QString saveFileAsk(QString p_old_filename);
  int saveFile( SoundFile * p_sound_file
              , QString p_new_filename
              );
  int closeFile(SoundFile * p_sound_file
               , t_saving_modes p_saving_mode
               /*, bool ask=true*/
               );
  void resetActiveIntThreshold(int p_threshold_percentage);

    /**
     * Slot used when vibrato views are refresh on Chunk event
     */
    void doChunkUpdate();

    /**
     * Slot used when vibrato views are refresh on FastUpdate event instead of Chunk event
     */
    void doFastChunkUpdate();

  private:
    void setAmplitudeMode(t_amplitude_modes p_amplitude_mode);

    GData();
    ~GData() override;

    inline
    void setView(View & p_view);

    QSettings * m_settings;

  /**
   * Indicate if playing of recording mode is on going when we wnat to refresh
   * vibrato view only on slider event
   * true : play or record mode started
   * false : play or record mode not started
   */
    bool m_play_or_record;

  SoundMode m_sound_mode;
  AudioStream * m_audio_stream;
  bool m_need_update;

  /**
     highpass filter
   */
  std::vector<Filter *> m_filter_hp;

  /**
     lowpass filter
   */
  std::vector<Filter *> m_filter_lp;
  AudioThread m_audio_thread;
  RunningMode m_running;

  std::vector<SoundFile *> m_sound_files;
  std::vector<Channel *> m_channels;

  // To check if really usefull
  std::vector<QColor> m_line_color;
  int m_next_color_index;
  // End to check

  View * m_view;

  /**
     Pointer to the active channel
   */
  Channel * m_active_channel;
  bool m_doing_harmonic_analysis;
  bool m_doing_freq_analysis;
  bool m_doing_equal_loudness;
  bool m_doing_auto_noise_floor;
  int m_doing_active_analysis;
  int m_doing_active_FFT;
  int m_doing_active_cepstrum;
  bool m_doing_detailed_pitch;
  int m_fast_update_speed;
  int m_slow_update_speed;
  bool m_polish;
  bool m_show_mean_variance_bars;
  t_saving_modes m_saving_mode;
  bool m_vibrato_sine_style;
  int m_music_key;
  MusicScale::ScaleType m_music_scale;
  MusicTemperament::TemperamentType m_music_temperament;
  bool m_mouse_wheel_zooms;
  double m_freq_A;
  double m_semitone_offset;

  t_amplitude_modes m_amplitude_mode;
  int m_pitch_contour_mode;
  t_analysis_modes m_analysis_type;
  double m_dB_floor;
  double m_amp_thresholds[static_cast<int>(t_amplitude_modes::NOTE_CHANGE_SCORE) + 1][2];
  double m_amp_weights[static_cast<int>(t_amplitude_modes::NOTE_CHANGE_SCORE) + 1];

  QPixmap * m_drawing_buffer;
  QColor m_background_color;
  QColor m_shading_1_color;
  QColor m_shading_2_Color;
  
  /**
     The lower bound of the start times of all channels
   */
  double m_left_time;

  /**
     The upper bound of the finish times of all channels 
   */
  double m_right_time;

  /**
     The highest possible note pitch allowed (lowest possible is 0)
   */
  double m_top_pitch;

  static
  GData * g_data;
};

#include "gdata.hpp"

#endif
