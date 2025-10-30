#include "mainwindow.h"

#include <QApplication>
#include <QtCore>
#include <QObject>
#include <QStyleFactory>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    qApp->setStyle(QStyleFactory::create("Fusion"));

    MainWindow w;
    w.show();
    return a.exec();
}
