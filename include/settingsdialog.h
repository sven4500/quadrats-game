#pragma once
#ifndef SETTINGSDIALOG_H_
#define SETTINGSDIALOG_H_

#include <QDialog>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <stdint.h> // uint16_t

class SettingsDialog: public QDialog
{
public:
    SettingsDialog(QWidget* parent = 0);
    virtual ~SettingsDialog();

    int getDim()const;
    int getFPS()const;
    QString getIpAddress()const;
    uint16_t getPort()const;

protected:
    QSpinBox* _frameRateSpin;
    QSpinBox* _dimSpin;
    QLineEdit* _ipAddrEdit;
    QLineEdit* _portEdit;

    QPushButton* _okBut;
    QPushButton* _cancelBut;

};

#endif
