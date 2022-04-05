#include "mainwindow.h"
#include<QDir>
#include <QApplication>
#include<QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file(QString(":/dark.css").arg("stylesheet"));
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);

    MainWindow w;

    w.show();

    return a.exec();
}
