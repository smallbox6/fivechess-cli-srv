#include "chatwidget.h"
#include "packetprocessor.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QPainter>
#include <QFontMetrics>
#include <QIcon>


ChatItem::ChatItem(ChatInfo info, QWidget *parent):
    QWidget(parent), m_chatInfo(info)
{
    m_pTextEdit = new QTextEdit(this);
    m_pTextEdit->verticalScrollBar()->hide();
    m_pTextEdit->horizontalScrollBar()->hide();
    m_pTextEdit->setStyleSheet("QTextEdit{\
                                color:white;\
                                font-size: 16px;\
                                background-color: rgba(13,133,188,80);\
                                border: 1px solid #333333;\
                                border-radius: 8px;\
                                selection-color: white;\
                                selection-background-color: #C19A6B;\
                                }"
                                "QMenu {\
                                    color:black;\
                                    background: rgba(255, 255, 255, 0.5);\
                                    border: 1px solid gray;\
                                    padding: 0px;\
                                    }"
                                "QMenu::item:selected {\
                                    color:black;\
                                    background: rgba(255, 255, 255, 0.7);\
                                }");

    initComponent();
}

ChatItem::~ChatItem()
{
    if(m_pTextEdit)
        delete m_pTextEdit;
}

void ChatItem::SetChatString()
{
    m_pTextEdit->setText(m_chatInfo.chatString);
}

void ChatItem::initComponent()
{
    m_pTextEdit->setReadOnly(true);
    SetChatString();
    if(false == m_chatInfo.isSelf)
    {
        m_pTextEdit->setGeometry(36, 0, m_chatInfo.lineLen, m_chatInfo.height);
    }
    else
    {
        m_pTextEdit->setGeometry(18, 0, m_chatInfo.lineLen, m_chatInfo.height);
    }
}

void ChatItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    QPixmap pic(m_chatInfo.charUrl);
    if(m_chatInfo.isSelf)
        painter.drawPixmap(208, 0, 32, 32, pic);
    else
        painter.drawPixmap(0, 0, 32, 32, pic);
}

/////////////////////////

ScrollChat::ScrollChat(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("QWidget{\
                   background-color:rgba(32, 32, 32, 0);\
                   }");
    initComponent();
}

ScrollChat::~ScrollChat()
{
    if(m_pScrollArea)
        delete m_pScrollArea;
    if(m_pVLayout)
        delete m_pVLayout;
    if(m_pWidget)
        delete m_pWidget;
}

void ScrollChat::initComponent()
{
    m_pWidget = new QWidget(this);
    m_pVLayout = new QVBoxLayout(this);
    m_pScrollArea = new QScrollArea(this);
    m_pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pWidget->setLayout(m_pVLayout);
    m_pScrollArea->setWidget(m_pWidget);
    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->show();
    m_pScrollArea->setGeometry(0, 30, 250, 220);
}

void ScrollChat::AddChatInfo(ChatInfo info)
{
    ChatItem  *item;
    static int h = 0;
    QFontMetrics ft(this->font());
    info.height = (ft.width(info.chatString) / info.lineLen + 1.5) * info.lineHeight;

    h += info.height;
    if(h > 220)
    {
        qDebug() << "h > m_pWidget->height() : " << h << " ?> " << m_pWidget->height();
        m_pScrollArea->widget()->resize(250, 2 * h);
        m_pScrollArea->verticalScrollBar()->setValue(h);
    }

    item = new ChatItem(info, m_pWidget);
    item->setFixedSize(240, info.height);

    m_pVLayout->addWidget(item);
}

/////////////////////////////////////
ChatWidget::ChatWidget(QWidget *parent) : QWidget(parent)
{
    setFocusPolicy(Qt::ClickFocus);
    setGeometry(600, 320, 250, 280);
    m_pListWidget = new ScrollChat(this);
    m_pTextEdit = new QTextEdit(this);
    m_pTextEdit->setGeometry(0, 250, 210, 30);
    m_pTextEdit->verticalScrollBar()->hide();
    m_pTextEdit->setStyleSheet("QTextEdit{\
                               color: black;\
                               border: 1px solid rgb(252,239,239,95);\
                               background-color:rgba(252,239,239,80);\
                               }");

    m_pSendBtn = new QPushButton(this);
    m_pSendBtn->setText(QStringLiteral("发送"));
    m_pSendBtn->setStyleSheet("QPushButton {\
                                color: black;\
                                border: 1px solid rgb(32,32,32);\
                                background-color:rgba(252,239,239,80);}"
                              "QPushButton:hover {\
                                color: white;\
                                background-color: rgba(252,239,239,90);};\
                              }");
    m_pSendBtn->setGeometry(215, 250, 35, 30);

    setWidgetEnable(false);

    PacketProcessor::GetPacketProcessor()->SetChatWig(this);

    connect( m_pSendBtn, SIGNAL(clicked()), this, SLOT(slot_SenddMsg()) );
}

void ChatWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QColor(255, 239, 213, 80));
    painter.setBrush(QColor(255, 239, 213, 80));
    painter.drawRect(0, 0, width(), height() - 30);

    painter.setBrush(QColor(252,239,239,90));
    painter.drawRect(0, 0, width(), 30);

    QRect rc(100, 0, 100, 30);
    painter.setPen(QColor(Qt::black));
    painter.drawText(rc, QStringLiteral("好友聊天"));
}

void ChatWidget::setWidgetEnable(bool live)
{
    m_pTextEdit->setEnabled(live);
    m_pSendBtn->setEnabled(live);
}

void ChatWidget::slot_SenddMsg()
{
    ChatInfo info;
    info.isSelf = true;
    info.charUrl = QString(":/Image/IMG/me.png");
    info.chatString = m_pTextEdit->toPlainText();
    if(info.chatString.size() == 0)
        return;
    m_pListWidget->AddChatInfo(info);
    m_pTextEdit->clear();
    PacketProcessor::GetPacketProcessor()->SendMsg(info.chatString.toLatin1());
}

void ChatWidget::SetMsg(QString msg, bool self)
{
    if(msg.size() == 0)
        return;
    ChatInfo info;
    info.isSelf = self;
    info.charUrl = QString(":/Image/IMG/counterpart.png");
    info.chatString = msg;
    m_pListWidget->AddChatInfo(info);
}
