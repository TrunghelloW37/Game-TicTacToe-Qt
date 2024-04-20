#include "tictactoewidet.h"
#include "ui_tictactoewidet.h"
#include <QDebug>
TicTacToeWidet::TicTacToeWidet(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TicTacToeWidet),
    m_player(Player::Player1),
    m_winner(Winner::NotWinnerYet),
    m_pointWin(3),
    m_mode(Mode::TwoPlayerMode)
{
    ui->setupUi(this);
    // Connect Signal AI
    // Tín hiệu SendAIMove kèm vị trí của Ô mà Ai đánh sang cho handleClickesOnBoard
    connect(this,&TicTacToeWidet::sendAiMove,this,&TicTacToeWidet::handleClickesOnBoard);
    // Tín hiệu này (Kiểm tra nếu Player2 đánh thì disable) để Ai đánh
    connect(this,&TicTacToeWidet::triggerAi,this,&TicTacToeWidet::triggerAiMoveCalculation);
}


TicTacToeWidet::~TicTacToeWidet()
{
    delete ui;
}

//
void TicTacToeWidet::restBoard()
{
    for(int i = 0; i<m_gameSize * m_gameSize; i++)
    {
        // board là QList<QPushPutton*> chứa các button
        board.at(i)->setText(MetaData::spaceCharacter);
    }
}

// setter player
void TicTacToeWidet::setCurrentPlayer(Player p)

{
    m_player = p;
    if(m_mode == Mode::AiMode)
    {
        emit triggerAi();
    }

}

Player TicTacToeWidet::getPlayer() const
{
    return m_player;
}

void TicTacToeWidet::setGameSize(int gs)
{
    m_gameSize = gs;
}

void TicTacToeWidet::setPointWin(int point)
{
    m_pointWin = point;
}

Winner TicTacToeWidet::getWinner()
{
    return m_winner;
}

void TicTacToeWidet::setGameOutComeMessage(const QString& message)
{
    gameOutComeMessage = message;
}

// Hàm đặt lại Vùng chứa
void TicTacToeWidet::resetContainers()
{
    player1Move.clear();
    playerAiMove.clear();
}

// Hàm Đánh X hay O vào trong bảng
void TicTacToeWidet::handleClickesOnBoard(int buttonIndex)
{
    //qDebug() << "Index Button Cliecked: " << buttonIndex;
    //
    if(buttonIndex < 0 || buttonIndex >= board.size())
    {
        return;
    }
    QPushButton* button = board.at(buttonIndex);
    button->setFont(QFont("Tahoma",12,QFont::Bold));

    // Nếu nhấn vào ô đã có dấu ("X" hoặc "O") thì return
    if(button->text() != MetaData::spaceCharacter)
    {
        return;
    }

    // Nếu Nhấn vào ô chưa có gì thì Player1 thì X còn Player2 thì O
    QString symbol; // biểu tượng
    if(m_player == Player::Player1)
    {
        player1LastMove = buttonIndex;
        symbol = MetaData::player1Symbol;
        button->setText(symbol);
        button->setStyleSheet(QString("QPushButton{color:")+ MetaData::player1Color +"; background: lightyellow;}");
        button->setDisabled(true); // --> Nhấn rồi thì không nhấn lại được nữa
    }
    else if(m_player == Player::Player2)
    {
        symbol = MetaData::player2Symbol;
        button->setText(symbol);
        button->setStyleSheet(QString("QPushButton{color:")+ MetaData::player2Color +"; background: lightgreen;}");
        button->setDisabled(true);
    }
    // Xác định Winner goi hàm determineWinner
    m_winner = determineWinner(symbol,buttonIndex);
    qDebug() << "m_winner: " << m_winner;
    // NotwinnerYet có nghĩa là người chơi đang đánh
    if(m_winner == Winner::NotWinnerYet)
    {
        if(m_player == Player::Player1)
        {
            setCurrentPlayer(Player::Player2);
            emit changePlayer();
        }
        else if(m_player == Player::Player2)
        {
            setCurrentPlayer(Player::Player1);
            emit changePlayer();
        }
    }
    //
    else
    {
        this->setDisabled(true);
        // Sau ba giây thì QTimer phát ra tín hiệu finishedGame()
        QTimer::singleShot(3000,this,SIGNAL(finishedGame()));
        connect(this,SIGNAL(finishedGame()),this,SLOT(handleEndOfGame()));
    }

}

