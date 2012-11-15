#include "doc_item.h"

docItem::docItem(QList<int> _list, int _curr, int _doc, QWidget *parent):QDialog(parent){
    ui.setupUi(this);
    list = _list;
    curr = _curr;
    doc = _doc;

    QSqlQuery _v(QString("select docs.vid from docs where docs.id = \'%1\'").arg(doc));
    _v.next();
    vid = _v.value(0).toInt();

    ui.spinBox_id_book->hide();
    ui.tableWidget_res->setColumnHidden(0, true);    
    ui.tableWidget_res->setColumnHidden(7, true);
    ui.tableWidget_res->setColumnHidden(8, true);

    if (vid == 1){
        ui.tableWidget_res->setColumnHidden(6, true);
        ui.radioButton_identifier->hide();
    } else if (vid == 2){
        ui.lineEdit_place->hide();
        ui.label_3->hide();
        ui.lineEdit_identifier->setReadOnly(true);
    }

    openItem();

    readSetting();
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui.lineEdit_search, SIGNAL(textEdited(QString)), this, SLOT(searchBook(QString)));
    connect(ui.tableWidget_res, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(selectBook()));

    connect(ui.pushButton_save, SIGNAL(clicked()), this, SLOT(saveItem()));
    connect(ui.pushButton_del, SIGNAL(clicked()), this, SLOT(deleteItem()));

    connect(ui.pushButton_toFirst, SIGNAL(clicked()), this, SLOT(toFirst()));
    connect(ui.pushButton_toLast, SIGNAL(clicked()), this, SLOT(toLast()));
    connect(ui.pushButton_toNext, SIGNAL(clicked()), this, SLOT(toNext()));
    connect(ui.pushButton_toPrev, SIGNAL(clicked()), this, SLOT(toPrev()));

}

void docItem::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("docs_item");
         int _w =  node.item(0).attributes().namedItem("width").nodeValue().toInt();
         int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
         int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
         int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
         setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void docItem::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream out(&file);
    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();
    QDomNodeList node = doc.elementsByTagName("docs_card");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));
    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void docItem::closeEvent(QCloseEvent *){
    writeSetting();
}

void docItem::searchBook(const QString text){
    for (int a = ui.tableWidget_res->rowCount(); a >= 0; a--){
        ui.tableWidget_res->removeRow(a);
    }
    if (vid == 1){
        QString search("select books.id, books.isbn, books.title, authors.name_k, publish.name, books.year "
                       "from books, publish, authors, book_auth "
                       "where book_auth.auth = authors.id and book_auth.book = books.id and publish.id = books.pub and ");
        if (ui.radioButton_isbn->isChecked()){
            search.append(QString("books.isbn like \'%1%\' ").arg(text));
        } else if (ui.radioButton_title->isChecked()){
            search.append(QString("books.title like \'%1%\' ").arg(text));
        }
        QSqlQuery query(search);
        int row = 0;
        while(query.next()){
            ui.tableWidget_res->insertRow(row);
            for (int col = 0; col < 6; col++){
                QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                ui.tableWidget_res->setItem(row, col, item);
            }
            row++;
        }
    } else if (vid == 2){
        QString search("select book_item.id, books.isbn, books.title, authors.name_k, publish.name, books.year,  book_item.identifier, "
                       "(select doc_item.id from docs, doc_item where docs.vid = '2' and doc_item.doc = docs.id and doc_item.book_item = book_item.id), "
                       "(select card_read.id from card_read where (card_read.date_e is null) and card_read.book_item = book_item.id) "
                       "from books, book_item, authors, book_auth, publish "
                       "where books.id = book_item.book and book_auth.book = books.id and book_auth.auth = authors.id "
                       "and books.pub = publish.id and ");
        if (ui.radioButton_title->isChecked()){
            search.append(QString("books.title LIKE  \'%%1%\' ").arg(text));
        } else if (ui.radioButton_isbn->isChecked()){
            search.append(QString("books.isbn LIKE  \'%%1%\' ").arg(text));
        } else if (ui.radioButton_identifier->isChecked()){
            search.append(QString("book_item.identifier LIKE \'%%1%\' ").arg(text));
        }
        QSqlQuery query(search);
        int row = 0;
        while (query.next()){
            ui.tableWidget_res->insertRow(row);
            for (int col = 0; col <9; col++){
                QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                ui.tableWidget_res->setItem(row, col, item);
            }
            row++;
        }
        // clearing
        for (int a = ui.tableWidget_res->rowCount()-1; a>=0; a--){
            if (!ui.tableWidget_res->item(a, 7)->text().isEmpty() or
                    !ui.tableWidget_res->item(a, 8)->text().isEmpty()){
                ui.tableWidget_res->removeRow(a);
            }
        }
    }
}


void docItem::selectBook(){
    ui.spinBox_id_book->setValue(ui.tableWidget_res->item(ui.tableWidget_res->currentRow(), 0)->text().toInt());
    ui.lineEdit_title->setText(ui.tableWidget_res->item(ui.tableWidget_res->currentRow(), 2)->text());
    if (vid == 2){
        ui.lineEdit_identifier->setText(ui.tableWidget_res->item(ui.tableWidget_res->currentRow(), 6)->text());
    }
}

