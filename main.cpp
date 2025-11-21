#include "bank_manager_project.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    bank_manager_project w;
    w.show();
    return a.exec();
}
