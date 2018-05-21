#include "chatcontext.h"

ChatContext::ChatContext(std::shared_ptr<sml::service> service, const std::string &id, QObject *parent)
    : QObject(parent),
      service_(service),
      myId_(QString::fromStdString(service_->conf().id())),
      id_(QString::fromStdString(id)),
      model_(std::make_unique<QStringListModel>()),
      state_(NoneSet),
      recvFileLength_(0)
{
}

//ChatContext::ChatContext(std::shared_ptr<sml::service> service, const std::string &id, const sml::address &addr, QObject *parent)
//    : QObject(parent),
//      service_(service),
//      myId_(QString::fromStdString(service_->conf().id())),
//      id_(QString::fromStdString(id)),
//      addr_(addr),
//      model_(std::make_unique<QStringListModel>()),
//      state_(NoneSet),
//      recvFileLength_(0)
//{
//    // control message
//    service_->post(boost::make_shared<sml::add_stream>(addr, 0));
//    // chat message
//    service_->post(boost::make_shared<sml::add_stream>(addr, 1));
//    // file data
//    service_->post(boost::make_shared<sml::add_stream>(addr, 2));
//    // group message
//    service_->post(boost::make_shared<sml::add_stream>(addr, 3));
//}

const QString &ChatContext::id() const
{
    return id_;
}

void ChatContext::appendMessage(const QString &from, const QString &msg)
{
    appendModelData(model_.get(), from + ":" + msg);
    emit newMessage(id_, from, msg);
}

//void ChatContext::appendPeerMessage(const std::string &msg)
//{
//    appendMessage(id_, QString::fromStdString(msg));
////    appendModelData(model_.get(), id_ + ":" + QString::fromStdString(msg));
////    emit newMessage(id_, id_, QString::fromStdString(msg));
//}

//void ChatContext::appendMyMessage(const QString &msg)
//{
//    service_->post(boost::make_shared<sml::send_data>(addr_, 1, msg.toStdString()));
//    appendMessage(myId_, msg);
//}

QStringListModel *ChatContext::model()
{
    return model_.get();
}

void ChatContext::sendYAMLNode(const sml::address &addr, sml::datagram::id_type id, const YAML::Node &node)
{
    const std::string dump = YAML::Dump(node) + '\0';
    service_->post(boost::make_shared<sml::send_data>(addr, id, dump));
}

YAML::Node ChatContext::extractYAMLNode(std::string &str)
{
    size_t sep = str.find('\0');
    if (sep != std::string::npos)
    {
        std::string firstPart = str.substr(0, sep);
        str = str.substr(sep + 1);
        return YAML::Load(firstPart);
    }
    else
    {
        return YAML::Node();
    }
}

//const sml::address &ChatContext::addr() const
//{
//    return addr_;
//}

bool ChatContext::fileToReceive() const
{
    return state_ & FileIncoming;
}

//void ChatContext::feed(sml::recv_data *msg)
//{
//    if (msg->addr() != addr_)
//    {
//        //ignore
//        return;
//    }

//    switch (msg->id())
//    {
//        case 0:
//            cmd_ += msg->data();
//            cmdHandler();
//            break;
//        case 1:
//            appendPeerMessage(msg->data());
//            break;
//        case 2:
//            appendFile(msg->data());
//            break;
//        default:
//            break;
//    }
//    std::cout<<msg->addr().ip()<<":"<<msg->addr().port()<<":"<<msg->data()<<std::endl;
//}

//void ChatContext::cmdHandler()
//{
//    while (1)
//    {
//        size_t sep = cmd_.find('\0');
//        if (sep != std::string::npos)
//        {
//            std::string cmd = cmd_.substr(0, sep);
//            cmd_ = cmd_.substr(sep + 1);
//            YAML::Node node = YAML::Load(cmd);
//            if (node["file"])
//            {
//                if (node["file"].as<std::string>() == "OK")
//                {
//                    startSendFile();
//                }
//                else
//                {
//                    state_ |= FileIncoming;
//                    setFileLength(node["file"].as<size_t>());
//                    emit fileIncoming();
//                }
//            }
//            else if (node["group"])
//            {
//                YAML::Node group = node["group"];
//            }
//        }
//        else
//        {
//            break;
//        }
//    }
//}

//void ChatContext::sendCmd(const std::string &cmd)
//{
//    service_->post(boost::make_shared<sml::send_data>(addr_, 0, cmd + '\0'));
//}

void ChatContext::startReceivingFile(const QString &path)
{
    state_ |= PathSet;
    recvFilePath_ = path;
    YAML::Node node;
    node["file"] = "OK";
    sendCmd(YAML::Dump(node));
}

void ChatContext::sendFile(const QString &path)
{
    sendFile_ = std::make_unique<QFile>(path);
    if (sendFile_->open(QIODevice::ReadOnly))
    {
        YAML::Node node;
        node["file"] = sendFile_->size();
        sendCmd(YAML::Dump(node));
    }
    else
    {
        std::cerr<<"open file failed:"<<path.toStdString()<<std::endl;
    }
}

void ChatContext::appendFile(const std::string &bytes)
{
    recvFile_.append(bytes);
    if (state_ == AllSet && recvFile_.size() == recvFileLength_)
    {
        QFile file(recvFilePath_);
        file.open(QFile::WriteOnly);
        file.write(recvFile_.c_str(), recvFile_.size());
        state_ = NoneSet;
    }
}

void ChatContext::setFileLength(size_t length)
{
    state_ |= LengthSet;
    recvFileLength_ = length;
    // reserver length bytes room
    recvFile_.reserve(length);
}

//void ChatContext::startSendFile()
//{
//    QByteArray bytes = sendFile_->readAll();
//    service_->post(boost::make_shared<sml::send_data>(addr_, 2, bytes.toStdString()));
//    sendFile_ = nullptr;
//}

