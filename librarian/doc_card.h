#ifndef DOC_CARD_H
#define DOC_CARD_H

#include "ui_docs_card.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>
#include <QMessageBox>

class docCard:public QDialog{
    Q_OBJECT
public:
    docCard(QList<int> _list, int _curr, QWidget *parent = 0);
    Ui::docs_card ui;
    QList<int> list;
    int curr;

    virtual void closeEvent(QCloseEvent *);

private slots:
    void readSetting();
    void writeSetting();
    void selectColumn();

    void readDoc();
    void saveDoc();
    void deleteDoc();

    void toFirst();
    void toPrev();
    void toNext();
    void toLast();

    void addItem();
    void openItem();
};


#endif // DOC_CARD_H
