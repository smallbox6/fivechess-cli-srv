#include "logondialog.h"
#include "packetprocessor.h"

#include <QWidget>
#include <QDebug>

LogOnDialog::LogOnDialog(QDialog *dlg) : QDialog(dlg)
{
    const char* buttonstyle = "QPushButton {\
            color: white;\
            background-color: rgba(33, 82, 244, 95);\
            border: 1px solid #333333;\
            border-radius: 6px;\
            }"
            "QPushButton:hover {\
            background-color: rgb(33, 82, 244);\
            border: 1px solid #333333;\
            border-radius: 6px;\
            }";

            const char* reg_buttonstyle = "QPushButton {\
                    font-size: 16px;\
                    text-decoration: underline;\
                    color: rgb(0, 255, 127);\
                    background-color: rgba(52, 52, 52, 85);\
                    border: none;\
                    }"
                    "QPushButton:hover {\
                    color: rgb(152, 251, 152);\
                    font-size: 16px;\
                    text-decoration: underline;\
                    background-color: rgba(52, 52, 52, 85);\
                    border: none;\
                    }";

    const char* lablestyle = "QLabel{color: white;}";

    m_LogonType = NONE;

    setModal(true);
    setWindowFlags(Qt::FramelessWindowHint);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(52, 52, 52));
    setPalette(pal);
    setWindowOpacity(0.85);

    setGeometry(MainWindow::getMainWindow()->pos().x() + (MainWindow::getMainWindow()->width() - 480) / 2,
                MainWindow::getMainWindow()->pos().y() + (MainWindow::getMainWindow()->height() - 300) / 2,
                500, 400);

    m_pNameLabel = new QLabel(QStringLiteral("输入用户名："), this);
    m_pNameLabel->setStyleSheet(lablestyle);
    m_pUserName = new QLineEdit(this);
    m_pPasswordLabel = new QLabel(QString(QStringLiteral("输入密码：")), this);
    m_pPasswordLabel->setStyleSheet(lablestyle);
    m_pPassword = new QLineEdit(this);
    m_pConfirmPasswordLabel = new QLabel(QStringLiteral("输入确认密码："), this);
    m_pConfirmPasswordLabel->setStyleSheet(lablestyle);
    m_pConfirmPassword = new QLineEdit(this);
    m_pCanselBtn = new QPushButton(QString(QStringLiteral("取消")), this);
    m_pCanselBtn->setStyleSheet(buttonstyle);
    m_pOkBtn = new QPushButton(QString(QStringLiteral("确认")), this);
    m_pOkBtn->setStyleSheet(buttonstyle);
    m_pregistryBtn = new QPushButton(QString(QStringLiteral("注册账号")), this);
    m_pregistryBtn->setStyleSheet(reg_buttonstyle);
    m_pTipLabel = new QLabel(QString(QStringLiteral("还未注册账号？")), this);
    m_pTipLabel->setStyleSheet(lablestyle);
    m_pLogonLabel = new QLabel(this);
    m_pLogonLabel->setWordWrap(true);
    m_pLogonLabel->setAlignment(Qt::AlignTop);
    m_pLogonLabel->adjustSize();
    m_pLogonLabel->setFixedSize(200, 200);
    m_pCloseBtn = new QPushButton(this);
    m_pCloseBtn->setGeometry(468, 0, 32, 32);
    m_pCloseBtn->setStyleSheet("QPushButton{background-image: url(:/Image/IMG/logon_closeImg.png); border:none;}"
                               "QPushButton:hover{background-image: url(:/Image/IMG/logon_closeImg_hover.png); border:none;}");

    ShowWidget(m_LogonType);

    connect(m_pregistryBtn, SIGNAL(clicked(bool)), this, SLOT(slot_Register()));
    connect(m_pCanselBtn, SIGNAL(clicked(bool)), this, SLOT(slot_Cancel()));
    connect(m_pCloseBtn, SIGNAL(clicked(bool)), this, SLOT(slot_Hide()));
    connect(m_pOkBtn, SIGNAL(clicked(bool)), this, SLOT(slot_Comfirm()));
}

LogOnDialog::~LogOnDialog()
{
    if(m_pNameLabel) delete m_pNameLabel;
    if(m_pUserName) delete m_pUserName;
    if(m_pPasswordLabel) delete m_pPasswordLabel;
    if(m_pPassword) delete m_pPassword;
    if(m_pConfirmPassword) delete m_pConfirmPassword;
    if(m_pConfirmPasswordLabel) delete m_pConfirmPasswordLabel;
    if(m_pCanselBtn) delete m_pCanselBtn;
    if(m_pOkBtn) delete m_pOkBtn;
    if(m_pregistryBtn) delete m_pregistryBtn;
    if(m_pTipLabel) delete m_pTipLabel;
    if(m_pNameLabel) delete m_pNameLabel;
    if(m_pCloseBtn) delete m_pCloseBtn;
}

LogOnDialog* LogOnDialog::getLogOnDialog()
{
    static LogOnDialog *dlg;
    if(dlg)
        return dlg;
    else
        dlg = new LogOnDialog();
    return dlg;
}

