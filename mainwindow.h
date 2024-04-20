#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gameconfiguration.h"


struct Data{
    static constexpr int widthFactor = 50;
    static constexpr int boardSpacing = 1;

};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void startNewGame(); // Bắt đầu Game
    void boldCurrentPlayerName(); // In đậm tên người chơi hiện tại
    void determineGameOutCome(); // Xác định kết quả trận đấu

signals:
    void startAiMode();

private:
    Ui::MainWindow *ui;
    GameConfiguration* configuration;
};

#endif // MAINWINDOW_H
