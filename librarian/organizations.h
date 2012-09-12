#ifndef ORGANIZATIONS_H
#define ORGANIZATIONS_H

#include "ui_organizations.h"
#include <QtSql/QtSql>
#include <QMessageBox>
#include <QtXml/QtXml>

class org:public QMainWindow{
    Q_OBJECT
public:
    org(QWidget *parent = 0);
    Ui::organizations ui;
    int id;

protected:
    virtual void closeEvent(QCloseEvent *);

private slots:
    void updateOrg();

    void addOrg();
    void editOrg();

    void saveOrg();
    void deleteOrg();

    void selectColumn();

    void readSettingColumns();
    void writeSettingColumns();
};

#endif // ORGANIZATIONS_H
