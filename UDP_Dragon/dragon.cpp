//#--------------------------------------------------#
//#                                                  #
//#  Project created by: Johannes de Lange 23689293  #
//#                                                  #
//#--------------------------------------------------#

#include "dragon.h"
#include "ui_dragon.h"
#include <QTime>
#include <QDir>
#include <QtEndian>
#include <QIODevice>

Dragon::Dragon(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Dragon)
{
    ui->setupUi(this);

    // Directory Model Creation
    Directory = new QDirModel(this);
    Directory->setReadOnly(false);
    ui->treeView->setModel(Directory);
    Directory->setSorting(QDir::DirsFirst |
                          QDir::IgnoreCase |
                          QDir::Name);
    // Set initial selection
    QModelIndex index = Directory->index("D:/");
    // Set initial view of directory
    // for the selected drive as expanded
    ui->treeView->expand(index);
    // Make it scroll to the selected
    ui->treeView->scrollTo(index);
    // Highlight the selected
    ui->treeView->setCurrentIndex(index);
    // Resizing the column - first column
    ui->treeView->resizeColumnToContents(0);

    // Create and Bind Socket
    socket = new QUdpSocket(this);
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(receiveData()));
    socket->bind(QHostAddress::Any, 5743);

    // Set default broadsast
    ui->That_IP->setText("255.255.255.255");
}

Dragon::~Dragon()
{
    delete ui;
}

void Dragon::on_Chat_clicked()
{
    QByteArray datagram = "C" + ui->ChatEdit->text().toUtf8();
    socket->writeDatagram(datagram, QHostAddress(ui->That_IP->text()), 5743);
    ui->ChatEdit->clear();
}

