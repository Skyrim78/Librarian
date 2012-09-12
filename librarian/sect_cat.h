#ifndef SECT_CAT_H
#define SECT_CAT_H

#include "ui_sect_cat.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>
#include <QMessageBox>

class sect_cat:public QMainWindow{
    Q_OBJECT
public:
    sect_cat(QWidget *parent = 0);
    Ui::sect_cat ui;
    int sectID;
    int catID;

protected:
    virtual void closeEvent(QCloseEvent *);

private slots:

    //---sections--//
    void updateSect();
    void addSect();
    void editSect();
    void saveSect();
    void delSect();
    void closeSect();
    void selectSect();

    //---category--//
    void updateCat();
    void addCat();
    void editCat();
    void saveCat();
    void delCat();
    void closeCat();
    //---setting--//

    void readSetting();
    void writeSetting();
};


#endif // SECT_CAT_H