// Đoạn này nó xóa các widget và các mục layout có trong GridLayout khi Game kêt thúc để giải phóng bộ nhớ
// Khi gàm này được gọi nó xóa các ô  trong bảng sau đó hiển thị thông báo
void TicTacToeWidet::handleEndOfGame()
{
    qDebug() <<"GameOver";
    // 1. Xóa mục layout và widget của đối tượng hiện tại  ( Ở đây là QGridLayout)
    QLayout* layout =  this->layout(); // lấy layout hiện tại của widget
    QLayoutItem* itemLayout; //
    while(layout != nullptr && (itemLayout = layout->takeAt(0))!= nullptr)
    {
        delete itemLayout->widget();
        delete itemLayout;
    }
    delete layout;
    board.clear(); // Làm sạch QList<QPushButton*>

    // 2. Creater tạo ra Button và Layout để bắt đầu game mới khi nhấn vào Button
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setAlignment(Qt::AlignCenter);
    //
    QLabel* restartLabel = new QLabel(this);
    QPushButton* restartButton = new QPushButton();
    QString restartLabelColor;
    QString restartButtonColor;
    if(m_winner == Winner::WinnerPlayer1)
    {
        restartLabelColor = QString("QLabel{color:")+ MetaData::player1Color + ";}";
        restartButtonColor = QString("QPushButton{color:")+ MetaData::player1Color + ";}";
    }
    else if(m_winner == Winner::WinnerPlayer2)
    {
        restartLabelColor = QString("QLabel{color:")+ MetaData::player2Color + ";}";
        restartButtonColor = QString("QPushButton{color:")+ MetaData::player2Color + ";}";
    }
    else if(m_winner == Winner::Draw)
    {
        restartLabelColor = QString("QLabel{color:")+ MetaData::drawColor + ";}";
        restartButtonColor = QString("QPushButton{color:")+ MetaData::drawColor + ";}";
    }

    //styleButton
    restartButton->setMinimumHeight(40);
    restartButton->setMinimumWidth(100);
    restartButton->setText("Restart");
    this->setMinimumWidth(MetaData::endOfGameWidth);
    restartButton->setFont(QFont("Liberation Serif" , 14 , QFont::Bold));
    restartButton->setStyleSheet(restartButtonColor);
    // StyleLabel
    restartLabel->setText(gameOutComeMessage);
    restartLabel->setFont(QFont("Liberation Serif" , 14 , QFont::Bold));
    restartLabel->setStyleSheet(restartLabelColor);
    // VerticalLayout
    vLayout->addWidget(restartLabel);
    vLayout->addWidget(restartButton);
        // Được sử dụng để kích hoạt một widget. Khi một widget được kích hoạt (enabled), người dùng có thể tương tác với nó
    this->setEnabled(true);
    connect(restartButton,SIGNAL(clicked()),this,SLOT(StartOrRestartGame()));
}

// Nếu đến người chơi thứ 2 thì Disable để AI đánh -
void TicTacToeWidet::triggerAiMoveCalculation()
{
    if(m_player == Player::Player2)
    {
        // Đến lượt Ai thì bật tính đường củaAI tính đường thông qua hàm calculateAiMove
        // Delay 1 giây tính AI Tính đường
        QTimer::singleShot(MetaData::aiDelayDuratuion,this,SIGNAL(startAiMoveCalculation()));
        connect(this,&TicTacToeWidet::startAiMoveCalculation,this,&TicTacToeWidet::calculateAiMove);
        this->setDisabled(true);
    }
    else if(m_player == Player::Player1)
    {
        // Nếu đến Player1 đến thì tắt tính đường của Ai
        disconnect(this,&TicTacToeWidet::startAiMoveCalculation,this,&TicTacToeWidet::calculateAiMove);
        this->setEnabled(true);
    }
}

