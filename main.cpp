#include "CQuadrats.h"
#include <QApplication>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    CQuadrats widget;
    widget.show();
    return app.exec();
}
