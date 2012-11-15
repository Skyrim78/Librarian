#include "readerCard.h"
#include "readerBook.h"

readCard::readCard(QList<int> _list, int _curr, QWidget *parent):QDialog(parent){
    ui.setupUi(this);

    list = _list;
    curr = _curr;

    ui.tableWidget_active->setColumnHidden(0, true);
    ui.tableWidget_history->setColumnHidden(0, true);
    readSetting();
    openReader();

    connect(ui.pushButton_toFirst, SIGNAL(clicked()), this, SLOT(toFirst()));
    connect(ui.pushButton_toPrev, SIGNAL(clicked()), this, SLOT(toPrev()));
    connect(ui.pushButton_toNext, SIGNAL(clicked()), this, SLOT(toNext()));
    connect(ui.pushButton_toLast, SIGNAL(clicked()), this, SLOT(toLast()));
    connect(ui.pushButton_save, SIGNAL(clicked()), this, SLOT(saveReader()));
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui.pushButton_del, SIGNAL(clicked()), this, SLOT(delReader()));

    connect(ui.pushButton_add_book, SIGNAL(clicked()), this, SLOT(takeBook()));
    connect(ui.tableWidget_active, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(getBook()));

}
void readCard::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("read_card");
        int _w = node.item(0).attributes().namedItem("width").nodeValue().toInt();
        int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
        int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
        int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
        setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void readCard::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();
    QDomNodeList node = doc.elementsByTagName("read_card");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));
    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void readCard::closeEvent(QCloseEvent *){
    writeSetting();
}

void readCard::openReader(){
    if (list.size() == 0){
        ui.lineEdit_fam->clear();
        ui.lineEdit_ima->clear();
        ui.lineEdit_otc->clear();
        ui.dateEdit_date_r->setDate(QDate::currentDate());
        ui.lineEdit_num->clear();
        ui.lineEdit_address->clear();
        ui.lineEdit_phone->clear();
        ui.lineEdit_doc->clear();
        for (int a = ui.tableWidget_active->rowCount(); a >= 0 ; a--){
            ui.tableWidget_active->removeRow(a);
        }
        for (int a = ui.tableWidget_history->rowCount(); a >= 0 ; a--){
            ui.tableWidget_history->removeRow(a);
        }
        ui.pushButton_del->setEnabled(false);
        ui.lineEdit_status->setText("New document |");
    } else if (list.size() > 0){
        QSqlQuery pers(QString("select readers.fam, readers.ima, readers.otc, readers.date_r, readers.num, "
                               "readers.address, readers.phone, readers.doc from readers "
                               "where readers.id = %1").arg(list.at(curr)));
        pers.next();
        ui.lineEdit_fam->setText(pers.value(0).toString());
        ui.lineEdit_ima->setText(pers.value(1).toString());
        ui.lineEdit_otc->setText(pers.value(2).toString());
        ui.dateEdit_date_r->setDate(pers.value(3).toDate());
        ui.lineEdit_num->setText(pers.value(4).toString());
        ui.lineEdit_address->setText(pers.value(5).toString());
        ui.lineEdit_phone->setText(pers.value(6).toString());
        ui.lineEdit_doc->setText(pers.value(7).toString());
        ui.lineEdit_status->setText(QString("ID: %1 |").arg(list.at(curr)));

        //**active books***//
        for (int a = ui.tableWidget_active->rowCount(); a >= 0; a--){
            ui.tableWidget_active->removeRow(a);
        }
        QSqlQuery actBook(QString("select card_read.id, book_item.identifier,  books.isbn, books.title, authors.name_k, card_read.date_s "
                                  "from card_read, books, book_item, authors, book_auth "
                                  "where card_read.book_item = book_item.id and book_item.book = books.id and "
                                  "book_auth.book = books.id and book_auth.auth = authors.id and "
                                  "card_read.date_e is null and card_read.read = \'%1\' group by card_read.id ").arg(list.at(curr)));
        int row = 0;
        while (actBook.next()){
            ui.tableWidget_active->insertRow(row);
            for (int col = 0; col < 6; col++){
                QTableWidgetItem *item = new QTableWidgetItem(actBook.value(col).toString());
                ui.tableWidget_active->setItem(row, col, item);
            }
            row++;
        }
        ui.tableWidget_active->resizeColumnsToContents();
        ui.tableWidget_active->horizontalHeader()->setStretchLastSection(true);

        //**history***//
        for (int a = ui.tableWidget_history->rowCount(); a >= 0; a-- ){
            ui.tableWidget_history->removeRow(a);
        }

        QSqlQuery hisBook(QString("select card_read.id, book_item.identifier,  books.isbn, books.title, authors.name_k, card_read.date_s, card_read.date_e "
                                  "from card_read, books, book_item, authors, book_auth "
                                  "where card_read.book_item = book_item.id and book_item.book = books.id and "
                                  "book_auth.book = books.id and book_auth.auth = authors.id and "
                                  "card_read.date_e is not null and card_read.read = \'%1\' group by card_read.id ").arg(list.at(curr)));
        int row2 = 0;
        while (hisBook.next()){
            ui.tableWidget_history->insertRow(row2);
            for (int col = 0; col < 7; col++){
                QTableWidgetItem *item = new QTableWidgetItem(hisBook.value(col).toString());
                ui.tableWidget_history->setItem(row2, col, item);
            }
            row2++;
        }
        ui.tableWidget_history->resizeColumnsToContents();
        ui.tableWidget_history->horizontalHeader()->setStretchLastSection(true);
        if (ui.tableWidget_active->rowCount() > 0 or ui.tableWidget_history->rowCount() > 0){
            ui.pushButton_del->setEnabled(false);
        } else {
            ui.pushButton_del->setEnabled(true);
        }
    }
    if (list.size() == 0){
        ui.pushButton_toFirst->setEnabled(false);
        ui.pushButton_toLast->setEnabled(false);
        ui.pushButton_toPrev->setEnabled(false);
        ui.pushButton_toNext->setEnabled(false);
    } else {
        ui.pushButton_toFirst->setEnabled(true);
        ui.pushButton_toLast->setEnabled(true);
        ui.pushButton_toPrev->setEnabled(true);
        ui.pushButton_toNext->setEnabled(true);
    }
    if (curr == 0){
        ui.pushButton_toFirst->setEnabled(false);
        ui.pushButton_toPrev->setEnabled(false);
    } else {
        ui.pushButton_toFirst->setEnabled(true);
        ui.pushButton_toPrev->setEnabled(true);
    }
    if (curr == list.size() - 1){
        ui.pushButton_toLast->setEnabled(false);
        ui.pushButton_toNext->setEnabled(false);
    } else {
        ui.pushButton_toLast->setEnabled(true);
        ui.pushButton_toNext->setEnabled(true);
    }
}

