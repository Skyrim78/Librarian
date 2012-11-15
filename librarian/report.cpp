#include "reports.h"
#include "doc_card.h"
#include "bookCard.h"
#include "readerCard.h"

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

    connect(ui.tableWidget_result, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(toDocsBooksReaders()));

    connect(ui.pushButton_print, SIGNAL(clicked()), this, SLOT(printReport()));
    connect(ui.pushButton_save, SIGNAL(clicked()), this, SLOT(saveReport()));

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


void reports::toDocsBooksReaders(){
    QList<int> list;
    for (int r = 0; r < ui.tableWidget_result->rowCount(); r++){
        list << ui.tableWidget_result->item(r, 0)->text().toInt();
    }
    int curr = ui.tableWidget_result->currentRow();
    if (report == 0 or report == 1){
        docCard *d = new docCard(list, curr, this);
        d->exec();
    } else if (report == 2 or report == 3 or report == 4){
        b_card *bc = new b_card(list, curr, false, this);
        bc->exec();
    } else if (report == 5){
        readCard *rc = new readCard(list, curr, this);
        rc->exec();
    }
}

QString reports::makeDocReport(){
    QString header;
    if (report == 0){
        header.append("Отчет о поступлении книг");
    } else if (report == 1){
        header.append("Отчет о выбытии книг");
    } else if (report == 2){
        header.append("Книги у читателей");
    } else if (report == 3){
        header.append("Книги в библиотеке");
    } else if (report == 4){
        header.append("Элекстронные книги в библиотеке");
    } else if (report == 5){
        header.append("Список задолжников");
    }
    QString header2;
    QString header3;
    if (report == 0 or report == 1){
        if (ui.checkBox_period->isChecked()){
            header2.append(QString("за период с %1 по %2")
                           .arg(ui.dateEdit_form->date().toString("yyyy-MM-dd"))
                                .arg(ui.dateEdit_to->date().toString("yyyy-MM-dd")));
        }
        if (ui.checkBox_org->isChecked()){
            header3.append(QString("по организации: %1").arg(ui.comboBox_org->currentText()));
        }
    }

    QString text;
    text.append("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"> "
                              "<html><head> <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"> "
                "<style type=\"text/css\"> p {border-style: solid; border-width: 2px; border-color: #c00; border-top: none; "
                 "border-left: none; border-right: none; font-size: 12pt;} </style>"
                "</head>"
                               "<body>");
    text.append(QString("<h3>%1</h3>").arg(header));
    if (report == 0 or report == 1){
        text.append(QString("<p><i>%1<br><b>%2</b></i></p>").arg(header2).arg(header3));
    } else {
        text.append("<p></p>");
    }
    text.append("<table cellpadding=10pt; border-top=2pt>" );
    text.append("<tr><th>№</th>");

    for (int col = 1; col < ui.tableWidget_result->columnCount(); col++){
        text.append(QString("<th>%1</th>").arg(ui.tableWidget_result->horizontalHeaderItem(col)->text()));
    }
    text.append("</tr>");
    for (int row = 0; row < ui.tableWidget_result->rowCount(); row++){
        text.append("<tr>");
        text.append(QString("<td>%1</td>").arg(row + 1));
        for (int col = 1; col < ui.tableWidget_result->columnCount(); col++){
            text.append(QString("<td>%1</td>").arg(ui.tableWidget_result->item(row, col)->text()));
        }
    }
    text.append("</table></body></html>");

    return text;
}

void reports::printReport(){
    QPrinter printerA(QPrinterInfo::defaultPrinter());
    printerA.setPageSize(QPrinter::A4);
    printerA.setPageMargins(10.00, 5.00, 10.00, 15.00, QPrinter::Millimeter);
    QPrintPreviewDialog preDialog(&printerA, this);
    connect(&preDialog, SIGNAL(paintRequested(QPrinter*)), SLOT(previewReport(QPrinter*)));
    preDialog.exec();
}

void reports::previewReport(QPrinter *p){
    QTextDocument *textDoc = new QTextDocument();
    textDoc->setHtml(makeDocReport());
    textDoc->print(p);
}

void reports::saveReport(){
    QString fileName(QFileDialog::getSaveFileName(this, tr("Save as..."), "/home", tr("HTML (*.html)")));
    QFile out(fileName);
    out.open(QIODevice::WriteOnly);
    out.write(makeDocReport().toUtf8());
    out.close();
}
