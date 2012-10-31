#include "readerBook.h"

readBook::readBook(QList<int> _list, int _curr, int _reader, QWidget *parent):QDialog(parent){
    ui.setupUi(this);
    list = _list;
    curr = _curr;
    reader = _reader;

    ui.spinBox_id->hide();
    ui.tableWidget_search->setColumnHidden(0, true);
    ui.tableWidget_search->setColumnHidden(5, true);
    ui.tableWidget_search->setColumnHidden(6, true);
    ui.dateEdit_get_date->setDate(QDate::currentDate());
    ui.dateEdit_take_date->setDate(QDate::currentDate());
    readSetting();

    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui.checkBox_take, SIGNAL(clicked(bool)), ui.dateEdit_take_date, SLOT(setEnabled(bool)));
    connect(ui.checkBox_take, SIGNAL(clicked(bool)), ui.checkBox_get, SLOT(setEnabled(bool)));
    connect(ui.checkBox_get, SIGNAL(clicked(bool)), ui.dateEdit_get_date, SLOT(setEnabled(bool)));

    openReadBook();

    connect(ui.pushButton_toFirst, SIGNAL(clicked()), this, SLOT(toFirst()));
    connect(ui.pushButton_toPrev, SIGNAL(clicked()), this, SLOT(toPrev()));
    connect(ui.pushButton_toNext, SIGNAL(clicked()), this, SLOT(toNext()));
    connect(ui.pushButton_toLast, SIGNAL(clicked()), this, SLOT(toLast()));

    connect(ui.lineEdit_search, SIGNAL(textEdited(QString)), this, SLOT(searchBook(QString)));
    connect(ui.tableWidget_search, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(selectBook()));

    connect(ui.pushButton_save, SIGNAL(clicked()), this, SLOT(saveReadBook()));
    connect(ui.pushButton_del, SIGNAL(clicked()), this, SLOT(delReadBook()));
}

void readBook::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("read_book");
        int _w = node.item(0).attributes().namedItem("width").nodeValue().toInt();
        int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
        int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
        int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
        setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void readBook::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();
    QDomNodeList node = doc.elementsByTagName("read_book");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));
    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void readBook::closeEvent(QCloseEvent *){
    writeSetting();
}

void readBook::openReadBook(){
    if (list.size() == 0){
        ui.spinBox_id->setValue(0);
        ui.lineEdit_title->clear();
        ui.checkBox_take->setChecked(false);
        ui.dateEdit_take_date->setEnabled(false);
        ui.checkBox_get->setChecked(false);
        ui.checkBox_get->setEnabled(false);
        ui.dateEdit_get_date->setEnabled(false);
        ui.pushButton_del->setEnabled(false);
    } else if (list.size() > 0){
        QSqlQuery query(QString("select card_read.book_item, books.title, book_item.identifier,  card_read.date_s "
                                "from card_read, books, book_item "
                                "where book_item.id = card_read.book_item and book_item.book = books.id and  card_read.id = \'%1\'  ")
                        .arg(list.at(curr)));
        query.next();
        ui.spinBox_id->setValue(query.value(0).toInt());
        ui.lineEdit_title->setText(query.value(1).toString());
        ui.lineEdit_identifier->setText(query.value(2).toString());
        ui.checkBox_take->setChecked(true);
        ui.dateEdit_take_date->setDate(query.value(3).toDate());
        ui.checkBox_get->setChecked(false);
        ui.dateEdit_get_date->setEnabled(false);
    }
    if (list.size() == 0){
        ui.pushButton_toFirst->setEnabled(false);
        ui.pushButton_toPrev->setEnabled(false);
        ui.pushButton_toNext->setEnabled(false);
        ui.pushButton_toLast->setEnabled(false);
    } else if (list.size() > 0){
        if (curr == 0 ){
            ui.pushButton_toFirst->setEnabled(false);
            ui.pushButton_toPrev->setEnabled(false);
        } else {
            ui.pushButton_toFirst->setEnabled(true);
            ui.pushButton_toPrev->setEnabled(true);
        }
        if (curr == list.size() - 1){
            ui.pushButton_toNext->setEnabled(false);
            ui.pushButton_toLast->setEnabled(false);
        } else {
            ui.pushButton_toNext->setEnabled(true);
            ui.pushButton_toLast->setEnabled(true);
        }
    }
}

