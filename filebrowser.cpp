#include "filebrowser.h"
#include "ui_filebrowser.h"

FileBrowser::FileBrowser(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::FileBrowser),
    dir_(QDir::home())
{
    ui_->setupUi(this);
    ui_->pathListView->setModel(&model_);
    ui_->pathLineEdit->setText(dir_.path());
    model_.setStringList(dir_.entryList());
}

FileBrowser::~FileBrowser()
{
    delete ui_;
}

const QString &FileBrowser::getPath() const
{
    return ui_->pathLineEdit->text();
}

void FileBrowser::on_pathListView_clicked(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }
    ui_->pathLineEdit->setText(dir_.absoluteFilePath(model_.data(index).toString()));
}

void FileBrowser::on_pathListView_activated(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }
    if (dir_.cd(model_.data(index).toString()))
    {
        ui_->pathLineEdit->setText(dir_.path());
        model_.setStringList(dir_.entryList());
    }
}

void FileBrowser::on_pathLineEdit_editingFinished()
{
    if (dir_.cd(ui_->pathLineEdit->text()))
    {
        model_.setStringList(dir_.entryList());
    }
}
