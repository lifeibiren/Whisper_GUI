#include "trustedpeerstablemodel.h"

TrustedPeersTableModel::TrustedPeersTableModel(YAML::Node trustedPeersNode, QObject *parent)
    : QAbstractTableModel(parent)
{
    for (YAML::Node::iterator it = trustedPeersNode.begin(); it != trustedPeersNode.end(); it ++)
    {
       idVec_.push_back(it->first.as<QString>());
       keyVec_.push_back(it->second.as<QString>());
    }
}

QVariant TrustedPeersTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if (section == 0)
        {
            return QString("ID");
        }
        else if (section == 1)
        {
            return QString("Public Key");
        }
    }
    return QVariant();
}

bool TrustedPeersTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}


int TrustedPeersTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    if (idVec_.size() != keyVec_.size())
    {
        throw QException();
    }

    return idVec_.size();
}

int TrustedPeersTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant TrustedPeersTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == 0)
        {
            return idVec_[index.row()];
        }
        else if (index.column() == 1)
        {
            return keyVec_[index.row()];
        }
    }
    return QVariant();
}

bool TrustedPeersTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        if (index.column() == 0)
        {
            idVec_[index.row()] = value.toString();
        }
        else if (index.column() == 1)
        {
            keyVec_[index.row()] = value.toString();
        }

        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags TrustedPeersTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return  QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

bool TrustedPeersTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    idVec_.insert(row, count, QString());
    keyVec_.insert(row, count, QString());
    endInsertRows();
    return true;
}

bool TrustedPeersTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    idVec_.remove(row, count);
    keyVec_.remove(row, count);
    endRemoveRows();
    return true;
}

YAML::Node TrustedPeersTableModel::toYAMLNode() const
{
    YAML::Node node;
    for (int i = 0; i < idVec_.size(); i ++)
    {
        node[idVec_[i]] = keyVec_[i];
    }
    return node;
}
