#include "chatcontextsingle.h"

#include "chatcontextgroup.h"

ChatContextSingle::ChatContextSingle(std::shared_ptr<sml::service> service, const std::string &id, const sml::address &addr, QObject *parent)
    : ChatContext(service, id, parent),
      addr_(addr)
{
    // control message
    service_->post(boost::make_shared<sml::add_stream>(addr, 0));
    // chat message
    service_->post(boost::make_shared<sml::add_stream>(addr, 1));
    // file data
    service_->post(boost::make_shared<sml::add_stream>(addr, 2));
    // group message
    service_->post(boost::make_shared<sml::add_stream>(addr, 3));
}

void ChatContextSingle::shutDown()
{
    service_->post(boost::make_shared<sml::del_peer>(addr_));
}

const sml::address &ChatContextSingle::addr() const
{
    return addr_;
}

void ChatContextSingle::feed(sml::recv_data *msg)
{
    if (msg->addr() != addr_)
    {
        //ignore
        return;
    }

    switch (msg->id())
    {
        case 0:
            cmd_ += msg->data();
            cmdHandler();
            break;
        case 1:
            appendPeerMessage(msg->data());
            break;
        case 2:
            appendFile(msg->data());
            break;
        default:
            break;
    }
    std::cout<<msg->addr().ip()<<":"<<msg->addr().port()<<":"<<msg->data()<<std::endl;
}

void ChatContextSingle::cmdHandler()
{
    while (1)
    {
        size_t sep = cmd_.find('\0');
        if (sep != std::string::npos)
        {
            std::string cmd = cmd_.substr(0, sep);
            cmd_ = cmd_.substr(sep + 1);
            YAML::Node node = YAML::Load(cmd);
            if (node["file"])
            {
                if (node["file"].as<std::string>() == "OK")
                {
                    startSendFile();
                }
                else
                {
                    state_ |= FileIncoming;
                    setFileLength(node["file"].as<size_t>());
                    emit fileIncoming();
                }
            }
            else if (node["group"])
            {
                YAML::Node group = node["group"];
                std::string groupId = group["id"].as<std::string>();
                emit newGroup(std::shared_ptr<ChatContext>(new ChatContextGroup(service_, groupId, addr_, parent())));
            }
        }
        else
        {
            break;
        }
    }
}

void ChatContextSingle::sendCmd(const std::string &cmd)
{
    service_->post(boost::make_shared<sml::send_data>(addr_, 0, cmd + '\0'));
}

//void ChatContextSingle::startReceivingFile(const QString &path)
//{
//    state_ |= PathSet;
//    recvFilePath_ = path;
//    YAML::Node node;
//    node["file"] = "OK";
//    sendCmd(YAML::Dump(node));
//}

//void ChatContextSingle::sendFile(const QString &path)
//{
//    sendFile_ = std::make_unique<QFile>(path);
//    if (sendFile_->open(QIODevice::ReadOnly))
//    {
//        YAML::Node node;
//        node["file"] = sendFile_->size();
//        sendCmd(YAML::Dump(node));
//    }
//    else
//    {
//        std::cerr<<"open file failed:"<<path.toStdString()<<std::endl;
//    }
//}

//void ChatContextSingle::appendFile(const std::string &bytes)
//{
//    recvFile_.append(bytes);
//    if (state_ == AllSet && recvFile_.size() == recvFileLength_)
//    {
//        QFile file(recvFilePath_);
//        file.open(QFile::WriteOnly);
//        file.write(recvFile_.c_str(), recvFile_.size());
//        state_ = NoneSet;
//    }
//}

//void ChatContextSingle::setFileLength(size_t length)
//{
//    state_ |= LengthSet;
//    recvFileLength_ = length;
//    // reserver length bytes room
//    recvFile_.reserve(length);
//}

void ChatContextSingle::startSendFile()
{
    QByteArray bytes = sendFile_->readAll();
    service_->post(boost::make_shared<sml::send_data>(addr_, 2, bytes.toStdString()));
    sendFile_ = nullptr;
}

void ChatContextSingle::appendPeerMessage(const std::string &msg)
{
    appendMessage(id_, QString::fromStdString(msg));
//    appendModelData(model_.get(), id_ + ":" + QString::fromStdString(msg));
//    emit newMessage(id_, id_, QString::fromStdString(msg));
}

void ChatContextSingle::appendMyMessage(const QString &msg)
{
    service_->post(boost::make_shared<sml::send_data>(addr_, 1, msg.toStdString()));
    appendMessage(myId_, msg);
}
