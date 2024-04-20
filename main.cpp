#include "tictactoewidet.h"
#include "gameconfiguration.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow windown;
    windown.show();
    return  a.exec();
}
