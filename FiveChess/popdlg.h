#ifndef POPDLG_H
#define POPDLG_H

#include <QDialog>

namespace Ui {
class PopDlg;
}

///////////////////////////////
class PopDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PopDlg(QWidget *parent = 0);
    ~PopDlg();

    void ShowWidget(int status);
    void ShowWidget(QByteArray name, int status);
    void ViewWidget();

public slots:
    void slot_AcceptBtn();
    void slot_RefuseBtn();

protected:
    void paintEvent(QPaintEvent *event);

private:
    int m_status;
    Ui::PopDlg *ui;
};

#endif // POPDLG_H
