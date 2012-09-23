#ifndef READERS_H
#define READERS_H

#include <QtSql/QtSql>
#include "ui_readers.h"
#include <QtXml/QtXml>

class readers:public QMainWindow {
    Q_OBJECT
public:
    readers(QWidget *parent = 0);
    Ui::readers ui;

    virtual void closeEvent(QCloseEvent *);
public slots:
    void readSetting();
    void writeSetting();

    void selectColumns();

    void updateReaders();
    void addReaders();
    void openReaders();

};

#endif // READERS_H
