#pragma once
#ifndef SETTINGSDIALOG_H_
#define SETTINGSDIALOG_H_

#include <QDialog>

class SettingsDialog: public QDialog
{
public:
    SettingsDialog(QWidget* parent = 0);
    virtual ~SettingsDialog();

protected:
    int _frameRate;
    int _dim;

};

#endif
