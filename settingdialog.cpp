#include "settingdialog.h"
#include "ui_settingdialog.h"

SettingDialog::SettingDialog(YAML::Node &conf, QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::SettingDialog),
    conf_(conf),
    trustedPeersTableModel_(new TrustedPeersTableModel(conf_["trusted_peers"]))
{
    ui_->setupUi(this);
    ui_->serverLineEdit->setText(conf_["server"].as<QString>());
    ui_->chatRecordLineEdit->setText(conf_["chat_record"].as<QString>());
    ui_->timeoutLineEdit->setText(conf_["timeout"].as<QString>());
    ui_->maxRetriesLineEdit->setText(conf_["max_retries"].as<QString>());
    ui_->privateKeyLineEdit->setText(conf_["private_key"].as<QString>());
    ui_->publicKeyLineEdit->setText(conf_["public_key"].as<QString>());

    ui_->trustedPeersTableView->setModel(trustedPeersTableModel_.get());
    ui_->trustedPeersTableView->showGrid();
    //ui_->trustedPeerstableView->resizeColumnsToContents();
    ui_->trustedPeersTableView->verticalHeader()->hide();
    //ui_->trustedPeerstableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui_->trustedPeersTableView->horizontalHeader()->setStretchLastSection(true);
    //ui_->trustedPeerstableView->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui_->trustedPeerstableView->setCornerButtonEnabled(false);
    //ui_->trustedPeerstableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui_->trustedPeersTableView->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *addAction = new QAction("Add", ui_->trustedPeersTableView);
    ui_->trustedPeersTableView->addAction(addAction);
    QAction *delAction = new QAction("delete", ui_->trustedPeersTableView);
    ui_->trustedPeersTableView->addAction(delAction);

    connect(addAction, &QAction::triggered, trustedPeersTableModel_.get(),
            [this](bool)
    {
        trustedPeersTableModel_->appendAnEmptyRow();
    });

    connect(delAction, &QAction::triggered, trustedPeersTableModel_.get(),
            [this](bool)
    {
        const QItemSelection selection = ui_->trustedPeersTableView->selectionModel()->selection();
        for (QItemSelection::const_iterator it = selection.begin(); it != selection.end(); it ++)
        {
            QModelIndexList list = it->indexes();
            for (QModelIndexList::const_iterator it2 = list.begin(); it2 != list.end(); it2 ++)
            {
                trustedPeersTableModel_->removeRow(it2->row());
            }
        }
    });
}

SettingDialog::~SettingDialog()
{
}

void SettingDialog::on_confirmButtonBox_accepted()
{
    conf_["server"] = ui_->serverLineEdit->text();
    conf_["chat_record"] = ui_->chatRecordLineEdit->text();
    conf_["timeout"] = ui_->timeoutLineEdit->text();
    conf_["max_retries"] = ui_->maxRetriesLineEdit->text();
    conf_["trusted_peers"] = trustedPeersTableModel_->toYAMLNode();
    emit config_change(conf_);
}
