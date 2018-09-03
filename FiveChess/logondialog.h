#ifndef LOGONDIALOG_H
#define LOGONDIALOG_H

#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class LogOnDialog : public QDialog
{
    Q_OBJECT

public:
    enum LogonType{
        NONE,
        LOGON,
        RIGISTER
    };

private:
    LogOnDialog(QDialog *dlg = nullptr);
    ~LogOnDialog();

protected:
    void paintEvent(QPaintEvent *event);

public:
    void ShowWidget(LogonType type);
    static LogOnDialog* getLogOnDialog();
    inline bool IsLogon(){if(m_LogonType == LOGON) return true; return false;}
    void LogonSucces();

signals:
    void sig_connnetwork(int);

public slots:
    void slot_Register();
    void slot_Comfirm();
    void slot_Cancel();
    void slot_Hide();

public:
    LogonType   m_LogonType;
    QLabel    *m_pNameLabel;
    QLineEdit *m_pUserName;
    QLabel    *m_pPasswordLabel;
    QLineEdit *m_pPassword;
    QLineEdit *m_pConfirmPassword;
    QLabel    *m_pConfirmPasswordLabel;
    QPushButton *m_pCanselBtn;
    QPushButton *m_pOkBtn;
    QPushButton *m_pregistryBtn;
    QLabel      *m_pTipLabel;
    QPushButton *m_pCloseBtn;
    QLabel      *m_pLogonLabel;
};

#endif // LOGONDIALOG_H
