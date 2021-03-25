#include "session.h"
#include "join.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Session w;
    Join w;
    w.show();
    return a.exec();
}
