#include "chatcontext.h"

ChatContext::ChatContext(std::shared_ptr<sml::service> service, const std::string &id, const sml::address &addr, QObject *parent)
    : QObject(parent),
      service_(service),
      myId_(QString::fromStdString(service_->conf().id())),
      id_(QString::fromStdString(id)),
      addr_(addr),
      model_(std::make_unique<QStringListModel>()),
      state_(NoneSet),
      file_length_(0)
{
}

const QString &ChatContext::id() const
{
    return id_;
}

void ChatContext::appendPeerMessage(const std::string &msg)
{
    appendModelData(model_.get(), id_ + ":" + QString::fromStdString(msg));
}

void ChatContext::appendMyMessage(const QString &msg)
{
    service_->post(boost::make_shared<sml::send_data>(addr_, 1, msg.toStdString()));
    appendModelData(model_.get(), myId_ + ":" + msg);
}


QStringListModel *ChatContext::model()
{
    return model_.get();
}

const sml::address &ChatContext::addr() const
{
    return addr_;
}

bool ChatContext::fileToReceive() const
{
    return state_ & FileIncoming;
}

void ChatContext::feed(sml::recv_data *msg)
{
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

void ChatContext::cmdHandler()
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
                setFileLength(node["file"].as<size_t>());
                emit fileIncoming();
            }
        }
        else
        {
            break;
        }
    }
}

void ChatContext::startReceivingFile(const QString &path)
{
    state_ |= PathSet;
    filePath_ = path;
    service_->post(boost::make_shared<sml::send_data>(addr_, 0, "OK"));
}

void ChatContext::appendFile(const std::string &bytes)
{
    file_.append(bytes);
    if (state_ == (LengthSet | PathSet) &&
            file_.size() == file_length_)
    {
        QFile file(filePath_);
        file.write(file_.c_str(), file_.size());
        state_ = NoneSet;
    }
}

void ChatContext::setFileLength(size_t length)
{
    state_ |= LengthSet;
    file_length_ = length;
    // reserver length bytes room
    file_.reserve(length);
}

