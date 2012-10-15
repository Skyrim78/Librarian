#ifndef SEARCH_H
#define SEARCH_H

#include "ui_search.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>

class search:public QDialog{
    Q_OBJECT
public:
    search(QWidget *parent = 0);
    Ui::search ui;

    QString queryString;

    virtual void closeEvent(QCloseEvent *);
protected slots:
    void readSetting();
    void writeSetting();

    void selectColumn();

    void selectPosition();
    void makeQuery();
    void delItemQuery();

    void runQuery();
    void clearQuery();
};

#endif // SEARCH_H
