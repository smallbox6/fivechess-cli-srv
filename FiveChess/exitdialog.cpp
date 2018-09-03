#include "exitdialog.h"
#include "ui_exitdialog.h"

#include <QPainter>

ExitDialog::ExitDialog(QString tip, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExitDialog)
{   
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowOpacity(0.8);
    ui->setupUi(this);
    ui->label->setText(tip);

    ui->CancelBtn->setStyleSheet("QPushButton {\
                                   color: white;\
                                   background-color: rgba(255, 69, 0, 90)\
                                   }\
                                  QPushButton:hover {\
                                   background-color: rgba(255, 0, 0, 98);\
                                  }");
    ui->OkBtn->setStyleSheet("QPushButton {\
                                   color: white;\
                                   background-color: rgba(60, 179, 113, 98)\
                                   }\
                                 QPushButton:hover {\
                                   background-color: rgba(46, 139, 87, 98);\
                                 }");
    ui->CloseBtn->setStyleSheet("QPushButton {\
                                   width: 28;\
                                   height: 28;\
                                   border: none;\
                                   background-image: url(:/Image/IMG/dlgclosenormal.png);\
                                   }\
                                 QPushButton:hover {\
                                   background-image: url(:/Image/IMG/dlgclosehover.png);\
                                 }");
}

ExitDialog::~ExitDialog()
{
    delete ui;
}

void ExitDialog::paintEvent(QPaintEvent *event)
{
    QPainter pt(this);
    pt.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    pt.setBrush(QColor(52, 52, 52, 97));
    pt.drawRect(QRect(0, 0, width(), height()));
}

void ExitDialog::on_CloseBtn_clicked()
{
    reject();
}

void ExitDialog::on_OkBtn_clicked()
{
    accept();
}

void ExitDialog::on_CancelBtn_clicked()
{
    reject();
}