void LogOnDialog::ShowWidget(LogonType type)
{
    if(m_LogonType == RIGISTER)
    {
        m_pNameLabel->setGeometry(70, 90, 90, 30);
        m_pUserName->setGeometry(175, 90, 260, 30);
        m_pPasswordLabel->setGeometry(70, 150, 90, 30);
        m_pPassword->setGeometry(175, 150, 260, 30);
        m_pConfirmPasswordLabel->setGeometry(70, 210, 100, 30);
        m_pConfirmPassword->setGeometry(175, 210, 260, 30);
        m_pCanselBtn->setGeometry(220, 270, 80, 30);
        m_pOkBtn->setGeometry(320, 270, 80, 30);
        m_pOkBtn->setText(QStringLiteral("注册"));
        m_pregistryBtn->hide();
        m_pTipLabel->hide();
        m_pConfirmPasswordLabel->show();
        m_pConfirmPassword->show();
    }
    else if(m_LogonType == NONE)
    {
        m_pNameLabel->setGeometry(70, 100, 90, 30);
        m_pUserName->setGeometry(175, 100, 260, 30);
        m_pPasswordLabel->setGeometry(70, 160, 90, 30);
        m_pPassword->setGeometry(175, 160, 260, 30);
        m_pCanselBtn->setGeometry(220, 250, 80, 30);
        m_pOkBtn->setGeometry(320, 250, 80, 30);
        m_pOkBtn->setText(QStringLiteral("登陆"));
        m_pTipLabel->setGeometry(150, 310, 105, 30);
        m_pregistryBtn->setGeometry(240, 310, 100, 30);
        m_pregistryBtn->show();
        m_pTipLabel->show();
        m_pConfirmPasswordLabel->hide();
        m_pConfirmPassword->hide();
    }
    else if(m_LogonType == LOGON)
    {
        m_pNameLabel->hide();
        m_pUserName->hide();
        m_pPasswordLabel->hide();
        m_pPassword->hide();
        m_pCanselBtn->hide();
        m_pOkBtn->hide();
        m_pTipLabel->hide();
        m_pregistryBtn->hide();
        m_pregistryBtn->hide();
        m_pTipLabel->hide();
        m_pConfirmPasswordLabel->hide();
        m_pConfirmPassword->hide();
        m_pLogonLabel->setStyleSheet("QLabel{\
                                     font-size: 25px;\
                                     color: white;\
                                     }");
        m_pLogonLabel->setText(m_pUserName->text() + QStringLiteral(" 欢迎回来！"));
        m_pLogonLabel->setGeometry(150, height() / 2, 200, 30);
    }
}

void LogOnDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap pic(":/Image/IMG/logo.png");
    painter.drawPixmap(200, 25, 100, 50, pic);
}

void LogOnDialog::LogonSucces()
{
    m_LogonType = LOGON;
    ShowWidget(m_LogonType);
}

void LogOnDialog::slot_Comfirm()
{
    char msg[128];
    QString name, password, compassword;
    memset(msg, 0, sizeof(msg));
    if(m_LogonType == NONE)
    {
        name = m_pUserName->text();
        password = m_pPassword->text();
        if(name.size() == 0)
        {
            PacketProcessor::GetPacketProcessor()->AddNoticeMsg(QStringLiteral("用户名不能为空！"));
            return;
        }
        if(password.size() == 0 || password.size() < 4 || password.size() > 16)
        {
            PacketProcessor::GetPacketProcessor()->AddNoticeMsg(QStringLiteral("密码要在4-16个字节之间！"));
            return;
        }
        if(PacketProcessor::GetPacketProcessor() == nullptr)
            return;
        PacketProcessor::GetPacketProcessor()->AddNoticeMsg(QStringLiteral("已发送登录请求！"));
        PacketProcessor::GetPacketProcessor()->Logon(name, password);
    }
    else if(m_LogonType == RIGISTER)
    {
        name = m_pUserName->text();
        password = m_pPassword->text();
        compassword = m_pConfirmPassword->text();
        if(name.size() == 0)
        {
            PacketProcessor::GetPacketProcessor()->AddNoticeMsg(QStringLiteral("用户名不能为空！"));
            return;
        }
        if(password.size() == 0 || password.size() < 4 || password.size() > 16)
        {
            PacketProcessor::GetPacketProcessor()->AddNoticeMsg(QStringLiteral("密码要在4-16个字节之间！"));
            return;
        }
        if(password != compassword)
        {
            PacketProcessor::GetPacketProcessor()->AddNoticeMsg(QStringLiteral("密码输入不一致，请重新输入！"));
            return;
        }
        if(PacketProcessor::GetPacketProcessor() == nullptr)
            return;
        PacketProcessor::GetPacketProcessor()->AddNoticeMsg(QStringLiteral("已发送注册请求！"));
        PacketProcessor::GetPacketProcessor()->Rigester(name, password);
    }
}

void LogOnDialog::slot_Register()
{
    m_LogonType = RIGISTER;
    ShowWidget(m_LogonType);
    this->show();
}

void LogOnDialog::slot_Cancel()
{
    if(m_LogonType == RIGISTER)
    {
        m_LogonType = NONE;
        ShowWidget(m_LogonType);
        this->show();
    }
    else
        this->hide();
}

void LogOnDialog::slot_Hide()
{
    this->hide();
}
