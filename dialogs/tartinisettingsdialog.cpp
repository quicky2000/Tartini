/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
#include <QColorDialog>
#include "mainwindow.h"
#include "tartinisettingsdialog.h"
#include "gdata.h"
#include "audio_stream.h"
#include <QFileDialog>
#include <sstream>
#include <iomanip>

//------------------------------------------------------------------------------
TartiniSettingsDialog::TartiniSettingsDialog(QWidget * p_parent)
: QDialog(p_parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    connect(okButton, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    connect( backgroundButton, SIGNAL( clicked() ), this, SLOT( getBackgroundColor() ) );
    connect( shading1Button, SIGNAL( clicked() ), this, SLOT( getShading1Color() ) );
    connect( shading2Button, SIGNAL( clicked() ), this, SLOT( getShading2Color() ) );
    connect( fileNumberOfDigits, SIGNAL( valueChanged(int) ), this, SLOT( fileNameChanged() ) );
    connect( fileGeneratingNumber, SIGNAL( valueChanged(int) ), this, SLOT( fileNameChanged() ) );
    connect( filenameGeneratingString, SIGNAL( textChanged(const QString&) ), this, SLOT( fileNameChanged() ) );
    connect( chooseTempFolderButton, SIGNAL( clicked() ), this, SLOT( changeTempFolder() ) );
    connect( noteRangeChoice, SIGNAL( activated(int) ), this, SLOT( onNoteRangeChoice(int) ) );
    connect( resetDefaultsButton, SIGNAL( clicked() ), this, SLOT( resetDefaults() ) );

    init();
}

//------------------------------------------------------------------------------
void TartiniSettingsDialog::loadSetting( QObject * p_object
                                       , const std::string & p_group
                                       )
{
    std::string l_key = p_object->objectName().toStdString();
    std::string l_full_key = p_group + "/" + l_key;
    std::string l_class_name = p_object->metaObject()->className();

    if(l_class_name == "QGroupBox")
    {
        //Iterate over the groupBox's children
        const QList<QObject*> & l_widgets = p_object->children();
        for(QList<QObject*>::const_iterator l_widget_iter = l_widgets.begin(); l_widget_iter < l_widgets.end(); ++l_widget_iter)
        {
            loadSetting(*l_widget_iter, p_group);
        }
    }
    else if(l_class_name == "QLineEdit")
    {
         static_cast<QLineEdit*>(p_object)->setText(QString::fromStdString(GData::getUniqueInstance().getSettingsStringValue(l_full_key)));
    }
    else if(l_class_name == "QComboBox")
    {
         static_cast<QComboBox*>(p_object)->setCurrentIndex(static_cast<QComboBox*>(p_object)->findText(QString::fromStdString(GData::getUniqueInstance().getSettingsStringValue(l_full_key))));
    }
    else if(l_class_name == "QPushButton" && static_cast<QPushButton*>(p_object)->isCheckable())
    {
        static_cast<QPushButton*>(p_object)->setChecked(GData::getUniqueInstance().getSettingsBoolValue(l_full_key));
    }
    else if(l_class_name == "QCheckBox")
    {
         static_cast<QCheckBox*>(p_object)->setChecked(GData::getUniqueInstance().getSettingsBoolValue(l_full_key));
    }
    else if(l_class_name == "QSpinBox")
    {
        static_cast<QSpinBox*>(p_object)->setValue(GData::getUniqueInstance().getSettingsIntValue(l_full_key));
    }
    else if(l_class_name == "QFrame")
    {
        QColor l_color;
        l_color.setNamedColor(QString::fromStdString(GData::getUniqueInstance().getSettingsStringValue(l_full_key)));
        QPalette l_palette = static_cast<QFrame*>(p_object)->palette();
        l_palette.setColor(l_palette.currentColorGroup(),QPalette::Window,l_color);
        static_cast<QFrame*>(p_object)->setPalette(l_palette);
    }
    else if("QVBoxLayout" != l_class_name &&
            "QHBoxLayout" != l_class_name &&
            "QLabel" != l_class_name &&
            "QPushButton" != l_class_name &&
            "QGridLayout" != l_class_name
            )
    {
        abort();
    }
}

//------------------------------------------------------------------------------
void TartiniSettingsDialog::init()
{
    // Go through all the categories on the left, and insert all the preferences we can into the fields.
    // Combo boxes must be done separately.
    soundInput->clear();
    soundInput->insertItems(0,AudioStream::getInputDeviceNames());
    soundOutput->clear();
    soundOutput->insertItems(0,AudioStream::getOutputDeviceNames());

    bool l_tab_names_empty = m_tab_names.empty();
    //Iterate over all groups
    for(int l_i = 0; l_i < tabWidget->count(); l_i++)
    {
        //Iterate over all widgets in the current group and load their settings
        std::string l_group(tabWidget->tabText(l_i).toStdString());
        if(l_tab_names_empty)
        {
            m_tab_names.insert(std::map<int, std::string>::value_type(l_i, l_group));
        }
        const QList<QObject*> & l_widgets = tabWidget->widget(l_i)->children();
        for(QList<QObject*>::const_iterator l_widget_iter = l_widgets.begin(); l_widget_iter < l_widgets.end(); ++l_widget_iter)
        {
            loadSetting(*l_widget_iter, l_group);
        }
    }
    checkAnalysisEnabled();
}

//------------------------------------------------------------------------------
QString TartiniSettingsDialog::getPath(const QString p_initial_path)
{
    return QFileDialog::getExistingDirectory(this, "Choose a directory", p_initial_path);
}

//------------------------------------------------------------------------------
void TartiniSettingsDialog::changeTempFolder()
{
    if(tempFilesFolder)
    {
        QString l_path = getPath(tempFilesFolder->text());
        if(l_path != "")
        {
            tempFilesFolder->setText(QDir::toNativeSeparators(l_path));
        }
    }
}

//------------------------------------------------------------------------------
void TartiniSettingsDialog::fileNameChanged()
{
    QString l_filename;
    int l_digits = fileNumberOfDigits->value();
    if(l_digits == 0)
    {
        l_filename = QString::fromStdString(filenameGeneratingString->text().toStdString() + ".wav");
    }
    else
    {
        std::stringstream l_stream;
        l_stream << filenameGeneratingString->text().toStdString() << std::setfill('0') << std::setw(l_digits) << fileGeneratingNumber->value();
        l_filename = QString::fromStdString(l_stream.str() + ".wav");
    }
    filenameExample->setText(l_filename);
}

//------------------------------------------------------------------------------
void TartiniSettingsDialog::getBackgroundColor()
{
    QColor l_color = QColorDialog::getColor(theBackgroundColor->palette().color(QPalette::Window));
    if(l_color.isValid())
    {
        QPalette l_palette = theBackgroundColor->palette();
        l_palette.setColor(l_palette.currentColorGroup(),QPalette::Window,l_color);
        theBackgroundColor->setPalette(l_palette);
    }
}

//------------------------------------------------------------------------------
void TartiniSettingsDialog::getShading1Color()
{
    QColor l_color = QColorDialog::getColor(shading1Color->palette().color(QPalette::Window));
    if(l_color.isValid())
    {
        QPalette l_palette = shading1Color->palette();
        l_palette.setColor(l_palette.currentColorGroup(),QPalette::Window,l_color);
        shading1Color->setPalette(l_palette);
    }
}

//------------------------------------------------------------------------------
void TartiniSettingsDialog::getShading2Color()
{
    QColor l_color = QColorDialog::getColor(shading2Color->palette().color(QPalette::Window));
    if(l_color.isValid())
    {
        QPalette l_palette = shading2Color->palette();
        l_palette.setColor(l_palette.currentColorGroup(),QPalette::Window,l_color);
        shading2Color->setPalette(l_palette);
    }
}

//------------------------------------------------------------------------------
void TartiniSettingsDialog::saveSetting(QObject *p_object, const std::string  & p_group)
{
    std::string l_key = p_object->objectName().toStdString();
    std::string l_full_key = p_group + "/" + l_key;
    std::string l_class_name = p_object->metaObject()->className();

    if(l_class_name == "QGroupBox")
    {
        //Iterate over the groupBox's children
        const QList<QObject*> & l_widgets = p_object->children();
        for(QList<QObject*>::const_iterator l_widget_iter = l_widgets.begin(); l_widget_iter < l_widgets.end(); ++l_widget_iter)
        {
            saveSetting(*l_widget_iter, p_group);
        }
    }
    else if(l_class_name == "QLineEdit")
    {
        GData::getUniqueInstance().setSettingsValue(l_full_key, static_cast<QLineEdit*>(p_object)->text().toStdString());
    }
    else if(l_class_name == "QComboBox")
    {
        GData::getUniqueInstance().setSettingsValue(l_full_key, static_cast<QComboBox*>(p_object)->currentText().toStdString());
    }
    else if(l_class_name == "QPushButton" && static_cast<QPushButton*>(p_object)->isCheckable())
    {
        GData::getUniqueInstance().setSettingsValue(l_full_key, static_cast<QPushButton*>(p_object)->isChecked());
    }
    else if(l_class_name == "QCheckBox")
    {
        GData::getUniqueInstance().setSettingsValue(l_full_key, static_cast<QCheckBox*>(p_object)->isChecked());
    }
    else if(l_class_name == "QSpinBox")
    {
        GData::getUniqueInstance().setSettingsValue(l_full_key, static_cast<QSpinBox*>(p_object)->value());
    }
    else if(l_class_name == "QFrame")
    {
        QColor l_color =  static_cast<QFrame*>(p_object)->palette().color(QPalette::Window);
        GData::getUniqueInstance().setSettingsValue(l_full_key,l_color.name().toStdString());
    }
    else if("QVBoxLayout" != l_class_name &&
            "QHBoxLayout" != l_class_name &&
            "QLabel" != l_class_name &&
            "QPushButton" != l_class_name &&
            "QGridLayout" != l_class_name
           )
    {
        abort();
    }
}

//------------------------------------------------------------------------------
void TartiniSettingsDialog::saveSettings()
{
    // Go through all the categories on the left, and save all the preferences we can from the fields.
    // Combo boxes must be done separately.
    //Iterate over all the groups
    for(int l_i = 0; l_i < tabWidget->count(); l_i++)
    {
        //Iterate over all widgets in the current group and save their settings
        std::string l_group(tabWidget->tabText(l_i).toStdString());
        // Search group in tab names saved during load settings and if not
        // found replace it with the one save, this is certainly due to a memory issue
        auto l_iter = m_tab_names.find(l_i);
        assert(m_tab_names.end() != l_iter);
        if(l_iter->second != l_group)
        {
            std::cerr << "Warning wrong tab name \"" << l_group << "\" should be \"" << l_iter->second << "\"" << std::endl;
            l_group = l_iter->second;
        }

        const QList<QObject*> & l_widgets = tabWidget->widget(l_i)->children();
        for(QList<QObject*>::const_iterator l_widget_iterator=l_widgets.begin(); l_widget_iterator < l_widgets.end(); ++l_widget_iterator)
        {
            saveSetting(*l_widget_iterator, l_group);
        }
    }
    GData::getUniqueInstance().syncSettings();
    QApplication::postEvent(g_main_window, new QEvent(static_cast<QEvent::Type>(SETTINGS_CHANGED)));
    TartiniSettingsDialog::accept();
}

//------------------------------------------------------------------------------
void TartiniSettingsDialog::checkAnalysisEnabled()
{
    QComboBox * l_note_range_choice = tabWidget->widget(0)->findChild<QComboBox *>("noteRangeChoice");
    myassert(l_note_range_choice);
  
    int l_choice = l_note_range_choice->currentIndex();

    QGroupBox * l_buffer_size_group_box = tabWidget->widget(2)->findChild<QGroupBox*>("bufferSizeGroupBox");
    myassert(l_buffer_size_group_box);
    QGroupBox * l_step_size_group_box = tabWidget->widget(2)->findChild<QGroupBox*>("stepSizeGroupBox");
    myassert(l_step_size_group_box);

    if(l_choice == 0)
    {
        l_buffer_size_group_box->setEnabled(true);
        l_step_size_group_box->setEnabled(true);
    }
    else
    {
        l_buffer_size_group_box->setEnabled(false);
        l_step_size_group_box->setEnabled(false);
    }
}

//------------------------------------------------------------------------------
void TartiniSettingsDialog::onNoteRangeChoice(int p_choice)
{
    QSpinBox * l_buffer_size_value = tabWidget->widget(2)->findChild<QSpinBox*>("bufferSizeValue");
    myassert(l_buffer_size_value);
    QComboBox * l_buffer_size_unit = tabWidget->widget(2)->findChild<QComboBox*>("bufferSizeUnit");
    myassert(l_buffer_size_unit);
    QCheckBox * l_buffer_size_round = tabWidget->widget(2)->findChild<QCheckBox*>("bufferSizeRound");
    myassert(l_buffer_size_round);
    QSpinBox * l_step_size_value = tabWidget->widget(2)->findChild<QSpinBox*>("stepSizeValue");
    myassert(l_step_size_value);
    QComboBox * l_step_size_unit = tabWidget->widget(2)->findChild<QComboBox*>("stepSizeUnit");
    myassert(l_step_size_unit);
    QCheckBox * l_step_size_round = tabWidget->widget(2)->findChild<QCheckBox*>("stepSizeRound");
    myassert(l_step_size_round);

    switch(p_choice)
    {
        case 1:
            l_buffer_size_value->setValue(96);
            l_step_size_value->setValue(48);
            break;
        case 2:
            l_buffer_size_value->setValue(48);
            l_step_size_value->setValue(24);
            break;
        case 3:
            l_buffer_size_value->setValue(24);
            l_step_size_value->setValue(12);
            break;
        case 4:
            l_buffer_size_value->setValue(12);
            l_step_size_value->setValue(6);
            break;
    }
    if(p_choice > 0)
    {
        l_buffer_size_unit->setCurrentIndex(l_buffer_size_unit->findText("milli-seconds"));
        l_step_size_unit->setCurrentIndex(l_step_size_unit->findText("milli-seconds"));
        l_buffer_size_round->setChecked(true);
        l_step_size_round->setChecked(true);
    }
    checkAnalysisEnabled();
}

#define SetIfMissing(p_key, p_value) \
  if(!p_gdata.settingsContains(p_key)) p_gdata.setSettingsValue(p_key, p_value)

//------------------------------------------------------------------------------
void TartiniSettingsDialog::setUnknownsToDefault(GData & p_gdata)
{
    SetIfMissing("General/bindOpenSaveFolders", true);
    SetIfMissing("General/tempFilesFolder", QDir::toNativeSeparators(QDir::currentPath()).toStdString());
    SetIfMissing("General/filenameGeneratingString", "Untitled");
    SetIfMissing("General/fileGeneratingNumber", 1);
    SetIfMissing("General/fileNumberOfDigits", 2);
    SetIfMissing("General/noteRangeChoice", "Notes F1 and higher - Cello, Guitar, Bass Clarinet, General sounds ...");

    SetIfMissing("View/autoFollow", false);
    SetIfMissing("View/backgroundShading", true);
    SetIfMissing("View/freqA", 440);

    SetIfMissing("Sound/soundInput", "Default");
    SetIfMissing("Sound/soundOutput", "Default");
    SetIfMissing("Sound/numberOfBuffers", 4);
    SetIfMissing("Sound/numberOfChannels", "Mono");
    SetIfMissing("Sound/sampleRate", 44100);
    SetIfMissing("Sound/bitsPerSample", 16);
    SetIfMissing("Sound/muteOutput", true);

    SetIfMissing("Analysis/bufferSizeValue", 48);
    SetIfMissing("Analysis/bufferSizeUnit", "milli-seconds");
    SetIfMissing("Analysis/bufferSizeRound", true);
    SetIfMissing("Analysis/stepSizeValue", 24);
    SetIfMissing("Analysis/stepSizeUnit", "milli-seconds");
    SetIfMissing("Analysis/stepSizeRound", true);
    SetIfMissing("Analysis/doingHarmonicAnalysis", true);
    SetIfMissing("Analysis/doingFreqAnalysis", true);
    SetIfMissing("Analysis/doingEqualLoudness", true);
    SetIfMissing("Analysis/doingAutoNoiseFloor", true);
    SetIfMissing("Analysis/doingDetailedPitch", true);
    SetIfMissing("Analysis/analysisType", "MPM");
    SetIfMissing("Analysis/thresholdValue", 93);

    SetIfMissing("Display/fastUpdateSpeed", 75);
    SetIfMissing("Display/slowUpdateSpeed", 150);
    SetIfMissing("Display/updateForEachChunk", false);
    SetIfMissing("Display/theBackgroundColor", "#BBCDE2");
    SetIfMissing("Display/shading1Color", "#BBCDEF");
    SetIfMissing("Display/shading2Color", "#CBCDE2");
    SetIfMissing("Display/displayBackgroundShading", true);
    SetIfMissing("Display/useTopLevelWidgets", false);

    SetIfMissing("Dialogs/rememberOpenFolder", true);
    SetIfMissing("Dialogs/openFilesFolder", QDir::toNativeSeparators(QDir::currentPath()).toStdString());
    SetIfMissing("Dialogs/rememberSaveFolder", true);
    SetIfMissing("Dialogs/saveFilesFolder", QDir::toNativeSeparators(QDir::currentPath()).toStdString());
    SetIfMissing("Dialogs/appendWav", true);

    SetIfMissing("Advanced/showMeanVarianceBars", false);
    SetIfMissing("Advanced/savingMode", "Ask when closing unsaved files (normal)");
    SetIfMissing("Advanced/vibratoSineStyle", false);
    SetIfMissing("Advanced/mouseWheelZooms", false);
}

//------------------------------------------------------------------------------
void TartiniSettingsDialog::resetDefaults()
{
    GData::getUniqueInstance().clearSettings();
    setUnknownsToDefault(GData::getUniqueInstance());
    GData::getUniqueInstance().syncSettings();
    init();
}

std::map<int, std::string> TartiniSettingsDialog::m_tab_names;
// EOF
