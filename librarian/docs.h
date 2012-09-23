#ifndef DOCS_H
#define DOCS_H

#include "ui_docs.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>
#include <QMessageBox>


class docs:public QMainWindow{
    Q_OBJECT
public:
    docs(int _v, QWidget *parent = 0);
    Ui::docs ui;
    int vid;

   virtual  void closeEvent(QCloseEvent *);

protected slots:
    void readSetting();
    void writeSetting();
    void selectColumn();

    void readDocs();
    void openDoc();
    void addDoc();


};

#endif // DOCS_H
