#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <fstream>

#include <QMainWindow>
#include <QTimer>

#include <yaml-cpp/yaml.h>

#include "settingdialog.h"
#include "peerlist.h"
#include "service.h"
#include "chatcontext.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //explicit MainWindow(QWidget *parent = 0);
    explicit MainWindow(const std::string &path, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void config_update(YAML::Node &config);

private slots:
    void on_timeout();

    void on_add_peer(const sml::address &addr);

    void on_settingsButton_clicked();

    void on_addButton_clicked();

    void on_sendButton_clicked();

    void on_peerListView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui_;
    std::unique_ptr<SettingDialog> settingDialog_;
    std::unique_ptr<PeerList> peerList_;
    std::unique_ptr<QStringListModel> peerListModel_;
    std::unique_ptr<QTimer> timer_;
    const std::string configPath_;
    YAML::Node config_;

    boost::asio::io_service ioContext_;
    std::shared_ptr<sml::service> service_;
    std::unique_ptr<boost::thread> thread_;
    std::vector<ChatContext> chat_;
    ChatContext *current_chat_; //point to current focus
};

#endif // MAINWINDOW_H
