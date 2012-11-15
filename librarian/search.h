#ifndef SEARCH_H
#define SEARCH_H

#include "ui_search.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>
#include "bookCard.h"

class search:public QDialog{
    Q_OBJECT
public:
    search(QWidget *parent = 0);
    Ui::search ui;
    QString qString;

     virtual void closeEvent(QCloseEvent *);
protected slots:
    void readSetting();
    void writeSetting();

    void selectColumn();

    void selectPosition();
    void makeQuery();
    void delPreItemQuery();
    void delItemQuery();
    void makeQuery_A();
    void close_makeQuery();

    void runQuery();
    void clearQuery();

    QString makeDocResult();
    void printResult();
    void previewResult(QPrinter *p);

    void saveResult();

    void toBook();
};

#endif // SEARCH_H
