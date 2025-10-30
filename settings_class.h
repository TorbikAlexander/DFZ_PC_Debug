#ifndef SETTINGS_CLASS_H
#define SETTINGS_CLASS_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

#include "vadc_class.h"

class Settings_class
{
public:
     Settings_class();
    ~Settings_class();
     void SaveSettings(QString sett_file, struct _vadc_params *VADC_Params);
     void ReadSettings(QString sett_file, struct _vadc_params *VADC_Params);
};

#endif // SETTINGS_CLASS_H
