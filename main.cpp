/****************************************************************************
**
** From Qt TreeModel example
**
****************************************************************************/

#include "treemodel.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QModelIndex>
#include <QTreeView>

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    TreeModel model;
    QTreeView view;

    view.setModel(&model);

    const QSize availableSize = QApplication::desktop()->availableGeometry(&view).size();
    view.resize(availableSize / 2);
    view.setColumnWidth(0, view.width() / 2);
    view.setColumnWidth(1, view.width() / 6);
    view.setColumnWidth(2, view.width() / 6);

    QObject::connect(&model,
         SIGNAL( expandpTree() ),
         &view,
         SLOT(expandAll()));

    view.expandAll();
    view.setWindowTitle(QObject::tr("Proc Tree Model"));
    view.show();

    return app.exec();
}
