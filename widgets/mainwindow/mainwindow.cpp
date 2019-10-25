/***************************************************************************
                          mainwindow.cpp  -  description
                             -------------------
    begin                : May 2002
    copyright            : (C) 2002-2007 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include <qstatusbar.h>
#include <qmenubar.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <q3filedialog.h>
#include <qworkspace.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <q3header.h>
#include <qmetaobject.h>
#include <qsizegrip.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <QToolBar>
#include <qtooltip.h>
#include <qmime.h>
#include <q3textedit.h>
#include <qtextedit.h>
#include <qtextstream.h>
#include <QTextBrowser>

#include <qaction.h>
#include <QKeyEvent>
#include <QEvent>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QCustomEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <QDesktopServices>

#include "gdata.h"
#include "mainwindow.h"

#include "freqpair.h"
#include "soundfile.h"
#include "channel.h"
#include "myscrollbar.h"
#include "mylabel.h"

// Views
#include "openfiles.h"
#include "freqview.h"
#include "summaryview.h"
#include "pitchcompassview.h"
#include "volumemeterview.h"
#include "tunerview.h"
#include "hblockview.h"
#include "hstackview.h"
#include "hbubbleview.h"
#include "hcircleview.h"
#include "waveview.h"
#include "pianoview.h"
#include "htrackview.h"
#include "correlationview.h"
#include "fftview.h"
#include "cepstrumview.h"
#include "debugview.h"
#include "scoreview.h"
#include "vibratoview.h"
#include "musicnotes.h"

#include "savedialog.h"
#include "opendialog.h"
#include "tartinisettingsdialog.h"
#include <qlineedit.h>
#include <q3listview.h>
#include <qmessagebox.h>
#include <qwt_slider.h>
#include <QLabel>
#include <QWhatsThis>

#include "wave_stream.h"
#ifdef USE_OGG
#include "ogg_stream.h"
#endif // USE_OGG
#ifdef USE_SOX
#include "sox_stream.h"
#endif // USE_SOX

#include <math.h>
#include "useful.h"
#include "mystring.h"
#include <string.h>

//include the icon .xpm pictures into the program directly
#include "pics/record32x32.xpm"
#include "pics/open.xpm"
#include "pics/close32x32.xpm"
#include "pics/save32x32.xpm"

#include "pics/beginning32x32.xpm"
#include "pics/rewind32x32.xpm"
#include "pics/play32x32.xpm"
#include "pics/playrecord32x32.xpm"
#include "pics/stop32x32.xpm"
#include "pics/fastforward32x32.xpm"
#include "pics/end32x32.xpm"

#include "pics/autofollow32x32.xpm"
#include "pics/shadingon32x32.xpm"
#include "pics/shadingoff32x32.xpm"

#include "myglfonts.h"

#ifdef MACX
#include "main.h"
#endif // MACX

#include "myassert.h"

MainWindow *g_main_window;
MyGLFont *g_mygl_font;

ViewData g_view_data[NUM_VIEWS] =
        // ViewData(title,                         m_menu_name,                            m_class_name,         menu type);
        { ViewData(QObject::tr("File List"),         QObject::tr("&File List"),          "OpenFiles",         0)
        , ViewData(QObject::tr("Pitch Contour"),     QObject::tr("&Pitch Contour"),      "FreqView",          0)
        , ViewData(QObject::tr("Chromatic Tuner"),   QObject::tr("&Chromatic Tuner"),    "TunerView",         0)
        , ViewData(QObject::tr("Harmonic Track"),    QObject::tr("3D Harmonic &Track"),  "HTrackView",        0)
        , ViewData(QObject::tr("Vibrato View"),      QObject::tr("V&ibrato View"),       "VibratoView",       0)
        , ViewData(QObject::tr("Musical Score"),     QObject::tr("&Musical Score"),      "ScoreView",         0)
        , ViewData(QObject::tr("Oscilloscope"),      QObject::tr("&Oscilloscope"),       "WaveView",          1)
        , ViewData(QObject::tr("Correlation View"),  QObject::tr("Corre&lation View"),   "CorrelationView",   1)
        , ViewData(QObject::tr("FFT View"),          QObject::tr("FF&T View"),           "FFTView",           1)
        , ViewData(QObject::tr("Cepstrum View"),     QObject::tr("C&epstrum View"),      "CepstrumView",      1)
        , ViewData(QObject::tr("Debug View"),        QObject::tr("&Debug View"),         "DebugView",         1)
        , ViewData(QObject::tr("Harmonic Block"),    QObject::tr("Harmonic &Block"),     "HBlockView",        2)
        , ViewData(QObject::tr("Harmonic Stack"),    QObject::tr("&Harmonic Stack"),     "HStackView",        2)
        , ViewData(QObject::tr("Harmonic Bubbles"),  QObject::tr("H&armonic Bubbles"),   "HBubbleView",       2)
        , ViewData(QObject::tr("Harmonic Circle"),   QObject::tr("Ha&rmonic Circle"),    "HCircleView",       2)
        , ViewData(QObject::tr("Pitch Compass"),     QObject::tr("Pitch &Compass"),      "PitchCompassView",  2)
        , ViewData(QObject::tr("Piano Keyboard"),    QObject::tr("2D Piano &Keyboard"),  "PianoView",         3)
        , ViewData(QObject::tr("Summary View"),      QObject::tr("&Summary View"),       "SummaryView",       3)
        , ViewData(QObject::tr("Volume Meter"),      QObject::tr("&Volume Meter"),       "VolumeMeterView",   3)
        };

//------------------------------------------------------------------------------
MainWindow::MainWindow(void)
: QMainWindow( NULL, Qt::WDestructiveClose)
{
    m_create_signal_mapper = new QSignalMapper(this);
    connect(m_create_signal_mapper, SIGNAL(mapped(int)), SLOT(openView(int)));

    QFont l_my_font = QFont();
    l_my_font.setPointSize(9);
    l_my_font.setStyleStrategy(QFont::PreferAntialias);
    g_mygl_font = new MyGLFont(l_my_font);

#ifndef WINDOWS
    //for running multi-threaded profiling
    getitimer(ITIMER_PROF, &g_profiler_value);
#endif // WINDOWS
    //6 pixel boarder
    resize(1024 - 6, 768 - 6);

#ifdef MYDEBUG
    setWindowTitle("Tartini (Debug)");
#else // MYDEBUG
    setWindowTitle("Tartini");
#endif // MYDEBUG

    //Create the main Workspace for the view widgets to go in (for MDI)
    m_the_workspace = new QWorkspace(this, "TheWorkspace");
    setCentralWidget( m_the_workspace );
  
    //Create the file Toolbar
    QToolBar * l_file_tool_bar = new QToolBar("File Actions", this);
    addToolBar(Qt::TopToolBarArea, l_file_tool_bar);
    l_file_tool_bar->setIconSize(QSize(32, 32));

    QAction * l_open_action = new QAction(QIcon(iconOpen), tr("&Open"), this);
    l_open_action->setShortcut(tr("Ctrl+O"));
    l_open_action->setWhatsThis(tr("Open a sound from file and process it using the current preferences"));
    l_file_tool_bar->addAction(l_open_action);
    connect(l_open_action, SIGNAL(triggered()), this, SLOT(openFile()));

    QAction * l_save_action = new QAction(QIcon(save32x32_xpm), tr("&Save"), this);
    l_save_action->setShortcut(tr("Ctrl+S"));
    l_save_action->setWhatsThis(tr("Save the active sound to a file"));
    l_file_tool_bar->addAction(l_save_action);
    connect(l_save_action, SIGNAL(triggered()), g_data, SLOT(saveActiveFile()));
  
    QAction * l_close_action = new QAction(QIcon(close32x32_xpm), tr("&Close"), this);
    l_close_action->setShortcut(tr("Ctrl+W"));
    l_close_action->setWhatsThis(tr("Close the active sound. If unsaved will ask to save. Note: Asking can be disabled in the preferences"));
    l_file_tool_bar->addAction(l_close_action);
    connect(l_close_action, SIGNAL(triggered()), g_data, SLOT(closeActiveFile()));

    QAction * l_close_all_action = new QAction(tr("Close All"), this);
    l_close_all_action->setWhatsThis(tr("Close all the sounds. If any sounds are unsaved, it will ask to save. Note: Asking can be disabled in the preferences"));
    connect(l_close_all_action, SIGNAL(triggered()), g_data, SLOT(closeAllFiles()));
  
    QAction * l_print_action = new QAction(tr("Print"), this);
    l_print_action->setShortcut(tr("Ctrl+P"));
    l_print_action->setWhatsThis(tr("Print the Pitch Contour, fitting the its current view onto a page"));
    connect(l_print_action, SIGNAL(triggered()), this, SLOT(printPitch()));

    //Create the sound Toolbar
    QToolBar * l_sound_tool_bar = new QToolBar("Sound Actions", this);
    addToolBar(Qt::BottomToolBarArea, l_sound_tool_bar);
    l_sound_tool_bar->setIconSize(QSize(32, 32));
  
    QToolButton * l_beginning_button = new QToolButton(QIcon(beginning32x32_xpm), tr("Beginning"), tr("Rewind to the beginning"), NULL, NULL, l_sound_tool_bar, tr("Beginning"));
    l_beginning_button->setWhatsThis(tr("Jump to the beginning of the sound"));
    l_sound_tool_bar->addWidget(l_beginning_button);
    connect(l_beginning_button, SIGNAL(pressed()), g_data, SLOT(beginning()));
  
    m_rewind_timer = new QTimer(this);
    connect(m_rewind_timer, SIGNAL(timeout()), g_data, SLOT(rewind()));
    QToolButton * l_rewind_button = new QToolButton(QIcon(rewind32x32_xpm), tr("Rewind"), tr("Rewind"), NULL, NULL, l_sound_tool_bar, tr("rewind"));
    l_rewind_button->setWhatsThis(tr("Rewind the sound"));
    l_sound_tool_bar->addWidget(l_rewind_button);
    connect(l_rewind_button, SIGNAL(pressed()), this, SLOT(rewindPressed()));
    connect(l_rewind_button, SIGNAL(released()), this, SLOT(rewindReleased()));

    m_play_icon_set = new QIcon(play32x32_xpm);
    m_play_record_icon_set = new QIcon(playrecord32x32_xpm);
    m_stop_icon_set = new QIcon(stop32x32_xpm);
    m_play_stop_action = new QAction(*m_play_icon_set, "&Play", this);
    m_play_stop_action->setShortcut(tr("Space"));
    m_play_stop_action->setWhatsThis(tr("Play/Stop the active sound"));
    l_sound_tool_bar->addAction(m_play_stop_action);
    connect(m_play_stop_action, SIGNAL(triggered()), this, SLOT(playStopClicked()));

    m_fast_forward_timer = new QTimer(this);
    connect(m_fast_forward_timer, SIGNAL(timeout()), g_data, SLOT(fastforward()));
    QToolButton * l_fast_forward_button = new QToolButton(QIcon(fastforward32x32_xpm), tr("Fast-forward"), tr("Fast-forward"), NULL, NULL, l_sound_tool_bar, tr("fastforward"));
    l_fast_forward_button->setWhatsThis(tr("Fastfoward the sound"));
    l_sound_tool_bar->addWidget(l_fast_forward_button);
    connect(l_fast_forward_button, SIGNAL(pressed()), this, SLOT(fastforwardPressed()));
    connect(l_fast_forward_button, SIGNAL(released()), this, SLOT(fastforwardReleased()));

    QAction * l_end_action = new QAction(QIcon(end32x32_xpm), tr("&End"), this);
    l_end_action->setWhatsThis(tr("Jump to the end of the sound"));
    l_end_action->setShortcut(tr("Ctrl+E"));
    connect(l_end_action, SIGNAL(triggered()), g_data, SLOT(end()));
    l_sound_tool_bar->addAction(l_end_action);

    //Create the Actions, to be used in Menus and Toolbars
    m_record_icon_set = new QIcon(record32x32_xpm);
    m_record_action = new QAction(*m_record_icon_set, tr("&Record"), this);
    m_record_action->setShortcut(tr("Return"));
    m_record_action->setWhatsThis(tr("Record a new sound, using the input device and settings selected in the preferences"));
    l_file_tool_bar->addAction(m_record_action);
    connect(m_record_action, SIGNAL(triggered()), this, SLOT(openRecord()));

    m_play_record_action = new QAction(*m_play_record_icon_set, tr("Play and Record"), this);
    m_play_record_action->setShortcut(tr("Shift+Return"));
    m_play_record_action->setWhatsThis(tr("Play the active sound and record a new one at the same time!"));
    l_file_tool_bar->addAction(m_play_record_action);
    connect(m_play_record_action, SIGNAL(triggered()), this, SLOT(openPlayRecord()));

    QAction * l_quit = new QAction(tr("&Quit"), this);
    l_quit->setShortcut(tr("Ctrl+Q"));
    l_quit->setWhatsThis(tr("Quit the Tartini application"));
    connect(l_quit, SIGNAL(triggered()), this, SLOT( close() ));

    //Create the File Menu
    QMenu * l_file_menu = menuBar()->addMenu(tr("&File"));
    l_file_menu->addAction(l_open_action);
    l_file_menu->addAction(l_save_action);
    l_file_menu->addAction(l_close_action);
    l_file_menu->addAction(l_close_all_action);
    l_file_menu->addSeparator();
    l_file_menu->addAction(m_play_stop_action);
    l_file_menu->addAction(m_record_action);
    l_file_menu->addAction(m_play_record_action);
    l_file_menu->addSeparator();
    l_file_menu->addAction(l_print_action);
    l_file_menu->addSeparator();
    l_file_menu->addAction(l_quit);

    //Channel Menu
    QAction * l_export_action_1 = new QAction(tr("&Export to plain text"), this);
    connect(l_export_action_1, SIGNAL(triggered()), this, SLOT(exportChannelPlainText()));
    QAction * l_export_action_2 = new QAction(tr("&Export to matlab"), this);
    connect(l_export_action_2, SIGNAL(triggered()), this, SLOT(exportChannelMatlab()));

    QMenu * l_channel_menu = menuBar()->addMenu(tr("C&hannel"));
    l_channel_menu->addAction(l_export_action_1);
    l_channel_menu->addAction(l_export_action_2);

    // Create actions for the new view menu
    m_new_view_menu = menuBar()->addMenu(tr("&Create"));
    connect(m_new_view_menu, SIGNAL(aboutToShow()), this, SLOT(newViewAboutToShow()));

    //Create the Window Menu
    m_window_menu = menuBar()->addMenu(tr("&Windows"));
    m_window_menu->setCheckable( true );
    connect(m_window_menu, SIGNAL( aboutToShow() ), this, SLOT( windowMenuAboutToShow() ) );

    //Create the Options Menu
    m_options_menu = menuBar()->addMenu(tr("&Options"));
    m_options_menu->addAction(tr("&Preferences"), this, SLOT(menuPreferences()));

    QAction * l_whats_this = QWhatsThis::createAction(this);
    l_whats_this->setToolTip(tr("What's this?"));
    l_whats_this->setWhatsThis(tr("Click this button, then click something to learn more about it"));

    m_help_menu = menuBar()->addMenu(tr("&Help"));
    m_help_menu->addAction(l_whats_this);
    m_help_menu->addSeparator();
    m_help_menu->addAction(tr("Documentation"), this, SLOT(showDocumentation()));
    m_help_menu->addSeparator();
    m_help_menu->addAction(tr("About Tartini"), this, SLOT(aboutTartini()));
    m_help_menu->addAction(tr("About Qt"), this, SLOT(aboutQt()));
  
    //Create the other toolbar, which contains some option stuff
    QToolBar * l_analysis_tool_bar = new QToolBar(tr("Analysis Toolbar"), this);
    addToolBar(l_analysis_tool_bar);
    l_analysis_tool_bar->setIconSize(QSize(32, 32));

    QIcon * l_auto_follow_icon_set = new QIcon();
    l_auto_follow_icon_set->setPixmap(QPixmap(autofollow32x32_xpm), QIcon::Small, QIcon::Normal);
    QToolButton * l_auto_follow_button = new QToolButton(*l_auto_follow_icon_set, tr("Auto Follow"), tr("Moves the view up and down automaticlly with the active channel when playing or recording"), NULL, NULL, l_analysis_tool_bar, tr("autoFollow"));
    l_analysis_tool_bar->addWidget(l_auto_follow_button);
    l_auto_follow_button->setToggleButton(true);
    l_auto_follow_button->setAutoRaise(true);
    l_auto_follow_button->setOn(g_data->getView().autoFollow());
    l_auto_follow_button->setWhatsThis(tr("Scrolls the Pitch Contour view up and down automaticlly with the active channel when playing or recording. Note: Manual scrolling (vertically) will be disabled during this time."));
    connect(l_auto_follow_button, SIGNAL(toggled(bool)), &(g_data->getView()), SLOT(setAutoFollow(bool)));

    QIcon * l_background_shading_icon_set = new QIcon();
    l_background_shading_icon_set->setPixmap(QPixmap(shadingon32x32_xpm), QIcon::Small, QIcon::Normal, QIcon::On);
    l_background_shading_icon_set->setPixmap(QPixmap(shadingoff32x32_xpm), QIcon::Small, QIcon::Normal, QIcon::Off);
    QToolButton * l_background_shading_button = new QToolButton(*l_background_shading_icon_set, "Background Shading", "Toggle background shading on/off", NULL, NULL, l_analysis_tool_bar, "backgroundShading");
    l_analysis_tool_bar->addWidget(l_background_shading_button);
    l_background_shading_button->setToggleButton(true);
    l_background_shading_button->setAutoRaise(true);
    l_background_shading_button->setOn(g_data->getView().backgroundShading());
    l_background_shading_button->setWhatsThis(tr("Draw solid color underneath the Pitch Contour, to help you find the line"));
    connect(l_background_shading_button, SIGNAL(toggled(bool)), &(g_data->getView()), SLOT(setBackgroundShading(bool)));

    l_analysis_tool_bar->addAction(l_whats_this);

    View & l_view = g_data->getView();
    QToolBar * l_time_bar_dock = new QToolBar(tr("Time-axis Slider"), this);
    addToolBar(Qt::BottomToolBarArea, l_time_bar_dock);
    l_time_bar_dock->setIconSize(QSize(32, 32));

#if QWT_VERSION >= 0x060000
    m_time_slider = new QwtSlider(Qt::Horizontal,l_time_bar_dock);
    m_time_slider->setScalePosition(QwtSlider::NoScale);
    // To define background style for Qwt slider there are some setters to control display of groove and trough
    // m_time_slider->setBackgroundStyle(QwtSlider::BgBoth) ;
    // Ensure that both Groove and Trough are displayed as it was the case with Qwt 5.x
    m_time_slider->setGroove(true);
    m_time_slider->setTrough(true);

    // Due to changes in Qwt desing it seems that the notion of range has been relaced by the notion of Scale defined in qwt_abstract_scale class
    //  m_time_slider->setRange(g_data->leftTime(), g_data->rightTime(), 1.0/10000.0, 1000);
    m_time_slider->setScale(g_data->leftTime(), g_data->rightTime());

    // the parameters defining how the slider behave inside the scale is now defined in class qwt_abstract_slider
    double l_range_wdith = ( g_data->leftTime() < g_data->rightTime() ? g_data->rightTime() - g_data->leftTime() : g_data->leftTime() - g_data->rightTime());
    unsigned int l_nb_steps = ((unsigned int)(l_range_wdith * 10000.0));
    m_time_slider->setTotalSteps(l_nb_steps);
    m_time_slider->setPageSteps(1000);

    // Graphical parameters
    // setThumbWidth and setThumbLength has been replaced by setHandleSize
    m_time_slider->setHandleSize(QSize(60,20));

    // Don't know how to deal with margins in Qwt 6.x

#else // QWT_VERSION >= 0x060000
#if QWT_VERSION >= 0x050000
    m_time_slider = new QwtSlider(l_time_bar_dock, Qt::Horizontal, QwtSlider::NoScale, QwtSlider::BgBoth);
#else // QWT_VERSION >= 0x050000
    m_time_slider = new QwtSlider(l_time_bar_dock, Qt::Horizontal, QwtSlider::None, QwtSlider::BgBoth);
#endif // QWT_VERSION >= 0x050000
    m_time_slider->setThumbWidth(20);
    m_time_slider->setThumbLength(60);
    m_time_slider->setMargins(2, 2);
    m_time_slider->setRange(g_data->leftTime(), g_data->rightTime(), 1.0/10000.0, 1000);
#endif // QWT_VERSION >= 0x060000
    m_time_slider->setValue(l_view.currentTime());
    m_time_slider->setTracking(true);
    m_time_slider->setBorderWidth(4);
    m_time_slider->setMinimumWidth(200);
    m_time_slider->setWhatsThis("Drag the time slider to move back and forward through the sound file");
    connect(m_time_slider, SIGNAL(sliderMoved(double)), g_data, SLOT(updateActiveChunkTime(double)));
    connect(m_time_slider, SIGNAL(sliderMoved(double)), &l_view, SLOT(doSlowUpdate()));
    connect(&l_view, SIGNAL(onSlowUpdate(double)), m_time_slider, SLOT(setValue(double)));
    connect(g_data, SIGNAL(timeRangeChanged(double, double)), this, SLOT(setTimeRange(double, double)));
    l_time_bar_dock->addWidget(m_time_slider);

    QToolBar * l_volume_meter_tool_bar = new QToolBar(tr("Volume Meter"), this);
    addToolBar(l_volume_meter_tool_bar);
    l_volume_meter_tool_bar->setIconSize(QSize(32, 32));
    VolumeMeterView * volumeMeterView = new VolumeMeterView(VIEW_VOLUME_METER, l_volume_meter_tool_bar);
    volumeMeterView->setWhatsThis(tr("Shows the volume (in dB) of the left and right channels of the active sound. Note: If a mono sound the both are the same"));
    l_volume_meter_tool_bar->addWidget(volumeMeterView);


    QToolBar * l_key_tool_bar = new QToolBar(tr("Key Toolbar"), this);
    l_key_tool_bar->setWhatsThis(tr("Used to set which reference lines are drawn in the Pitch Contour View."));
    addToolBar(l_key_tool_bar);
    QLabel * l_key_label = new QLabel(tr("Key"), l_key_tool_bar);
    l_key_tool_bar->addWidget(l_key_label);

    QComboBox * l_key_combo_box = new QComboBox(l_key_tool_bar);
    l_key_combo_box->setWindowTitle(tr("Key"));
    QStringList l_string_list;
    for(int l_j = 0; l_j < NUM_MUSIC_KEYS; l_j++)
    {
        l_string_list << g_music_key_name[l_j];
    }
    l_key_combo_box->addItems(l_string_list);
    l_key_combo_box->setCurrentIndex(g_data->musicKey());
    l_key_tool_bar->addWidget(l_key_combo_box);
    connect(l_key_combo_box, SIGNAL(activated(int)), g_data, SLOT(setMusicKey(int)));
    connect(g_data, SIGNAL(musicKeyChanged(int)), l_key_combo_box, SLOT(setCurrentIndex(int)));
    connect(g_data, SIGNAL(musicKeyChanged(int)), &(g_data->getView()), SLOT(doUpdate()));

    m_key_type_combo_box = new QComboBox(l_key_tool_bar);
    m_key_type_combo_box->setWindowTitle(tr("Scale type"));
    l_string_list.clear();
    for(uint l_j = 0; l_j < g_music_scales.size(); l_j++)
    {
        l_string_list << g_music_scales[l_j].name();
    }
    m_key_type_combo_box->addItems(l_string_list);
    m_key_type_combo_box->setCurrentIndex(g_data->musicKeyType());
    l_key_tool_bar->addWidget(m_key_type_combo_box);
    connect(m_key_type_combo_box, SIGNAL(activated(int)), g_data, SLOT(setMusicKeyType(int)));
    connect(g_data, SIGNAL(musicKeyTypeChanged(int)), m_key_type_combo_box, SLOT(setCurrentIndex(int)));
    connect(g_data, SIGNAL(musicKeyTypeChanged(int)), &(g_data->getView()), SLOT(doUpdate()));

    QComboBox * l_tempered_combo_box = new QComboBox(l_key_tool_bar);
    l_tempered_combo_box->setWindowTitle(tr("Tempered type"));
    l_string_list.clear();
    for(uint j = 0; j < g_music_keys.size(); j++)
    {
        l_string_list << g_music_keys[j].name();
    }
    l_tempered_combo_box->addItems(l_string_list);
    l_tempered_combo_box->setCurrentIndex(g_data->temperedType());
    l_key_tool_bar->addWidget(l_tempered_combo_box);
    connect(l_tempered_combo_box, SIGNAL(activated(int)), g_data, SLOT(setTemperedType(int)));
    connect(g_data, SIGNAL(temperedTypeChanged(int)), l_tempered_combo_box, SLOT(setCurrentIndex(int)));
    connect(g_data, SIGNAL(temperedTypeChanged(int)), &(g_data->getView()), SLOT(doUpdate()));

    QToolBar * l_freq_A_tool_bar = new QToolBar(tr("Frequency Offset Toolbar"), this);
    l_freq_A_tool_bar->setWhatsThis(tr("The frequency of an even-tempered 'A' used for reference lines in the Pitch Contour View. Default 440 Hz."
                                       "Note: For other scales the root note is chosen from the even-tempered scale with that 'A'."
                                      )
                                   );
    addToolBar(l_freq_A_tool_bar);

    QSpinBox * l_freq_A_spin_box = new QSpinBox(400, 600, 1, l_freq_A_tool_bar, "freqASpinBox");
    l_freq_A_spin_box->setPrefix("A=");
    l_freq_A_spin_box->setSuffix(" Hz");
    l_freq_A_spin_box->setFocusPolicy(Qt::NoFocus);
    l_freq_A_spin_box->setValue(toInt(g_data->freqA()));
    l_freq_A_tool_bar->addWidget(l_freq_A_spin_box);
    connect(l_freq_A_spin_box, SIGNAL(valueChanged(int)), g_data, SLOT(setFreqA(int)));
    connect(l_freq_A_spin_box, SIGNAL(valueChanged(int)), &(g_data->getView()), SLOT(doUpdate()));
    QFont l_font("Courier", 12, QFont::Bold);

    m_note_label = new MyLabel("Note: 9999", statusBar(), "notelabel");
    statusBar()->addPermanentWidget(m_note_label, 0);
    setNoteLabel();
    QToolTip::add(m_note_label, "The current note number in the active file");
    connect(&(g_data->getView()), SIGNAL(onSlowUpdate(double)), this, SLOT(setNoteLabel()));


    m_chunk_label = new MyLabel("Chunk: 999999", statusBar(), "chunklabel");
    statusBar()->addPermanentWidget(m_chunk_label, 0);
    setChunkLabel();
    QToolTip::add(m_chunk_label, "The current chunk number in the active file");
    connect(&(g_data->getView()), SIGNAL(onSlowUpdate(double)), this, SLOT(setChunkLabel()));

    m_time_label = new MyLabel("Time: -00:00.000", statusBar(), "timelabel");
    statusBar()->addPermanentWidget(m_time_label, 0);
    setTimeLabel(0);
    QToolTip::add(m_time_label, tr("The current time positon for all files (mins:sec)"));
    connect(&(g_data->getView()), SIGNAL(onSlowUpdate(double)), this, SLOT(setTimeLabel(double)));

    statusBar()->message( "Ready", 2000 );
    
    connect(g_data, SIGNAL(activeChannelChanged(Channel*)), this, SLOT(setTitle(Channel*)));
}

//------------------------------------------------------------------------------
MainWindow::~MainWindow(void)
{
    delete m_rewind_timer;
    delete m_fast_forward_timer;
    fprintf(stderr, "Has pending events = %s\n", (qApp->hasPendingEvents()) ? "Yes" : "No");
}

//------------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *p_event)
{
    if(g_data->closeAllFiles())
    {
        saveViewGeometry();
        p_event->accept();
    }
    else
    {
        p_event->ignore();
    }
}

//------------------------------------------------------------------------------
bool MainWindow::event(QEvent * p_event)
{
    if(p_event->type() == UPDATE_FAST || p_event->type() == UPDATE_SLOW)
    {
        SoundFile * l_sound_file = g_data->getAudioThread().curSoundFile();
        if(l_sound_file)
        {
            l_sound_file->lock();
            g_data->getView().setCurrentTime(l_sound_file->timeAtCurrentChunk());
            l_sound_file->unlock();
        }
        g_data->updateViewLeftRightTimes();
        g_data->setNeedUpdate(false);
        return true;
    }
    else if(p_event->type() == SOUND_STARTED)
    {
        m_play_stop_action->setIconSet(*m_stop_icon_set);
        m_play_stop_action->setText("Stop");
        m_record_action->setIconSet(*m_stop_icon_set);
        m_record_action->setText("Stop");
        m_play_record_action->setIconSet(*m_stop_icon_set);
        m_play_record_action->setText("Stop");
    }
    else if(p_event->type() == SOUND_STOPPED)
    {
        m_play_stop_action->setIconSet(*m_play_icon_set);
        m_play_stop_action->setText("Play");
        m_record_action->setIconSet(*m_record_icon_set);
        m_record_action->setText("Record");
        m_play_record_action->setIconSet(*m_play_record_icon_set);
        m_play_record_action->setText("Play and Record");
    }
    else if(p_event->type() == SETTINGS_CHANGED)
    {
        g_data->updateQuickRefSettings();
    }
    return QMainWindow::event(p_event);
}

//------------------------------------------------------------------------------
void MainWindow::keyPressEvent (QKeyEvent * p_event)
{
    View & l_view = g_data->getView();

    double l_speed = 1;
    if(p_event->state() & Qt::ShiftModifier)
    {
        l_speed *= 4;
    }
    if(p_event->state() & Qt::ControlModifier)
    {
        l_speed *= 10;
    }
    double l_new_time;

    switch(p_event->key())
    {
#ifdef MYDEBUG
        case Qt::Key_Escape:
            g_data->stopAndWaitAudioThread();
            qApp->closeAllWindows();
        break;
#endif // MYDEBUG
        case Qt::Key_Left:
            if(g_data->getRunning() == STREAM_FORWARD)
            {
                g_data->rewind();
            }
            else
            {
                if(g_data->getActiveChannel())
                {
                    l_new_time = l_view.currentTime() - g_data->getActiveChannel()->timePerChunk() * l_speed;
                }
                else
                {
                    //move 1/5th of a second back
                    l_new_time = l_view.currentTime() - 0.10 * l_speed;
                }
                g_data->updateActiveChunkTime(l_new_time);
            }
        break;
        case Qt::Key_Right:
            if(g_data->getRunning() == STREAM_FORWARD)
            {
                g_data->fastforward();
            }
            else
            {
                if(g_data->getActiveChannel())
                {
                    l_new_time = l_view.currentTime() + g_data->getActiveChannel()->timePerChunk()*l_speed;
                }
                else
                {
                    //move 1/5th of a second forward
                    l_new_time = l_view.currentTime() + 0.10 * l_speed;
                }
                g_data->updateActiveChunkTime(l_new_time);
            }
        break;
        case Qt::Key_Up:
            //move 1/5 of a semi-tone
            l_view.setViewBottom(l_view.viewBottom() + 0.2 * l_speed);
            g_data->getView().doSlowUpdate();
        break;
        case Qt::Key_Down:
            //move 1/5 of a semi-tone
            l_view.setViewBottom(l_view.viewBottom() - 0.2 * l_speed);
            g_data->getView().doSlowUpdate();
        break;
        case Qt::Key_PageUp:
            //move 1/5 of a semi-tone
            l_view.setViewBottom(l_view.viewBottom() + 1.0 * l_speed);
            g_data->getView().doSlowUpdate();
        break;
        case Qt::Key_PageDown:
            //move 1/5 of a semi-tone
            l_view.setViewBottom(l_view.viewBottom() - 1.0 * l_speed);
            g_data->getView().doSlowUpdate();
        break;
        case Qt::Key_Home:
            g_data->updateActiveChunkTime(g_data->leftTime());
        break;
        case Qt::Key_End:
            g_data->updateActiveChunkTime(g_data->rightTime());
        break;
        case Qt::Key_Comma:
            l_view.setViewOffset(l_view.viewOffset() - l_view.viewWidth()/20.0);
            l_view.doSlowUpdate();
        break;
        case Qt::Key_Period:
            l_view.setViewOffset(l_view.viewOffset() + l_view.viewWidth()/20.0);
            l_view.doSlowUpdate();
        break;
        case Qt::Key_Plus:
        case Qt::Key_Equal:
            emit zoomInPressed();
        break;
        case Qt::Key_Minus:
            emit zoomOutPressed();
        break;
        default:
            p_event->ignore();
    }
}

//------------------------------------------------------------------------------
void MainWindow::openFile(void)
{
    QString l_last_folder = QDir::convertSeparators(g_data->getSettingsValue("Dialogs/openFilesFolder", QDir::currentDirPath()));
    QString l_file_name = QFileDialog::getOpenFileName(this, "Open File", l_last_folder, "Sounds (*.wav)");
    if(l_file_name.isEmpty())
    {
        return;
    }
    l_file_name = QDir::convertSeparators(l_file_name);
    g_data->setSettingsValue("Dialogs/openFilesFolder", l_file_name);
    openFile(l_file_name.latin1());
}

//------------------------------------------------------------------------------
void MainWindow::openFile(const char *p_file_name)
{
    SoundFile * l_new_sound_file = new SoundFile();
    if(!l_new_sound_file->openRead(p_file_name))
    {
        fprintf(stderr, "Error opening %s\n", p_file_name);
        delete l_new_sound_file;
        return;
    }

    l_new_sound_file->preProcess();
    g_data->updateViewLeftRightTimes();

    g_data->addFileToList(l_new_sound_file);
    g_data->setActiveChannel(&(l_new_sound_file->getChannel(0)));
    QApplication::postEvent(g_main_window, new QCustomEvent(UPDATE_SLOW));
    g_data->getView().doUpdate();
}

//------------------------------------------------------------------------------
void MainWindow::openRecord(void)
{
    openRecord(false);
}

//------------------------------------------------------------------------------
void MainWindow::openPlayRecord(void)
{
    openRecord(true);
}

//------------------------------------------------------------------------------
void MainWindow::openRecord(bool p_and_play)
{
    if(g_data->getRunning())
    {
        g_data->stop();
        return;
    }

    SoundFile * l_play_sound_file = (p_and_play) ? g_data->getActiveSoundFile() : NULL;

    int l_rate = g_data->getSettingsValue("Sound/sampleRate", 44100);
    QString l_number_of_channels = g_data->getSettingsValue("Sound/numberOfChannels", QString("mono"));
    int l_channels;
    if(l_number_of_channels.lower() == "mono")
    {
        l_channels = 1;
    }
    else
    {
        l_channels = 2;
    }
    int l_bits = g_data->getSettingsValue("Sound/bitsPerSample", 16);
    int l_window_size = g_data->getAnalysisBufferSize(l_rate);
    int l_step_size = g_data->getAnalysisStepSize(l_rate);

    //If playing and recording then overide the record settings to the same as the playing file
    if(l_play_sound_file)
    {
        l_rate = l_play_sound_file->rate();
        l_channels = l_play_sound_file->numChannels();
        l_bits = l_play_sound_file->bits();
        l_window_size = l_play_sound_file->bufferSize();
        l_step_size = l_play_sound_file->bufferSize()/2;
    }

    QString l_temp_file_folder = g_data->getSettingsValue("General/tempFilesFolder", QDir::convertSeparators(QDir::currentDirPath()));
    QDir l_dir = QDir(l_temp_file_folder);
    QFileInfo l_file_info;
    QString l_file_name;
    bool l_file_exists;
    do
    {
        l_file_exists = false;
        l_file_name = g_data->getFilenameString();
        l_file_info.setFile(l_dir, l_file_name);
        if(l_file_info.exists())
        {
            l_file_exists = true;
            int l_file_generating_number = g_data->getSettingsValue("General/fileGeneratingNumber", 1);\
            g_data->setSettingsValue("General/fileGeneratingNumber", l_file_generating_number + 1);
        }
    }
    while(l_file_exists);

    SoundFile * l_new_sound_file = new SoundFile();
    QString l_new_filename(l_file_info.absFilePath().latin1());
    l_new_filename = QDir::convertSeparators(l_new_filename);
    if(!l_new_sound_file->openWrite(l_new_filename, l_rate, l_channels, l_bits, l_window_size, l_step_size))
    {
        delete l_new_sound_file; l_new_sound_file = NULL;
        return;
    }

    if(!g_data->openPlayRecord(l_new_sound_file, l_play_sound_file))
    {
        g_data->closeFile(l_new_sound_file, GData::NEVER_SAVE);
        QMessageBox::warning(g_main_window, "Error", QString("Error opening sound device for recording"), QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    g_data->addFileToList(l_new_sound_file);
    g_data->getView().setCurrentTime(0.0);
    g_data->setActiveChannel(&(l_new_sound_file->getChannel(0)));

    int l_file_generating_number = g_data->getSettingsValue("General/fileGeneratingNumber", 1);
    g_data->setSettingsValue("General/fileGeneratingNumber", l_file_generating_number + 1);
}

//------------------------------------------------------------------------------
void MainWindow::windowMenuAboutToShow(void)
{
    m_window_menu->clear();

    QWidgetList l_windows = m_the_workspace->windowList();
    for(int l_i = 0; l_i < int(l_windows.count()); ++l_i )
    {
        int l_id = m_window_menu->insertItem(l_windows.at(l_i)->caption(), this, SLOT( windowMenuActivated( int ) ) );
        m_window_menu->setItemParameter(l_id, l_i );
        m_window_menu->setItemChecked(l_id, m_the_workspace->activeWindow() == l_windows.at(l_i) );
    }

    m_window_menu->insertSeparator();
    int l_cascade = m_window_menu->insertItem( "&Cascade", m_the_workspace, SLOT( cascade() ), 0 );
    int l_close = m_window_menu->insertItem( "Close &All", this, SLOT( closeAllWidgets() ), 0 );

    if(l_windows.isEmpty())
    {
        m_window_menu->setItemEnabled( l_cascade, false );
        m_window_menu->setItemEnabled( l_close, false );
    }
}

//------------------------------------------------------------------------------
void MainWindow::windowMenuActivated(int p_id)
{
    QWidget * l_widget = m_the_workspace->windowList().at(p_id);
    if( l_widget )
    {
        l_widget->showNormal();
        l_widget->setFocus();
    }
}

//------------------------------------------------------------------------------
void MainWindow::message( QString p_string
                        , int p_msec
                        )
{
    QStatusBar *l_status_bar = statusBar();
    if(l_status_bar)
    {
        l_status_bar->message(p_string, p_msec);
    }
}

//------------------------------------------------------------------------------
void MainWindow::closeAllWidgets(void)
{
    QWidgetList l_opened = m_the_workspace->windowList();
    for(QWidgetList::iterator l_iterator = l_opened.begin(); l_iterator < l_opened.end(); l_iterator++)
    {
        (*l_iterator)->close();
    }
}

//------------------------------------------------------------------------------
void MainWindow::menuPreferences(void)
{
    TartiniSettingsDialog *l_settings = new TartiniSettingsDialog(this);
    l_settings->show();
}


//------------------------------------------------------------------------------
QWidget * MainWindow::openView(int p_view_id)
{
    QWidget * l_widget = NULL;
    int l_use_top_level_widgets = g_data->getSettingsValue("Display/useTopLevelWidgets", false);
    QWidget * l_parent = (l_use_top_level_widgets) ? NULL : m_the_workspace;

    switch(p_view_id)
    {
        case VIEW_OPEN_FILES:
            l_widget = new OpenFiles(p_view_id, l_parent);
        break;
        case VIEW_FREQ:
        {
            FreqView * l_freq_view = new FreqView(p_view_id, l_parent);
            connect(this, SIGNAL(zoomInPressed()), l_freq_view, SLOT(zoomIn()));
            connect(this, SIGNAL(zoomOutPressed()), l_freq_view, SLOT(zoomOut()));
            l_widget = l_freq_view;
        }
        break;
        case VIEW_SUMMARY:
            l_widget = new SummaryView( p_view_id, l_parent);
        break;
        case VIEW_PITCH_COMPASS:
            l_widget = new PitchCompassView(p_view_id, l_parent);
        break;
        case VIEW_VOLUME_METER:
            l_widget = new VolumeMeterView(p_view_id, l_parent);
        break;
        case VIEW_TUNER:
            l_widget = new TunerView(p_view_id, l_parent);
        break;
        case VIEW_HBLOCK:
            l_widget = new HBlockView(p_view_id, l_parent);
        break;
        case VIEW_HSTACK:
            l_widget = new HStackView(p_view_id, l_parent);
        break;
        case VIEW_HBUBBLE:
            l_widget = new HBubbleView(p_view_id, l_parent);
        break;
        case VIEW_HCIRCLE:
            l_widget = new HCircleView(p_view_id, l_parent);
        break;
        case VIEW_WAVE:
            l_widget = new WaveView(p_view_id, l_parent);
        break;
        case VIEW_PIANO:
            l_widget = new PianoView(p_view_id, l_parent);
        break;
        case VIEW_HTRACK:
            l_widget = new HTrackView(p_view_id, l_parent);
        break;
        case VIEW_CORRELATION:
            l_widget = new CorrelationView(p_view_id, l_parent);
        break;
        case VIEW_FFT:
            l_widget = new FFTView(p_view_id, l_parent);
        break;
        case VIEW_CEPSTRUM:
            l_widget = new CepstrumView(p_view_id, l_parent);
        break;
        case VIEW_DEBUGVIEW:
            l_widget = new DebugView(p_view_id, l_parent);
        break;
        case VIEW_SCORE:
            l_widget = new ScoreView(p_view_id, l_parent);
        break;
        case VIEW_VIBRATO:
            l_widget = new VibratoView(p_view_id, l_parent);
        break;
    }
    if(l_widget)
    {
        if(l_parent)
        {
            m_the_workspace->addWindow(l_widget);
        }
        l_widget->show();
    }
    return l_widget;
}

//------------------------------------------------------------------------------
void MainWindow::newViewAboutToShow(void)
{
    m_new_view_menu->clear();

    QMenu * l_technical_menu = new QMenu("Technical");
    QMenu * l_experimental_menu = new QMenu("Experimental");
    QMenu * l_other_menu = new QMenu("Other");

    QWidgetList l_opened = m_the_workspace->windowList();

    for(int j = 0; j < NUM_VIEWS; j++)
    {
        QAction *l_action;
        if(g_view_data[j].m_menu_type == 0)
        {
            l_action = m_new_view_menu->addAction(g_view_data[j].m_menu_name);
        }
        else if(g_view_data[j].m_menu_type == 1)
        {
            l_action = l_technical_menu->addAction(g_view_data[j].m_menu_name);
        }
        else if(g_view_data[j].m_menu_type == 2)
        {
            l_action = l_experimental_menu->addAction(g_view_data[j].m_menu_name);
        }
        else if(g_view_data[j].m_menu_type == 3)
        {
            l_action = l_other_menu->addAction(g_view_data[j].m_menu_name);
        }
        else
        {
            continue;
        }

        connect(l_action, SIGNAL(triggered()), m_create_signal_mapper, SLOT(map()));
        m_create_signal_mapper->setMapping(l_action, j);
        for(QWidgetList::iterator l_iterator=l_opened.begin(); l_iterator<l_opened.end(); l_iterator++)
        {
            if(QString((*l_iterator)->metaObject()->className()) == g_view_data[j].m_class_name)
            {
                l_action->setEnabled(false);
                break;
            }
        }
    }
    m_new_view_menu->addSeparator();
    m_new_view_menu->addMenu(l_technical_menu);
    m_new_view_menu->addMenu(l_experimental_menu);
    m_new_view_menu->addMenu(l_other_menu);
}

//------------------------------------------------------------------------------
void MainWindow::setTimeLabel(double p_t)
{
    char l_temp[128];
    if(m_time_label)
    {
        int l_sign = (p_t < 0) ? -1 : 1;
        p_t = fabs(p_t);
        int l_mins = int(floor(p_t)) / 60;
        int l_secs = int(floor(p_t - double(l_mins * 60)));
        int l_milli_secs = int(floor((p_t - double(l_mins * 60) - double(l_secs))*1000.0));
        sprintf(l_temp, "Time: %s%02d:%02d.%03d", (l_sign == -1) ? "-" : " ", l_mins, l_secs, l_milli_secs);
        m_time_label->setText(QString(l_temp));
    }
}

//------------------------------------------------------------------------------
void MainWindow::setChunkLabel(void)
{
    char l_temp[128];
    Channel *l_active_channel = g_data->getActiveChannel();
    if(l_active_channel)
    {
        sprintf(l_temp, "Chunk: %d", l_active_channel->currentChunk());
    }
    else
    {
        sprintf(l_temp, "Chunk: 0");
    }
    m_chunk_label->setText(QString(l_temp));
}

//------------------------------------------------------------------------------
void MainWindow::setNoteLabel(void)
{
    char l_temp[128];
    Channel *l_active_channel = g_data->getActiveChannel();
    if(l_active_channel)
    {
        l_active_channel->lock();
        AnalysisData *l_data = l_active_channel->dataAtCurrentChunk();
        if(l_data && l_active_channel->isVisibleNote(l_data->getNoteIndex()) && l_active_channel->isLabelNote(l_data->getNoteIndex()))
        {
            sprintf(l_temp, "Note: %d", l_data->getNoteIndex());
        }
        else
        {
            sprintf(l_temp, "Note:    ");
        }
        l_active_channel->unlock();
    }
    else
    {
        sprintf(l_temp, "Note:    ");
    }
    m_note_label->setText(QString(l_temp));
}

//------------------------------------------------------------------------------
void MainWindow::setTimeRange( double p_min
                             , double p_max
                             )
{
    if(m_time_slider)
    {
#if QWT_VERSION >= 0x060000

        // In Qwt 6.x range has been replaced by scale
        m_time_slider->setScale(p_min, p_max);
        // and steps and pages are managed by qwt_abstract_slider class
        double l_range_wdith = ( p_min < p_max ? p_max - p_min : p_min - p_max);
        unsigned int l_nb_steps = ((unsigned int)(l_range_wdith * 10000.0));
        m_time_slider->setTotalSteps(l_nb_steps > m_time_slider->totalSteps() ? l_nb_steps : m_time_slider->totalSteps());
        m_time_slider->setPageSteps(1000);
#else // QWT_VERSION >= 0x060000
        m_time_slider->setRange(p_min, p_max, m_time_slider->step(),1000);
#endif // QWT_VERSION >= 0x060000
    }
}

//------------------------------------------------------------------------------
void MainWindow::rewindPressed(void)
{
    g_data->rewind();
    //every 0.2 seconds
    m_rewind_timer->start(g_data->fastUpdateSpeed());
}

//------------------------------------------------------------------------------
void MainWindow::rewindReleased(void)
{
    //every 0.2 seconds
    m_rewind_timer->stop();
}

//------------------------------------------------------------------------------
void MainWindow::playStopClicked(void)
{
    if(g_data->getRunning())
    {
        g_data->stop();
    }
    else
    {
        g_data->play();
    }  
}

//------------------------------------------------------------------------------
void MainWindow::fastforwardPressed(void)
{
    g_data->fastforward();
    //every 0.2 seconds
    m_fast_forward_timer->start(g_data->fastUpdateSpeed());
}

//------------------------------------------------------------------------------
void MainWindow::fastforwardReleased(void)
{
    //every 0.2 seconds
    m_fast_forward_timer->stop();
}

//------------------------------------------------------------------------------
void MainWindow::setTitle(Channel *p_channel)
{
#ifdef MYDEBUG
    if(p_channel)
    {
        setCaption(QString("Tartini (debug) - ") + p_channel->getUniqueFilename());
    }
    else
    {
        setCaption(QString("Tartini (debug)"));
    }
#else // MYDEBUG
    if(p_channel)
    {
        setCaption(QString("Tartini - ") + p_channel->getUniqueFilename());
    }
    else
    {
        setCaption(QString("Tartini"));
    }
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void MainWindow::aboutTartini(void)
{
    TartiniDialog *l_tartini_dialog = new TartiniDialog(this);
    l_tartini_dialog->exec();
    delete l_tartini_dialog;
}

//------------------------------------------------------------------------------
void MainWindow::aboutGPL(void)
{
    GPLDialog *l_gpl_dialog = new GPLDialog(this);
    l_gpl_dialog->exec();
    delete l_gpl_dialog;
}

//------------------------------------------------------------------------------
void MainWindow::aboutQt(void)
{
    QMessageBox::aboutQt(this, "About Qt");
}

//------------------------------------------------------------------------------
void MainWindow::showDocumentation(void)
{
    QDesktopServices::openUrl(QUrl("http://www.tartini.net/doc"));
}

//------------------------------------------------------------------------------
TartiniDialog::TartiniDialog(QWidget *p_parent)
: QDialog(p_parent, NULL, true)
{
    setCaption("About Tartini - Version " TARTINI_VERSION_STR);
    setBackgroundColor(Qt::black);
    Q3BoxLayout * l_layout = new Q3VBoxLayout(this, 6, 6);

    Q3BoxLayout * l_top_layout = new Q3HBoxLayout(l_layout);
    QLabel * l_tartini_picture = new QLabel(this, "TartiniPicture");
    QPixmap l_tartini_pixmap(":/res/tartinilogo.jpg", "JPG");
    l_tartini_picture->setPixmap(l_tartini_pixmap);
    l_tartini_picture->setBackgroundColor(Qt::black);
    l_top_layout->addStretch(10);
    l_top_layout->addWidget(l_tartini_picture);
    l_top_layout->addStretch(10);

    QTextBrowser * l_tartini_text_edit = new QTextBrowser(this, "TartiniTextEdit");
    l_tartini_text_edit->setOpenExternalLinks(true);
    l_tartini_text_edit->setTextFormat(Qt::RichText);

    QColor l_link_color(Qt::cyan);
    QString l_sheet = QString::fromLatin1("a { text-decoration: underline; color: %1 }").arg(l_link_color.name());
    l_tartini_text_edit->document()->setDefaultStyleSheet(l_sheet);

    l_tartini_text_edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    l_tartini_text_edit->setHtml(
    "Tartini is a graphical tool for analysing the music of solo instruments.<br>"
    "This program was created by Philip McLeod as part of PhD work at the University of Otago, New Zealand<br>"
    "You can find the latest info about Tartini at <a href=\"http://www.tartini.net\">http://www.tartini.net</a><br><br>"
    "Copyright 2002-2007 Philip McLeod (pmcleod@cs.otago.ac.nz).<br><br>"

    "New features in version 1.2 include:<br>"
    "- The ability to set an offset frequency. i.e. not fixed to A=440 Hz<br>"
    "- Vertical reference lines<br>"
    "- Fixed a bug in note detection which caused a number of pitch detection problems<br><br>"

    "New features in version 1.1 include:<br>"
    "- A vibrato analysis tool<br>"
    "- Full duplex sound, allowing play and record at the same time<br>"
    "- Choice of musical scales<br>"
    "- Some basic printing abilities (pitch view only)<br>"
    "- Some basic exporting abilities (plain text or matlab code)<br>"
    "- A refined pitch algorithm<br>"
    "- A musical score generator (still in very early stages of development)<br><br>"

    "<i>Thanks to:-</i><br>"
    "Prof. Geoff Wyvill - PhD Supervisor (Computer Science)<br>"
    "Dr. Donald Warrington - PhD Supervisor (Physics)<br>"
    "Rob Ebbers - Programming<br>"
    "Maarten van Sambeek - Programming<br>"
    "Stuart Miller - Programming<br>"
    "Mr Kevin Lefohn - Support (Violin)<br>"
    "Miss Judy Bellingham - Support (Voice)<br>"
    "Jean-Philippe Grenier - Logo design<br>"
    "People at my lab, especially Alexis Angelidis, Sui-Ling Ming-Wong, Brendan McCane and Damon Simpson<br><br>"
    "Tartini version " TARTINI_VERSION_STR " is released under the GPL license <a href=\"http://www.gnu.org/licenses/gpl.html\">http://www.gnu.org/licenses/gpl.html</a>.<br><br>"

    "<i>Libraries used by tartini include:</i><br>"
    "Qt - <a href=\"http://www.trolltech.com/qt\">http://www.trolltech.com/qt</a><br>"
    "FFTW - <a href=\"http://www.fftw.org\">http://www.fftw.org</a><br>"
    "Qwt - <a href=\"http://qwt.sourceforge.net\">http://qwt.sourceforge.net</a><br>"
    "RtAudio - <a href=\"http://www.music.mcgill.ca/~gary/rtaudio/index.html\">http://www.music.mcgill.ca/~gary/rtaudio/index.html</a><br>"
    "<br>"
    "This program is free software; you can redistribute it and/or modify "
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation; either version 2 of the License, or "
    "(at your option) any later version.<br><br>"
    "This program is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
    "GNU General Public License for more details.<br><br>"
    "You should have received a copy of the GNU General Public License "
    "along with this program; if not, write to the Free Software "
    "Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.<br><br>"
    "Please click 'Read License' or see LICENSE.txt for details.<br>"
    );

    QPalette l_palette;
    l_palette = palette();
    l_palette.setColor(QPalette::Base, Qt::black);
    l_palette.setColor(QPalette::Text, Qt::white);
    l_tartini_text_edit->setPalette(l_palette);
    l_layout->addWidget(l_tartini_text_edit);
  
    Q3BoxLayout * l_bottom_layout = new Q3HBoxLayout(l_layout);
    QPushButton * l_ok_button = new QPushButton("&Ok", this);
    QPushButton * l_GPL_button = new QPushButton("&Read License", this);
    l_bottom_layout->addStretch(10);
    l_bottom_layout->addWidget(l_ok_button);
    l_bottom_layout->addStretch(10);
    l_bottom_layout->addWidget(l_GPL_button);
    l_bottom_layout->addStretch(10);
    setFocusProxy(l_ok_button);
    connect(l_ok_button, SIGNAL(clicked()), this, SLOT(accept()));
    connect(l_GPL_button, SIGNAL(clicked()), g_main_window, SLOT(aboutGPL()));
}

//------------------------------------------------------------------------------
GPLDialog::GPLDialog(QWidget *p_parent)
: QDialog(p_parent, NULL, true)
{
    setCaption("GPL Licence");
    Q3BoxLayout * l_layout = new Q3VBoxLayout(this, 6, 6);

    Q3TextEdit * l_GPL_text_edit = new Q3TextEdit(this, "GPLTextEdit");
    l_GPL_text_edit->setReadOnly(true);
    l_GPL_text_edit->setTextFormat(Qt::PlainText);
    QString l_the_text;
    {
        QFile l_the_file(":res/LICENSE.txt");
        l_the_file.open(QIODevice::ReadOnly);
        QTextStream l_text_stream(&l_the_file);
        l_the_text = l_text_stream.read();
        l_the_file.close();
    }
    l_GPL_text_edit->setText(l_the_text);
  
    l_layout->addWidget(l_GPL_text_edit);

    Q3BoxLayout * l_bottom_layout = new Q3HBoxLayout(l_layout);
    QPushButton * l_ok_button = new QPushButton("&Ok", this);
    l_bottom_layout->addStretch(10);
    l_bottom_layout->addWidget(l_ok_button);
    l_bottom_layout->addStretch(10);
    setFocusProxy(l_ok_button);
    connect(l_ok_button, SIGNAL(clicked()), this, SLOT(accept()));
}

#include "freqdrawwidget.h"
#include "drawwidget.h"

//------------------------------------------------------------------------------
void MainWindow::printPitch(void)
{
    QPrinter l_printer(QPrinter::HighResolution);
    l_printer.setOrientation(QPrinter::Landscape);
    QPrintDialog l_print_dialog(&l_printer, this);
    if(l_print_dialog.exec() == QDialog::Accepted)
    {
        View & l_view = g_data->getView();
        QPainter l_painter;
        l_painter.begin(&l_printer);
        int l_width = l_printer.width();
        int l_height = l_printer.height();
        double l_left_time = l_view.viewLeft();
        double l_right_time = l_view.viewRight();
        double l_view_bottom = l_view.viewBottom();
        double l_view_top = l_view.viewTop();
        double l_zoom_X = (l_right_time-l_left_time) / double(l_width);
        double l_zoom_Y = (l_view_top-l_view_bottom) / double(l_height);
        FreqDrawWidget::drawReferenceLines(l_printer, l_painter, 0.0, l_zoom_X, l_view_bottom, l_zoom_Y, DRAW_VIEW_PRINT);

        double l_dots_per_line_step_Y = 1.0 / l_zoom_Y;
        double l_dots_per_MM = double(l_printer.height()) / double(l_printer.heightMM());
        //1mm thick line
        DrawWidget::setLineWidth(toInt(std::min(l_dots_per_MM * 1.0, l_dots_per_line_step_Y * 0.2)));
        //draw all the visible channels
        for (uint l_i = 0; l_i < g_data->getChannelsSize(); l_i++)
        {
            Channel *l_channel = g_data->getChannelAt(l_i);
            if(!l_channel->isVisible())
            {
                continue;
            }
            DrawWidget::drawChannel(l_printer, l_channel, l_painter, l_left_time, 0.0, l_zoom_X, l_view_bottom, l_zoom_Y, DRAW_VIEW_PRINT);
            if(l_channel == g_data->getActiveChannel())
            {
                l_painter.setPen(Qt::black);
                QString l_file_name = l_channel->getUniqueFilename();
                QFont l_font = l_painter.font();
                l_font.setPointSize(18);
                l_painter.setFont(l_font);
                l_painter.drawText(QRect(0, 0, l_printer.width(), l_printer.height()), Qt::AlignHCenter | Qt::AlignTop, l_file_name);
            }
        }
        l_painter.end();
    }
}

//------------------------------------------------------------------------------
void MainWindow::exportChannel(int p_type, QString p_type_string)
{
    Channel *l_channel = g_data->getActiveChannel();
    if(l_channel)
    {
        l_channel->exportChannel(p_type, p_type_string);
    }
    else
    {
        fprintf(stderr, "No channel to export.\n");
    }
}

//------------------------------------------------------------------------------
void MainWindow::exportChannelPlainText(void)
{
    exportChannel(0, "Text (*.txt)");
}

//------------------------------------------------------------------------------
void MainWindow::exportChannelMatlab(void)
{
    exportChannel(1, "Matlab code (*.m)");
}

//------------------------------------------------------------------------------
bool MainWindow::loadViewGeometry(void)
{
    QPoint l_pos;
    QSize l_size;
    int l_counter = 0;

    for(int l_j = 0; l_j < NUM_VIEWS; l_j++)
    {
        QString l_base = QString("geometry/") + g_view_data[l_j].m_class_name;
        QString l_key = l_base+"/visible";
        if(g_data->settingsContains(l_key) && g_data->getSettingsValue(l_key, false) == true)
        {
            l_pos = g_data->getSettingsValue(l_base+"/pos", QPoint(0, 0));
            l_size = g_data->getSettingsValue(l_base+"/size", QSize(100, 100));
            QWidget * l_widget = openView(l_j);
            //get the subwindow frame
            QWidget * l_parent_widget = (QWidget*)(l_widget->parent());
            if(l_parent_widget)
            {
                l_parent_widget->resize(l_size);
                l_parent_widget->move(l_pos);
            }
            l_counter++;
        }
    }
    if(l_counter == 0)
    {
        openView(VIEW_OPEN_FILES);
        openView(VIEW_FREQ);
        openView(VIEW_TUNER);
        openView(VIEW_VIBRATO);
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
void MainWindow::saveViewGeometry(void)
{
    QWidgetList l_opened = m_the_workspace->windowList();

    for(int l_j = 0; l_j < NUM_VIEWS; l_j++)
    {
        QString l_base = QString("geometry/") + g_view_data[l_j].m_class_name;

        bool l_found = false;
        for(QWidgetList::iterator l_iterator = l_opened.begin(); l_iterator < l_opened.end(); l_iterator++)
        {
            if(QString((*l_iterator)->metaObject()->className()) == g_view_data[l_j].m_class_name)
            {
                //get the subwindow frame
                QWidget * l_parent_widget = (QWidget*)((*l_iterator)->parent());
                if(!l_parent_widget)
                {
                    break;
                }
                g_data->setSettingsValue(l_base + "/visible", true);
                g_data->setSettingsValue(l_base + "/pos", l_parent_widget->pos());
                g_data->setSettingsValue(l_base + "/size", l_parent_widget->size());
                l_found = true;
                break;
            }
        }
        if(!l_found)
        {
            g_data->setSettingsValue(l_base+"/visible", false);
        }
    }
}

//------------------------------------------------------------------------------
QSize TartiniDialog::sizeHint(void) const
{
    return QSize(600, 600);
}

//------------------------------------------------------------------------------
QSize GPLDialog::sizeHint(void) const
{
    return QSize(600, 480);
}
// EOF
