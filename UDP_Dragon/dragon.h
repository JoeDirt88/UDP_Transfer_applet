//#--------------------------------------------------#
//#                                                  #
//#  Project created by: Johannes de Lange 23689293  #
//#                                                  #
//#--------------------------------------------------#

#ifndef DRAGON_H
#define DRAGON_H

#include <QMainWindow>
#include <QtNetwork/QUdpSocket>
#include <QFileInfo>
#include <QDirModel>
#include <Qfile>
#include <QIODevice>

namespace Ui {
class Dragon;
}

class Dragon : public QMainWindow
{
    Q_OBJECT

public:
    explicit Dragon(QWidget *parent = 0);
    ~Dragon();

private slots:
    void on_Chat_clicked();

    void receiveData();

    void on_LOAD_BTN_clicked();

    void on_DELETE_BTN_clicked();

    void on_SEND_BTN_clicked();

    void delay();

    void on_Grabber_clicked();

    void on_Jabber_clicked();

    void on_STOP_BTN_clicked();

    void on_RTB_BTN_clicked();

    void on_RTN_BTN_clicked();

private:
    Ui::Dragon *ui;

    QUdpSocket *socket;

    QDirModel *Directory;
};

#endif // DRAGON_H
