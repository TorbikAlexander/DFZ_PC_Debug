#include "settings_class.h"
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QList>
#include <string>


Settings_class::Settings_class()
{

}

Settings_class::~Settings_class()
{

}

void Settings_class::SaveSettings(QString sett_file, struct _vadc_params *VADC_Params)
{
    QSettings *settings = new QSettings(sett_file,QSettings::IniFormat);
    QString sett;

    sett = "VADC/EN";
    settings->setValue(sett,VADC_Params->en_VADC);

    sett = "VADC/syncUa";
    settings->setValue(sett,VADC_Params->syncUa);

    sett = "VADC/Ua";
    settings->setValue(sett,VADC_Params->Ua);
    sett = "VADC/degUa";
    settings->setValue(sett,VADC_Params->degUa);

    sett = "VADC/Ub";
    settings->setValue(sett,VADC_Params->Ub);
    sett = "VADC/degUb";
    settings->setValue(sett,VADC_Params->degUb);

    sett = "VADC/Uc";
    settings->setValue(sett,VADC_Params->Uc);
    sett = "VADC/degUc";
    settings->setValue(sett,VADC_Params->degUc);

    sett = "VADC/U3U0";
    settings->setValue(sett,VADC_Params->U3U0);
    sett = "VADC/deg3U0";
    settings->setValue(sett,VADC_Params->deg3U0);

    sett = "VADC/Ia";
    settings->setValue(sett,VADC_Params->Ia);
    sett = "VADC/degIa";
    settings->setValue(sett,VADC_Params->degIa);

    sett = "VADC/Ib";
    settings->setValue(sett,VADC_Params->Ib);
    sett = "VADC/degIb";
    settings->setValue(sett,VADC_Params->degIb);

    sett = "VADC/Ic";
    settings->setValue(sett,VADC_Params->Ic);
    sett = "VADC/degIc";
    settings->setValue(sett,VADC_Params->degIc);

    settings->sync(); //записываем настройки

    delete settings;
}

void Settings_class::ReadSettings(QString sett_file, struct _vadc_params *VADC_Params)
{
    QSettings *settings = new QSettings(sett_file,QSettings::IniFormat);
    QString sett;

    sett = "VADC/EN";
    VADC_Params->en_VADC = settings->value(sett,0).toBool();

    sett = "VADC/syncUa";
    VADC_Params->syncUa = settings->value(sett,0).toBool();

    sett = "VADC/Ua";
    VADC_Params->Ua = settings->value(sett,0).toFloat();
    sett = "VADC/degUa";
    VADC_Params->degUa = settings->value(sett,0).toUInt();

    sett = "VADC/Ub";
    VADC_Params->Ub = settings->value(sett,0).toFloat();
    sett = "VADC/degUb";
    VADC_Params->degUb = settings->value(sett,0).toUInt();

    sett = "VADC/Uc";
    VADC_Params->Uc = settings->value(sett,0).toFloat();
    sett = "VADC/degUc";
    VADC_Params->degUc = settings->value(sett,0).toUInt();

    sett = "VADC/U3U0";
    VADC_Params->U3U0 = settings->value(sett,0).toFloat();
    sett = "VADC/deg3U0";
    VADC_Params->deg3U0 = settings->value(sett,0).toUInt();

    sett = "VADC/Ia";
    VADC_Params->Ia = settings->value(sett,0).toFloat();
    sett = "VADC/degIa";
    VADC_Params->degIa = settings->value(sett,0).toUInt();

    sett = "VADC/Ib";
    VADC_Params->Ib = settings->value(sett,0).toFloat();
    sett = "VADC/degIb";
    VADC_Params->degIb = settings->value(sett,0).toUInt();

    sett = "VADC/Ic";
    VADC_Params->Ic = settings->value(sett,0).toFloat();
    sett = "VADC/degIc";
    VADC_Params->degIc = settings->value(sett,0).toUInt();

    delete settings;
}
