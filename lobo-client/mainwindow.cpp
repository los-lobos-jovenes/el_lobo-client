#include "mainwindow.h"
#include "./ui_mainwindow.h"

#define WRITE_BUF_SIZE 128
#define READ_BUF_SIZE 4096

bool timeSort(std::vector<std::string> v1, std::vector<std::string> v2){
    return v1[0] < v2[0];
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    tcpSocket(new QTcpSocket(this))
{
    connect(tcpSocket, &QIODevice::readyRead, this, &MainWindow::readData);
    typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
    connect(tcpSocket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::errorOccurred), this, &MainWindow::displayError);
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
    char write_buf[WRITE_BUF_SIZE];
    strcpy(write_buf, message.concat().c_str());

    QString debug;
    for(unsigned int i=0; i<message.parts.size(); i++){
        debug += QString::fromStdString(message[i]);
    }
    qDebug() << "[DEBUG]: Sending message: " << debug;
    tcpSocket->write(write_buf, sizeof(write_buf));

    //immidiately display sent message
    displayed_text.append("@");
    displayed_text.append(username);
    displayed_text.append(": ");
    displayed_text.append(str);
    displayed_text.append("\n");
    ui->textBrowser->setText(QString::fromStdString(displayed_text));
    //READ
}

//connect to host
void MainWindow::on_pushButton_2_clicked(){
    QString address = ui->lineEdit->text();
    QString port = ui->lineEdit_5->text();
    if(tcpSocket->state() == QTcpSocket::ConnectedState){
        tcpSocket->disconnectFromHost();
    }
    tcpSocket->setProxy(QNetworkProxy::NoProxy); //podobno przyspiesza
    tcpSocket->connectToHost(address, port.toInt(), QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);

    /*
    QElapsedTimer connection_timer;
    connection_timer.start();
    while(tcpSocket->state() == QTcpSocket::ConnectingState && connection_timer.elapsed() < 2000){
        qDebug() << connection_timer.elapsed();
    }
    */
    tcpSocket->waitForConnected(3000);
    qDebug() << "[DEBUG]: Connection state:" << tcpSocket->state();
    if(tcpSocket->state() == QTcpSocket::ConnectedState){
        displayed_text.append("@SUCCESS: CONNECTED\n");
        qDebug() << "[DEBUG]: Connected to host; IP:" << address << " ;port:" << port;
    } else{
        displayed_text.append("@ERROR: NOT CONNECTED\n");
        qDebug() << "[DEBUG]: NOT connected to host; IP:" << address << " ;port:" << port;
    }
    ui->textBrowser->setText(QString::fromStdString(displayed_text));
}

//change or create user
void MainWindow::on_pushButton_3_clicked(){
    message_list.clear();
    displayed_text.clear();
    ui->textBrowser->clear();
    username = ui->lineEdit_3->text().toStdString();
    password = ui->lineEdit_6->text().toStdString();

    if(ui->checkBox->checkState()){
        //send CREA to server
        msg user_creation;
        user_creation.form("1", "CREA", "2", username, password);
        char write_buf[WRITE_BUF_SIZE];
        strcpy(write_buf, user_creation.concat().c_str());

        QString debug;
        for(unsigned int i=0; i<user_creation.parts.size(); i++){
            debug += QString::fromStdString(user_creation[i]);
        }
        qDebug() << "[DEBUG]: Sending creation request: " << debug;
        tcpSocket->write(write_buf, sizeof(write_buf));

        //READ
    }
    else{
        //displayed_text.append("@SUCCESS\n");
        //ui->textBrowser->setText(QString::fromStdString(displayed_text));
        qDebug() << "[DEBUG]: Changed user to: " << QString::fromStdString(username);
    }
}

//change conversation
void MainWindow::on_pushButton_4_clicked(){
    message_list.clear();
    displayed_text.clear();
    ui->textBrowser->clear();
    target_user = ui->lineEdit_4->text().toStdString();
    qDebug() << "[DEBUG]: Conversation changed to: " << QString::fromStdString(target_user);
}

