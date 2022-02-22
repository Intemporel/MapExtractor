#include "MapExtractor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MapExtractor w;
    w.show();
    return a.exec();
}