void Dragon::receiveData()
{
    QByteArray BufferSize;
    QByteArray BufferName;
    int FinalSize;

    while (socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        socket->readDatagram(datagram.data(), datagram.size(),
                             &sender, &senderPort);

        // Chat Protocol
        if (datagram.left(1) == "C")
        {
            if (datagram.left(5) == "Chelp")
            {
                ui->ChatWindow->setText("Type the following commands for effect:\nclear - to clear consol window\nhelp - to display this text\nstop - to stop the current file transfer\nsetup - to see step by step instructions to use this tool\nexit - to exit program");
                datagram.clear();
            }
            else if (datagram.left(6) == "Cclear")
            {
                ui->ChatWindow->clear();
                datagram.clear();
            }
            else if (datagram.left(5) == "Cexit")
            {
                QApplication::quit();
            }
            else if (datagram.left(5) == "Cstop")
            {
                on_STOP_BTN_clicked();
            }
            else if (datagram.left(6) == "Csetup")
            {
                ui->ChatWindow->clear();
                ui->ChatWindow->setText(ui->ChatWindow->text() +
                                        "Welcome t the STEP-BY-STEP instruction guide:" +
                                        "\n" +
                                        "\nStep [ 1]: Click on the 'Grab This PC's IP Address' button" +
                                        "\nStep [ 2]: Tick the checkbox underneath the now displaying IP address" +
                                        "\nStep [ 3]: Repeat Step 1 and 2 on the other node PC" +
                                        "\nStep [ 4]: If the file transfer must be broadcast continue to Step [8]" +
                                        "\nStep [ 5]: Click on the 'Broadcast My IP' button" +
                                        "\nStep [ 6]: Tick the checkbox underneath the now displaying IP address" +
                                        "\nStep [ 7]: Repeat Step 5 and 6 on the other node PC" +
                                        "\nStep [ 8]: On the Sender PC, select the file to transfer from the dir tree" +
                                        "\nStep [ 9]: Click on the 'LOAD' button" +
                                        "\nStep [10]: Click on the 'SEND' button" +
                                        "\nStep [11]: If you are happy with the file transfer feel free to exit" +
                                        "\nStep [12]: If the transfer should be cancelled, simply click the 'STOP' button" +
                                        "\n" +
                                        "\nThank you for using the 'Here be Dragons' FTP over UDP software");
            }
            else
            {
                datagram.remove(0, 1);
                ui->ChatWindow->setText(ui->ChatWindow->text() + "\n\nMessage from: " + sender.toString() +
                                        "\n[ " + datagram + " ]");
                datagram.clear();
            }
        }

        // File and Addressing Protocol
        if (sender.toString() == ui->This_IP->text())
        {
            break;
        }
        else
        {
            if (datagram.left(1) == "S")
            {
                ui->ChatWindow->setText(ui->ChatWindow->text()+ "\nFile Details Received");
                // Create QByteArray Buffers for reading from the datagram
                BufferSize.clear();
                BufferName.clear();
                datagram.remove(0, 1);
                // Read File Size From Datagram

                while (datagram.left(1) != "P")
                {
                    BufferSize.append(datagram.left(1));
                    datagram.remove(0, 1);
                }
                // Store Received File Total Size
                QString FS(BufferSize);
                FinalSize = FS.toInt();
                double ESR = FinalSize/(1024*36);
                ui->RingFS_LBL->setText(QString::number(FinalSize));
                ui->ESR_LBL->setText(QString::number(ESR + 1));
                if (FinalSize < 1024)
                {
                    QString RPFSize = QString::number(FinalSize);
                    ui->RSize_LBL->setText(RPFSize + " B");
                }
                else
                {
                    QString RPFSize = QString::number(FinalSize/1024);
                    ui->RSize_LBL->setText(RPFSize + " KB");
                }
                ui->ChatWindow->setText(ui->ChatWindow->text()+ "\nFile Size Received");
                datagram.remove(0, 1);

                // Read File Path From Datagram

                while (datagram.size() > 0)
                {
                    BufferName.append(datagram.left(1));
                    datagram.remove(0, 1);
                }

                ui->ChatWindow->setText(ui->ChatWindow->text()+ "\nFile Name Received");
                ui->RStatus_LBL->clear();




                // Create New file in Download Directory to Write to
                QString F_Name(BufferName);
                ui->RPath_LBL->setText(F_Name);
                QFile Receive("C:/QTDL/"+F_Name);
                ui->RPath_LBL->setText("C:/QTDL/"+F_Name);
                Receive.open(QIODevice::WriteOnly);
                Receive.flush();
                Receive.close();

            }

            if (datagram.left(1) == "F")
            {
                QByteArray Ack;
                int ProVal;
                datagram.remove(0,1);
                QString PV(datagram.left(1));
                ProVal = PV.toInt();
                ui->Seg_Prog->setValue((ProVal+1)*10);
                datagram.remove(0,1);
                QFile Combine(ui->RPath_LBL->text().toUtf8());
                if (!Combine.open(QIODevice::ReadOnly)) return;
                QByteArray Contents = Combine.readAll();
                Contents.append(datagram);
                Combine.close();

                QFile Construct(ui->RPath_LBL->text().toUtf8());
                Construct.open(QIODevice::WriteOnly);
                Construct.write(Contents);

                double FP = Construct.size();
                int FPi = Construct.size();
                ui->RedFS_LBL->setText(QString::number(FPi));
                double FPD = ui->RingFS_LBL->text().toInt();
                int SegRecing = (((ui->ESR_LBL->text().toInt())/10)+1);
                int SegReced = (((ui->SR_LBL->text().toInt())/10)+1);

                ui->FilRe_LBL->clear();
                ui->FilRe_LBL->setText(ui->FilRe_LBL->text() + "Receiving segment [" + QString::number(SegReced) + "/" + QString::number(SegRecing) + "]");
                ui->File_Prog->setValue((FP/FPD)*100);
                Construct.close();

                if(ui->SR_LBL->text() == "")
                {
                    ui->SR_LBL->setText("1");
                }
                else
                {
                    int Seg = ui->SR_LBL->text().toInt();
                    Seg++;
                    ui->SR_LBL->setText(QString::number(Seg));
                }
                Ack.clear();
                Ack.append("ACK");
                socket->writeDatagram(Ack, QHostAddress(ui->That_IP->text()), 5743);
            }

            if (datagram.left(3) == "EOF")
            {
                datagram.remove(0,3);
                //QFile CRC(ui->RPath_LBL->text().toUtf8());
                ui->RStatus_LBL->setText("DONE");
                ui->Seg_Prog->setValue(100);
                ui->CurSe_LBL->setText("Complete");
                ui->FilRe_LBL->setText("Complete");
                QPixmap RICONDONE = "C:/Users/atara/Documents/UDP_Dragon/ICONS/File.png";
                int RI_W = ui->RICON_LBL->width();
                ui->RICON_LBL->setPixmap(RICONDONE.scaledToHeight(RI_W));
            }

            if (datagram.left(3) == "ACK")
            {
                datagram.remove(0,3);
                ui->ACK_LBL->setText("R");
                ui->CurSe_LBL->setText("Receiving");

            }

            if (datagram.left(4) == "iPee")
            {
                if (ui->IP_HBS_CB->isChecked())
                {
                    ui->ChatWindow->setText(ui->ChatWindow->text()+ "\nPlease untic IP checkbox to change this address");
                }
                else
                {
                    ui->This_IP->setText(sender.toString());
                }
            }

            if (datagram.left(4) == "uPee")
            {
                if (ui->UP_HBS_CB->isChecked())
                {
                    ui->ChatWindow->setText(ui->ChatWindow->text()+ "\nPlease untic IP checkbox to change that address");
                }
                else
                {
                    if (sender.toString() != ui->This_IP->text())
                    {
                        ui->ChatWindow->setText(ui->ChatWindow->text()+ "\nSender and Receiver IP can not be equal");
                    }
                    else
                    {
                        QString SSend = sender.toString();
                        SSend.remove(0,7);
                        ui->That_IP->setText(SSend);
                    }
                }
            }
        }
    }
}

