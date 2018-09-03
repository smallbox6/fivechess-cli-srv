#ifndef EXITDIALOG_H
#define EXITDIALOG_H

#include <QDialog>

namespace Ui {
class ExitDialog;
}

class ExitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExitDialog(QString tip, QWidget *parent = 0);
    ~ExitDialog();

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void on_CloseBtn_clicked();

    void on_OkBtn_clicked();

    void on_CancelBtn_clicked();

private:
    Ui::ExitDialog *ui;
};

#endif // EXITDIALOG_H
