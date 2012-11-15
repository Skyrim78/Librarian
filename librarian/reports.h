#ifndef REPORTS_H
#define REPORTS_H

#include "ui_reports.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>
#include <QTextDocument>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintPreviewDialog>

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

    void toDocsBooksReaders();

    QString makeDocReport();
    void printReport();
    void previewReport(QPrinter *p);
    void saveReport();

};

#endif // REPORTS_H
