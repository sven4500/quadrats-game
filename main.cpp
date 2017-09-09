#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include "CQuadrats.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    CQuadrats widget;

    // центрируем окно по середине экрана
    {
        QDesktopWidget* const desktop = app.desktop();

        if(desktop != 0)
        {
            QRect const rect = desktop->availableGeometry();
            unsigned int const size = std::min(rect.width(), rect.height()) * 0.6;
            widget.resize(size, size);
            widget.move(rect.center().x() - size / 2, rect.center().y() - size / 2);
        }
    }

    widget.show();
    return app.exec();
}