//disconnect from host
void MainWindow::on_pushButton_5_clicked(){
    message_list.clear();
    displayed_text.clear();
    ui->textBrowser->clear();
    ui->lineEdit->clear();
    ui->lineEdit_5->clear();
    tcpSocket->disconnectFromHost();

    tcpSocket->waitForDisconnected(3000);
    qDebug() << "[DEBUG]: Connection state:" << tcpSocket->state();
    if(tcpSocket->state() == QTcpSocket::UnconnectedState){
        displayed_text.append("@SUCCESS: DISCONNECTED\n");
        qDebug() << "[DEBUG]: Disconnected from host.";
    } else{
        displayed_text.append("@ERROR: NOT DISCONNECTED\n");
        qDebug() << "[DEBUG]: NOT disconnected from host properly.";
    }
    ui->textBrowser->setText(QString::fromStdString(displayed_text));
}

//pull all messages
void MainWindow::on_pushButton_6_clicked(){
    message_list.clear();
    displayed_text.clear();
    ui->textBrowser->clear();

    if(username.empty() || password.empty() || target_user.empty()){
        qDebug() << "[DEBUG]: Error - all-pull without specified username, password or target.";
        displayed_text.append("@ERROR: USERNAME, PASSWORD OR TARGET MISSING\n");
        ui->textBrowser->setText(QString::fromStdString(displayed_text));
    } else{
        //send APLL to server
        msg puller;
        puller.form("2", "APLL", "3", username, password, target_user);
        char write_buf[WRITE_BUF_SIZE];
        strcpy(write_buf, puller.concat().c_str());
        QString debug;
        for(unsigned int i=0; i<puller.parts.size(); i++){
            debug += QString::fromStdString(puller[i]);
        }
        qDebug() << "[DEBUG]: Sending all-pull request: " << debug;
        tcpSocket->write(write_buf, sizeof(write_buf));
        //READ
    }
}

//read data on readyRead()
void MainWindow::readData(){
    msg received;
    char buf[READ_BUF_SIZE];
    int n = tcpSocket->readLine(buf, READ_BUF_SIZE);
    buf[n] = 0;
    std::string tmp_str(buf);
    received.decode(tmp_str);
    QString debug;
    for(unsigned int i=0; i<received.parts.size(); i++){
        debug += QString::fromStdString(received[i]);
    }
    qDebug() << "[DEBUG]: Received: " << debug;
    std::vector<std::string> tmp;

    for(unsigned int i=0; i<received.parts.size(); i++){
        if(received.extract(i).compare("RETN") == 0){
            /* succes action
            if(received.extract(i+1).compare("1") == 0 && received.extract(i+2).compare("SUCCESS") == 0){
                displayed_text.append("@SUCCESS\n");
                ui->textBrowser->setText(QString::fromStdString(displayed_text));
                qDebug() << "[DEBUG]: Success displayed";
                i += 3;
            } else
            */
            if(received.extract(i+1).compare("2") == 0){
                displayed_text.append("@");
                displayed_text.append(received.extract(i+2));
                displayed_text.append(": ");
                displayed_text.append(received.extract(i+3));
                displayed_text.append("\n");
                ui->textBrowser->setText(QString::fromStdString(displayed_text));
                qDebug() << "[DEBUG]: Error displayed";
                i += 3;
            }
            else if(received.extract(i+1).compare("3") == 0){
                tmp.clear();
                tmp.push_back(received.extract(i+2));
                tmp.push_back(received.extract(i+3));
                tmp.push_back(received.extract(i+4));
                message_list.push_back(tmp);
                i += 4;
            }
        } else if(received.extract(i).compare("ENDT") == 0){
            displayed_text.clear();
            ui->textBrowser->clear();
            std::sort(message_list.begin(), message_list.end(), timeSort);
            std::unique(message_list.begin(), message_list.end());
            for(auto m : message_list){
                displayed_text.append("@");
                displayed_text.append(m[1]);
                displayed_text.append(": ");
                displayed_text.append(m[2]);
                displayed_text.append("\n");
            }
            ui->textBrowser->setText(QString::fromStdString(displayed_text));
            qDebug() << "[DEBUG]: Messages displayed";
        }
    }
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError){

    qDebug() << "[DEBUG]:" << socketError;
}
