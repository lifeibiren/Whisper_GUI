#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <memory>

#include "yamlcpp-qt.h"
#include "trustedpeerstablemodel.h"

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(YAML::Node &conf, QWidget *parent = 0);
    ~SettingDialog();

signals:
    void config_change(YAML::Node &conf);

private slots:
    void on_confirmButtonBox_accepted();

private:
    std::unique_ptr<Ui::SettingDialog> ui_;
    YAML::Node conf_;
    std::unique_ptr<TrustedPeersTableModel> trustedPeersTableModel_;
};

#endif // SETTINGDIALOG_H
