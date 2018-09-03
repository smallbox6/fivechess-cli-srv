#include "PlayerOnLineList.h"
#include "packetprocessor.h"

#include <QFile>
#include <QDebug>
#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QIcon>
#include <QScrollBar>
#include <QMessageBox>
#include <QPainter>


MyListWidget::MyListWidget(QWidget *parent)
: QListWidget(parent)
{
    m_bSendMatch = false;
    m_parentWig = nullptr;
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pRandomAtc = new QAction(QStringLiteral("随机匹配"), this);
    m_pInviteAtc = new QAction(QStringLiteral("邀请游戏"), this);
    connect( m_pInviteAtc, SIGNAL(triggered()), this, SLOT(slot_PlayingWith()) );
    connect( m_pRandomAtc, SIGNAL(triggered()), this, SLOT(slot_RandomPlayingWith()) );
}

MyListWidget::~MyListWidget()
{
}

void MyListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if(m_bSendMatch == true)
        return;
    QMenu* popMenu = new QMenu(this);
    if(itemAt(event->pos()) != NULL &&
       PacketProcessor::GetPacketProcessor()->IsLogon())
    {
        popMenu->addAction(m_pInviteAtc);
        popMenu->addAction(m_pRandomAtc);
        popMenu->exec(QCursor::pos());
    }
    delete popMenu;
}

void MyListWidget::slot_PlayingWith()
{
    m_bSendMatch = true;
    QByteArray arr = m_parentWig->GetCountName(currentRow());
    PacketProcessor::GetPacketProcessor()->PlayingWith(arr);
}

void MyListWidget::slot_RandomPlayingWith()
{
    m_bSendMatch = true;
    PacketProcessor::GetPacketProcessor()->RandomPlay();
}

//////////////////////////////////////////////////////

PlayerOnLineList::PlayerOnLineList(QWidget *parent) : QWidget(parent)
{
    setFocusPolicy(Qt::ClickFocus);
    m_pListWidget = new MyListWidget(this);
    m_pListWidget->SetParentWig(this);
    m_pListWidget->setGeometry(0, 30, 250, 200);
    m_pListWidget->setStyleSheet("QListWidget{\
                                       color:rgba(255, 255, 255, 100);\
                                       background-color:rgba(255, 239, 213, 80);\
                                       font-size: 12px;\
                                       border:rgb(81,81,81);\
                                   }\
                                   QListWidget::item{\
                                       height: 20px;\
                                       border: none;\
                                   }\
                                   QListWidget::item:hover{\
                                       background-color:rgba(255,255,255,60);\
                                       border:none;\
                                   }\
                                   QListWidget::item:selected{\
                                       background-color:rgba(255,255,255,90);\
                                   }");\

    PacketProcessor::GetPacketProcessor()->SetPlaylistWig(this);
}

void PlayerOnLineList::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(QColor(252,239,239,90));
    painter.setPen(QColor(252,239,239,90));
    painter.drawRect(0, 0, width(), 30);

    QRect rc(100, 0, 100, 30);
    painter.setPen(Qt::black);
    painter.drawText(rc, QStringLiteral("在线玩家"));
}

void PlayerOnLineList::addItem(QByteArray name, bool bgameing)
{
    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QPixmap pix;
    for(int i = 0; i < m_playerStr.size(); i++)
        if(m_playerStr[i] == name)
            return;
    m_playerStr.push_back(name);
    if(bgameing)
        pix = QPixmap(":/Image/IMG/playing.png");
    else
        pix = QPixmap(":/Image/IMG/online.png");
    QLabel *pic = new QLabel;
    pic->setFixedSize(8, 8);
    pic->setPixmap(pix);
    hLayout->addWidget(pic);
    QLabel *text = new QLabel(name);
    text->setFixedHeight(16);
    hLayout->addWidget(text);
    hLayout->setAlignment(Qt::AlignLeft | Qt::AlignCenter);
    widget->setLayout(hLayout);

    QListWidgetItem *item = new QListWidgetItem(m_pListWidget);
    item->setSizeHint(QSize(0, 30));
    m_pListWidget->setItemWidget(item, widget);
    m_pListWidget->addItem(item);
}

QByteArray PlayerOnLineList::GetCountName(int row)
{
    if(row > m_playerStr.size())
        return QByteArray();
    else {
        return m_playerStr[row];
    }
}

void PlayerOnLineList::UpdateItems(std::vector<OnlinePlayer> &players)
{
    m_pListWidget->clear();
    m_playerStr.clear();
    for(int i = 0; i < players.size(); i++)
        addItem(players[i].name, players[i].isplaying);
}
