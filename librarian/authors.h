#ifndef AUTHORS_H
#define AUTHORS_H

#include "ui_authors.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>
#include <QMessageBox>

class auth:public QMainWindow{
    Q_OBJECT
public:
    auth(QWidget *parent = 0);
    Ui::authors ui;
    int id;

    virtual void closeEvent(QCloseEvent *);

protected slots:
    void readSetting();
    void writeSetting();

    void updateAuth();
    void addAuth();
    void editAuth();
    void saveAuth();
    void deleteAuth();
    void closeAuth();

    void makeNameK();
    void toBook();
};

#endif // AUTHORS_H
