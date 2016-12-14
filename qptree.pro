QT += widgets

HEADERS     = treeitem.h \
              treemodel.h
SOURCES     = treeitem.cpp \
              treemodel.cpp \
              main.cpp

# install
target.path = qptree
INSTALLS += target
