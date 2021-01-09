#include "mainwindow.h"
#include "./ui_mainwindow.h"

#define BUF_SIZE 256

/*
//concatanated string of msg into a vector of msg
std::vector<msg> breakdown(std::string str){
    std::vector<msg> res;
    unsigned int start = 0;
    unsigned int stop = 0;
    for(unsigned int i=0; i<str.size(); i++){
        if(str[i] == MSG_SEPARATOR && str[i-1] == MSG_SEPARATOR){
            stop = i-1;
            msg tmp;
            tmp.decode(str.substr(start, stop-start));
            res.push_back(tmp);
            start = i;
        }
    }
    stop = str.size()-1;
    msg tmp;
    tmp.decode(str.substr(start, stop-start));
    res.push_back(tmp);

    return res;
}
*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    tcpSocket(new QTcpSocket(this))
{
    connect(tcpSocket, &QIODevice::readyRead, this, &MainWindow::readData);
    //typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
    //connect(tcpSocket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error), this, &MainWindow::displayError);
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//send message
void MainWindow::on_pushButton_clicked(){

    std::string str = ui->lineEdit_2->text().toStdString();
    ui->lineEdit_2->clear();

    //send SEND to server
    msg message;
    message.form("1", "SEND", "4", username, password, target_user, str);
    char write_buf[BUF_SIZE];
    strcpy(write_buf, message.concat().c_str());

    QString debug;
    for(int i=0; i<10; i++){
        debug += QString::fromStdString(message[i]);
    }
    qDebug() << "[DEBUG]: Sending message: " << debug;
    tcpSocket->write(write_buf, sizeof(write_buf));

    //READ
    /*
    if(received.size() == 1 && received[0][3].compare("SUCCESS")){
        qDebug() << "[DEBUG]: Successfully sent message: " << QString::fromStdString(str);
    }
    else{
        qDebug() << "[DEBUG]: Error while sending message: " << QString::fromStdString(str);
    }
    */
}

//connect to host
void MainWindow::on_pushButton_2_clicked(){
    QString address = ui->lineEdit->text();
    QString port = ui->lineEdit_5->text();
    tcpSocket->connectToHost(address, port.toInt());
    qDebug() << "[DEBUG]: Connected to host; IP: " << address << " ; port: " << port;
}

//change or create user
void MainWindow::on_pushButton_3_clicked(){
    username = ui->lineEdit_3->text().toStdString();
    password = ui->lineEdit_6->text().toStdString();

    if(ui->checkBox->checkState()){
        //send CREA to server
        msg user_creation;
        user_creation.form("1", "CREA", "2", username, password);
        char write_buf[BUF_SIZE];
        strcpy(write_buf, user_creation.concat().c_str());

        QString debug;
        for(int i=0; i<10; i++){
            debug += QString::fromStdString(user_creation[i]);
        }
        qDebug() << "[DEBUG]: Sending creation request: " << debug;
        tcpSocket->write(write_buf, sizeof(write_buf));

        //READ
        /*
        if(received.size() == 1 && received[0][3].compare("SUCCESS")){
            qDebug() << "[DEBUG]: Successfully created user: " << QString::fromStdString(username);
        }
        else{
            qDebug() << "[DEBUG]: Error while creating user: " << QString::fromStdString(username);
        }
        */
    }
    else{
        qDebug() << "[DEBUG]: Changed user to: " << QString::fromStdString(username);
    }
}

//change conversation (and PULL ALL MESSAGES)
void MainWindow::on_pushButton_4_clicked(){
    displayed_text.clear();
    ui->textBrowser->clear();
    target_user = ui->lineEdit_4->text().toStdString();
    qDebug() << "[DEBUG]: Conversation changed to: " << QString::fromStdString(target_user);

    if(ui->checkBox_2->checkState()){
        //send APLL to server
        msg puller;
        puller.form("2", "APLL", "3", username, password, target_user);
        char write_buf[BUF_SIZE];
        strcpy(write_buf, puller.concat().c_str());
        QString debug;
        for(int i=0; i<10; i++){
            debug += QString::fromStdString(puller[i]);
        }
        qDebug() << "[DEBUG]: Sending all-pull request: " << debug;
        tcpSocket->write(write_buf, sizeof(write_buf));

        //READ
        /*
        //simple displaying messages WITHOUT SORTING FOR NOW
        for(unsigned int i=0; i < received.size()-1; i++){
            displayed_text.append("@");
            displayed_text.append(received[i][3]);
            displayed_text.append(": ");
            displayed_text.append(received[i][4]);
            displayed_text.append("\n");
        }
        ui->textBrowser->setText(QString::fromStdString(displayed_text));
        qDebug() << "[DEBUG]: Messages displayed";
        */
    }
}

//disconnect from host
void MainWindow::on_pushButton_5_clicked(){
    ui->lineEdit->clear();
    ui->lineEdit_5->clear();
    tcpSocket->disconnectFromHost();
    qDebug() << "[DEBUG]: Disconnected from host.";
}

//odbieranie danych
void MainWindow::readData(){
    msg received;
    char buf[BUF_SIZE];
    int n = tcpSocket->readLine(buf, BUF_SIZE);
    buf[n] = 0;
    std::string tmp_str(buf);
    received.decode(tmp_str);
    QString debug;
    for(int i=0; i<10; i++){
        debug += QString::fromStdString(received[i]);
    }
    qDebug() << "[DEBUG]: Received: " << debug;
    //simple displaying messages WITHOUT SORTING FOR NOW
    if(std::stoi(received.extract(3)) == 2){
        displayed_text.append("@");
        displayed_text.append(received[4]);
        displayed_text.append(": ");
        displayed_text.append(received[5]);
        displayed_text.append("\n");
        ui->textBrowser->setText(QString::fromStdString(displayed_text));
        qDebug() << "[DEBUG]: Error displayed";
    }
    else if(std::stoi(received.extract(3)) == 3){
        //received[4] == TIMESTAMP
        displayed_text.append("@");
        displayed_text.append(received[5]);
        displayed_text.append(": ");
        displayed_text.append(received[6]);
        displayed_text.append("\n");
        ui->textBrowser->setText(QString::fromStdString(displayed_text));
        qDebug() << "[DEBUG]: Message displayed";
    }

    /*
    do{
        char buf[BUF_SIZE];
        int n = tcpSocket->readLine(buf, BUF_SIZE);
        buf[n] = 0;
        std::string tmp_str(buf);
        msg tmp_msg;
        tmp_msg.decode(tmp_str);
        received.push_back(tmp_msg);
        qDebug() << "[DEBUG]: Received: " << QString::fromStdString(tmp_msg.concat());
    }while(received.back()[3] != "ENDT" && received.back()[3] != "SUCCESS" && received.back()[3] != "ERROR");
    */
    //zawiesza się gdy brak wiadomości?
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError){

    qDebug() << socketError;
}
