#ifndef TICTACTOEWIDET_H
#define TICTACTOEWIDET_H

#include <QWidget>
#include <QList>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSignalMapper>
#include <QTimer>
#include <time.h>
#include <stdlib.h>
#include <memory.h>
QT_BEGIN_NAMESPACE
namespace Ui { class TicTacToeWidet; }
QT_END_NAMESPACE



struct MetaData{
    //static constexpr int ROWSCOLUMNS = 4;
    //static constexpr int BOARD_SIZE = ROWSCOLUMNS * ROWSCOLUMNS;
    static constexpr const char* spaceCharacter = " ";
    static constexpr const char* player1Symbol = "X";
    static constexpr const char* player2Symbol = "O";
    static constexpr const char* player1Color = "blue";
    static constexpr const char* player2Color = "red";
    static constexpr const char* drawColor = "purple";
    static constexpr int endOfGameWidth = 500;
    static constexpr int widthFactor = 50;
    static constexpr int boardSpacing = 1;
    static constexpr int aiDelayDuratuion = 800;
};

// enum Player
enum Player{
    Player1,
    Player2
};

// enum Winner
enum Winner{
    WinnerPlayer1 , WinnerPlayer2 , Draw, NotWinnerYet
};

enum Mode{
    AiMode, TwoPlayerMode
};

class TicTacToeWidet : public QWidget
{
    Q_OBJECT

public:
    TicTacToeWidet(QWidget *parent = nullptr);
    ~TicTacToeWidet();
    // Làm mới bảng đãnh (x - o)
    void restBoard();
    // setter player
    void setCurrentPlayer(Player p);
    // getter player
    Player getPlayer() const;
    // setter Game Size
    void setGameSize(int);
    // setter PointWin
    void setPointWin(int);
    // getter Winner
    Winner getWinner();
    //
    void setGameOutComeMessage(const QString&);
    void resetContainers(); // đặt lại Vùng chứa
    void transmitAiMove(int);



signals:
    void finishedGame();
    void changePlayer();
    // a  tín hiệu truyền các hướng đi của AI
    void sendAiMove(int);
    // singal which signal ai mode
    void triggerAi();
    void startAiMoveCalculation();

private slots:
    void handleClickesOnBoard(int); // Hàm này được gọi khi nhấn vào ô kèm vị trí
    void handleEndOfGame();
    // a slot which trigger ai move calculation
    void triggerAiMoveCalculation();
    void calculateAiMove();

public slots:
    void StartOrRestartGame();
    // a slot to set AI mode ( Singal MainWindown : startAiMode )
    void setAiMode();
    void setTwoPlayerMode();

private:
    void creatBoard();
    // Winner : Kiểu Enum
    // determine Winner : Xác định người chiến thắng
    Winner determineWinner(const QString& , int);

private:
    Ui::TicTacToeWidet *ui;
    QList<QPushButton*> board; //
    Player m_player;
    Winner m_winner;
    // Kích thước Ma Trận
    int m_gameSize;
    // Điểm để thắng
    int m_pointWin;
    QString gameOutComeMessage;
    Mode m_mode;
    int player1LastMove;
    int playerAiLastMove;
    QList<int> player1Move;
    QList<int> playerAiMove;

};
#endif // TICTACTOEWIDET_H
