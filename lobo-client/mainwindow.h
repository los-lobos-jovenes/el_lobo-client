#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>
#include <QtNetwork>
//#include <QMutex>
//#include <QTime>
#include <string>
#include <vector>
#include <algorithm>

#include "msg.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    std::string displayed_text = "";
    std::string username = "";
    std::string password = "";
    std::string target_user = "";
    std::vector<std::vector<std::string>> message_list; //timestamp//sender//text
    //QMutex mutex;

private slots:
     void on_pushButton_clicked();
     void on_pushButton_2_clicked();
     void on_pushButton_3_clicked();
     void on_pushButton_4_clicked();
     void on_pushButton_5_clicked();
     void on_pushButton_6_clicked();
     void readData();
     void displayError(QAbstractSocket::SocketError socketError);

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
};
#endif // MAINWINDOW_H
