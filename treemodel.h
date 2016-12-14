/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
****************************************************************************/

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QFileSystemWatcher>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QTimer>
#include <QDebug>
#include <QDir>

#define PROC_FS     "/proc/"
#define STATUS_FL   "/status"

#define pname   0
#define pid     1
#define ppid    2

typedef QMap<QString, QStringList> ProcessMap;
typedef ProcessMap::const_iterator Iter;

class TreeItem;

//! [0]
class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(QObject *parent = 0);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

private slots:
    void slotTimerAlarm();

signals:
    void expandpTree();

private:
    uint setItem(Iter &o, TreeItem* parent);
    TreeItem * findParent(TreeItem * p, uint id);
    void updateModelData(const QString &fs, TreeItem *parent);

    TreeItem *rootItem;
    QTimer *timer;
};
//! [0]

#endif // TREEMODEL_H
