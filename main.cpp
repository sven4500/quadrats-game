#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <quadrats.h>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    CQuadrats widget;

    QDesktopWidget* const desktop = app.desktop();

    // Центрируем окно по середине экрана.
    if(desktop != 0)
    {
        QRect const rect = desktop->availableGeometry();
        int const size = std::min(rect.width(), rect.height()) * 0.75;

        widget.resize(size, size);
        widget.move(rect.center().x() - size / 2, rect.center().y() - size / 2);
    }

    widget.show();
    return app.exec();
}
