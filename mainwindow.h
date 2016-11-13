#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *);
    void timerEvent(QTimerEvent *);

private slots:
    void onOpen();
    void onClose();
    void onSend();
    void onClear();
    void onReceive();

private:
    Ui::MainWindow *ui;

    QSerialPort *m_serialPort;
    int m_timer;
};

#endif // MAINWINDOW_H
