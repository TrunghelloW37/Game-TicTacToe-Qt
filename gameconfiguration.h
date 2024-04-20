#ifndef GAMECONFIGURATION_H
#define GAMECONFIGURATION_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
class GameConfiguration;
}

struct SizeRange{
    static constexpr int minRange = 3;
    static constexpr int maxRange = 15;
};

struct DataSpeical{
    static constexpr char* aiName = "Mister AI";
};


class GameConfiguration : public QDialog
{
    Q_OBJECT

public:
    ~GameConfiguration();
    // setter player
    void setPlayer1Name(const QString& name) const;
    void setPlayer2Name(const QString& name) const;
    // getter for playerName
    QString getPlayer1Name() const;
    QString getPlayer2Name() const;
    // Setter - Getter for the game Size
    void setGameSize(int size) const;
    int getGameSize() const;
    // setter - getter PointWin
    void setPointWin(int);
    int getPointWin();
    void resetSomeConfig();
    // singleton Function
    static GameConfiguration* getInstance();

public slots:
    void updateOkButton();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_radioButton_AI_Mode_clicked();
    void on_radioButton_PlayerMode_clicked();

private:
    Ui::GameConfiguration *ui;
    explicit GameConfiguration(QWidget *parent = nullptr);
    static GameConfiguration* gameConfiguration;
};

#endif // GAMECONFIGURATION_H