void docItem::openItem(){    
    if (list.size() == 0){
        ui.lineEdit_title->clear();
    } else if (list.size() > 0){
        QSqlQuery query(QString("select book_item.book, books.title, book_item.identifier  "
                                "from book_item,doc_item, books "
                                "where doc_item.book_item = book_item.id and books.id = book_item.book "
                                "and doc_item.id = \'%1\' ").arg(list.at(curr)));
        query.next();
        ui.spinBox_id_book->setValue(query.value(0).toInt());
        ui.lineEdit_title->setText(query.value(1).toString());
        //ui.spinBox_coun->setValue(query.value(2).toInt());
        ui.lineEdit_identifier->setText(query.value(2).toString());
    }
    viewItems();

    //test for deleting
    if (vid == 1){
        QSqlQuery testRead(QString("select Count(card_read.id) "
                                   "from card_read, book_item "
                                   "where card_read.book_item = book_item.id and book_item.identifier = \'%1\'")
                           .arg(ui.lineEdit_identifier->text()));
        testRead.next();
        QSqlQuery testDoc(QString("select Count(doc_item.id) "
                                  "from doc_item, docs, book_item "
                                  "where doc_item.book_item = book_item.id and doc_item.doc = docs.id and docs.vid = 2 "
                                  "and book_item.identifier = \'%1\'")
                          .arg(ui.lineEdit_identifier->text()));
        testDoc.next();
        if (testRead.value(0).toInt() == 0 and testDoc.value(0).toInt() == 0){
            ui.pushButton_del->setEnabled(true);
        } else if (testRead.value(0).toInt() > 0 or testDoc.value(0).toInt() > 0){
            ui.pushButton_del->setEnabled(false);
        }
    } else if (vid == 2){
        ui.pushButton_del->setEnabled(true);
    }

    //

    if (list.size() == 0){
        ui.pushButton_toFirst->setEnabled(false);
        ui.pushButton_toLast->setEnabled(false);
        ui.pushButton_toNext->setEnabled(false);
        ui.pushButton_toPrev->setEnabled(false);
    } else if (list.size() > 0) {
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
}

void docItem::saveItem(){
    if (list.size() == 0){
        if (vid == 1){
            QSqlQuery query("insert into book_item (book, identifier) values (:book,  :identifier)");
            query.bindValue(0, QString("%1").arg(ui.spinBox_id_book->value()));
            query.bindValue(1, ui.lineEdit_identifier->text());
            query.bindValue(2, ui.lineEdit_place->text());
            query.exec();
            QSqlQuery queryDocs("insert into doc_item (doc, book_item) values (:doc, :book_item)");
            queryDocs.bindValue(0, QString("%1").arg(doc));
            queryDocs.bindValue(1, QString("%1").arg(query.lastInsertId().toString()));
            queryDocs.exec();
        } else if (vid == 2){
            QSqlQuery queryDocs("insert into doc_item (doc, book_item) values (:doc, :book_item)");
            queryDocs.bindValue(0, QString("%1").arg(doc));
            queryDocs.bindValue(1, QString("%1").arg(ui.spinBox_id_book->value()));
            queryDocs.exec();
        }

    } else if (list.size() > 0){
        QSqlQuery queryID(QString("select doc_item.book_item from doc_item where doc_item.id = \'%1\'").arg(list.at(curr)));
        queryID.next();
        QSqlQuery query(QString("update book_item set book = \'%1\', identifier = \'%2\' where book_item.id = \'%4\' ")
                        .arg(ui.spinBox_id_book->value())
                        .arg(ui.lineEdit_identifier->text())
                        .arg(queryID.value(0).toInt()));
        query.exec();
    }
    viewItems();
    openItem();
}

void docItem::deleteItem(){
    QSqlQuery queryID(QString("select doc_item.book_item from doc_item where doc_item.id = \'%1\'").arg(list.at(curr)));
    queryID.next();
    QSqlQuery queryDel(QString("delete from book_item where book_item.id = \'%1\'").arg(queryID.value(0).toInt()));
    queryDel.exec();
    QSqlQuery query(QString("delete from doc_item where doc_item.id = \'%1\'").arg(list.at(curr)));
    query.exec();
    close();
}

void docItem::viewItems(){
    for (int a = ui.tableWidget_docs->rowCount(); a >=0; a--){
        ui.tableWidget_docs->removeRow(a);
    }
    QSqlQuery query(QString("select book_item.book, books.isbn, books.title, publish.name, books.year, book_item.identifier "
                    "from book_item, doc_item, books, publish "
                    "where books.id = book_item.book and book_item.id = doc_item.book_item "
                            "and publish.id = books.pub and doc_item.doc = \'%1\' ").arg(doc));
    int row = 0;
    while (query.next()){
        ui.tableWidget_docs->insertRow(row);
        for (int col = 0; col < 2; col++){
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col+1).toString());
            ui.tableWidget_docs->setItem(row, col, item);
        }
        for (int col =3; col < 6; col++){
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui.tableWidget_docs->setItem(row, col, item);
        }
        //******
        QSqlQuery queryAuth(QString("select authors.name_k from authors, book_auth "
                                    "where book_auth.auth = authors.id and book_auth.book = \'%1\' ").arg(query.value(0).toString()));
        QString authBook;
        while (queryAuth.next()){
            authBook.append(QString("%1 ").arg(queryAuth.value(0).toString()));
        }
        QTableWidgetItem *itemAuth = new QTableWidgetItem(authBook);
        ui.tableWidget_docs->setItem(row, 2, itemAuth);
        //******
        row++;
    }
}

void docItem::toFirst(){
    curr = 0;
    openItem();
}

void docItem::toPrev(){
    curr = curr - 1;
    openItem();
}

void docItem::toNext(){
    curr = curr + 1;
    openItem();
}

void docItem::toLast(){
    curr = list.size() - 1;
    openItem();
}
