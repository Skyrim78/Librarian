#include "doc_item.h"

docItem::docItem(QList<int> _list, int _curr, int _doc, QWidget *parent):QDialog(parent){
    ui.setupUi(this);
    list = _list;
    curr = _curr;
    doc = _doc;
    ui.spinBox_id_book->hide();

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
    QString search("select books.id, books.isbn, books.title, publish.name, books.year "
                   "from books, publish where publish.id = books.pub and ");
    if (ui.radioButton_isbn->isChecked()){
        search.append(QString("books.isbn like \'%1%\' ").arg(text));
    } else if (ui.radioButton_title->isChecked()){
        search.append(QString("books.title like \'%1%\' ").arg(text));
    }
    QSqlQuery query(search);
    int row = 0;
    while(query.next()){
        ui.tableWidget_res->insertRow(row);
        for (int col = 0; col < 3; col++){
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui.tableWidget_res->setItem(row, col, item);
        }
        for (int col = 4; col < 6; col++){
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col-1).toString());
            ui.tableWidget_res->setItem(row, col, item);
        }
        //***auth
        QSqlQuery queryAuth(QString("select authors.name_k from authors, book_auth "
                                    "where book_auth.auth = authors.id and book_auth.book = \'%1\' ").arg(query.value(0).toString()));
        QString authBook;
        while (queryAuth.next()){
            authBook.append(QString("%1 ").arg(queryAuth.value(0).toString()));
        }
        QTableWidgetItem *itemAuth = new QTableWidgetItem(authBook);
        ui.tableWidget_res->setItem(row, 3, itemAuth);
        //*******
        row++;
    }
}

void docItem::selectBook(){
    ui.spinBox_id_book->setValue(ui.tableWidget_res->item(ui.tableWidget_res->currentRow(), 0)->text().toInt());
    ui.lineEdit_name->setText(ui.tableWidget_res->item(ui.tableWidget_res->currentRow(), 2)->text());
}

void docItem::openItem(){    
    if (list.size() == 0){
        ui.lineEdit_name->clear();
    } else if (list.size() > 0){
        QSqlQuery query(QString("select doc_item.book, books.title, doc_item.coun from doc_item, books "
                                "where doc_item.book = books.id and doc_item.id = \'%1\' ").arg(list.at(curr)));
        query.next();
        ui.spinBox_id_book->setValue(query.value(0).toInt());
        ui.lineEdit_name->setText(query.value(1).toString());
        ui.spinBox_coun->setValue(query.value(2).toInt());
    }
    viewItems();
}

void docItem::saveItem(){
    if (list.size() == 0){
        QSqlQuery query("insert into doc_item (doc, book, coun) values (:doc, :book,  :coun)");
        query.bindValue(0, QString("%1").arg(doc));
        query.bindValue(1, QString("%1").arg(ui.spinBox_id_book->value()));
        query.bindValue(2, QString("%1").arg(ui.spinBox_coun->value()));
        query.exec();
    } else if (list.size() > 0){
        QSqlQuery query(QString("update doc_item set book = \'%1\', coun = \'%2\' where doc_item.id = \'%3\' ")
                        .arg(ui.spinBox_id_book->value())
                        .arg(ui.spinBox_coun->value())
                        .arg(list.at(curr)));
        query.exec();
    }
    viewItems();
}

void docItem::deleteItem(){
    QSqlQuery query(QString("delete from doc_item where doc_item.id = \'%1\'").arg(list.at(curr)));
    query.exec();
    close();
}

void docItem::viewItems(){
    for (int a = ui.tableWidget_docs->rowCount(); a >=0; a--){
        ui.tableWidget_docs->removeRow(a);
    }
    QSqlQuery query(QString("select doc_item.book, books.isbn, books.title, publish.name, books.year, doc_item.coun "
                    "from doc_item, books, publish "
                    "where books.id = doc_item.book and publish.id = books.pub and doc_item.doc = \'%1\' ").arg(doc));
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
