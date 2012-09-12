#ifndef PUBLISH_H
#define PUBLISH_H

#include "ui_publish.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>
#include <QMessageBox>

class pub:public QMainWindow{
    Q_OBJECT
public:
    pub(QWidget *parent = 0);
    Ui::publish ui;
    int id;

    virtual void closeEvent(QCloseEvent *);

protected slots:
    void readSetting();
    void writeSetting();

    void updatePub();
    void addPub();
    void editPub();
    void savePub();
    void deletePub();
    void closePub();
};

#endif // PUBLISH_H
