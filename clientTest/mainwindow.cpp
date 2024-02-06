#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    QWidget *centralWidget = new QWidget(this);

    QGridLayout *layout = new QGridLayout(centralWidget);

    QRegularExpression ipRegex("^([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
                             "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
                             "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
                             "([01]?\\d\\d?|2[0-4]\\d|25[0-5])$");
    ipValidator = new QRegularExpressionValidator(ipRegex, this);


    QRegularExpression portRegex("^[1-9][0-9]{0,4}$");
    portValidator = new QRegularExpressionValidator(portRegex, this);

    disAndConnectServer = new QPushButton("Подключиться",this);
    disAndConnectServer->setStyleSheet("background-color: green;");

    QMainWindow::connect(disAndConnectServer, &QPushButton::pressed, this, &MainWindow::on_connectServer_pressed );

    ipLabel = new QLabel("Укажите ip:port", this);

//    ipLabel->setGeometry(0,0,200,30);
    inIp = new QLineEdit(this);

//    inIp->setFixedSize(100, 25);
    inIp->setText("127.0.0.1");
    inIp->setValidator(ipValidator);

    inputPort = new QLineEdit(this);
    inputPort->setValidator(portValidator);
    inputPort->setFixedSize(40, 25);

    inputPort->setText("52725");


    outMessLabel = new QLabel("Сообщения:", this);
//    outMessLabel->setGeometry(0,50,100,30);
    outputMess = new QTextBrowser(this);
    outputMess->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    outputMess->setGeometry(0,75,400,100);

    inMessLabel = new QLabel("Введите сообщение:", this);
//    inMessLabel->setGeometry(0,170,400,35);
    inputMess = new QLineEdit(this);
    inputMess->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//    inputMess->setGeometry(0,200,400,35);

    sendMessButton = new QPushButton("Отправить сообщение", this);
//    sendMessButton->setGeometry(50,250,200,30);

    sendMessButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QMainWindow::connect(sendMessButton, &QPushButton::pressed, this, &MainWindow::on_sendMessButton_pressed );

    inIp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    inputPort->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


    layout->addWidget(ipLabel, 0, 0);
    layout->addWidget(inIp, 0, 1);
    layout->addWidget(inputPort, 0, 2);

    QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addItem(spacer, 0, 3);

    layout->addWidget(disAndConnectServer, 0, 4);
    layout->addWidget(outMessLabel, 1, 0, 1, 5);
    layout->addWidget(outputMess, 2, 0, 1, 5);
    layout->addWidget(inMessLabel, 3, 0, 1, 5);
    layout->addWidget(inputMess, 4, 0, 1, 4);
    layout->addWidget(sendMessButton, 4, 4);

    setCentralWidget(centralWidget);

    if (isConnected==false){
        sendMessButton->setEnabled(false);

    }


}


MainWindow::~MainWindow()
{

}


void MainWindow::onSocketConnected()
{
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::handleDisconnected);
    QString str = "Hello Server!";
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_1);
    out<<str;
    socket->write(Data);

    outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") +"|" + QHostAddress(QHostAddress::LocalHost).toString() + ":"+ QString::number(socket->localPort()) + "|" + "Подключено к " + inIp->text() +":"+inputPort->text());
}

void MainWindow::connectServer()
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &MainWindow::onSocketConnected);
    connect(socket,&QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);

    QString serverAddress = inIp->text();
    int serverPort = inputPort->text().toInt();

    // Устанавливаем таймаут соединения
    socket->connectToHost(serverAddress, serverPort);
    if (socket->waitForConnected(1000))  // Ожидание соединения в течение 1 секунды
    {
        // Соединение успешно установлено/ надпись копки меняется
        disAndConnectServer->setText("Отключиться");
        disAndConnectServer->setStyleSheet("background-color: red;");
        isConnected = true;
        sendMessButton->setEnabled(true);

    }
    else
    {
         outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"|" + QHostAddress(QHostAddress::LocalHost).toString() + ":"+ QString::number(socket->localPort()) +"|" + "Cервер не доступен");
    }
}

void MainWindow::disconnectServer()
{
    disAndConnectServer->setStyleSheet("background-color: green;");
    socket->waitForDisconnected(100);
    disconnect(socket,&QTcpSocket::connected,0,0);
    disconnect(socket, &QTcpSocket::readyRead, 0, 0);
    disAndConnectServer->setText("Подключиться");

    switch (socket->state())
    {
    case 0:

        socket->disconnectFromHost();
        socket->abort();
        isConnected = false;
        break;
    case 2:

        socket->abort();

        isConnected = false;
        break;
    default:

        socket->abort();

        isConnected = false;
    }


}


void MainWindow:: handleDisconnected(){

    disAndConnectServer->setText("Подключиться");
    isConnected = false;
    sendMessButton->setEnabled(false);
    disAndConnectServer->setStyleSheet("background-color: green;");
    outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"|" + QHostAddress(QHostAddress::LocalHost).toString() + ":"+ QString::number(socket->localPort()) +"|" + "Оключен от сервера!!!");
}




void MainWindow::on_connectServer_pressed()
{

    if (isConnected) {
            disconnectServer();

        } else if (!isConnected){
            connectServer();

        }
}

void MainWindow::on_sendMessButton_pressed()
{
    if (isConnected == true and !inputMess->text().isEmpty()) {
        SendToServer(inputMess->text());
        inputMess->clear();
    } else if(isConnected == false){
        outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"|"+ QHostAddress(QHostAddress::LocalHost).toString() + ":" + "|"+ "Не подключен к серверу. Cooбщение не отправлено");
    } else if (inputMess->text().isEmpty()) {
          outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"|"+ QHostAddress(QHostAddress::LocalHost).toString() + ":" + QString::number(socket->localPort()) + "|"+"Перед отправкой введите сообщение" );
    }

}

void MainWindow::slotReadyRead()
{

    if (!socket){
          outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") + "|" + socket->localAddress().toString() + ":" + QString::number(socket->localPort()) + "|" + "Сервер отключился");

          return;
    }
    if (socket->state() != QAbstractSocket::ConnectedState) {
          outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") + "|" + socket->localAddress().toString() + ":" + QString::number(socket->localPort()) + "|" + "Сервер отключился");

    }
        QDataStream in(socket);
        in.setVersion(QDataStream::Qt_5_1);
        if(in.status()==QDataStream::Ok)
        {
            QString str;
            in>>str;

            outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"|" + inIp->text() +":"+inputPort->text() + "|" + str );

        }

        else if(!socket->waitForConnected())
        {
            outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"|" + QHostAddress(QHostAddress::LocalHost).toString() +":"+ QString::number(socket->localPort()) +"|"+"DataStream Error");

        }
//      socket->disconnectFromHost();


}

void MainWindow::SendToServer(QString str)
{
        if (!socket)
            return;

        if (socket->state() != QAbstractSocket::ConnectedState) {

            isConnected = false;
            disAndConnectServer->setText("Подключиться");

            return;
        }
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_1);
    out<<str;
    socket->write(Data);

    outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"|"+ socket->localAddress().toString() +":"+ QString::number(socket->localPort()) + "|"+ str );


}


