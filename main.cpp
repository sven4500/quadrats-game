#include "CQuadrats.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CQuadrats w;
    w.show();

    return a.exec();
}
