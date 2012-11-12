#include "reports.h"

reports::reports(int r, QWidget *parent):QDialog(parent){
    ui.setupUi(this);

    ui.listWidget->setCurrentRow(r);
    readSetting();
    selectReport();

    connect(ui.pushButton_close, SIGNAL(clicked()), SLOT(close()));
    connect(ui.listWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(selectReport()));
    //-----report 6
    connect(ui.spinBox_days, SIGNAL(valueChanged(int)), ui.horizontalSlider_days, SLOT(setValue(int)));
    connect(ui.horizontalSlider_days, SIGNAL(valueChanged(int)), ui.spinBox_days, SLOT(setValue(int)));
    //-----report 0, 1
    ui.comboBox_org->setEnabled(false);
    QSqlQuery org("select organizations.name from organizations");
    while (org.next()){
        ui.comboBox_org->addItem(org.value(0).toString());
    }
    connect(ui.checkBox_org, SIGNAL(clicked(bool)), ui.comboBox_org, SLOT(setEnabled(bool)));
    ui.dateEdit_form->setDate(QDate::fromJulianDay(QDate::currentDate().toJulianDay() - 365));
    ui.dateEdit_form->setEnabled(false);
    ui.dateEdit_to->setDate(QDate::currentDate());
    ui.dateEdit_to->setEnabled(false);
    ui.label_2->setEnabled(false);
    connect(ui.checkBox_period, SIGNAL(clicked(bool)), ui.dateEdit_form, SLOT(setEnabled(bool)));
    connect(ui.checkBox_period, SIGNAL(clicked(bool)), ui.dateEdit_to, SLOT(setEnabled(bool)));
    connect(ui.checkBox_period, SIGNAL(clicked(bool)), ui.label_2, SLOT(setEnabled(bool)));

    connect(ui.pushButton_ok, SIGNAL(clicked()), this, SLOT(makeReport()));

}

void reports::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("reports");
         int _w =  node.item(0).attributes().namedItem("width").nodeValue().toInt();
         int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
         int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
         int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
         setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void reports::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();
    QDomNodeList node = doc.elementsByTagName("reports");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));
    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void reports::closeEvent(QCloseEvent *){
    writeSetting();
}

void reports::selectReport(){
    //clear
    for (int a = ui.tableWidget_result->rowCount(); a >= 0; a--){
        ui.tableWidget_result->removeRow(a);
    }
    report = ui.listWidget->currentRow();
    if (report != 5){
        ui.label_3->hide();
        ui.label_4->hide();
        ui.spinBox_days->hide();
        ui.horizontalSlider_days->hide();
    } else {
        ui.label_3->setVisible(true);
        ui.label_4->setVisible(true);
        ui.spinBox_days->setVisible(true);
        ui.horizontalSlider_days->setVisible(true);
    }
    if (report == 0 or report == 1 or report == 5){
        ui.groupBox_terms->setVisible(true);
    } else {
        ui.groupBox_terms->hide();
        makeReport();
    }
    if (report == 0 or report == 1){
        ui.checkBox_org->setVisible(true);
        ui.comboBox_org->setVisible(true);
        ui.checkBox_period->setVisible(true);
        ui.dateEdit_form->setVisible(true);
        ui.label_2->setVisible(true);
        ui.dateEdit_to->setVisible(true);
    } else {
        ui.checkBox_org->setVisible(false);
        ui.comboBox_org->setVisible(false);
        ui.checkBox_period->setVisible(false);
        ui.dateEdit_form->setVisible(false);
        ui.label_2->setVisible(false);
        ui.dateEdit_to->setVisible(false);
    }
}

