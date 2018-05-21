#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QModelIndexList>

MainWindow::MainWindow(const std::string &path, QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow),
    passwordDialog_(new PasswordDialog(this)),
    fileBrowser_(new FileBrowser(this)),
    peerListModel_(new QStringListModel),
    timer_(new QTimer(this)),
    configPath_(path),
    config_(YAML::LoadFile(path)),
    current_chat_(nullptr)
{
    ui_->setupUi(this);
    ui_->peerListView->setModel(peerListModel_.get());
    ui_->receiveFileButton->setDisabled(true);

    connect(timer_.get(), SIGNAL(timeout()), this, SLOT(on_timeout()));
    timer_->start(1000);

    service_ = std::make_shared<sml::service>(ioContext_, sml::configuration(config_));
    thread_ = std::make_unique<boost::thread>(bind(&boost::asio::io_context::run, &ioContext_));

    peerListDialog_ = std::make_unique<PeerListDialog>(service_, this);
    connect(peerListDialog_.get(), &PeerListDialog::add_peer, this, &MainWindow::on_peerList_confirmed);
    connect(fileBrowser_.get(), &FileBrowser::accepted, this, &MainWindow::on_fileBrowser_confirmed);

    chatRecord_ = std::make_unique<ChatRecord>(config_["chat_record"].as<QString>(), this);
    connect(passwordDialog_.get(), &PasswordDialog::passwordEntered, chatRecord_.get(), &ChatRecord::setPassword);

    ui_->peerListView->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *groupAction = new QAction(tr("Group"), ui_->peerListView);
    ui_->peerListView->addAction(groupAction);
    connect(groupAction, &QAction::triggered, this, [this](bool)
    {
        const QItemSelection selection = ui_->peerListView->selectionModel()->selection();
        std::list<std::shared_ptr<ChatContext>> group;
        QString name(tr("Group:"));
        name += QString::fromStdString(service_->conf().id());
        for (QItemSelection::const_iterator it = selection.begin(); it != selection.end(); it ++)
        {
            QModelIndexList list = it->indexes();
            for (QModelIndexList::const_iterator it2 = list.begin(); it2 != list.end(); it2 ++)
            {
                group.push_back(chat_[it2->row()]);
                name += ',' + chat_[it2->row()]->id();
            }
        }
        on_appendToPeerListView(std::make_shared<ChatContextGroupLeader>(service_, name.toStdString(), group));
    });

    passwordDialog_->show();
}

MainWindow::~MainWindow()
{

    ioContext_.stop();
    //thread_->interrupt();
    thread_->join();
    delete ui_;
}

void MainWindow::config_update(YAML::Node &config)
{
    config_ = config;
    std::ofstream fout(configPath_);
    fout << config_;
}

void MainWindow::on_appendToPeerListView(std::shared_ptr<ChatContext> chatContext)
{
    // append to peer list
    int tail = peerListModel_->rowCount();
    peerListModel_->insertRow(tail);
    peerListModel_->setData(peerListModel_->index(tail), chatContext->id());

    // select tail
    ui_->peerListView->selectionModel()->select(peerListModel_->index(tail, 0), QItemSelectionModel::Select);

    // set current model
    ui_->textListView->setModel(chatContext->model());

    // add new chatcontext
    chat_.push_back(chatContext);

    // update current_chat_
    current_chat_ = chat_.rbegin()->get();

    // connect signal
    connect(chatContext.get(), &ChatContext::fileIncoming, this, &MainWindow::refresh);
    connect(chatContext.get(), &ChatContext::newMessage, chatRecord_.get(), &ChatRecord::appendMessage);
    connect(chatContext.get(), &ChatContext::newGroup, this, &MainWindow::on_appendToPeerListView);
}

void MainWindow::on_timeout()
{
    boost::shared_ptr<sml::message> msg;
    while ((msg = service_->query()))
    {
        switch (msg->type())
        {
            case sml::message::msg_type::new_peer:
            {
                sml::new_peer *new_peer_msg = dynamic_cast<sml::new_peer *>(msg.get());
                if (new_peer_msg)
                {
                    std::cout<<new_peer_msg->addr().ip()<<":"<<new_peer_msg->addr().port()<<std::endl;

                    std::shared_ptr<ChatContext> chatContext = std::make_shared<ChatContextSingle>(service_, new_peer_msg->id(), new_peer_msg->addr());

                    on_appendToPeerListView(chatContext);
                }
                else
                {
                    throw std::runtime_error("dynamic cast failed");
                }
                break;
            }
            case sml::message::msg_type::new_stream:
                //unhandled
                break;
            case sml::message::msg_type::recv_data:
            {
                sml::recv_data *recv_data_msg = dynamic_cast<sml::recv_data *>(msg.get());
                if (recv_data_msg)
                {
                    for (int i = 0; i < (int)chat_.size(); i ++)
                    {
                        chat_[i]->feed(recv_data_msg);
                    }
                }
                else
                {
                    throw std::runtime_error("dynamic cast failed");
                }
                break;
            }
            default:
                std::cout<<"unhandled message received"<<std::endl;
                break;
        }
    }

}

void MainWindow::on_peerList_confirmed(const sml::address &addr)
{
    std::cout<<addr.ip()<<' '<<(int)addr.port()<<std::endl;
    service_->post(boost::make_shared<sml::add_peer>(addr));
}

void MainWindow::refresh()
{
    if (current_chat_)
    {
        ui_->receiveFileButton->setEnabled(current_chat_->fileToReceive());
    }
}

void MainWindow::on_fileBrowser_confirmed()
{
    if (!current_chat_)
    {
        return;
    }
    switch (state_)
    {
        case FileBrowserState::save:
        {
            current_chat_->startReceivingFile(fileBrowser_->getPath());
            break;
        }
        case FileBrowserState::send:
        {
            current_chat_->sendFile(fileBrowser_->getPath());
            break;
        }
        default:
            break;
    }


}

void MainWindow::on_settingsButton_clicked()
{
    settingDialog_ = std::make_unique<SettingDialog>(config_, this);
    connect(settingDialog_.get(), &SettingDialog::config_change, this, &MainWindow::config_update);
    settingDialog_->show();
}

void MainWindow::on_addButton_clicked()
{
    peerListDialog_->show();
}

void MainWindow::on_sendButton_clicked()
{
    if (current_chat_)
    {
        current_chat_->appendMyMessage(ui_->textEdit->toPlainText());
    }
}

void MainWindow::on_peerListView_clicked(const QModelIndex &index)
{
    std::cout<<"moved"<<std::endl;
    if (index.isValid())
    {
        // only take the first row
        int row = index.row();
        if (row < (int)chat_.size())
        {
            current_chat_ = chat_[row].get();
        }
        else if (chat_.size())
        {
            current_chat_ = chat_[0].get();
        }
        else
        {
            current_chat_ = nullptr;
        }
        if (current_chat_)
        {
            ui_->textListView->setModel(current_chat_->model());
        }
    }
    else
    {
        current_chat_ = nullptr;
    }

    if (current_chat_)
    {
        refresh();
    }
}

void MainWindow::on_sendFileButton_clicked()
{
    state_ = FileBrowserState::send;
    fileBrowser_->show();
}

void MainWindow::on_receiveFileButton_clicked()
{
    state_ = FileBrowserState::save;
    fileBrowser_->show();
}

void MainWindow::on_chatRecordButton_clicked()
{
    chatRecord_->show();
}

