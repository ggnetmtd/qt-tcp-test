#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_connect = new Connection(DEFAULT_HOST, DEFAULT_PORT, this);

    connect(m_connect, &Connection::gotResult, this, &dataReceive);
    connect(m_connect, &Connection::changedStatus, this, &setStatus);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_connect;
}

void MainWindow::on_ConnectionButton_clicked()
{
    m_connect->open();
}


void MainWindow::on_SendMessageButton_clicked()
{
    if(!m_connect->isConnected())
    {
        statusBar()->showMessage("No connection. Can't send the message.");
        return;
    }

    // to-do: header builder
    MessageHeader msgh = MessageHeader(MathMessage);


    m_connect->send(msgh, ui->MathField->text().toStdString());
}

void MainWindow::dataReceive(QByteArray data)
{
    // remove header
    data.removeFirst();

    // current time
    const auto now = std::chrono::system_clock::now();
    const auto ct = std::chrono::system_clock::to_time_t(now);

    QString result = "[" + QString(std::ctime(&ct));
    result[result.size() - 1] = ']'; // уберем '\n'
    result += ": " + QString(data);

    ui->ReceivedText->appendPlainText(result);
}

void MainWindow::setStatus(QString text)
{
    statusBar()->showMessage(text);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(nullptr, "TcpClient", "Простой клиент, устанавливающий соединение\n"
                                             "с TCP сервером. Посылает заголовок вместе с 'сообщением',\n"
                                             "а так же принимает результат.\n"
                                             "valid message: 15+8*3-4/2\n"
                                             "not-valid message: (18+8)*3-4/2\n"
                                             "zota-test-homework");
}

