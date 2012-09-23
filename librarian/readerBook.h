#ifndef READERBOOK_H
#define READERBOOK_H

#include "ui_readerBook.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>
#include <QMessageBox>

class readBook:public QDialog{
    Q_OBJECT
public:
    readBook(QList<int> _list, int _curr, int _reader, QWidget *parent = 0);
    Ui::readerBook ui;

    QList<int> list;
    int curr;
    int reader;

    virtual void closeEvent(QCloseEvent *);

public slots:
    void readSetting();
    void writeSetting();

    void searchBook(const QString _str);
    void selectBook();

    void openReadBook();

    void saveReadBook();
    void delReadBook();

    void toFirst();
    void toPrev();
    void toNext();
    void toLast();

};

#endif // READERBOOK_H
