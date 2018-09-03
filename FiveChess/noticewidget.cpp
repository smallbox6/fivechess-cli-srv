#include "noticewidget.h"
#include "packetprocessor.h"
#include "exitdialog.h"
#include "common.h"

#include <QPainter>
#include <QDebug>


NoticeWidget::NoticeWidget(QWidget *parent) : QWidget(parent)
{
    m_bPieceColor = true;
    m_pixmap = QPixmap(":/Image/IMG/black_pieces.png");

    setFocusPolicy(Qt::ClickFocus);
    PacketProcessor::GetPacketProcessor()->SetNoticeWidget(this);

    m_pOkbtn = new QPushButton(QStringLiteral("开始游戏"), this);
    m_pOkbtn->setGeometry(380, 10, 70, 25);
    m_pCancelBtn = new QPushButton(QStringLiteral("退出游戏"), this);
    m_pCancelBtn->setGeometry(380, 45, 70, 25);

    m_pCancelBtn->setStyleSheet("QPushButton {\
                                   color: white;\
                                   background-color: rgba(255, 69, 0, 90)\
                                   }\
                                  QPushButton:hover {\
                                   background-color: rgba(178, 34, 34, 98);\
                                  }");
    m_pOkbtn->setStyleSheet("QPushButton {\
                                   color: white;\
                                   background-color: rgba(60, 179, 113, 98)\
                                   }\
                                 QPushButton:hover {\
                                   background-color: rgba(46, 139, 87, 98);\
                                 }");

    m_pNoticeText = new QTextEdit(this);
    m_pNoticeText->setGeometry(60, 10, 310, 60);
    m_pNoticeText->setReadOnly(true);
    m_pNoticeText->setStyleSheet("QTextEdit{\
                                font-size: 16px;\
                                background-color: rgba(252,239,239,80);\
                                border: 1px solid rgb(193,187,187);\
                                selection-color: white;\
                                selection-background-color: #C19A6B;\
                                }\
                                QScrollBar:vertical {\
                                width: 10px;\
                                background: transparent;\
                                margin-left: 1px;\
                                margin-right: 1px;\
                                }\
                                QScrollBar::handle:vertical {\
                                width: 10px;\
                                min-height: 10px;\
                                background: rgb(170, 200, 230);\
                                margin-top: 1px;\
                                margin-bottom: 1px;\
                                }\
                                QScrollBar::handle:vertical:hover {\
                                background: rgb(165, 195, 225);\
                                }");
    connect( m_pOkbtn, SIGNAL(clicked(bool)), this, SLOT(slot_StartGame()) );
    connect( m_pCancelBtn, SIGNAL(clicked(bool)), this, SLOT(slot_Exit()) );
}

NoticeWidget::~NoticeWidget()
{
    if(m_pNoticeText)
        delete m_pNoticeText;
    if(m_pOkbtn)
        delete m_pOkbtn;
    if(m_pCancelBtn)
        delete m_pCancelBtn;
}

void NoticeWidget::slot_StartGame()
{
    PacketProcessor::GetPacketProcessor()->GameStartBtn();
}

void NoticeWidget::slot_Exit()
{
    if(PacketProcessor::GetPacketProcessor()->GetGameStatus() == PLAYING)
    {
        ExitDialog *dlg = new ExitDialog(QStringLiteral("你当前正在游戏，是否确定要退出？"));
        if(dlg->exec() == QDialog::Accepted)
        {
            PacketProcessor::GetPacketProcessor()->ExitGame();
        }
        delete dlg;
    }
}

void NoticeWidget::SetPiecePixmap(int ncolor)
{
    m_bPieceColor = ncolor == -1 ? true : false;
    if(m_bPieceColor)
        m_pixmap = QPixmap(":/Image/IMG/black_pieces.png");
    else
        m_pixmap = QPixmap(":/Image/IMG/white_pieces.png");
    update();
}

void NoticeWidget::paintEvent(QPaintEvent *event)
{
    QPainter pt(this);
    pt.setBrush(QColor(255, 239, 213, 80));
    pt.drawRect(0,0, width() - 3, height() - 3);
    pt.drawPixmap(10, 20, 40, 40, m_pixmap);
}
