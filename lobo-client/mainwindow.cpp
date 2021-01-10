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
    connect(tcpSocket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error), this, &MainWindow::displayError);

    //PULL every 2 seconds
    QTimer *timer1 = new QTimer(this);
    connect(timer1, &QTimer::timeout, this, &MainWindow::pullUnread);
    timer1->start(2000);

    //PEND every 3 seconds
    QTimer *timer2 = new QTimer(this);
    connect(timer2, &QTimer::timeout, this, &MainWindow::pendUnread);
    timer2->start(3000);

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//send message
void MainWindow::on_pushButton_clicked(){

    if(!username.empty() && !password.empty() && !target_user.empty() && tcpSocket->waitForConnected(500)){
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
    tcpSocket->write(write_buf, message.concat().size() * sizeof(char));

        //immidiately display sent message
        std::vector<std::string> tmp;
        std::string time;
        if(message_list.size() > 0){
            time = std::to_string(std::stoi(message_list.back()[0])+1);
        } else{
            time = "1";
        }
        tmp.push_back(time);
        tmp.push_back(username);
        tmp.push_back(str);
        message_list.push_back(tmp);

        displayed_text.append("@");
        displayed_text.append(username);
        displayed_text.append(": ");
        displayed_text.append(str);
        displayed_text.append("\n");
        ui->textBrowser->setText(QString::fromStdString(displayed_text));

        tmp.clear();
        //READ
    } else{
        ui->textBrowser_2->append("@ERROR: CONNECTION OR USERS OR PASSWORD MISSING");
    }
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

    tcpSocket->waitForConnected(3000);
    qDebug() << "[DEBUG]: Connection state:" << tcpSocket->state();
    if(tcpSocket->state() == QTcpSocket::ConnectedState){
        ui->textBrowser_2->append("@SUCCESS: CONNECTED");
        qDebug() << "[DEBUG]: Connected to host; IP:" << address << " ;port:" << port;
    } else{
        ui->textBrowser_2->append("@ERROR: NOT CONNECTED");
        qDebug() << "[DEBUG]: NOT connected to host; IP:" << address << " ;port:" << port;
    }
}

//change or create user
void MainWindow::on_pushButton_3_clicked(){
    message_list.clear();
    displayed_text.clear();
    ui->textBrowser->clear();
    username = ui->lineEdit_3->text().toStdString();
    password = ui->lineEdit_6->text().toStdString();

    if(!username.empty() && !password.empty() && tcpSocket->waitForConnected(500)){
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
        tcpSocket->write(write_buf, user_creation.concat().size() * sizeof(char));
        //READ
        } else{
             ui->textBrowser_2->append("@SUCCESS");
        }          
    } else{
        ui->textBrowser_2->append("@ERROR: CONNECTION OR USER OR PASSWORD MISSING");
        qDebug() << "[DEBUG]: Cannot change or create user";
    }
}

//change conversation
void MainWindow::on_pushButton_4_clicked(){
    message_list.clear();
    displayed_text.clear();
    ui->textBrowser->clear();
    target_user = ui->lineEdit_4->text().toStdString();
    if(!username.empty() && !password.empty() && tcpSocket->waitForConnected(500) && target_user.compare(username) != 0){
        ui->textBrowser_2->append("@SUCCESS: CHAT WITH: " + QString::fromStdString(target_user));
        qDebug() << "[DEBUG]: Conversation changed to: " << QString::fromStdString(target_user);
        MainWindow::on_pushButton_6_clicked(); //APLL
        //MainWindow::pullUnread(); //PULL
    } else{
        ui->textBrowser_2->append("@ERROR: CONNECTION OR USERS WRONG");
        qDebug() << "[DEBUG]: Cannot change conversation";
    }
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
        ui->textBrowser_2->append("@SUCCESS: DISCONNECTED");
        qDebug() << "[DEBUG]: Disconnected from host.";
    } else{
        ui->textBrowser_2->append("@ERROR: NOT DISCONNECTED");
        qDebug() << "[DEBUG]: NOT disconnected from host properly.";
    }
}

