#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "msg.hpp"
#include <string>

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
    str += ui->lineEdit_2->text().toStdString();
    str += '\n';
    ui->textBrowser->setText(QString::fromStdString(str));
    ui->lineEdit_2->clear();
    qDebug() << "[DEBUG]: Message sent";
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
        qDebug() << "[DEBUG]: Sending creation request for user: " << QString::fromStdString(user_creation.concat());
        tcpSocket->write(write_buf, sizeof(write_buf));

        //MUTEX?
        if(received_msg[2].compare("RETN") && received_msg[3].compare("SUCCESS")){
            qDebug() << "[DEBUG]: Successfully created user: " << QString::fromStdString(username);
        }
        else{
            qDebug() << "[DEBUG]: Error while creating user: " << QString::fromStdString(username);
        }

    }
    else{
        qDebug() << "[DEBUG]: Changed user to: " << QString::fromStdString(username);
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
    std::string tmp(buf);
    received_msg.decode(tmp);
    qDebug() << "[DEBUG]: Received: " << QString::fromStdString(received_msg.concat());
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError){

    qDebug() << socketError;
}
