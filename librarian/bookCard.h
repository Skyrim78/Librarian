#ifndef BOOKCARD_H
#define BOOKCARD_H

#include "ui_bookCard.h"
#include <QtSql/QtSql>
#include <QtXml/QtXml>
#include <QFileDialog>
#include <QMessageBox>

class b_card:public QDialog{
    Q_OBJECT
public:
    b_card(QList<int> _list, int _item, bool _card, QWidget *parent = 0);
    Ui::book_card ui;
    QList<int> list;
    int item;
    bool card;

    QString file_photo;

    virtual void closeEvent(QCloseEvent *);

protected slots:
    void readSetting();
    void writeSetting();

    void updateCard();
    void selectSections();

    void updateAuthors(int id_book);
    void addAuthors();
    void delAuthors();

    void addPhoto();
    void delPhoto();

    void addEBook();
    void delEBook();
    void openBook();

    void updateIdentifiers();
    void lookHistoryBook();

    void saveCard();
    void deleteCard();
    void closeCard();
    void addCard();


    void toFirst();
    void toPrev();
    void toNext();
    void toLast();
};

#endif // BOOKCARD_H
