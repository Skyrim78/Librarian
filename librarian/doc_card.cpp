#include "doc_card.h"
#include "doc_item.h"

docCard::docCard(QList<int> _list, int _curr, QWidget *parent):QDialog(parent){
    ui.setupUi(this);
    list = _list;
    curr = _curr;
    ui.groupBox_column->hide();

    QSqlQuery org("select organizations.name from organizations");
    while (org.next()){
        ui.comboBox_org->addItem(org.value(0).toString());
    }

    readDoc();
    readSetting();
    selectColumn();

    connect(ui.toolButton_col, SIGNAL(clicked(bool)), ui.groupBox_column, SLOT(setVisible(bool)));
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui.pushButton_save, SIGNAL(clicked()), this, SLOT(saveDoc()));
    connect(ui.pushButton_del, SIGNAL(clicked()), this, SLOT(deleteDoc()));

    connect(ui.pushButton_add_book, SIGNAL(clicked()), this, SLOT(addItem()));
    connect(ui.tableWidget_book, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openItem()));

    connect(ui.checkBox_auth, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect(ui.checkBox_title, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect(ui.checkBox_isbn, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect(ui.checkBox_pub, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect(ui.checkBox_count, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect(ui.checkBox_year, SIGNAL(clicked()), this, SLOT(selectColumn()));

    connect(ui.pushButton_toFirst, SIGNAL(clicked()), this, SLOT(toFirst()));
    connect(ui.pushButton_toLast, SIGNAL(clicked()), this, SLOT(toLast()));
    connect(ui.pushButton_toNext, SIGNAL(clicked()),this, SLOT(toNext()));
    connect(ui.pushButton_toPrev, SIGNAL(clicked()), this, SLOT(toPrev()));
}

void docCard::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("docs_card");
         int _w =  node.item(0).attributes().namedItem("width").nodeValue().toInt();
         int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
         int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
         int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
         ui.checkBox_isbn->setChecked(node.item(0).attributes().namedItem("isbn").nodeValue().toInt());
         ui.checkBox_title->setChecked(node.item(0).attributes().namedItem("title").nodeValue().toInt());
         ui.checkBox_auth->setChecked(node.item(0).attributes().namedItem("auth").nodeValue().toInt());
         ui.checkBox_pub->setChecked(node.item(0).attributes().namedItem("pub").nodeValue().toInt());
         ui.checkBox_year->setChecked(node.item(0).attributes().namedItem("year").nodeValue().toInt());
         ui.checkBox_count->setChecked(node.item(0).attributes().namedItem("coun").nodeValue().toInt());
         setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void docCard::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
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
    node.item(0).attributes().namedItem("isbn").setNodeValue(QString("%1").arg(ui.checkBox_isbn->isChecked()));
    node.item(0).attributes().namedItem("title").setNodeValue(QString("%1").arg(ui.checkBox_title->isChecked()));
    node.item(0).attributes().namedItem("auth").setNodeValue(QString("%1").arg(ui.checkBox_auth->isChecked()));
    node.item(0).attributes().namedItem("pub").setNodeValue(QString("%1").arg(ui.checkBox_pub->isChecked()));
    node.item(0).attributes().namedItem("year").setNodeValue(QString("%1").arg(ui.checkBox_year->isChecked()));
    node.item(0).attributes().namedItem("coun").setNodeValue(QString("%1").arg(ui.checkBox_count->isChecked()));
    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void docCard::closeEvent(QCloseEvent *){
    writeSetting();
}

void docCard::selectColumn(){
    ui.tableWidget_book->setColumnHidden(0, true);
    ui.tableWidget_book->setColumnHidden(1, true);
    ui.tableWidget_book->setColumnHidden(2, !ui.checkBox_isbn->isChecked());
    ui.tableWidget_book->setColumnHidden(3, !ui.checkBox_title->isChecked());
    ui.tableWidget_book->setColumnHidden(4, !ui.checkBox_auth->isChecked());
    ui.tableWidget_book->setColumnHidden(5, !ui.checkBox_pub->isChecked());
    ui.tableWidget_book->setColumnHidden(6, !ui.checkBox_year->isChecked());
    ui.tableWidget_book->setColumnHidden(7, !ui.checkBox_count->isChecked());
    ui.tableWidget_book->resizeColumnsToContents();
    ui.tableWidget_book->horizontalHeader()->setStretchLastSection(true);
}

void docCard::readDoc(){
    QSqlQuery query(QString("select docs.num, docs.date_d, docs.name, organizations.name, docs.note "
                            "from docs, organizations where organizations.id = docs.org and docs.id = \'%1\' ")
                    .arg(list.at(curr)));
    query.next();
    ui.lineEdit_num->setText(query.value(0).toString());
    ui.dateEdit_doc->setDate(query.value(1).toDate());
    ui.lineEdit_name->setText(query.value(2).toString());
    ui.comboBox_org->setCurrentIndex(ui.comboBox_org->findText(query.value(3).toString()));
    ui.lineEdit_note->setText(query.value(4).toString());

    //**items
    for (int a = ui.tableWidget_book->rowCount(); a >=0; a--){
        ui.tableWidget_book->removeRow(a);
    }
    QSqlQuery queryA(QString("select doc_item.id, book_item.book, books.isbn, books.title, publish.name, books.year, book_item.identifier "
                             "from book_item, doc_item, books, publish "
                             "where book_item.id = doc_item.book_item and books.id = book_item.book and publish.id = books.pub and doc_item.doc = \'%1\' ").arg(list.at(curr)));
    int row = 0;
    while (queryA.next()){
        ui.tableWidget_book->insertRow(row);
        for (int col = 0; col < 4; col++){
            QTableWidgetItem *item = new QTableWidgetItem(queryA.value(col).toString());
            ui.tableWidget_book->setItem(row, col, item);
        }
        //**authors
        QSqlQuery queryAuth(QString("select authors.name_k from authors, book_auth "
                                    "where book_auth.auth = authors.id and book_auth.book = \'%1\' ").arg(queryA.value(1).toString()));
        QString authBook;
        while (queryAuth.next()){
            authBook.append(QString("%1 ").arg(queryAuth.value(0).toString()));
        }
        QTableWidgetItem *itemAuth = new QTableWidgetItem(authBook);
        ui.tableWidget_book->setItem(row, 4, itemAuth);
          ///***///
        for (int col = 5; col < 8; col++){
            QTableWidgetItem *item = new QTableWidgetItem(queryA.value(col-1).toString());
            ui.tableWidget_book->setItem(row, col, item);
        }
        row++;
        ui.tableWidget_book->resizeColumnsToContents();
        ui.tableWidget_book->horizontalHeader()->setStretchLastSection(true);
    }
    //total
    ui.spinBox_total->setValue(ui.tableWidget_book->rowCount());

    if (list.size() == 0){
        ui.pushButton_toFirst->setEnabled(false);
        ui.pushButton_toFirst->setEnabled(false);
        ui.pushButton_toFirst->setEnabled(false);
        ui.pushButton_toFirst->setEnabled(false);
    } else {
        if (curr == 0){
            ui.pushButton_toFirst->setEnabled(false);
            ui.pushButton_toPrev->setEnabled(false);
        } else {
            ui.pushButton_toFirst->setEnabled(true);
            ui.pushButton_toPrev->setEnabled(true);
        }
        if (curr == list.size() - 1){
            ui.pushButton_toNext->setEnabled(false);
            ui.pushButton_toLast->setEnabled(false);
        } else  {
            ui.pushButton_toNext->setEnabled(true);
            ui.pushButton_toLast->setEnabled(true);
        }
    }
}

void docCard::saveDoc(){
    QSqlQuery queryOrg(QString("select organizations.id from organizations where organizations.name = \'%1\' ")
                       .arg(ui.comboBox_org->currentText()));
    queryOrg.next();
    QSqlQuery query(QString("update docs set name = \'%1\', num = \'%2\', date_d = \'%3\', org = \'%4\', note = \'%5\' "
                            "where docs.id = \'%6\' ")
                    .arg(ui.lineEdit_name->text())
                    .arg(ui.lineEdit_num->text())
                    .arg(ui.dateEdit_doc->date().toString("yyyy-MM-dd"))
                    .arg(queryOrg.value(0).toString())
                    .arg(ui.lineEdit_note->text())
                    .arg(list.at(curr)));
    query.exec();
    ui.lineEdit_status->setText(query.lastError().text());
}

void docCard::deleteDoc(){
    QSqlQuery queryA(QString("select doc_item.book_item from doc_item where doc_item.doc = \'%1\'")
                     .arg(list.at(curr)));
    while (queryA.next()){
        QSqlQuery delItems(QString("delete from book_item where book_item.id = \'%1\'")
                           .arg(queryA.value(0).toInt()));
        delItems.exec();
    }
    QSqlQuery queryB(QString("delete from doc_item where doc_item.doc = \'%1\' ").arg(list.at(curr)));
    queryB.exec();
    QSqlQuery queryC(QString("delete from docs where docs.id = \'%1\'").arg(list.at(curr)));
    queryC.exec();
    close();
}

void docCard::addItem(){
    QList<int> listA;

    docItem *di = new docItem(listA, 0, list.at(curr), this);
    di->exec();
    readDoc();
}

void docCard::openItem(){
    QList<int> listA;
    for (int a = 0; a < ui.tableWidget_book->rowCount(); a++){
        listA << ui.tableWidget_book->item(a, 0)->text().toInt();
    }
    docItem *di = new docItem(listA, ui.tableWidget_book->currentRow(), list.at(curr), this);
    di->exec();
    readDoc();
}

void docCard::toFirst(){
    curr = 0;
    readDoc();
}

void docCard::toPrev(){
    curr = curr - 1;
    readDoc();
}

void docCard::toNext(){
    curr = curr + 1;
    readDoc();
}

void docCard::toLast(){
    curr = list.size() - 1;
    readDoc();
}

