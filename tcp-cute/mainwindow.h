#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connection.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Connection *m_connect;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void dataReceive(QByteArray data);
    void setStatus(QString text);

private slots:
    void on_ConnectionButton_clicked();

    void on_SendMessageButton_clicked();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
