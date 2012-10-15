#ifndef IMPORT_H
#define IMPORT_H

#include "ui_import.h"
#include <QtSql/QtSql>
#include <QMessageBox>

class import:public QDialog{
    Q_OBJECT
public:
    import(QWidget *parent = 0);
    Ui::import_data ui;

public slots:
    void openFile();
    void importData();
    void selectObject();

};

#endif // IMPORT_H
