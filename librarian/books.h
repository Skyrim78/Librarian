#ifndef BOOKS_H
#define BOOKS_H

#include "ui_books.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>

class books:public QMainWindow{
    Q_OBJECT
public:
    books(QWidget *parent = 0);
    Ui::books ui;

    virtual void closeEvent(QCloseEvent *);

protected slots:
    void readSetting();
    void writeSetting();
    void selectColumns();

    void readBook();
    void addBook();
    void editBook();
};

#endif // BOOKS_H
