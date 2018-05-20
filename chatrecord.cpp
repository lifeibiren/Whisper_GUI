#include "chatrecord.h"
#include "ui_chatrecord.h"

#include <QFile>
#include "encryptor.h"

ChatRecord::ChatRecord(const QString &path, QWidget *parent) :
    QDialog(parent),
    filePath_(path),
    ui_(new Ui::ChatRecord),
    encrypted_(false)
{
    ui_->setupUi(this);

    ui_->peerListView->setModel(&peerStringListModel_);


}

ChatRecord::~ChatRecord()
{
    delete ui_;
}

void ChatRecord::load()
{
    if (!encrypted_)
    {
        record = YAML::LoadFile(filePath_.toStdString());
    }
    else
    {
        QFile file(filePath_);
        file.open(QFile::ReadOnly);
        QByteArray bytes = file.readAll();
        Encryptor encryptor(passwd_);
        std::string yaml = encryptor.decrypt(bytes.toStdString());
        record = YAML::Load(yaml);
    }
}

void ChatRecord::setPassword(const QString &passwd)
{
    encrypted_ = passwd.size() ? true : false;
    passwd_ = passwd;
    load();
    refresh();
}

void ChatRecord::refresh()
{
//    ui_->messageListView->reset();
    messageStringListModel_.clear();
    std::unique_ptr<QStringList> peerStringList = std::make_unique<QStringList>();
    for (YAML::Node::const_iterator it = record.begin(); it != record.end(); it ++)
    {
        std::unique_ptr<QStringListModel> model = std::make_unique<QStringListModel>();
        std::unique_ptr<QStringList> list = std::make_unique<QStringList>();

        peerStringList->append(it->first.as<QString>());
        if (!it->second.IsSequence())
        {
            throw std::runtime_error("YAML of chat record is invalid");
        }
        for (size_t i = 0; i < it->second.size(); i ++)
        {
            list->append(it->second[i].as<QString>());
        }
        model->setStringList(*list);
        messageStringListModel_.push_back(std::move(model));
    }
    peerStringListModel_.setStringList(*peerStringList);
}

void ChatRecord::save() const
{
    if (!encrypted_)
    {
        std::fstream file(filePath_.toStdString());
        file << record;
    }
    else
    {
        std::fstream file(filePath_.toStdString());
        Encryptor encryptor(passwd_);
        std::string dump = YAML::Dump(record);
        file << encryptor.encrypt(dump);
    }
}

void ChatRecord::appendMessage(const QString &peer, const QString &speaker, const QString &content)
{
    record[peer].push_back(speaker + ":" + content);
    refresh();
    save();
}

void ChatRecord::on_peerListView_clicked(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }

    ui_->messageListView->setModel(messageStringListModel_[index.row()].get());
}