void readCard::saveReader(){
    QString err;
    if (list.size() == 0){
        QSqlQuery query("insert into readers (fam, ima, otc, date_r, num, address, phone, doc) "
                        "values (:fam, :ima, :otc, :date_r, :num, :address, :phone, :doc)");
        query.bindValue(0, ui.lineEdit_fam->text());
        query.bindValue(1, ui.lineEdit_ima->text());
        query.bindValue(2, ui.lineEdit_otc->text());
        query.bindValue(3, ui.dateEdit_date_r->date().toString("yyyy-MM-dd"));
        query.bindValue(4, ui.lineEdit_num->text());
        query.bindValue(5, ui.lineEdit_address->text());
        query.bindValue(6, ui.lineEdit_phone->text());
        query.bindValue(7, ui.lineEdit_doc->text());
        query.exec();
        err = query.lastError().text();

    } else if (list.size() > 0){
        QSqlQuery upd(QString("update readers set fam = \'%1\', ima  = \'%2\', otc  = \'%3\', date_r  = \'%4\', num  = \'%5\', "
                               "address  = \'%6\', phone  = \'%7\', doc  = \'%8\' where readers.id  = \'%9\'")
                       .arg(ui.lineEdit_fam->text())
                       .arg(ui.lineEdit_ima->text())
                       .arg(ui.lineEdit_otc->text())
                       .arg(ui.dateEdit_date_r->date().toString("yyyy-MM-dd"))
                       .arg(ui.lineEdit_num->text())
                       .arg(ui.lineEdit_address->text())
                       .arg(ui.lineEdit_phone->text())
                       .arg(ui.lineEdit_doc->text())
                       .arg(list.at(curr)));
        upd.exec();
        err = upd.lastError().text();

    }
    if (err.size() == 1){
        ui.lineEdit_status->setText("Сохранено |");
    } else {
        ui.lineEdit_status->setText(err);
    }
}

void readCard::delReader(){
    QSqlQuery del(QString("delete from readers where readers.id = %1").arg(list.at(curr)));
    del.exec();
    close();
}

void readCard::takeBook(){
    QList<int> listA;
    readBook *rBook = new readBook(listA, 0, list.at(curr), this);
    rBook->exec();
    openReader();
}

void readCard::getBook(){
    QList<int> listB;
    for (int a = 0; a < ui.tableWidget_active->rowCount(); a++){
       listB << ui.tableWidget_active->item(a, 0)->text().toInt();
    }
    readBook *rBook = new readBook(listB, ui.tableWidget_active->currentRow(), list.at(curr), this);
    rBook->exec();
    openReader();
}


void readCard::toFirst(){
    curr = 0;
    openReader();
}

void readCard::toPrev(){
    curr = curr - 1;
    openReader();
}

void readCard::toNext(){
    curr = curr + 1;
    openReader();
}

void readCard::toLast(){
    curr = list.size() - 1;
    openReader();
}


