#include "sideconfigbar.h"
#include "packetprocessor.h"
#include "common.h"

#include <QApplication>
#include <QPainter>
#include <QDebug>


SideConfigBar::SideConfigBar(QWidget *parent) :
    QWidget(parent)
{
    setFocusPolicy(Qt::ClickFocus);

    m_isShow = false;
    m_bFightOnline = false;
    m_bPiecesColor = true;
    const char* radio = "QRadioButton{font-size: 16px; spacing: 10px;}\
                        QRadioButton::indicator:unchecked{\
                        image:url(:/Image/IMG/radio_off.png);}\
                        QRadioButton::indicator:checked{\
                        image:url(:/Image/IMG/radio_on.png);}";

    m_pSideBtn = new QPushButton(this);

    m_pBackGrndMsc = new QCheckBox(QStringLiteral("背景音乐"), this);
    m_pBackGrndMsc->setFocusPolicy(Qt::NoFocus);
    m_pBackGrndMsc->setStyleSheet("QCheckBox{font-size: 16px; spacing: 10px;}\
                                    QCheckBox::indicator:unchecked{\
                                    image:url(:/Image/IMG/checkbox_off.png);}\
                                    QCheckBox::indicator:checked{\
                                    image:url(:/Image/IMG/checkbox_on.png);}");

    m_pPiecesLabel = new QLabel(QStringLiteral("请选择棋子颜色"), this);
    m_pPiecesLabel->setStyleSheet("QLabel{font-size: 16px;}");
    m_pPiecesGroup = new QButtonGroup(this);
    m_pBlackRadio = new QRadioButton(QStringLiteral("执黑先行"), this);
    m_pWhiteRadio = new QRadioButton(QStringLiteral("执白后行"), this);
    m_pBlackRadio->setFocusPolicy(Qt::NoFocus);
    m_pWhiteRadio->setFocusPolicy(Qt::NoFocus);
    m_pWhiteRadio->setStyleSheet(radio);
    m_pBlackRadio->setStyleSheet(radio);
    m_pBlackRadio->click();
    m_pPiecesGroup->addButton(m_pBlackRadio, 0);
    m_pPiecesGroup->addButton(m_pWhiteRadio, 1);

    m_pFightModeLabel = new QLabel(QStringLiteral("请选择对战模式"), this);
    m_pFightModeLabel->setStyleSheet("QLabel{font-size: 16px;}");
    m_pFightModeGroup = new QButtonGroup(this);
    m_pComRadio = new QRadioButton(QStringLiteral("人机对战"), this);
    m_pNetRadio = new QRadioButton(QStringLiteral("联网对战"), this);
    m_pComRadio->setFocusPolicy(Qt::NoFocus);
    m_pNetRadio->setFocusPolicy(Qt::NoFocus);
    m_pComRadio->setStyleSheet(radio);
    m_pComRadio->click();
    m_pNetRadio->setStyleSheet(radio);
    m_pFightModeGroup->addButton(m_pComRadio, 0);
    m_pFightModeGroup->addButton(m_pNetRadio, 1);

    HideBigWidget();
    PacketProcessor::GetPacketProcessor()->SetSidebarWig(this);

    connect( m_pSideBtn, SIGNAL(clicked(bool)), this, SLOT(slot_ShowSideBar()) );
    connect( m_pPiecesGroup, SIGNAL(buttonClicked(int)), this, SLOT(slot_PiecesBtnClk(int)) );
    connect( m_pFightModeGroup, SIGNAL(buttonClicked(int)), this, SLOT(slot_FightModeBtnClk(int)) );
}

SideConfigBar::~SideConfigBar()
{
    if(m_pSideBtn)
        delete m_pSideBtn;
    if(m_pBackGrndMsc)
        delete m_pBackGrndMsc;
    if(m_pPiecesGroup)
        delete m_pPiecesGroup;
    if(m_pWhiteRadio)
        delete m_pWhiteRadio;
    if(m_pBlackRadio)
        delete m_pBlackRadio;
    if(m_pPiecesLabel)
        delete m_pPiecesLabel;
    if(m_pFightModeLabel)
        delete m_pFightModeLabel;
    if(m_pFightModeGroup)
        delete m_pFightModeGroup;
    if(m_pComRadio)
        delete m_pComRadio;
    if(m_pNetRadio)
        delete m_pNetRadio;
}

