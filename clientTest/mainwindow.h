#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextBrowser>
#include <QDateTime>
#include <QFile>
#include <QValidator>
#include <QRegularExpression>
#include <QString>
#include <QDebug>
#include <QGridLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void handleDisconnected();
private slots:
    void on_connectServer_pressed();
    void on_sendMessButton_pressed();


private:
    QTcpSocket   *socket;
    QValidator   *ipValidator;
    QValidator   *portValidator;
    QLabel       *ipLabel;
    QLineEdit    *inIp;
    QPushButton  *disAndConnectServer;
    QLineEdit    *inputPort;
    QLabel       *outMessLabel;
    QTextBrowser *outputMess;
    QLabel       *inMessLabel;
    QLineEdit    *inputMess;
    QPushButton  *sendMessButton;
    QString td;
    QDateTime t;
    QHostAddress address;

    QByteArray Data;
    void SendToServer(QString str);
    bool isConnected = false;

public slots:
    void slotReadyRead();
    void onSocketConnected();
    void connectServer();
    void disconnectServer();

};
#endif // MAINWINDOW_H
