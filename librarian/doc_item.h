#ifndef DOC_ITEM_H
#define DOC_ITEM_H

#include "ui_doc_items.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>
#include <QMessageBox>

class docItem:public QDialog{
    Q_OBJECT
public:
    docItem(QList<int> _list, int _curr, int _doc, QWidget *parent = 0);
    Ui::doc_items ui;
    QList<int> list;
    int curr;
    int doc;
    int vid;


    virtual void closeEvent(QCloseEvent *);

private slots:
    void readSetting();
    void writeSetting();

    void searchBook(const QString text);
    void selectBook();

    void saveItem();
    void deleteItem();
    void openItem();

    void viewItems();

    void toFirst();
    void toPrev();
    void toNext();
    void toLast();

};

#endif // DOC_ITEM_H