void Dragon::on_LOAD_BTN_clicked()
{
    //Open file for transfer
    QModelIndex index = ui->treeView->currentIndex();
    if(!index.isValid()) return;

    QString PackageFilePath = Directory->filePath(index);
    QString PackageFileName = Directory->fileName(index);
    QFile PackageFile(PackageFilePath);
    quint64 PackageFilSize = PackageFile.size();

    // Reset GUI Values
    ui->RingFS_LBL->clear();
    ui->RedFS_LBL->clear();
    ui->ESR_LBL->clear();
    ui->SR_LBL->clear();
    ui->CurSe_LBL->clear();
    ui->FilRe_LBL->clear();
    ui->RPath_LBL->clear();
    ui->RSize_LBL->clear();
    ui->RStatus_LBL->clear();
    ui->Seg_Prog->setValue(0);
    ui->File_Prog->setValue(0);
    ui->ACK_LBL->clear();
    ui->ChatWindow->clear();
    ui->ChatWindow->setText(ui->ChatWindow->text()+ "\nFull File Path : " + PackageFilePath);

    ui->Path_LBL->setText(PackageFileName);
    ui->FPath_LBL->setText(PackageFilePath);
    ui->Status_LBL->setText("READY");

    QPixmap Icon = "C:/Users/atara/Documents/UDP_Dragon/ICONS/File.png";
    int I_W = ui->ICON_LBL->width();
    ui->ICON_LBL->setPixmap(Icon.scaledToHeight(I_W));

    if (PackageFilSize < 1024)
    {
        QString PFSize = QString::number(PackageFilSize);
        ui->Size_LBL->setText(PFSize + " B");
    }
    else
    {
        QString PFSize = QString::number(PackageFilSize/1024);
        ui->Size_LBL->setText(PFSize + " KB");
    }

}

void Dragon::on_DELETE_BTN_clicked()
{
    //Delete file from disk
    QModelIndex index = ui->treeView->currentIndex();
    if(!index.isValid()) return;
    if(Directory->fileInfo(index).isDir())
    {
        //dir
        Directory->rmdir(index);
    }
    else
    {
        //file
        Directory->remove(index);
    }
}