void reports::makeReport(){
    //clear
    for (int a = ui.tableWidget_result->rowCount(); a >= 0; a--){
        ui.tableWidget_result->removeRow(a);
    }

    QString query;
    QStringList labels;
    int columns = 0;
    if (report == 0){
        query.append("select docs.id, docs.name, docs.num, docs.date_d, organizations.name, "
                     "(select Count(doc_item.id) from doc_item where doc_item.doc = docs.id) "
                     "from docs, organizations where docs.org = organizations.id and docs.vid = 1 ");
        if (ui.checkBox_org->isChecked()){
            query.append(QString("and organizations.name = \'%1\' ").arg(ui.comboBox_org->currentText()));
        }
        if (ui.checkBox_period->isChecked()){
            query.append(QString("and (docs.date_d between \'%1\' and \'%2\') ")
                         .arg(ui.dateEdit_form->date().toString("yyyy-MM-dd"))
                         .arg(ui.dateEdit_to->date().toString("yyyy-MM-dd")));
        }
        labels << "id" << "Name" << "Num" << "Date" << "Organization" << "Books";
        columns = 6;

    } else if (report == 1){
        query.append("select docs.id, docs.name, docs.num, docs.date_d, organizations.name, "
                     "(select Count(doc_item.id) from doc_item where doc_item.doc = docs.id) "
                     "from docs, organizations where docs.org = organizations.id and docs.vid = 2 ");
        if (ui.checkBox_org->isChecked()){
            query.append(QString("and organizations.name = \'%1\' ").arg(ui.comboBox_org->currentText()));
        }
        if (ui.checkBox_period->isChecked()){
            query.append(QString("and (docs.date_d between \'%1\' and \'%2\') ")
                         .arg(ui.dateEdit_form->date().toString("yyyy-MM-dd"))
                         .arg(ui.dateEdit_to->date().toString("yyyy-MM-dd")));
        }
        labels << "id" << "Name" << "Num" << "Date" << "Organization" << "Books";
        columns = 6;

    } else if (report == 2){
        query.append("select books.id, book_item.identifier, books.isbn, books.title, authors.name_k, publish.name, books.year  "
                     "from books, publish, authors, book_auth, book_item, card_read "
                     "where book_auth.book = books.id and book_auth.auth = authors.id and publish.id = books.pub "
                     "and card_read.book_item = book_item.id and book_item.book = books.id and (card_read.date_e is null) "
                     "group by book_item.identifier ");
        labels << "id" << "Identifier" << "ISBN" << "Title" << "Author" << "Publisher" << "Year";
        columns = 7;
    } else if (report == 3){
        query.append("select books.id, book_item.identifier, books.isbn, books.title, authors.name_k, publish.name, "
                     "books.year, books.place, "
                  "(select Count(doc_item.id) from docs, doc_item where docs.vid = 1 and doc_item.doc = docs.id "
                  "and doc_item.book_item = book_item.id) -  " // приход
                  "(select Count(doc_item.id) from docs, doc_item where docs.vid = 2 and doc_item.doc = docs.id "
                  "and doc_item.book_item = book_item.id ) - " // расход
                  "(select Count(card_read.id) from card_read where (card_read.date_e is null) "
                  "and card_read.book_item = book_item.id ) "//readers' book
                      "from books, publish, authors, book_auth, book_item "
                     "where book_auth.book = books.id and book_auth.auth = authors.id and publish.id = books.pub "
                     "and book_item.book = books.id "
                     "group by book_item.identifier " );
        labels << "id" << "Identifier" << "ISBN" << "Title" << "Author" << "Publisher" << "Year" << "Place" << "Count";
        columns = 9;
    } else if (report == 4){
        query.append("select books.id, books.isbn, books.title, authors.name_k, publish.name, books.year, books.ebook "
                     "from books, authors, book_auth, publish "
                     "where book_auth.book = books.id and book_auth.auth = authors.id and books.pub = publish.id "
                     "and books.ebook is not null "
                     "group by books.id ");
        labels << "id" << "ISBN" << "Title" << "Author" << "Publish" << "Year" << "E-book";
        columns = 7;
    } else if (report == 5){
        QDate dateX = QDate::fromJulianDay(QDate::currentDate().toJulianDay() - ui.spinBox_days->value());
        query.append(QString("select readers.id, readers.fam, readers.ima, readers.otc, card_read.date_s, book_item.identifier, books.title "
                     "from readers, card_read, book_item, books "
                     "where card_read.read = readers.id and card_read.book_item = book_item.id and book_item.book = books.id "
                             "and (card_read.date_e is null) and (card_read.date_s <= \'%1\') ").arg(dateX.toString("yyyy-MM-dd")));
        labels << "id" << "Full Name" << " " <<" " <<"Date" << "Identifier" <<"Title";
        columns = 7;
    } else if (report == 6){
        query.append("");
        labels << " ";
        columns = 0;
    }
    // tables
    ui.tableWidget_result->setColumnCount(columns);
    ui.tableWidget_result->setHorizontalHeaderLabels(labels);
    int row = 0;
    QSqlQuery _q(query);
    while (_q.next()){
        ui.tableWidget_result->insertRow(row);
        for (int col = 0; col < columns; col++){
            QTableWidgetItem *item = new QTableWidgetItem(_q.value(col).toString());
            ui.tableWidget_result->setItem(row, col, item);
        }
        row++;
    }
    ui.tableWidget_result->resizeColumnsToContents();
    ui.tableWidget_result->horizontalHeader()->setStretchLastSection(true);
    ui.tableWidget_result->setColumnHidden(0, true);

    //clearing
    if (report == 3){
        for (int a = ui.tableWidget_result->rowCount()-1; a>=0; a--){
            if (ui.tableWidget_result->item(a, 8)->text().toInt() <= 0){
                ui.tableWidget_result->removeRow(a);
            }
        }
        ui.tableWidget_result->setColumnHidden(8, true);
    }
}
