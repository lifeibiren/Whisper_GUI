#include "peerlistdialog.h"
#include "ui_peerlistdialog.h"

PeerListDialog::PeerListDialog(std::shared_ptr<sml::service> service, QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::PeerListDialog),
    model_(new QStringListModel(this)),
    timer_(new QTimer(this)),
    service_(service)
{
    ui_->setupUi(this);
    ui_->peerListView->setModel(model_.get());
    connect(timer_.get(), SIGNAL(timeout()), this, SLOT(update_peer_list()));
    timer_->start(2000);
}

PeerListDialog::~PeerListDialog()
{
    delete ui_;
}

void PeerListDialog::update_peer_list()
{
    alive_peer_list_ = service_->peer_list();

    QStringList list;
    int newIndex = 0;
    if (alive_peer_list_->size())
    {
        int count = 0;
        for (auto it = alive_peer_list_->begin(); it != alive_peer_list_->end(); it ++)
        {
            QString str = QString::fromStdString(it->ip() + ":" + std::to_string(it->port()));
            list << str;
            if (str == selected)
            {
                newIndex = count;
            }
            count ++;
        }
        model_->setStringList(list);
        ui_->peerListView->selectionModel()->select(model_->index(newIndex), QItemSelectionModel::Select);
    }
}

void PeerListDialog::on_buttonBox_accepted()
{
    // find selected peer and emit a signal
    for (auto it = alive_peer_list_->begin(); it != alive_peer_list_->end(); it ++)
    {
        QString str = QString::fromStdString(it->ip() + ":" + std::to_string(it->port()));
        if (str == selected)
        {
            emit add_peer(*it);
        }
    }
}


void PeerListDialog::on_peerListView_clicked(const QModelIndex &index)
{
    selected = model_->data(index).toString();
}
