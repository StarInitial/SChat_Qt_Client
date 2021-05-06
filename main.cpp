#include "session.h"
#include "join.h"

#include <QApplication>
#include <setting.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Join w;
    //Setting w;
    w.show();
    return a.exec();
}
