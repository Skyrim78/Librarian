#ifndef OWNER_H
#define OWNER_H

#include "ui_owner.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>

class owner:public QDialog{
    Q_OBJECT
public:
    owner(QWidget *parent = 0);
    Ui::owner ui;

protected:
    virtual void closeEvent(QCloseEvent *);

protected slots:
    void readSetting();
    void writeSetting();

    void clearOwner();
    void saveOwner();
    void openOwner();
};

#endif // OWNER_H
