#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);

    QGridLayout *layout = new QGridLayout(centralWidget);

    QRegularExpression portRegex("^[1-9][0-9]{0,4}$");
    portValidator = new QRegularExpressionValidator(portRegex, this);

    startOffServer = new QPushButton("Старт", this);
    startOffServer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    startOffServer->setStyleSheet("background-color: green;");
    countClients = new QLabel(this);

    portLabel = new QLabel("Укажите порт:", this);
    portLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    inputPort = new QLineEdit(this);
    inputPort->setValidator(portValidator);
    inputPort->setText("52725");
    inputPort->setFixedSize(40, 24);
    inputPort->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    outMessLabel = new QLabel("Сообщения:", this);

    outputMess = new QTextBrowser(this);
    outputMess->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    inMessLabel = new QLabel("Введите сообщение:", this);
//    inMessLabel->setGeometry(0,170,400,35);
    inputMess = new QLineEdit(this);
    inputMess->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    sendMessButton = new QPushButton("Отправить сообщение", this);
    sendMessButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    layout->addWidget(portLabel, 0, 0);
    layout->addWidget(inputPort, 0, 1);

    QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addItem(spacer, 0, 3);

    layout->addWidget(startOffServer, 0, 3);
    layout->addWidget(outMessLabel, 1, 0, 1, 5);
    layout->addWidget(outputMess, 2, 0, 1, 5);
    layout->addWidget(inMessLabel, 3, 0, 1, 5);
    layout->addWidget(inputMess, 4, 0, 1, 4);
    layout->addWidget(sendMessButton, 4, 4);

    setCentralWidget(centralWidget);


    QMainWindow::connect(startOffServer, &QPushButton::pressed, this, &MainWindow::on_startServer_pressed );
    QMainWindow::connect(sendMessButton, &QPushButton::pressed, this, &MainWindow::on_sendMessButton_pressed );
    connect(outputMess, &QTextBrowser::textChanged, this, &MainWindow::on_textBrowser_textChanged);



    tcpServer = new QTcpServer(this);


    if (ClientsSockets.size()==0){
        sendMessButton->setEnabled(false);

    }else if (ClientsSockets.size()!=0) {
        sendMessButton->setEnabled(true);
}

}


MainWindow::~MainWindow()
{

}

void MainWindow:: createLogFile()
{
    QString filename = "server.log";

    QFile file(filename);

    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream stream(&file);
        if (file.size()==0){
        // Записываем заголовки
         QString headers = "ДАТА          ВРЕМЯ|АДРЕС          |СООБЩЕНИЕ   ";
        stream << headers << "\n";
        }
        file.close();
    }
}

// для записи логов
void MainWindow::on_textBrowser_textChanged()
{
    QString message = outputMess->toPlainText();
    QStringList messageList = message.split("\n");
    messageList.removeAll("");
    QString lastMessage = messageList.last();

    QFile file("server.log");


    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream stream(&file);

        stream  << lastMessage << '\n';

        file.close();
    }
}

// для обработки нажатия кнопки старт
void MainWindow::on_startServer_pressed()
{

    if (tcpServer->isListening()) {

            stopServer();

        } else {
            startServer();
        }
    if (!tcpServer->isListening()){
        outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") + "|"+ serverAddressString +":" + inputPort->text()+"|"+"Сервер остановлен" );
    } else if(tcpServer->isListening()){
//        outputMess->append(td+"|"+ serverAddressString +":" + inputPort->text()+"|"+"Сервер запущен" );
    }


}

// для обработки нового подключения
void MainWindow::incomingConnecting()
{

    QTcpSocket* socket = tcpServer->nextPendingConnection();
    //если клиент подключен выводится сообщение

    int idUser =  socket->socketDescriptor();

    outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") + "|"+ serverAddressString +":" + inputPort->text()+"|"+ "Клиент подключен: " + QString::number( idUser));

    ClientsSockets.insert(idUser, socket); // сохраняем socket в карту. дескриптор в роли ключа

    if (socket != nullptr){
        connectClient = true;
         sendMessButton->setEnabled(true);
    }
     // для каждого клиента создается сокет и сохраняектся в вектор

    connect(socket,&QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, [this, socket]() {
        removeClientSocket(socket);
    });
//    countClients->setText( QString::number(ClientsSockets.count()) );
//    connect(socket,&QTcpSocket::disconnected, this, &MainWindow::deleteLater); // при отключении клиента, сокет удалиться



    // при подключении клиента ему сообщение Hello client
    QString welcomeMessage = "Hello, client!";
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_1);
    out<<welcomeMessage;
    socket->write(Data);

}

