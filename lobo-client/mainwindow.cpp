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

void MainWindow::on_pushButton_clicked(){

    //qDebug() << ui->textBrowser->toPlainText();
    //tcpSocket->connectToHost("127.0.0.1", 1234);
    str += ui->lineEdit_2->text();
    str += '\n';
    ui->textBrowser->setText(str);
    ui->lineEdit_2->clear();
}

void MainWindow::on_pushButton_2_clicked(){


    QString address = ui->lineEdit->text();
    QString port = ui->lineEdit_5->text();
    tcpSocket->connectToHost(address, port.toInt());
}

void MainWindow::on_pushButton_5_clicked(){


    ui->lineEdit->clear();
    ui->lineEdit_5->clear();
    tcpSocket->disconnectFromHost();
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