/*void SideConfigBar::focusOutEvent(QFocusEvent *event)
{
    if(m_isShow == true)
    {
        HideBigWidget();
        m_isShow = false;
        update();
    }
}*/

void SideConfigBar::SetRadiosEnable(bool active)
{
    m_pBlackRadio->setEnabled(active);
    m_pWhiteRadio->setEnabled(active);
    m_pComRadio->setEnabled(active);
    m_pNetRadio->setEnabled(active);
}

void SideConfigBar::paintEvent(QPaintEvent *event)
{
    if(m_isShow)
    {
        QPainter painter(this);
        painter.setBrush(QColor(130, 130, 130, 98));
        painter.drawRect(0, 3, width() - 13, height() - 3);
    }
}

void SideConfigBar::HideBigWidget()
{
    m_pWhiteRadio->hide();
    m_pBlackRadio->hide();
    m_pBackGrndMsc->hide();
    m_pPiecesLabel->hide();

    m_pFightModeLabel->hide();
    m_pComRadio->hide();
    m_pNetRadio->hide();

    setGeometry(0, 294, 13, 55);
    m_pSideBtn->setStyleSheet("QPushButton{background-image: url(:/Image/IMG/listshow.png);  border:none;}");
    m_pSideBtn->setGeometry(0, 0, 13, 60);
}

void SideConfigBar::ShowBigWidget()
{
    setGeometry(0, 60, 260, 540);
    m_pWhiteRadio->show();
    m_pBlackRadio->show();
    m_pBackGrndMsc->show();
    m_pPiecesLabel->show();

    m_pFightModeLabel->show();
    m_pComRadio->show();
    m_pNetRadio->show();

    m_pBackGrndMsc->setGeometry(30, 20, 100, 30);
    m_pPiecesLabel->setGeometry(30, 60, 150, 30);
    m_pBlackRadio->setGeometry(60, 100, 100, 30);
    m_pWhiteRadio->setGeometry(60, 140, 100, 30);

    m_pFightModeLabel->setGeometry(30, 180, 150, 30);
    m_pComRadio->setGeometry(60, 220, 100, 30);
    m_pNetRadio->setGeometry(60, 260, 100, 30);

    m_pSideBtn->setStyleSheet("QPushButton{background-image: url(:/Image/IMG/listhide.png); border:none;}");
    m_pSideBtn->setGeometry(247, 242, 13, 55);
}

void SideConfigBar::slot_ShowSideBar()
{
    if(m_isShow)
    {
        HideBigWidget();
    }
    else
    {
        ShowBigWidget();
    }
    setFocus();
    m_isShow = !m_isShow;
    update();
}

void SideConfigBar::slot_PiecesBtnClk(int id)
{
    if(id == 0)
        m_bPiecesColor = true;
    else
        m_bPiecesColor = false;
    int color = m_bPiecesColor ? -1 : 1;
    PacketProcessor::GetPacketProcessor()->SetPieceColor(color);
}

void SideConfigBar::slot_FightModeBtnClk(int id)
{
    if(id == 0)
        m_bFightOnline = false;
    else
        m_bFightOnline = true;
    GameType type = m_bFightOnline ? HUMAN : COMPUTER;
    PacketProcessor::GetPacketProcessor()->SetGameType(type);
}

void SideConfigBar::SetGameType(GameType type)
{
    m_bFightOnline = type == HUMAN ? true : false;
    if(m_bFightOnline)
        m_pNetRadio->setChecked(true);
    else {
        m_pComRadio->setChecked(true);
    }
}

void SideConfigBar::SetPiecesColor(int color)
{
    m_bPiecesColor = color == -1 ? true : false;
    if(m_bPiecesColor)
        m_pBlackRadio->setChecked(true);
    else
        m_pWhiteRadio->setChecked(true);
}
