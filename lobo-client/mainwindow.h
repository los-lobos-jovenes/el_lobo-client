#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>
#include <QtNetwork>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString str = "";

private slots:
     void on_pushButton_clicked();
     void readData();
     void displayError(QAbstractSocket::SocketError socketError);

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
};
#endif // MAINWINDOW_H
