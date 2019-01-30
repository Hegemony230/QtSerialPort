#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())//获取可用的串口
        ui->serialPortComboBox->addItem(info.portName());

    ui->radioButtonTextR->setChecked(true);
    ui->radioButtonTextS->setChecked(true);
    serialPort = new QSerialPort();
    setWindowTitle(tr("串口助手"));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete serialPort;
}

void MainWindow::on_openButton_clicked()          //打开串口槽函数
{
    serialPort->setPortName(ui->serialPortComboBox->currentText());//获取要打开的串口
    if(!serialPort->isOpen())                     //如果之前是没有打开的则进行open动作
    {
        if(serialPort->open(QIODevice::ReadWrite))//如果打开成功了，则按钮显示“关闭”
        {
            //××××××××××××××××××××××××××设置默认串口参数××××××××××××××××××××××××××××××××//
            //serialPort->setBaudRate(QSerialPort::Baud115200);       //波特率19200
            serialPort->setBaudRate(ui->comboBoxBaud->currentText().toInt(),QSerialPort::AllDirections);
            serialPort->setDataBits(QSerialPort::Data8);           //数据位8位
            serialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控
            setPortStopBits(ui->comboBoxStop->currentIndex());//设置停止位
            setPortParity(ui->comboBoxParity->currentIndex());//设置较验位

            serialPort->setReadBufferSize(500);                    //设置数据缓冲区大小,以后可以写成宏的形式
            //××××××××××××××××××××××××××设置默认串口参数××××××××××××××××××××××××××××××××//
            connect(serialPort, SIGNAL(readyRead()), this, SLOT(readyReadSlot()));//连接信号和槽
            ui->openButton->setText(tr("关闭"));
        }
        else
        {
            qDebug()<<serialPort->errorString();
            this->statusBar()->showMessage(serialPort->errorString());
        }
    }
    else                                          //如果串口处于打开的状态，则关闭
    {
        serialPort->close();
        ui->openButton->setText(tr("打开"));
    }
}

void MainWindow::setPortStopBits(const int stopBit)
{
    switch (stopBit) {
    case 0:
        serialPort->setStopBits(QSerialPort::OneStop);         //1位停止位
        break;
    case 1:
        serialPort->setStopBits(QSerialPort::OneAndHalfStop);         //1.5位停止位
        break;
    case 2:
        serialPort->setStopBits(QSerialPort::TwoStop);         //2位停止位
        break;
    default:
        serialPort->setStopBits(QSerialPort::UnknownStopBits);
        break;
    }
}

void MainWindow::setPortParity(const int parity)
{
    switch (parity)
    {
    case 0:
        serialPort->setParity(QSerialPort::NoParity);          //无较验位
        break;
    case 1:
        serialPort->setParity(QSerialPort::OddParity);          //奇较验位
        break;
    case 2:
        serialPort->setParity(QSerialPort::EvenParity);          //偶较验位
        break;
    default:
        break;
    }
}

void MainWindow::on_sendButton_clicked()
{
    QString str = ui->sendTextEdit->toPlainText();//获取要发送的内容
    if(serialPort->isOpen())                      //如果之前是没有打开的则进行open动作
    {
        if(ui->radioButtonTextS->isChecked())
        {
            QByteArray sendData = str.toUtf8();
            qDebug() << "发送文本";
            serialPort->write(sendData);
        }else if(ui->radioButtonHexS->isChecked())
        {
            QByteArray str2 = QByteArray::fromHex (str.toLatin1().data());
            qDebug() << "发送十六进制";
            serialPort->write(str2);
        }
    }
}

void MainWindow::readyReadSlot()                 //接收数据槽函数
{
    QByteArray arr = serialPort->readAll();
    ui->receiveTextEdit->moveCursor(QTextCursor::EndOfLine);//把光标移到文本末尾
    if(ui->radioButtonTextR->isChecked())
    {
        ui->receiveTextEdit->insertPlainText(arr);
        qDebug() << "接收文本";
    }else if(ui->radioButtonHexR->isChecked())
    {
        qDebug() << "接收十六进制";
        QString strDisplay;
        QString str3 = arr.toHex().data();//以十六进制显示
        str3 = str3.toUpper ();//把字母小写变成大写(a~f)
        for(int i = 0;i<str3.length();i+=2)//加空格的程序
        {
            QString st = str3.mid (i,2);
            strDisplay += st;
            strDisplay += " ";
        }
        ui->receiveTextEdit->insertPlainText(strDisplay);
    }
}

void MainWindow::on_pushButtonClearR_clicked()
{
    ui->receiveTextEdit->clear();
}

void MainWindow::on_pushButtonClearS_clicked()
{
    ui->sendTextEdit->clear();
}
