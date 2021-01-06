#include "mainwindow.h"
#include "./ui_mainwindow.h"

#define BUF_SIZE 128

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

    //tcpSocket->connectToHost("127.0.0.1", 1234);
    str += ui->lineEdit_2->text();
    str += '\n';
    ui->textBrowser->setText(str);
    ui->lineEdit_2->clear();
    qDebug() << "[DEBUG]: Message sent: " << str;
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
    username = ui->lineEdit_3->text();
    password = ui->lineEdit_6->text();

    //uwaga na \031
    if(ui->checkBox->checkState()){
        //send CREA to server
        char write_buf[BUF_SIZE];
        QString user_creation = "\0311\031CREA\0312\031";
        user_creation += username;
        user_creation += "\031";
        user_creation += password;
        user_creation += "\031";
        strcpy(write_buf, user_creation.toStdString().c_str());
        qDebug() << "[DEBUG]: Sending: " << user_creation;
        tcpSocket->write(write_buf, sizeof(write_buf));

        //receive RETN from server
        char read_buf[BUF_SIZE];
        int n = tcpSocket->readLine(read_buf, BUF_SIZE);
        read_buf[n] = 0;
        qDebug() << "[DEBUG]: Received: " << read_buf;

        //zamienić na rozkładanie korzystając z kodu z serwera
        QString response = QString::fromUtf8(read_buf);
        if(response.mid(10,7).compare("SUCCESS")){
            qDebug() << "[DEBUG]: Created user: " << username;
        }
        else{
            qDebug() << "[DEBUG]: Error while creating user: " << username;
        }
    }
    else{
        qDebug() << "[DEBUG]: Changed user to: " << username;
    }
}

//disconnect from host
void MainWindow::on_pushButton_5_clicked(){
    ui->lineEdit->clear();
    ui->lineEdit_5->clear();
    tcpSocket->disconnectFromHost();
    qDebug() << "[DEBUG]: Disconnected from host.";
}

void MainWindow::readData(){
    //odbieranie danych
    char buf[BUF_SIZE];
    int n = tcpSocket->readLine(buf, BUF_SIZE);
    buf[n] = 0;
    qDebug() << buf;
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError){

    qDebug() << socketError;
}
