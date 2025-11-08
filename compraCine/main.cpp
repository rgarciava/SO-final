#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w(6, 8); // filas x columnas (ajusta)
    w.show();

    return a.exec();
}