void MainWindow::removeClientSocket(QTcpSocket* socket)
{
   connect(socket,&QTcpSocket::disconnected, this, &MainWindow::deleteLater);
    // Отключаем все соединения для сокета
//    socket->disconnect();


    //   Удаляем  из карты сокет клиента, который отключается
    ClientsSockets.remove(ClientsSockets.key(socket));

//    socket->close();
    outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") + "|" + serverAddressString +":" + inputPort->text()+"|" + "Клиент " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort())+" отключился " );

//    countClients->setText( QString::number(ClientsSockets.count()) );
    qDebug() << socket->errorString();

    if (ClientsSockets.size()==0){
        sendMessButton->setEnabled(false);

    }else if (ClientsSockets.size()!=0) {
        sendMessButton->setEnabled(true);
}


}


// для обработки сокета
void MainWindow::slotReadyRead()
{

    socket = qobject_cast<QTcpSocket*>(sender());

//    currentSocket = socket;
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_1);
    if (in.status() == QDataStream::Ok)

    {
        QString str;
        in >> str;
        outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") + "|" + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()) + "|" + str); // выводим сообщение от клиента

    }
    else
    {
        outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") + "|" + socket->peerAddress().toString() + ":" + inputPort->text() + "|" + "ErrorStream");
    }

}

// для запуска сервера
void MainWindow::startServer()
{


    connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::incomingConnecting);

    if (tcpServer->listen(QHostAddress("127.0.0.1"), inputPort->text().toInt()))
    {
        serverAddress = tcpServer->serverAddress();
        serverAddressString = serverAddress.toString();
        startOffServer->setText("Стоп");
        startOffServer->setStyleSheet("background-color: red;");
        outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") +"|"+ serverAddressString +":" + inputPort->text()+ "|" + "Сервер запущен: " + serverAddressString + ":" + inputPort->text());
    }
    else
    {
        outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") + "|" + "Ошибка сервера: " + tcpServer->errorString());
        return;
    }
}


// для остановки сервера
void MainWindow::stopServer()
{


    if(tcpServer->isListening()){

        disconnect(tcpServer,&QTcpServer::newConnection, this, &MainWindow::incomingConnecting );

        // Освобождение ресурсов и удаление сокетов из карты
        foreach (int i, ClientsSockets.keys()) {

            QDataStream os(ClientsSockets[i]);
            os.setVersion(QDataStream::Qt_5_1);

            ClientsSockets[i]->close();
            ClientsSockets.remove(i);
        }
        tcpServer->close();

    }

    startOffServer->setText("Старт");
    startOffServer->setStyleSheet("background-color: green;");

}

// Для отправки сообщения клиенту
void MainWindow::SendToClient(QString str)
{
        Data.clear();
        QDataStream out(&Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_1);
        out << str;

//        qDebug()<<QString::number(ClientsSockets.count());
        for (QTcpSocket* socket : ClientsSockets) {

                socket->write(Data);

        }


     outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") + "|" + serverAddressString + ":" + inputPort->text() + "|" + str);


}




//для обработки нажатия на кнопку Оправить сообщение
void MainWindow::on_sendMessButton_pressed()
{
    if (tcpServer->isListening() and !inputMess->text().isEmpty() and ClientsSockets.size() != 0) {
        SendToClient(inputMess->text());
        inputMess->clear();
    } else if(!tcpServer->isListening()){
    outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"|"+ QHostAddress(QHostAddress::LocalHost).toString() +":" + inputPort->text()+"|" + "Сервер не запущен. Сообщение не отправлено" );

    } else if (tcpServer->isListening() and !inputMess->text().isEmpty() and ClientsSockets.size() == 0) {
        outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"|"+ QHostAddress(QHostAddress::LocalHost).toString() +":" + inputPort->text()+"|" + "Сервер запущен,клиент не подключен" );
    } else if (inputMess->text().isEmpty()) {
        outputMess->append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"|"+ QHostAddress(QHostAddress::LocalHost).toString() +":" + inputPort->text()+"|" + "Перед отправкой введите сообщение" );
    }
   else { qDebug() << "какая то ошибка"; }



}