void Dragon::on_SEND_BTN_clicked()
{
    // Initialise and Sanitize File For Sending
    ui->ChatWindow->setText(ui->ChatWindow->text()+ "\nInitialise and Sanitize File For Sending");
    ui->Status_LBL->setText("INITIALISING FILE");
    QString Package = ui->Path_LBL->text().toUtf8();
    QFile SendThis(ui->FPath_LBL->text().toUtf8());
    // Sanity Check
    ui->ChatWindow->setText(ui->ChatWindow->text()+ "\nSanity Check");
    if(!SendThis.open(QFile::ReadOnly))
    {
        ui->ChatWindow->setText(ui->ChatWindow->text() + "\ncould not read file");
    }
    QByteArray Thing = SendThis.readAll();

    // Create File Transfer Loop Variables
    ui->ChatWindow->setText(ui->ChatWindow->text()+ "\nCreate File Transfer Loop Variables");
    bool flag = 0;
    int pos = 0;
    int pax = 36*1024;
    int frames = 0;
    QByteArray Frame;
    Frame.resize(2+pax);

    // Send File Details For Reception
    ui->ChatWindow->setText(ui->ChatWindow->text()+ "\nSend File Details For Reception");
    ui->Status_LBL->setText("SENDING DETAILS");
    Frame.clear();
    Frame.append("S");
    Frame.append(QString::number(Thing.size()));
    Frame.append("P");
    Frame.append(Package);
    socket->writeDatagram(Frame, QHostAddress(ui->That_IP->text()), 5743);

    // Start File Package Sending Loop
    ui->ChatWindow->setText(ui->ChatWindow->text()+ "\nStart File Package Sending Loop");
    while (flag == 0)
    {
        if (ui->Status_LBL->text() == "STOPPED")
        {
            break;
        }
        ui->Status_LBL->setText("SENDING FILE");
        if (frames < 10)
        {
            Frame.clear();
            Frame.append("F");
            Frame.append(QString::number(frames));
            if (Thing.size() > pax)
            {
                Frame.append(Thing, pax);
                Thing.remove(0, pax);
            }
            else
            {
                Frame.append(Thing, Thing.size());
                Thing.remove(0, Thing.size());
            }
            frames++;
        }
        else
        {
            Frame.clear();
            frames = 0;
            Frame.append("F");
            Frame.append(QString::number(frames));
            if (Thing.size() > pax)
            {
                Frame.append(Thing, pax);
                Thing.remove(0, pax);
            }
            else
            {
                Frame.append(Thing, Thing.size());
                Thing.remove(0, Thing.size());
            }
            frames++;
        }
        socket->writeDatagram(Frame,QHostAddress(ui->That_IP->text()), 5743);
        pos++;
        if (Thing.size() == 0)
        {
            flag = 1;
            ui->Status_LBL->setText("SENT");
            ui->ChatWindow->setText("Sending Completed");
            ui->ChatWindow->setText(ui->ChatWindow->text() + "\n"+ QString::number(pos) + " Packages Sent");
        }
        // add wait state...
        int WaitFor = 0;
        while (ui->ACK_LBL->text() != "R")
        {
            ui->CurSe_LBL->setText("Waiting");
            delay();
            WaitFor++;
            if (WaitFor == 5)
            {
                socket->writeDatagram(Frame,QHostAddress(ui->That_IP->text()), 5743);
            }
        }
        ui->ACK_LBL->setText("W");
        ui->CurSe_LBL->setText("Waiting");
    }
    Frame.clear();
    Frame.append("EOF");
    socket->writeDatagram(Frame, QHostAddress(ui->That_IP->text()), 5743);
}

void Dragon::delay()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
}

void Dragon::on_Grabber_clicked()
{
    QByteArray iPee;
    iPee.clear();
    iPee.append("iPee");
    socket->writeDatagram(iPee, QHostAddress::Broadcast, 5743);
}

void Dragon::on_Jabber_clicked()
{
    QByteArray uPee;
    uPee.clear();
    uPee.append("uPee");
    socket->writeDatagram(uPee, QHostAddress::Broadcast, 5743);
}

void Dragon::on_STOP_BTN_clicked()
{
    ui->ChatWindow->setText("\nSending Stopped");
    ui->Status_LBL->setText("STOPPED");
    ui->ICON_LBL->clear();
    ui->Path_LBL->clear();
    ui->Size_LBL->clear();
    ui->CurSe_LBL->clear();
    delay();
    ui->ChatWindow->setText(ui->ChatWindow->text()+ "\nResetting Program Status");
    delay();

    on_LOAD_BTN_clicked();
}

void Dragon::on_RTB_BTN_clicked()
{
    ui->That_IP->setText("255.255.255.255");
}

void Dragon::on_RTN_BTN_clicked()
{
    ui->This_IP->setText("0.0.0.0");
}
