#include <settingsdialog.h>

SettingsDialog::SettingsDialog(QWidget* parent):
    QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    setWindowTitle("Настройки");
}

SettingsDialog::~SettingsDialog()
{}
