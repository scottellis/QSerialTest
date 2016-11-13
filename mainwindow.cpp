#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

// declare USE_TIMER if you want to poll the serial port
// otherwise the readyRead() signal will be used
#define USE_TIMER

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_serialPort = NULL;
    m_timer = 0;

    connect(ui->actionExit, SIGNAL(triggered()), SLOT(close()));
    connect(ui->actionOpen, SIGNAL(triggered()), SLOT(onOpen()));
    connect(ui->actionClose, SIGNAL(triggered()), SLOT(onClose()));
    connect(ui->actionSend, SIGNAL(triggered()), SLOT(onSend()));
    connect(ui->actionClear, SIGNAL(triggered()), SLOT(onClear()));

    onClose();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    onClose();
}

void MainWindow::onOpen()
{
    m_serialPort = new QSerialPort;

    m_serialPort->setPortName("ttyAMA0");

    if (!m_serialPort->setBaudRate(QSerialPort::Baud57600) ||
        !m_serialPort->setDataBits(QSerialPort::Data8) ||
        !m_serialPort->setParity(QSerialPort::NoParity) ||
        !m_serialPort->setStopBits(QSerialPort::OneStop) ||
        !m_serialPort->setFlowControl(QSerialPort::NoFlowControl)) {

        QMessageBox::warning(this, "Error", "Failed to setup serial port");
        delete m_serialPort;
        m_serialPort = NULL;
        return;
    }

    if (!m_serialPort->open(QIODevice::ReadWrite)) {
        QMessageBox::warning(this, "Error", "Failed to open serial port");
        delete m_serialPort;
        m_serialPort = NULL;
        return;
    }

#ifdef USE_TIMER
    m_timer = startTimer(100);
#else
    connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(onReceive()));
#endif

    ui->actionSend->setEnabled(true);
    ui->actionOpen->setEnabled(false);
    ui->actionClose->setEnabled(true);
}

void MainWindow::onClose()
{
    if (m_serialPort) {
#ifdef USE_TIMER
        killTimer(m_timer);
        m_timer = 0;
#else
        disconnect(m_serialPort, SIGNAL(readyRead()), this, SLOT(onReceive()));
#endif
        delete m_serialPort;
        m_serialPort = NULL;
    }

    ui->actionSend->setEnabled(false);
    ui->actionOpen->setEnabled(true);
    ui->actionClose->setEnabled(false);
}

void MainWindow::onSend()
{
    QString tx = ui->txEdit->text();

    if (tx.length() > 0) {
        m_serialPort->write(tx.toLatin1() + "\r\n");
        ui->txEdit->clear();
    }
}

void MainWindow::onClear()
{
    ui->rxText->clear();
}

void MainWindow::timerEvent(QTimerEvent *)
{
    onReceive();
}

void MainWindow::onReceive()
{
    // have to read everything since we won't get another signal
    // if we do a partial read like a readLine()
    QString rx = m_serialPort->readAll();

    if (rx.length() > 0)
        ui->rxText->appendPlainText(rx);
}