//
void TicTacToeWidet::calculateAiMove()
{
    // Tính ô vừa đi của Player1
    int rowNumber = player1LastMove / m_gameSize;
    int columnNumber = player1LastMove % m_gameSize;
    player1Move.push_back(player1LastMove);

    int verticalCounter = 0;
    bool skipSencondCheck = false;
    // unique_ptr : con trỏ độc quyền ( 1 con trỏ có quyền sở hữu độc quyền đối với đối tượng mà nó quản lý.)
    // không chia sẻ có các unique_ptr khác
    std::unique_ptr<int> verticalMove(nullptr);
    // I . Hàng dọc
    // Hướng lên
    while(--rowNumber >= 0 )
    {
        int verticalUpwardNeighbour = rowNumber * m_gameSize + columnNumber;
        // Kiểm tra đường đi của Player1 hướng lên xem đã có "O" chặn hay chưa
        if(playerAiMove.contains(verticalUpwardNeighbour))
        {
            // Hướng lên gặp "O" thì để nullptr để verticaMove dưới được gắn giá trij
            verticalMove = nullptr;
            break;
        }
        // Kiểm tra đường đi hướng lên của Player1 hướng lên xem đã có "X" hay chưa
        else if(player1Move.contains(verticalUpwardNeighbour))
        {
            ++verticalCounter;
        }
        // Nếu ô phía trên không có "X" hoặc "O" thì có thể ô này AI sẽ đánh vào
        else if(verticalMove == nullptr)
        {
            // trong vòng lặp while , verticalMove chỉ được gắn giá trị khi nó chưa có giá tri (nullptr)
            // Khi gắn verticalMove == nullptr lại thì mới được gắn giá trị tiếp theo
            verticalMove = std::make_unique<int>(verticalUpwardNeighbour);
        }
    }
    // Hướng xuống
    rowNumber = player1LastMove / m_gameSize;
    while( ++rowNumber < m_gameSize)
    {
        int verticalDownwarNeighbour = rowNumber * m_gameSize + columnNumber;
        if(playerAiMove.contains(verticalDownwarNeighbour))
        {
            break;
        }
        else if(player1Move.contains(verticalDownwarNeighbour))
        {
            ++verticalCounter;
        }
        else if(verticalMove == nullptr)
        {
            verticalMove = std::make_unique<int>(verticalDownwarNeighbour);
        }
    }


    // II Hàng ngang
    // 1. Hướng sang phải
    rowNumber = player1LastMove / m_gameSize;
    skipSencondCheck = false;
    int horizontalCounter = 0;
    std::unique_ptr<int> horizontalMove(nullptr);
    while(++columnNumber < m_gameSize)
    {
        int horizontalFowardNeighbour = rowNumber * m_gameSize + columnNumber;
        if(playerAiMove.contains(horizontalFowardNeighbour))
        {
            horizontalMove = nullptr;
            break;
        }
        else if(player1Move.contains(horizontalFowardNeighbour))
        {
            horizontalCounter++;
        }
        else if(horizontalMove == nullptr)
        {
            horizontalMove = std::make_unique<int>(horizontalFowardNeighbour);
        }
    }
    // 2. Hướng sang trái
    columnNumber = player1LastMove % m_gameSize;
    while(--columnNumber >= 0 )
    {
        int horizontalDownwarNeighbour = rowNumber * m_gameSize + columnNumber;
        if(playerAiMove.contains(horizontalDownwarNeighbour))
        {
            //horizontalCounter = 0;
            //horizontalMove = nullptr;
            break;
        }
        else if(player1Move.contains(horizontalDownwarNeighbour))
        {
            horizontalCounter++;
        }
        else if(horizontalMove == nullptr)
        {
            horizontalMove = std::make_unique<int>(horizontalDownwarNeighbour);
        }
    }

    // III > Hướng chéo
    // 1.
    /*
    0   1    2   3
    4   5    6   7
    8   9    10  11
    12  13   14  15
    */
    int forwardSlashCounter = 0; // bộ đếm dấu gạch chéo về phía trước
    std::unique_ptr<int> forwardSlashMove;
    skipSencondCheck = false;
    // Kiếm tra vị trị có nằm trên đường chéo ( 3 - 6 - 9 -12 ) hày không ??
    // 1. Hướng lên bên phải
    rowNumber = player1LastMove / m_gameSize;
    columnNumber = player1LastMove % m_gameSize;

    // Kiểm tra hướng lên bên phải ( chéo phải)
    while(--rowNumber >= 0 && ++columnNumber < m_gameSize)
    {
        // Vị trị hiện tại ở người chơi nhưng hướng lên bên phải 1 ô
        int forwardSlashUpwardNeighbour = rowNumber * m_gameSize + columnNumber;
        if(playerAiMove.contains(forwardSlashUpwardNeighbour))
        {
            forwardSlashMove = nullptr;
            break;
        }
        else if(player1Move.contains(forwardSlashUpwardNeighbour))
        {
            forwardSlashCounter++;
        }
        else if(forwardSlashMove == nullptr)
        {
            forwardSlashMove = std::make_unique<int>(forwardSlashUpwardNeighbour);
        }
    }

    // 2 Hướng xuống bên trái
    rowNumber = player1LastMove / m_gameSize;
    columnNumber = player1LastMove % m_gameSize;
    while(++rowNumber < m_gameSize && --columnNumber >= 0)
    {
        int forwardSlashDownNeighbour = rowNumber * m_gameSize + columnNumber;
        if(playerAiMove.contains(forwardSlashDownNeighbour))
        {
            //forwardSlashCounter = 0;
            //forwardSlashMove = nullptr;
            break;
        }
        else if(player1Move.contains(forwardSlashDownNeighbour))
        {
            forwardSlashCounter++;
        }
        else if(forwardSlashMove == nullptr)
        {
            forwardSlashMove = std::make_unique<int>(forwardSlashDownNeighbour);
        }
    }
    //


    // 2. Hướng dưới bên trái
    int backSlashCounter = 0;
    std::unique_ptr<int> backSlashMove(nullptr);
    skipSencondCheck = true;


    rowNumber = player1LastMove / m_gameSize;
    columnNumber = player1LastMove % m_gameSize;
    // hướng lên bên trái
    while(--rowNumber >= 0 && --columnNumber >= 0)
    {
        int backSlashUpwarNeighbour = rowNumber * m_gameSize + columnNumber;
        if(playerAiMove.contains(backSlashUpwarNeighbour))
        {
            backSlashMove = nullptr;
            //skipSencondCheck = true;
            break;

        }
        else if(player1Move.contains(backSlashUpwarNeighbour))
        {
            backSlashCounter++;
        }
        else if(backSlashMove == nullptr)
        {
            backSlashMove = std::make_unique<int>(backSlashUpwarNeighbour);
        }
    }
    // hướng xuống bên phải
    rowNumber = player1LastMove / m_gameSize;
    columnNumber = player1LastMove % m_gameSize;
    while(++rowNumber <  m_gameSize && ++columnNumber < m_gameSize)
    {
        int backSlashDownwarNeighbour = rowNumber * m_gameSize + columnNumber;
        if(playerAiMove.contains(backSlashDownwarNeighbour))
        {
            //backSlashCounter = 0;
            //backSlashMove = nullptr;
            //skipSencondCheck = true;
            break;
        }
        else if(player1Move.contains(backSlashDownwarNeighbour))
        {
            backSlashCounter++;
        }
        else if(backSlashMove == nullptr)
        {
            backSlashMove = std::make_unique<int>(backSlashDownwarNeighbour);
        }
    }

    // Đếm
    qDebug() << "VerticalCounter: " << verticalCounter;
    qDebug() << "HorizontalCounter: " << horizontalCounter;
    qDebug() << "forwardSlashCounter: " << forwardSlashCounter;
    qDebug() << "backSlashMove: " << backSlashCounter;

    // 1 Dọc
    if(verticalMove != nullptr && (verticalCounter >= horizontalCounter
        && verticalCounter >= forwardSlashCounter && verticalCounter >= backSlashCounter))
    {
        transmitAiMove(*verticalMove);
        playerAiMove.push_back(*verticalMove);
        return;
    }
    // Ngang
    else if (horizontalMove != nullptr && (horizontalCounter >= verticalCounter
         && horizontalCounter >= forwardSlashCounter && horizontalCounter >= backSlashCounter))
    {
        transmitAiMove(*horizontalMove);
        playerAiMove.push_back(*horizontalMove);
        return;
    }
    // Đường chéo từ phải qua trái
    else if (forwardSlashMove != nullptr && (forwardSlashCounter >= verticalCounter
                                             && forwardSlashCounter >= horizontalCounter && forwardSlashCounter >= backSlashCounter))
    {
        transmitAiMove(*forwardSlashMove);
        playerAiMove.push_back(*forwardSlashMove);
        return;
    }
    // Đường chéo từ trái qua phải
    else if (backSlashMove != nullptr && (backSlashCounter >= verticalCounter
                                          && backSlashCounter >= horizontalCounter && backSlashCounter >= forwardSlashCounter))
    {
        transmitAiMove(*backSlashMove);
        playerAiMove.push_back(*backSlashMove);
        return;
    }
    //
    for(int move = 0 ; move < m_gameSize * m_gameSize; move++)
    {
        if(!player1Move.contains(move) && playerAiMove.contains(move) == false)
        {
            transmitAiMove(move);
            playerAiMove.push_back(move);
            break;
        }
    }
}