void readBook::searchBook(const QString _str){
    for (int a = ui.tableWidget_search->rowCount()-1; a >= 0; a--){
        ui.tableWidget_search->removeRow(a);
    }
    QString search("select book_item.id, books.isbn, books.title, authors.name_k, book_item.identifier, "
                   "(select doc_item.id from docs, doc_item where docs.vid = '2' and doc_item.doc = docs.id and doc_item.book_item = book_item.id), "
                   "(select card_read.id from card_read where (card_read.date_e is null) and card_read.book_item = book_item.id) "
                   "from books, book_item, authors, book_auth "
                   "where books.id = book_item.book and book_auth.book = books.id and book_auth.auth = authors.id and ");
    if (ui.radioButton_name->isChecked()){
        search.append(QString("books.title LIKE  \'%%1%\' ").arg(_str));
    } else if (ui.radioButton_isbn->isChecked()){
        search.append(QString("books.isbn LIKE  \'%%1%\' ").arg(_str));
    } else if (ui.radioButton_identifier->isChecked()){
        search.append(QString("book_item.identifier LIKE \'%%1%\' ").arg(_str));
    }
    QSqlQuery query(search);
    int row = 0;
    while (query.next()){
        ui.tableWidget_search->insertRow(row);
        for (int col = 0; col <7; col++){
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui.tableWidget_search->setItem(row, col, item);
        }
        row++;
    }
    // clearing
    for (int a = ui.tableWidget_search->rowCount()-1; a>=0; a--){
        if (!ui.tableWidget_search->item(a, 5)->text().isEmpty() or
                !ui.tableWidget_search->item(a, 6)->text().isEmpty()){
            ui.tableWidget_search->removeRow(a);
        }
    }

}

void readBook::selectBook(){
    int row = ui.tableWidget_search->currentRow();
    ui.spinBox_id->setValue(ui.tableWidget_search->item(row, 0)->text().toInt());
    ui.lineEdit_title->setText(ui.tableWidget_search->item(row, 2)->text());
    ui.lineEdit_identifier->setText(ui.tableWidget_search->item(row, 4)->text());
}

void readBook::saveReadBook(){
    if (list.size() == 0){
        QString ins("insert into card_read (read, book_item, date_s ");
        if (ui.checkBox_get->isChecked()){
            ins.append(", date_e) ");
        } else {
            ins.append(") ");
        }
        ins.append("values (:read, :book_item, :date_s");
        if (ui.checkBox_get->isChecked()){
            ins.append(", :date_e) ");
        } else {
            ins.append(") ");
        }
        QSqlQuery query(ins);
        query.bindValue(0, QString("%1").arg(reader));
        query.bindValue(1, QString("%1").arg(ui.spinBox_id->value()));
        query.bindValue(2, ui.dateEdit_take_date->date().toString("yyyy-MM-dd"));
        if (ui.checkBox_get->isChecked()){
            query.bindValue(3, ui.dateEdit_get_date->date().toString("yyyy-MM-dd"));
        }
        query.exec();
    } else if (list.size() > 0){
        QString upd(QString("update card_read set read = \'%1\', book_item = \'%2\', date_s = \'%3\'")
                    .arg(reader)
                    .arg(ui.spinBox_id->value())
                    .arg(ui.dateEdit_take_date->date().toString("yyyy-MM-dd")));
        if (ui.checkBox_get->isChecked()){
            upd.append(QString(", date_e = \'%1\'").arg(ui.dateEdit_get_date->date().toString("yyyy-MM-dd")));
        }
        upd.append(QString(" where card_read.id = \'%1\' ").arg(list.at(curr)));
        QSqlQuery query2(upd);
        query2.exec();
    }
}

void readBook::delReadBook(){
    QSqlQuery del(QString("delete from card_read where card_read.id = \'%1\' ").arg(list.at(curr)));
    del.exec();
    close();
}

void readBook::toFirst(){
    curr = 0;
    openReadBook();
}

void readBook::toPrev(){
    curr = curr - 1;
    openReadBook();
}

void readBook::toNext(){
    curr = curr + 1;
    openReadBook();
}

void readBook::toLast(){
    curr = list.size() - 1;
    openReadBook();
}
