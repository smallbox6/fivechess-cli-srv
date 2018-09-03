#include "mainwindow.h"
#include "TopWidget.h"
#include "logondialog.h"
#include "packetprocessor.h"

#include <QDebug>
#include <QPainter>
#include <QFile>
#include <QFontMetrics>
#include <QApplication>
#include <QDesktopWidget>


PortraitWidget::PortraitWidget(QWidget *parent):QWidget(parent)
{
    m_blogOn = false;
}

void PortraitWidget::paintEvent(QPaintEvent *event)
{
    QPixmap pixmapa(":/Image/IMG/portrait.jpg");
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    path.addEllipse(0, 0, 60, 60);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, 60, 60, pixmapa);
    QPainter painter1(this);
    painter1.drawPixmap(0, 0, 60, 60, pixmap);
    painter1.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter1.setBrush(QColor(5, 5, 5, 95));
    painter1.drawEllipse(0,0,60,60);
    painter1.setPen(QColor(255,0,0));
    if(m_blogOn == false)
        painter1.drawText(QPoint(17, 35), QStringLiteral("登陆"));
}

void PortraitWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit sig_Clicked();
    }
}

void PortraitWidget::LogOn(bool blogon)
{
    m_blogOn = blogon;
    update();
}

//////////////////////////////////////////
TopWidget::TopWidget(QWidget *parent) :
    QWidget(parent)
{
    m_isLogon = false;
    m_movePos = QPoint(0, 0);

    setFocusPolicy(Qt::ClickFocus);

    m_pCloseBtn = new QPushButton(this);
    m_pCloseBtn->setStyleSheet("QPushButton{background-image: url(:/Image/IMG/closeImg.png); border:none;}"
                               "QPushButton:hover{background-image: url(:/Image/IMG/closeImg_hover.png); border:none;}");
    connect( m_pCloseBtn, SIGNAL(clicked()), this, SLOT(slot_CloseWindow()) );

    m_pMinBtn = new QPushButton(this);
    m_pMinBtn->setStyleSheet("QPushButton{background-image: url(:/Image/IMG/minImg.png); border:none;}"
                             "QPushButton:hover{background-image: url(:/Image/IMG/minImg_hover.png); border:none;}");
    connect( m_pMinBtn, SIGNAL(clicked()), this, SLOT(slot_MinWindow()) );

    m_pPortaitBtn = new PortraitWidget(this);
    connect( m_pPortaitBtn, SIGNAL(sig_Clicked()), this, SLOT(slot_ShowWindow()) );

    PacketProcessor::GetPacketProcessor()->SetTopWig(this);
}

TopWidget::~TopWidget()
{
    delete m_pCloseBtn;
}

void TopWidget::LogOn(bool blogon)
{
    m_isLogon = blogon;
    m_pPortaitBtn->LogOn(blogon);
}

void TopWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(QColor(130, 130, 130, 96));
    painter.drawRect(0, 0, width(), height());
    QPixmap pic(":/Image/IMG/logo.png");
    painter.drawPixmap(70, 10, 100, 50, pic);
}

void TopWidget::mouseMoveEvent(QMouseEvent*event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QRect desktopRc = QApplication::desktop()->availableGeometry();
        QPoint curPoint = event->globalPos() - m_movePos;
        if (event->globalY() > desktopRc.height())
        {
            curPoint.setY(desktopRc.height() - m_movePos.y());
        }
        MainWindow::getMainWindow()->move(curPoint);
    }
    QWidget::mouseMoveEvent(event);
}

void TopWidget::mousePressEvent(QMouseEvent*event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_movePos = QCursor::pos() - MainWindow::getMainWindow()->pos();
    }
    QWidget::mousePressEvent(event);
}

void TopWidget::slot_CloseWindow()
{
    emit sig_CloseWindow(true);
}

void TopWidget::slot_MinWindow()
{
    emit sig_MinWindow();
}

void TopWidget::slot_ShowWindow()
{
    if(m_isLogon)
       LogOnDialog::getLogOnDialog()->ShowWidget(LogOnDialog::LOGON);
    else
       LogOnDialog::getLogOnDialog()->ShowWidget(LogOnDialog::RIGISTER);
    LogOnDialog::getLogOnDialog()->show();
}

void TopWidget::SetWindowsSizeAndPos(int width, int height)
{
    setGeometry(0, 0, width, height);
    m_pCloseBtn->setGeometry(width - 32, 0, 32, 32);
    m_pMinBtn->setGeometry(width - 70, 0, 32, 32);
    m_pPortaitBtn->setGeometry(5, 2, 64, 64);
}
