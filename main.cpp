#include "TTLEditor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TTLEditor w;
    w.show();
    return a.exec();
}
