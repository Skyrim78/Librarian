#ifndef READERCARD_H
#define READERCARD_H

#include "ui_readerCard.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>
#include <QMessageBox>

class readCard:public QDialog{
    Q_OBJECT
public:
    readCard(QList<int> _list, int _curr, QWidget *parent = 0);
    Ui::reader_card ui;

private:
    QList<int> list;
    int curr;

    virtual void closeEvent(QCloseEvent *);

private slots:
    void readSetting();
    void writeSetting();

    void openReader();
    void saveReader();
    void delReader();

    void takeBook();
    void getBook();

    void toFirst();
    void toPrev();
    void toNext();
    void toLast();    
};

#endif // READERCARD_H
