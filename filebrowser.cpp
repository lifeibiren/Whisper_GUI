#include "filebrowser.h"
#include "ui_filebrowser.h"

FileBrowser::FileBrowser(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::FileBrowser),
    dir(QDir::home())
{
    ui_->setupUi(this);
    ui_->pathListView->setModel(&model_);
    ui_->pathLineEdit->setText(dir.path());
    model_.setStringList(dir.entryList());
}

FileBrowser::~FileBrowser()
{
    delete ui_;
}

const QString &FileBrowser::getPath() const
{
    return path_;
}

void FileBrowser::on_pathListView_clicked(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }
    path_ = model_.data(index).toString();
}

void FileBrowser::on_pathListView_activated(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }
    if (dir.cd(model_.data(index).toString()))
    {
        ui_->pathLineEdit->setText(dir.path());
        model_.setStringList(dir.entryList());
    }
}

void FileBrowser::on_pathLineEdit_editingFinished()
{
    if (dir.cd(ui_->pathLineEdit->text()))
    {
        model_.setStringList(dir.entryList());
    }
}
