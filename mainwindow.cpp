#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->labelPlayer1->setFont(QFont("Arial",14,QFont::Normal));
    ui->labelPlayer1->setStyleSheet("QLabel{color:blue;}");
    ui->labelPlayer2->setFont(QFont("Arial",14,QFont::Normal));
    ui->labelPlayer2->setStyleSheet("QLabel{color:red;}");
    // triggered là được kích hoat;
    connect(ui->actionNewGame,&QAction::triggered,this,&MainWindow::startNewGame);
    connect(ui->actionQuitGame,&QAction::triggered,qApp,&QApplication::quit);
    // Singleton
    configuration = GameConfiguration::getInstance();
    // Singal TicTacToeWidge.cpp
    connect(ui->tictactoe,&TicTacToeWidet::changePlayer,this,&MainWindow::boldCurrentPlayerName);
    // determineGameOutCome : Xác định người chiến thắng
    connect(ui->tictactoe,&TicTacToeWidet::finishedGame,this,&MainWindow::determineGameOutCome);
    // a connection to start a Ai Mode
    connect(this,&MainWindow::startAiMode,ui->tictactoe,&TicTacToeWidet::setAiMode);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startNewGame()
{
    qDebug() <<"Create new Game";


    ui->labelPlayer1->setText("");
    ui->labelPlayer2->setText("");

    // Set game size
    configuration->setGameSize(SizeRange::minRange);
    configuration->setPlayer1Name("");
    configuration->setPlayer2Name("");

    // Thiết lập chế độ 2 người chơi đầu tiên
    ui->tictactoe->setTwoPlayerMode();
    // reset some game configuration
    configuration->resetSomeConfig();

    //  Phương thức exec() sẽ mở một dialog modal, nghĩa là nó sẽ chặn tất cả các sự kiện đến các cửa sổ khác trong ứng dụng cho đến khi dialog này được đóng.
    // mở cửa sổ GameConfiguration.h
    if(configuration->exec() == QDialog::Rejected)
    {
        return;
    }

    if(configuration->getPlayer2Name() == DataSpeical::aiName)
    {
        qDebug() << "Mode : --> AI";
        ui->tictactoe->resetContainers();
        emit startAiMode();
    }

    //
    ui->labelPlayer1->setText(configuration->getPlayer1Name());
    ui->labelPlayer2->setText(configuration->getPlayer2Name());
    //  Game Size
    int gameSize = configuration->getGameSize();
    // Chế độ chơi

    // Thiết lập chiều cao , chiều rộng của tictactoewidet.cpp
    ui->tictactoe->setFixedHeight(Data::widthFactor * (gameSize + Data::boardSpacing));
    ui->tictactoe->setFixedWidth(Data::widthFactor * (gameSize + Data::boardSpacing));
    // tictactoe là Widget(tictactoewidet.h) được liên kết trong MainWindown.ui
    ui->labelGameTitle->setVisible(false);
    // Thiết Lâp SizeGame( Ma Trân) và PointWin( Điểm để chiến thắng)
    ui->tictactoe->setGameSize(gameSize);
    ui->tictactoe->setPointWin(configuration->getPointWin());
    // Sstart Game
    ui->tictactoe->StartOrRestartGame();
    ui->tictactoe->setEnabled(true);
}


// In đậm người chơi hiện tại
void MainWindow::boldCurrentPlayerName()
{
    QFont playerFont = ui->labelPlayer1->font();
    // In đậm người chơi player1
    playerFont.setBold(ui->tictactoe->getPlayer() == Player::Player1);
    ui->labelPlayer1->setFont(playerFont);

    // In đậm người chơi player2
    playerFont.setBold(ui->tictactoe->getPlayer() == Player::Player2);
    ui->labelPlayer2->setFont(playerFont);
}

void MainWindow::determineGameOutCome()
{
    Winner winner = ui->tictactoe->getWinner();
    if(winner == Winner::WinnerPlayer1)
    {
        ui->tictactoe->setGameOutComeMessage("The Winner is: " + ui->labelPlayer1->text());
    }
    else if(winner == Winner::WinnerPlayer2)
    {
        ui->tictactoe->setGameOutComeMessage("The Winner is: " + ui->labelPlayer2->text());
    }
    else if(winner == Winner::Draw)
    {
        ui->tictactoe->setGameOutComeMessage("Draw Game");
    }
}
