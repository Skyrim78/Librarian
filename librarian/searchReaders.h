#ifndef SEARCHREADERS_H
#define SEARCHREADERS_H

#include "ui_searchReaders.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>
#include <QMessageBox>

class sReaders:public QDialog{
    Q_OBJECT
public:
    sReaders(QWidget *parent = 0);
    Ui::search_readers ui;

    virtual void closeEvent(QCloseEvent *);
protected slots:
    void readSetting();
    void writeSetting();
    void selectColumn();

    void search(const QString text);

    void toReader();

};


#endif // SEARCHREADERS_H
