#include <QLabel>
#include <QGridLayout>
#include <settingsdialog.h>

SettingsDialog::SettingsDialog(QWidget* parent):
    QDialog(parent)
{
    _frameRateSpin = new QSpinBox(this);
    _frameRateSpin->setSuffix(" к/с");
    _frameRateSpin->setMinimum(1);
    _frameRateSpin->setMaximum(60);
    _frameRateSpin->setValue(20);

    _dimSpin = new QSpinBox(this);
    _dimSpin->setSingleStep(2);
    _dimSpin->setMinimum(5);
    _dimSpin->setMaximum(999);
    _dimSpin->setValue(11);

    _ipAddrEdit = new QLineEdit(this);
    _ipAddrEdit->setInputMask("D00.900.900.900");
    _ipAddrEdit->setText("127.0.0.1");

    _portEdit = new QLineEdit(this);
    _portEdit->setText("8200");

    _okBut = new QPushButton(this);
    _okBut->setText("ОК");

    _cancelBut = new QPushButton(this);
    _cancelBut->setText("Отмена");

    QLabel* const label[4] = {
        new QLabel("Частота кадров", this),
        new QLabel("Размер поля", this),
        new QLabel("IP адрес", this),
        new QLabel("Порт", this)
    };

    QGridLayout* const layout = new QGridLayout(this);

    layout->addWidget(label[0], 0, 0);
    layout->addWidget(label[1], 1, 0);
    layout->addWidget(label[2], 2, 0);
    layout->addWidget(label[3], 3, 0);

    layout->addWidget(_frameRateSpin, 0, 1, 1, 3);
    layout->addWidget(_dimSpin, 1, 1, 1, 3);
    layout->addWidget(_ipAddrEdit, 2, 1, 1, 3);
    layout->addWidget(_portEdit, 3, 1, 1, 3);

    layout->addWidget(_okBut, 4, 2);
    layout->addWidget(_cancelBut, 4, 3);

    setLayout(layout);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint |
        Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setWindowTitle("Настройки");
}

SettingsDialog::~SettingsDialog()
{}