//pull all messages
void MainWindow::on_pushButton_6_clicked(){
    message_list.clear();
    displayed_text.clear();
    ui->textBrowser->clear();

    if(!username.empty() && !password.empty() && !target_user.empty() && tcpSocket->waitForConnected(500)){
        //send APLL to server
        msg puller;
        //puller.form("2", "APLL", "3", username, password, target_user);
        puller.form("2", "APLL", "4", username, password, target_user, "read");
        char write_buf[WRITE_BUF_SIZE];
        strcpy(write_buf, puller.concat().c_str());
        QString debug;
        for(unsigned int i=0; i<puller.parts.size(); i++){
            debug += QString::fromStdString(puller[i]);
        }
        qDebug() << "[DEBUG]: Sending all-pull request: " << debug;
        tcpSocket->write(write_buf, puller.concat().size() * sizeof(char));
        //READ
    } else{
        qDebug() << "[DEBUG]: Error - all-pull without specified username, password or target.";
        ui->textBrowser_2->append("@ERROR: CONNECTION OR USERS OR PASSWORD MISSING");
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
    tmp.clear();

    for(unsigned int i=0; i<received.parts.size(); i++){
        if(received.extract(i).compare("RETN") == 0){
            if(received.extract(i+1).compare("1") == 0){
                //regular success message
                if(received.extract(i+2).compare("SUCCESS") == 0){
                    ui->textBrowser_2->append("@SUCCESS");
                    qDebug() << "[DEBUG]: Success displayed";
                }
                //PEND answer
                else{
                    ui->textBrowser_2->append("@PENDING:" + QString::fromStdString(received.extract(i+2)));
                    qDebug() << "[DEBUG]: Pending displayed";
                }

                i += 2;
            } else if(received.extract(i+1).compare("2") == 0){
                ui->textBrowser_2->append("@" + QString::fromStdString(received.extract(i+2)) + ": " + QString::fromStdString(received.extract(i+3)));
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
        } else if(received.extract(i).compare("ENDT") == 0 && !tmp.empty()){
            QScrollBar *scrollBar = ui->textBrowser->verticalScrollBar();
            int scroll_val = scrollBar->value();

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
            //scroll to the previous place after displaying new messages
            scrollBar->setValue(scroll_val);
            qDebug() << "[DEBUG]: Messages displayed";
            i += 1;
        }
    }
    tmp.clear();
}

void MainWindow::pullUnread(){
    if(ui->checkBox_2->checkState()){
        if(!username.empty() && !password.empty() && !target_user.empty() && tcpSocket->waitForConnected(500)){

        //message_list.clear();
        //displayed_text.clear();
        //ui->textBrowser->clear();

        //send PULL to server
        msg puller;
        puller.form("2", "PULL", "3", username, password, target_user);
        char write_buf[WRITE_BUF_SIZE];
        strcpy(write_buf, puller.concat().c_str());
        QString debug;
        for(unsigned int i=0; i<puller.parts.size(); i++){
            debug += QString::fromStdString(puller[i]);
        }
        qDebug() << "[DEBUG]: Sending unread-pull request: " << debug;
        tcpSocket->write(write_buf, puller.concat().size() * sizeof(char));
        //READ

        } else{
        qDebug() << "[DEBUG]: Unread-pull ERROR.";
        }
    }
}

void MainWindow::pendUnread(){
    if(ui->checkBox_3->checkState()){
        if(!username.empty() && !password.empty() && tcpSocket->waitForConnected(500)){

        //send PEND to server
        msg pender;
        pender.form("1", "PEND", "2", username, password);
        char write_buf[WRITE_BUF_SIZE];
        strcpy(write_buf, pender.concat().c_str());
        QString debug;
        for(unsigned int i=0; i<pender.parts.size(); i++){
            debug += QString::fromStdString(pender[i]);
        }
        qDebug() << "[DEBUG]: Sending pend request: " << debug;
        tcpSocket->write(write_buf, pender.concat().size() * sizeof(char));
        //READ

        } else{
        qDebug() << "[DEBUG]: Pend ERROR.";
        }
    }
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError){

    qDebug() << "[DEBUG]:" << socketError;
}
