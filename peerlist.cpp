#include "peerlist.h"
#include "ui_peerlist.h"

PeerList::PeerList(std::shared_ptr<sml::service> service, QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::PeerList),
    model_(new QStringListModel(this)),
    timer_(new QTimer(this)),
    service_(service)
{
    ui_->setupUi(this);
    ui_->peerListView->setModel(model_.get());
    connect(timer_.get(), SIGNAL(timeout()), this, SLOT(update_peer_list()));
    timer_->start(2000);
}

PeerList::~PeerList()
{
    delete ui_;
}

void PeerList::update_peer_list()
{
    alive_peer_list_ = service_->peer_list();

    QStringList list;

    if (alive_peer_list_->size())
    {
        for (auto it = alive_peer_list_->begin(); it != alive_peer_list_->end(); it ++)
        {
            list<<QString::fromStdString(it->ip() + ":" + std::to_string(it->port()));
        }
    }

    model_->setStringList(list);
}

void PeerList::on_buttonBox_accepted()
{
    // current selected item
    QModelIndex index = ui_->peerListView->currentIndex();
    if (index.row() != -1)
    {
        emit add_peer((*alive_peer_list_)[index.row()]);
    }
}

