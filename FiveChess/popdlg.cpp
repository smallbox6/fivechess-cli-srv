#include "popdlg.h"
#include "ui_popdlg.h"
#include "packetprocessor.h"

#include <QPainter>


///////////////////////////////////////////

const int hideall = 0;
const int sendwait = 1;
const int invited = 2;

PopDlg::PopDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PopDlg)
{
    m_status = 0;
    setWindowFlags(Qt::FramelessWindowHint);
    ui->setupUi(this);
    ui->tiplabel->setFont(QFont(QStringLiteral("黑体"), 50, 75));
    ui->tiplabel->setGeometry(100, 100, 260, 50);
    ui->tiplabel->setText("");
    ui->tiplabel->setWordWrap(true);
    ui->tiplabel->setAlignment(Qt::AlignTop);
    ui->tiplabel->adjustSize();
    ui->tiplabel->setFixedSize(200, 200);


    hide();

    PacketProcessor::GetPacketProcessor()->SetPopDlg(this);

    connect(ui->refusebtn, SIGNAL(clicked(bool)), this, SLOT(slot_RefuseBtn()));
    connect(ui->acceptbtn, SIGNAL(clicked(bool)), this, SLOT(slot_AcceptBtn()));
}

PopDlg::~PopDlg()
{
    delete ui;
}

void PopDlg::paintEvent(QPaintEvent *event)
{
    QPainter pt(this);
    pt.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    pt.setBrush(QColor(52, 52, 52, 85));
    pt.drawRoundRect(QRect(20, 20, 360, 260), 40, 40);
    if(m_status == sendwait)
        pt.drawPixmap(100, 70, 200, 100, QPixmap(":/Image/IMG/match.png"));
}

void PopDlg::ShowWidget(int status)
{
    m_status = status;
    ViewWidget();
    update();
}

void PopDlg::ShowWidget(QByteArray name, int status)
{
    m_status = status;
    ui->tiplabel->setText(name + QStringLiteral(", 邀请你一起游戏！"));
    ViewWidget();
    update();
}

void PopDlg::ViewWidget()
{
    if(m_status == sendwait)
    {
        ui->refusebtn->hide();
        ui->tiplabel->hide();
        ui->acceptbtn->setText(QStringLiteral("取消"));
        show();
        return;
    }
    else if(m_status == invited)
    {
        ui->refusebtn->show();
        ui->tiplabel->show();
        ui->acceptbtn->setText(QStringLiteral("接受"));
        show();
        return;
    }
    hide();
}

void PopDlg::slot_AcceptBtn()
{
    if(m_status == sendwait)
    {
    }
    else if(m_status == invited)
    {
        PacketProcessor::GetPacketProcessor()->SendFightY();
    }
    hide();
}

void PopDlg::slot_RefuseBtn()
{
    PacketProcessor::GetPacketProcessor()->SendFightN();
    hide();
}
