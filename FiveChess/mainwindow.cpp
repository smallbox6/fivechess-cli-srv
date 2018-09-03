#include "mainwindow.h"
#include "logondialog.h"
#include "packetprocessor.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QPainter>
#include <QFile>


void BackGroundWidget::paintEvent(QPaintEvent *event)
{
    QPixmap pixmap(":/Image/IMG/background01.jpeg");
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), pixmap);
}

/////////////////////////////////////

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowIcon(QIcon(":/Image/IMG/app_icon.ico"));

    setFixedSize(900, 640);
    setFocusPolicy(Qt::ClickFocus);
    setWindowFlags(Qt::FramelessWindowHint);

    m_pBackgroundWidget = new BackGroundWidget(this);
    m_pBackgroundWidget->setGeometry(0, 0, this->width(), this->height());

    m_pTopWidget = new TopWidget(this);
    m_pTopWidget->setGeometry(0, 0, this->width(), 64);
    m_pTopWidget->SetWindowsSizeAndPos(width(), 64);
    connect( m_pTopWidget, SIGNAL(sig_CloseWindow(bool)), this, SLOT(slot_CloseWindow()) );
    connect( m_pTopWidget, SIGNAL(sig_MinWindow()), this, SLOT(slot_MinWindow()) );
    m_pChessboard = new CheeseBoardWidget(this);
    m_pChessboard->SetOriginPos(80, 80);

    m_pPlayerOnLineList = new PlayerOnLineList(this);
    m_pPlayerOnLineList->setGeometry(600, 80, 250, 230);
    m_pChatWidget = new ChatWidget(this);

    m_pSideBar = new SideConfigBar(this);
    m_pSideBar->installEventFilter(this);

    m_pNoticeWidget = new NoticeWidget(this);
    m_pNoticeWidget->setGeometry(80, 550, 460, 80);

    m_ppopDlg = new PopDlg(this);
    m_ppopDlg->setGeometry(width() / 2 - 200, height() / 2 - 150, 400, 300);

    PacketProcessor::GetPacketProcessor()->SetMainWig(this);
}

MainWindow::~MainWindow()
{
    if(m_pBackgroundWidget) delete m_pBackgroundWidget;
    if(m_pChessboard) delete m_pChessboard;
    if(m_pPlayerOnLineList) delete m_pPlayerOnLineList;
    if(m_pChatWidget) delete m_pChatWidget;
    if(m_pSideBar) delete m_pSideBar;
    if(m_ppopDlg) delete m_ppopDlg;
    if(m_pNoticeWidget) delete m_pNoticeWidget;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_pSideBar)
    {

        if (event->type()==QEvent::FocusIn)
        {
        }
        else if (event->type()==QEvent::FocusOut)
        {
             if(m_pSideBar->IsShow() == true)
                m_pSideBar->slot_ShowSideBar();
        }
    }
}

void MainWindow::slot_CloseWindow()
{
    if(LogOnDialog::getLogOnDialog())
        LogOnDialog::getLogOnDialog()->close();
    this->close();
}

void MainWindow::slot_MinWindow()
{
    showMinimized();
}
