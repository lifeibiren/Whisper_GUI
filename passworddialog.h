#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class PasswordDialog;
}

class PasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordDialog(QWidget *parent = 0);
    ~PasswordDialog();

signals:
    void passwordEntered(const QString &passwd);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::PasswordDialog *ui;
};

#endif // PASSWORD_H
