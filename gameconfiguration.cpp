#include "gameconfiguration.h"
#include "ui_gameconfiguration.h"
#include <QDebug>

GameConfiguration::GameConfiguration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameConfiguration)
{
    ui->setupUi(this);
    updateOkButton();
    //
    ui->spinBox->setRange(SizeRange::minRange , SizeRange::maxRange);
    ui->horizontalSlider->setRange(SizeRange::minRange , SizeRange::maxRange);
    ui->spinBox_PointWin->setRange(SizeRange::minRange , SizeRange::maxRange);
    // set game mode
    ui->radioButton_PlayerMode->setChecked(true);
}

GameConfiguration::~GameConfiguration()
{
    delete ui;
}

void GameConfiguration::setPlayer1Name(const QString &name) const
{
    ui->lineEdit_Player1->setText(name);
}

void GameConfiguration::setPlayer2Name(const QString &name) const
{
    ui->lineEdit_Player2->setText(name);

}

QString GameConfiguration::getPlayer1Name() const
{
    return ui->lineEdit_Player1->text();
}

QString GameConfiguration::getPlayer2Name() const
{
    return ui->lineEdit_Player2->text();

}

void GameConfiguration::setGameSize(int size) const
{
    if(size < SizeRange::minRange || size > SizeRange::maxRange)
    {
        return;
    }
    ui->spinBox->setValue(size);
    ui->horizontalSlider->setValue(size);

}

int GameConfiguration::getGameSize() const
{
    return ui->spinBox->value();
}

void GameConfiguration::setPointWin(int number)
{
    if(number < SizeRange::minRange || number > SizeRange::maxRange )
    {
        return;
    }
    ui->spinBox_PointWin->setValue(number);
}

int GameConfiguration::getPointWin()
{
    return ui->spinBox_PointWin->value();
}

// Hàm đặt lại một số cấu hình
void GameConfiguration::resetSomeConfig()
{
    ui->radioButton_PlayerMode->setChecked(true);
    ui->lineEdit_Player2->setEnabled(true);
    ui->lineEdit_Player2->setText("");
}


// singleton
GameConfiguration* GameConfiguration::gameConfiguration = nullptr;

GameConfiguration* GameConfiguration::getInstance()
{
    if(gameConfiguration == nullptr)
    {
        gameConfiguration = new GameConfiguration;
    }
    return gameConfiguration;
}

// Khi lineEdit(Player1 - Player2)  phát tín hiệu textChange nó sẽ gọi hàm này
void GameConfiguration::updateOkButton()
{
    bool player1NameChanged = ui->lineEdit_Player1->text().isEmpty();
    bool player2NameChanged = ui->lineEdit_Player2->text().isEmpty();
    // ButtonBox(Ok -Cannel) ở đây đang trỏ đến nút Ok
    QPushButton* okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    // Nếu cả 2 tên đều trống thì không nhấn Ok được
    okButton->setDisabled(player1NameChanged || player2NameChanged);


}

void GameConfiguration::on_buttonBox_clicked(QAbstractButton *button)
{
    // Nếu nút Ok được nhấn
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        if(ui->radioButton_PlayerMode->isChecked())
        {
            qDebug() << "Mode had Choosen: Player Mode";
        }
        else if(ui->radioButton_AI_Mode->isChecked())
        {
            qDebug() << "Mode had Choosen: AI Mode" << ui->lineEdit_Player2->text();

        }
    }
}


void GameConfiguration::on_radioButton_AI_Mode_clicked()
{
    qDebug() << "GameConfiguration::on_radioButton_AI_Mode_clicked()";
    ui->lineEdit_Player2->setText(DataSpeical::aiName);
    ui->lineEdit_Player2->setDisabled(true);

}

void GameConfiguration::on_radioButton_PlayerMode_clicked()
{
    qDebug()<< "GameConfiguration::on_radioButton_PlayerMode_clicked()";
    if(ui->lineEdit_Player2->text() == DataSpeical::aiName)
        ui->lineEdit_Player2->setText("");
    ui->lineEdit_Player2->setEnabled(true);
}

