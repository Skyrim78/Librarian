#ifndef REPORTS_H
#define REPORTS_H

#include "ui_reports.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>

class reports:public QDialog{
    Q_OBJECT
public:
    reports(int r, QWidget *parent = 0);
    Ui::reports ui;

    int report;

    virtual void closeEvent(QCloseEvent *);
protected slots:
    void readSetting();
    void writeSetting();

    void selectReport();
    void makeReport();

};

#endif // REPORTS_H
