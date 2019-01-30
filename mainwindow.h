#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QSerialPort;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void setPortStopBits(const int value);
    void setPortParity(const int parity);
    ~MainWindow();
signals:

private slots:
    void on_openButton_clicked();

    void on_sendButton_clicked();
    void readyReadSlot();
    void on_pushButtonClearR_clicked();

    void on_pushButtonClearS_clicked();

private:
    QSerialPort *serialPort;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