void TicTacToeWidet::transmitAiMove(int move)
{
    emit sendAiMove(move);
}

/**
 * @brief TicTacToeWidet::creatBoard
 * a function which created the board of the tictactoe Game
 */
void TicTacToeWidet::creatBoard()
{
    QGridLayout* grid = new QGridLayout(this);
    // QSignalMapper được sử dụng để ánh xạ mỗi nút QPushButton tới một chỉ số duy nhất.
    // Điều này cho phép bạn xác định được nút nào đã được nhấn mà không cần tạo ra một slot riêng biệt cho mỗi nút.
    QSignalMapper* mapper = new QSignalMapper(this);
    for(int row = 0 ; row < m_gameSize;row++)
    {
        for(int column = 0 ; column < m_gameSize; column++)
        {
            QPushButton* button = new QPushButton(this);
            button->setText(MetaData::spaceCharacter);
            button->setMinimumWidth(50);
            button->setMinimumHeight(50);
            grid->addWidget(button,row,column);
            // thiết lập một ánh xạ giữa button và chỉ số hiện tại của button trong danh sách board
            // Khi một button được nhấn,
            // QSignalMapper sẽ phát ra tín hiệu mapped(int) với giá trị là chỉ số của nút đó trong danh sách board
            // VD Button1 ---> 0  |   Button2 ---> 1
            mapper->setMapping(button,board.size());
            /**
             * tín hiệu clicked() của QPushButton có một tham số là bool nhưng map() của QSignalMapper không chấp nhận bất kỳ tham số nào.
             * connect(button,&QPushButton::clicked,mapper,&QSignalMapper::map); --> Lỗi
             */
            // --> Cần sử dụng phiên bản không tham số của tín hiệu clicked()
            // Khi nút button được nhấn, tín hiệu clicked() sẽ được phát ra và slot map() (Phương thức của đối tượng mapper)
            // map() sau đó sẽ phát ra tín hiệu mapped(int) với giá trị là chỉ số của nút trong danh sách board
            connect(button,SIGNAL(clicked()),mapper,SLOT(map()));
            board.append(button);
        }
    }
    connect(mapper,&QSignalMapper::mappedInt,this,&TicTacToeWidet::handleClickesOnBoard);
}

