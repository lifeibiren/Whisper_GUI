#ifndef PEERLIST_H
#define PEERLIST_H

#include <memory>
#include <vector>

#include <QDialog>
#include <QMutex>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QStringListModel>
#include <QTimer>
#include <QThread>

#include "address.h"
#include "service.h"

namespace Ui {
class PeerList;
}

class PeerList : public QDialog
{
    Q_OBJECT

public:
    explicit PeerList(std::shared_ptr<sml::service> service_, QWidget *parent = 0);
    ~PeerList();

signals:
    void add_peer(const sml::address &addr);

private slots:
    void update_peer_list();

    void on_buttonBox_accepted();

private:
    void query_server();
    void receive();


    Ui::PeerList *ui_;
    std::unique_ptr<QStringListModel> model_;
//    QMutex mutex_;
//    QUdpSocket socket_;
    std::unique_ptr<QTimer> timer_;
//    std::unique_ptr<QThread> thread_;
    std::shared_ptr<sml::service> service_;
    boost::shared_ptr<std::vector<sml::address>> alive_peer_list_;
//    std::unique_ptr<std::vector<sml::address>> new_alive_peer_list_;
};

#endif // PEERLIST_H
