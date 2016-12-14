/****************************************************************************
**
** From Qt TreeModel example
**
****************************************************************************/

/*
    treemodel.cpp

    Provides a simple tree model to show hierarchical list of process
*/

#include "treeitem.h"
#include "treemodel.h"

#include <QStringList>
#include <QFile>

//! [0]
TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Name" << "pID" << "ppID";
    rootItem = new TreeItem(rootData);
    updateModelData(QString(PROC_FS), rootItem);

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));
    timer->start(1000);

}
//! [0]

//! [1]
TreeModel::~TreeModel()
{
    delete rootItem;
}
//! [1]

//! [2]
int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}
//! [2]

//! [3]
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}
//! [3]

//! [4]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}
//! [4]

//! [5]
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}
//! [5]

//! [6]
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
//! [6]

//! [7]
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
//! [7]

//! [8]
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}
//! [8]

uint TreeModel::setItem(Iter &o, TreeItem* parent)
{
//    qDebug() << o.key() << " : " << o.value();
    QList<QVariant> procData;
    procData << o.value()[pname] << o.value()[pid] << o.value()[ppid] ;
    parent->appendChild( new TreeItem(procData, parent ));
    return o.value()[ppid].toUInt();
}

bool idSort( const QList<QVariant> &p1, const QList<QVariant> &p2 )
{
            return( p1[ppid].toUInt() < p2[ppid].toUInt() );
}

TreeItem * TreeModel::findParent(TreeItem * p, uint id){
    TreeItem * f;
//    qDebug() << "Need id=" << id << "->" << p->data(pid);
    if(p->data(pid).toUInt()==id) return p;

    for(int i=0; i < p->childCount(); i++ )
    {
        f = findParent(p->child(i), id) ;
        if(f)
            return f;
    }

    return 0;
}

void TreeModel::updateModelData(const QString &pdir, TreeItem *parent)
{
    QList<TreeItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;
    ProcessMap map;

    QString procIDstatus, lineData, procName, parentID;
    QDir procDir(pdir);

    for(const QString& dir : procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        bool ok;
        dir.toUInt(&ok);
        if(ok) // if dir is nummber of process
        {   // read its' status file to parse PPID & Name
            procIDstatus = QString(PROC_FS) + dir + QString(STATUS_FL);
            QFile statusFile(procIDstatus);
            statusFile.open(QIODevice::ReadOnly);
            lineData = statusFile.readAll();

            QStringList columnStrings = lineData.split(QString("\n"));
            foreach(const QString& col, columnStrings )
            {   // need to find process name && parent process ID
                QStringList itemString = col.split("\t", QString::SkipEmptyParts);
                if(itemString.count()>=2) {
                    if(itemString[0] == "Name:") { procName = itemString[1]; continue; }
                        else if(itemString[0] == "PPid:") { parentID = itemString[1]; break; }
                }
            }
            statusFile.close();
            // forming process map by pID as key
            QStringList procList;
            procList << procName << dir << parentID;
            map[dir] = procList;
        }
    }
//    for ( auto proc : map ) {
//        qDebug() << proc;
//    }

// sort all processes by PPid ascending to improve model tree building
// QMap sorting iterator

    QVector<Iter> byppID;
    Iter im = map.cbegin();
    while (im != map.cend())
        byppID.append(im++);
    qSort(byppID.begin(), byppID.end(), [] (Iter i0, Iter i1) { // ... hm in Qt 5.7 declared as obsolete
        if (i0.value()[ppid] == i1.value()[ppid])  return i0.value()[pid].toUInt() < i1.value()[pid].toUInt();
        else return i0.value()[ppid].toUInt() < i1.value()[ppid].toUInt();
    });

// build tree
    TreeItem *cItem = parent;
    uint pcount = byppID.count(), i, cpID, pID;
    i = cpID = pID = 0;
    while ( i < pcount ){
        for (pID = byppID[i].value()[ppid].toUInt(); i < pcount && (pID=byppID[i].value()[ppid].toUInt())==cpID; i++) {
            pID = setItem(byppID[i],cItem);
        }
        // find parent in childs from root
        for(int pp=0; pp<parent->childCount();pp++)
        {
            cItem = findParent(parent->child(pp),pID);
            if(cItem) break;
        }
        cpID = pID;
    }
}

void TreeModel::slotTimerAlarm()
{
//    qDebug() << "Timer...";
    beginResetModel();
    QList<QVariant> rootData;
    rootData << "Name" << "pID" << "ppID";
    delete rootItem;
    rootItem = new TreeItem(rootData);
    updateModelData(QString(PROC_FS), rootItem);
    endResetModel();
    emit expandpTree();
}