// Hàm xác định người chiến thắng
// position : vị trí hiện tại
// newPosition : Ví trí ô mới để so sánh
// symbol : biểu tượng của ô hiện tại
Winner TicTacToeWidet::determineWinner(const QString &symbol, int position)
{
    // Xác định ô bằng vị trí của button.
    // Bảng 4x4 -
    // 0   1   2   3
    // 4   5   6   7
    // 7   9   10  11
    // 12  13  14  15
    // Nếu nhấn vào button có index == 6
    // rowNumber = 6/4 = 1
    // coloumNumer = 6 % 4 = 2
    // --> Button nằm ở ô [1,2]
    int rowNumber = position/ m_gameSize;
    int columnNumber = position % m_gameSize;

    // Couting Varibale
    int count = 0;
    bool validateSecondCheck = true; // Kiểm tra lần 2
    // validateSecondCheck: biến để kiểm tra ô bên phải có cùng dấu không ??
    //  + Nếu cùng dấu thì tiếp tục dịch sang bên phải
    //       - nếu hết cột vẫn cùng dấu thì validateSecondCheck == true thì bắt đầu tìm bên trái
    //  + Nếu không cùng dấu thì không cần dịch sang bên phải để tìm nữa tìm dịch chuyển sang hướng khác

    // I Kiểm tra theo hàng ngang (HoriZontal)
    // 1. Dịch từ ô hiện tại sang bên phải ( --->)
    int newColumn = columnNumber; // 2
    while(++newColumn < m_gameSize)
    {
        int newPosition = rowNumber * m_gameSize + newColumn; // 1*4 + 3 = 7
        QPushButton *button = board.at(newPosition);
        // Nếu button tại vị trí mới  không cùng dấu
        if(button->text() != symbol)
        {
            //validateSecondCheck = false;
            break;
        }
        else
        {
            count++;
        }
    }
    // 2. Dịch từ ô hiện tại sang bên trái ( <---)
    newColumn = columnNumber; // 2
    while(validateSecondCheck && --newColumn >= 0)
    {
        int newPosition = rowNumber * m_gameSize + newColumn;  // 1*4 + 1 = 5
        QPushButton* button = board.at(newPosition);
        if(button->text() != symbol)
        {
            break;
        }
        else
        {
            count++;
        }
    }

    if(++count >= m_pointWin) // m_gameSize
    {
        if(symbol == MetaData::player1Symbol)
            return Winner::WinnerPlayer1;
        else if(symbol == MetaData::player2Symbol)
            return Winner::WinnerPlayer2;
    }

    // II Kiểm tra theo hàng dọc (Vertical Check)
    count = 0;
    int newRow = rowNumber; // 1
    // 1. Hướng lên
    while(--newRow >= 0)
    {
        int newPositionIndex = newRow * m_gameSize + columnNumber; // 0 + 2 = 2
        QPushButton* button = board.at(newPositionIndex);
        if(button->text() != symbol)
        {
            break;
        }
        else
        {
            count++;
        }
    }
    // 2. Hướng xuống
    newRow = rowNumber;
    while( ++newRow < m_gameSize)
    {
        int newpPositionIndex = newRow * m_gameSize + columnNumber; // 2 * 4 + 2 = 10
        QPushButton* button = board.at(newpPositionIndex);
        if(button->text() != symbol)
        {
            break;
        }
        else
        {
            count++;
        }
    }

    if(++count >= m_pointWin) // m_gameSize
    {
        if(symbol == MetaData::player1Symbol)
            return Winner::WinnerPlayer1;
        else if(symbol == MetaData::player2Symbol)
            return Winner::WinnerPlayer2;
    }

    // III. Kiểm tra hướng chéo
    // 0   1   2   3
    // 4   5   6   7
    // 7   9   10  11
    // 12  13  14  15
    // 1 Hướng lên bên trái
    count = 0;
    newRow = rowNumber;
    newColumn = columnNumber;
    while(--newRow >= 0 && --newColumn >= 0)
    {
        int newPositionIndex = newRow * m_gameSize + newColumn;
        QPushButton* button = board.at(newPositionIndex);
        if(button->text() != symbol)
        {
            break;
        }
        else{
            ++count;
        }
    }
    // 2 Hướng xuống bên phải
    newRow = rowNumber;
    newColumn = columnNumber;
    while( ++newRow < m_gameSize && ++newColumn < m_gameSize)
    {
        int newPositionIndex = newRow * m_gameSize + newColumn;
        QPushButton* button = board.at(newPositionIndex);
        if(button->text() != symbol)
        {
            break;
        }
        else{
            ++count;
        }
    }

    if(++count >= m_pointWin) // m_gameSize
    {
        if(symbol == MetaData::player1Symbol)
            return Winner::WinnerPlayer1;
        else if(symbol == MetaData::player2Symbol)
            return Winner::WinnerPlayer2;
    }

    // 3. Hướng lên bên phải
    // 0   1   2   3
    // 4   5   6   7
    // 7   9   10  11
    // 12  13  14  15
    count = 0;
    newRow = rowNumber;
    newColumn = columnNumber;
    while(--newRow >= 0 && ++newColumn < m_gameSize)
    {
        int newPositionIndex = newRow * m_gameSize + newColumn;
        QPushButton* button = board.at(newPositionIndex);
        if(button->text() != symbol)
        {
            break;
        }
        else
        {
            count++;
        }
    }

    // 4. Hướng dưới bên trái
    newRow = rowNumber;
    newColumn = columnNumber;
    while(++newRow < m_gameSize && --newColumn >= 0)
    {
        int newPositionIndex = newRow * m_gameSize + newColumn;
        QPushButton* button = board.at(newPositionIndex);
        if(button->text() != symbol)
        {
            break;
        }
        else
        {
            count++;
        }
    }

    if(++count >= m_pointWin) // m_gameSize
    {
        if(symbol == MetaData::player1Symbol)
            return Winner::WinnerPlayer1;
        else if(symbol == MetaData::player2Symbol)
            return Winner::WinnerPlayer2;
    }

    // check hàm ma trận đã đầy hay chưa ( nếu đã đầy là không tìm được winner --> Draw)
    for(int i=0;i<m_gameSize * m_gameSize;i++)
    {
        if(board.at(i)->text() == MetaData::spaceCharacter)
        {
            return Winner::NotWinnerYet;
        }
    }
    return Winner::Draw;
}

