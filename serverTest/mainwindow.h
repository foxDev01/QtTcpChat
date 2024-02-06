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
#include <QVector>
#include <QListWidgetItem>
#include <QSizePolicy>
#include <QValidator>
#include <QRegularExpression>
#include <QWidget>
#include <QFile>
#include <QMap>
#include <QGridLayout>


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
     MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
     QTcpServer *tcpServer;
     QTcpSocket *socket = nullptr; // nullptr для проверки на то, что создан сокет
     bool isSocketDeleted = false;


private:
     QList<QTcpSocket*> disconnectedSockets;

    QMap<int,QTcpSocket *> ClientsSockets;
    bool connectClient = false;
    void SendToClient(QString str);
    QByteArray Data;
    QValidator *portValidator;
    QPushButton *startOffServer;
    QLabel *portLabel;
    QLineEdit *inputPort;
    QTextBrowser *outputMess;
    QLabel *outMessLabel;
    QLabel *inMessLabel;
    QLineEdit *inputMess;
    QPushButton *sendMessButton;
    QString td;
    QDateTime t;
    QLabel *countClients;

    QVBoxLayout *layout;
    QWidget *widget;
    int server_status =0;

    QHostAddress serverAddress;
    QString serverAddressString;

//    QVector<QTcpSocket*> Sockets;
//    QMap<int, QTcpSocket*> Sockets;

private slots:
    void on_startServer_pressed();
    void on_sendMessButton_pressed();
    void on_textBrowser_textChanged();


public slots:
    void incomingConnecting();
    void slotReadyRead();
    void startServer();
    void stopServer();
    void createLogFile();
    void removeClientSocket(QTcpSocket *socket);
};
#endif // MAINWINDOW_H