// Đầu tiên: Khi Hàm này được gọi nó sẽ xóa Lable và Button trước đó là ( Label và Button kêu Restart Game)
void TicTacToeWidet::StartOrRestartGame()
{
    qDebug() <<"Restart Game";
    // Reset the width of the game Board
    this->setMinimumWidth(MetaData::widthFactor * (m_gameSize + MetaData::boardSpacing));
    m_player = Player::Player1; // Thiết lập lại
    emit changePlayer();
    // Rest the container if it is the AI ( new Game)
    if(m_mode == Mode::AiMode && !player1Move.isEmpty() && !playerAiMove.isEmpty())
    {
        resetContainers();
    }
    // Xóa Layout hiện tại để thêm bảng mới
    QLayout* layout =  this->layout(); // lấy layout hiện tại của widget
    QLayoutItem* itemLayout; //
    while(layout != nullptr && (itemLayout = layout->takeAt(0))!= nullptr)
    {
        delete itemLayout->widget();
        delete itemLayout;
    }
    delete layout;
    board.clear();

    // creter a new Board
    creatBoard();
    // Được sử dụng để kích hoạt một widget.
    // Khi một widget được kích hoạt (enabled), người dùng có thể tương tác với nó
    this->setEnabled(true);
}

void TicTacToeWidet::setAiMode()
{
    qDebug() << "TicTacToeWidet::setAiMode()";
    m_mode = Mode::AiMode;
}

void TicTacToeWidet::setTwoPlayerMode()
{
    qDebug() <<"TicTacToeWidet::setTwoPlayerMode()";
    m_mode = Mode::TwoPlayerMode;
}

