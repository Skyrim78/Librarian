/*
  **Конструктор запросов**
  Реализация конструктора запросов для поиска по книгам.

-- Ev -- 16.10.2012 -- Skyrim78@yandex.ru --
 */
#include "search.h"
#include <QTextDocument>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintPreviewDialog>
#include <QFileDialog>

search::search(QWidget *parent):QDialog(parent){
    ui.setupUi(this);
    ui.groupBox_search->hide();
    ui.groupBox_column->hide();
    ui.groupBox_result_title->hide();
    ui.lineEdit_res_title->setEnabled(false);
    QButtonGroup *groupA = new QButtonGroup();
    groupA->addButton(ui.radioButton_ravno);
    groupA->addButton(ui.radioButton_bol);
    groupA->addButton(ui.radioButton_men);
    groupA->addButton(ui.radioButton_null);
    ui.radioButton_ravno->setChecked(true);
    QButtonGroup *groupB = new QButtonGroup();
    groupB->addButton(ui.radioButton_and);
    groupB->addButton(ui.radioButton_or);
    ui.radioButton_and->setChecked(true);
    ui.tableWidget_result->setColumnHidden(0, true);


    readSetting();
    selectColumn();
    connect(ui.listWidget_tables, SIGNAL(clicked(QModelIndex)), this, SLOT(selectPosition()));
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));

    connect(ui.pushButton_add, SIGNAL(clicked()), this, SLOT(makeQuery()));
    connect(ui.pushButton_run, SIGNAL(clicked()), this, SLOT(runQuery()));
    connect(ui.pushButton_clear, SIGNAL(clicked()), this, SLOT(clearQuery()));
    connect(ui.tableWidget_items, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(delItemQuery()));
    connect(ui.tableWidget_pre_items, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(delPreItemQuery()));
    connect(ui.pushButton_ok, SIGNAL(clicked()), this, SLOT(makeQuery_A()));
    connect(ui.pushButton_cancel, SIGNAL(clicked()), this, SLOT(close_makeQuery()));
    connect(ui.toolButton_col, SIGNAL(clicked(bool)), ui.groupBox_column, SLOT(setVisible(bool)));
    //
    connect(ui.checkBox_isbn, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect(ui.checkBox_title, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect(ui.checkBox_sect, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect(ui.checkBox_cat, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect(ui.checkBox_auth, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect(ui.checkBox_pub, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect(ui.checkBox_year, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect(ui.checkBox_lang, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect(ui.checkBox_place, SIGNAL(clicked()), this, SLOT(selectColumn()));
    //
    connect(ui.pushButton_print, SIGNAL(clicked()), SLOT(printResult()));
    connect(ui.toolButton_result, SIGNAL(clicked(bool)), ui.groupBox_result_title, SLOT(setVisible(bool)));
    connect(ui.checkBox_res_title, SIGNAL(clicked(bool)), ui.lineEdit_res_title, SLOT(setEnabled(bool)));
    //
    connect(ui.pushButton_save, SIGNAL(clicked()), this, SLOT(saveResult()));
    //
    connect(ui.tableWidget_result, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(toBook()));
}

void search::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("search");
        int _w = node.item(0).attributes().namedItem("width").nodeValue().toInt();
        int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
        int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
        int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
        ui.checkBox_isbn->setChecked(node.item(0).attributes().namedItem("isbn").nodeValue().toInt());
        ui.checkBox_title->setChecked(node.item(0).attributes().namedItem("title").nodeValue().toInt());
        ui.checkBox_sect->setChecked(node.item(0).attributes().namedItem("sect").nodeValue().toInt());
        ui.checkBox_cat->setChecked(node.item(0).attributes().namedItem("cat").nodeValue().toInt());
        ui.checkBox_auth->setChecked(node.item(0).attributes().namedItem("auth").nodeValue().toInt());
        ui.checkBox_pub->setChecked(node.item(0).attributes().namedItem("pub").nodeValue().toInt());
        ui.checkBox_year->setChecked(node.item(0).attributes().namedItem("year").nodeValue().toInt());
        ui.checkBox_lang->setChecked(node.item(0).attributes().namedItem("lang").nodeValue().toInt());
        ui.checkBox_place->setChecked(node.item(0).attributes().namedItem("place").nodeValue().toInt());
        setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void search::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();
    QDomNodeList node = doc.elementsByTagName("search");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));
    node.item(0).attributes().namedItem("isbn").setNodeValue(QString("%1").arg(ui.checkBox_isbn->isChecked()));
    node.item(0).attributes().namedItem("title").setNodeValue(QString("%1").arg(ui.checkBox_title->isChecked()));
    node.item(0).attributes().namedItem("sect").setNodeValue(QString("%1").arg(ui.checkBox_sect->isChecked()));
    node.item(0).attributes().namedItem("cat").setNodeValue(QString("%1").arg(ui.checkBox_cat->isChecked()));
    node.item(0).attributes().namedItem("auth").setNodeValue(QString("%1").arg(ui.checkBox_auth->isChecked()));
    node.item(0).attributes().namedItem("pub").setNodeValue(QString("%1").arg(ui.checkBox_pub->isChecked()));
    node.item(0).attributes().namedItem("year").setNodeValue(QString("%1").arg(ui.checkBox_year->isChecked()));
    node.item(0).attributes().namedItem("lang").setNodeValue(QString("%1").arg(ui.checkBox_lang->isChecked()));
    node.item(0).attributes().namedItem("place").setNodeValue(QString("%1").arg(ui.checkBox_place->isChecked()));
    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void search::closeEvent(QCloseEvent *){
    writeSetting();
}

void search::selectColumn(){
    ui.tableWidget_result->setColumnHidden(0, true);
    ui.tableWidget_result->setColumnHidden(1, !ui.checkBox_isbn->isChecked());
    ui.tableWidget_result->setColumnHidden(2, !ui.checkBox_title->isChecked());
    ui.tableWidget_result->setColumnHidden(3, !ui.checkBox_sect->isChecked());
    ui.tableWidget_result->setColumnHidden(4, !ui.checkBox_cat->isChecked());
    ui.tableWidget_result->setColumnHidden(5, !ui.checkBox_auth->isChecked());
    ui.tableWidget_result->setColumnHidden(6, !ui.checkBox_pub->isChecked());
    ui.tableWidget_result->setColumnHidden(7, !ui.checkBox_year->isChecked());
    ui.tableWidget_result->setColumnHidden(8, !ui.checkBox_lang->isChecked());
    ui.tableWidget_result->setColumnHidden(9, !ui.checkBox_place->isChecked());
    ui.tableWidget_result->resizeColumnsToContents();
    ui.tableWidget_result->horizontalHeader()->setStretchLastSection(true);
}

void search::selectPosition(){
    ui.groupBox_search->setVisible(true);
    int cP = ui.listWidget_tables->currentRow();
    ui.comboBox->clear();
    if (cP == 2){
        QSqlQuery q("select sections.name from sections");
        while (q.next()){
            ui.comboBox->addItem(q.value(0).toString());
        }
        ui.comboBox->setVisible(true);
        ui.lineEdit->hide();
    } else if (cP == 3){
        QSqlQuery q("select category.name from category");
        while (q.next()){
            ui.comboBox->addItem(q.value(0).toString());
        }
        ui.comboBox->setVisible(true);
        ui.lineEdit->hide();
    } else if (cP == 4){
        QSqlQuery q("select authors.name_k from authors");
        while (q.next()){
            ui.comboBox->addItem(q.value(0).toString());
        }
        ui.comboBox->setVisible(true);
        ui.lineEdit->hide();
    } else if (cP == 5){
        QSqlQuery q("select publish.name from publish");
        while (q.next()){
            ui.comboBox->addItem(q.value(0).toString());
        }
        ui.comboBox->setVisible(true);
        ui.lineEdit->hide();
    } else if (cP == 7){
        QSqlQuery q("select languages.name from languages");
        while (q.next()){
            ui.comboBox->addItem(q.value(0).toString());
        }
        ui.comboBox->setVisible(true);
        ui.lineEdit->hide();
    } else if (cP == 0 or cP == 1 or cP == 6 or cP == 8){
        ui.comboBox->hide();
        ui.lineEdit->clear();
        ui.lineEdit->setVisible(true);
    }
}

void search::makeQuery(){
    QString preQuery;
    if (ui.tableWidget_pre_items->rowCount() >= 1){
        if (ui.radioButton_and->isChecked()){
            preQuery.append("and ");
        } else if (ui.radioButton_or->isChecked()){
            preQuery.append("or ");
        }
    }
    if (ui.listWidget_tables->currentRow() == 0){
        preQuery.append("books.isbn ");
    } else if (ui.listWidget_tables->currentRow() == 1){
        preQuery.append("books.title ");
    } else if (ui.listWidget_tables->currentRow() == 2){
        preQuery.append("sections.name ");
    } else if (ui.listWidget_tables->currentRow() == 3){
        preQuery.append("category.name ");
    } else if (ui.listWidget_tables->currentRow() == 4){
        preQuery.append("authors.name_k ");
    } else if (ui.listWidget_tables->currentRow() == 5){
        preQuery.append("publish.name ");
    } else if (ui.listWidget_tables->currentRow() == 6){
        preQuery.append("books.year ");
    } else if (ui.listWidget_tables->currentRow() == 7){
        preQuery.append("languages.name ");
    } else if (ui.listWidget_tables->currentRow() == 8){
        preQuery.append("books.place ");
    }
    if (ui.radioButton_ravno->isChecked()){
        if (ui.checkBox_not->isChecked()){
            if (ui.listWidget_tables->currentRow() == 0
                    or ui.listWidget_tables->currentRow() == 1
                        or ui.listWidget_tables->currentRow() == 8){
                preQuery.append("is not ");
            } else {
                preQuery.append("!= ");
            }
        } else {
            if (ui.listWidget_tables->currentRow() == 0
                    or ui.listWidget_tables->currentRow() == 1
                        or ui.listWidget_tables->currentRow() == 8){
                preQuery.append("like ");
            } else {
                preQuery.append("= ");
            }
        }
    } else if (ui.radioButton_bol->isChecked()){
        preQuery.append(">= ");
    } else if (ui.radioButton_men->isChecked()){
        preQuery.append("<= ");
    } else if (ui.radioButton_null->isChecked()){
        if (ui.checkBox_not->isChecked()){
            preQuery.append("is not null ");
        } else {
            preQuery.append("is null ");
        }
    }
    if (!ui.radioButton_null->isChecked()){
        if (ui.listWidget_tables->currentRow() == 0
                or ui.listWidget_tables->currentRow() == 1
                    or ui.listWidget_tables->currentRow() == 6
                        or ui.listWidget_tables->currentRow() == 8){
            preQuery.append(QString("\'%%1%\'  ").arg(ui.lineEdit->text()));
        } else {
            preQuery.append(QString("\'%1\'  ").arg(ui.comboBox->currentText()));
        }
    }

    ui.tableWidget_pre_items->insertRow(ui.tableWidget_pre_items->rowCount());
    QTableWidgetItem *item = new QTableWidgetItem(preQuery);
    ui.tableWidget_pre_items->setItem(ui.tableWidget_pre_items->rowCount()-1, 0, item);
}
void search::makeQuery_A(){
    QString queryA = "and (";
    for (int a = 0; a < ui.tableWidget_pre_items->rowCount(); a++){
        queryA.append(ui.tableWidget_pre_items->item(a, 0)->text());
    }
    queryA.append(")");
    ui.tableWidget_items->insertRow(ui.tableWidget_items->rowCount());
    QTableWidgetItem *item = new QTableWidgetItem(queryA);
    ui.tableWidget_items->setItem(ui.tableWidget_items->rowCount()-1, 0, item);
    close_makeQuery();
}

void search::close_makeQuery(){
    for (int a = ui.tableWidget_pre_items->rowCount(); a >=0; a--){
        ui.tableWidget_pre_items->removeRow(a);
    }
    ui.groupBox_search->hide();
}

void search::runQuery(){
    QString queryString;
    queryString.append("select distinct books.id, books.isbn, books.title, sections.name, category.name, authors.name_k, publish.name, "
                       "books.year, languages.name, books.place "
                       "from books, sections, category, authors, book_auth, publish, languages "
                       "where sections.id = books.sect and category.id = books.cat and book_auth.book = books.id and "
                       "book_auth.auth = authors.id and publish.id = books.pub and languages.id = books.lang ");
    for (int a = ui.tableWidget_result->rowCount(); a >= 0; a--){
        ui.tableWidget_result->removeRow(a);
    }
    for (int row = 0; row < ui.tableWidget_items->rowCount(); row++){
        queryString.append(ui.tableWidget_items->item(row, 0)->text());
    }
    queryString.append(" group by books.id ");
    QSqlQuery mainQuery(queryString);

    int row = 0;
    while (mainQuery.next()){
        ui.tableWidget_result->insertRow(row);
        for (int col = 0; col < 10; col++){
            QTableWidgetItem *item = new QTableWidgetItem(mainQuery.value(col).toString());
            ui.tableWidget_result->setItem(row, col, item);
        }
        row++;
    }
    ui.tableWidget_result->resizeColumnsToContents();
    ui.tableWidget_result->horizontalHeader()->setStretchLastSection(true);
    ui.tabWidget->setCurrentIndex(1);
    qString = QString("<p>SQL [%1]<br>%2</p>").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(queryString);
}

void search::clearQuery(){
    for (int a = ui.tableWidget_items->rowCount(); a >=0; a--){
        ui.tableWidget_items->removeRow(a);
    }
}

void search::delPreItemQuery(){
    int row = ui.tableWidget_pre_items->currentRow();
    ui.tableWidget_pre_items->removeRow(row);
}

void search::delItemQuery(){
    int row = ui.tableWidget_items->currentRow();
    ui.tableWidget_items->removeRow(row);
}

/////////////////////////////////////////////////////////////*make result*///////////
QString search::makeDocResult(){
    QString text;
    text.append("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"> "
                              "<html><head> <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
                "<style type=\"text/css\">h3, p {border-style: solid; border-width: 2px; border-color: #c00; border-top: none; "
                 "border-left: none; border-right: none;} p{font-size: 9pt;} </style>"
                "</head>"
                               "<body>");
    if (ui.checkBox_res_title->isChecked()){
        text.append(QString("<h3>%1</h3>").arg(ui.lineEdit_res_title->text()));
    }
    if (ui.checkBox_res_sql->isChecked()){
        text.append(qString);
    }
    text.append("<table cellpadding=10pt;>" );
    text.append("<tr>");
    if (ui.checkBox_isbn->isChecked()){
        text.append(tr("<th>ISBN</th>"));
    }
    if (ui.checkBox_title->isChecked()){
        text.append(tr("<th>Title</th>"));
    }
    if (ui.checkBox_sect->isChecked()){
        text.append(tr("<th>Section</th>"));
    }
    if (ui.checkBox_cat->isChecked()){
        text.append(tr("<th>Category</th>"));
    }
    if (ui.checkBox_auth->isChecked()){
        text.append(tr("<th>Author</th>"));
    }
    if (ui.checkBox_pub->isChecked()){
        text.append(tr("<th>Publisher</th>"));
    }
    if (ui.checkBox_year->isChecked()){
        text.append(tr("<th>Year</th>"));
    }
    if (ui.checkBox_lang->isChecked()){
        text.append(tr("<th>Language</th>"));
    }
    if (ui.checkBox_place->isChecked()){
        text.append(tr("<th>Place</th>"));
    }
    text.append("</tr>");
    for (int row = 0; row < ui.tableWidget_result->rowCount(); row++){
        text.append("<tr>");
        if (ui.checkBox_isbn->isChecked()){
            text.append(QString("<td>%1</td>").arg(ui.tableWidget_result->item(row, 1)->text()));
        }
        if (ui.checkBox_title->isChecked()){
            text.append(QString("<td>%1</td>").arg(ui.tableWidget_result->item(row, 2)->text()));
        }
        if (ui.checkBox_sect->isChecked()){
            text.append(QString("<td>%1</td>").arg(ui.tableWidget_result->item(row, 3)->text()));
        }
        if (ui.checkBox_cat->isChecked()){
            text.append(QString("<td>%1</td>").arg(ui.tableWidget_result->item(row, 4)->text()));
        }
        if (ui.checkBox_auth->isChecked()){
            text.append(QString("<td>%1</td>").arg(ui.tableWidget_result->item(row, 5)->text()));
        }
        if (ui.checkBox_pub->isChecked()){
            text.append(QString("<td>%1</td>").arg(ui.tableWidget_result->item(row, 6)->text()));
        }
        if (ui.checkBox_year->isChecked()){
            text.append(QString("<td>%1</td>").arg(ui.tableWidget_result->item(row, 7)->text()));
        }
        if (ui.checkBox_lang->isChecked()){
            text.append(QString("<td>%1</td>").arg(ui.tableWidget_result->item(row, 8)->text()));
        }

        if (ui.checkBox_place->isChecked()){
            text.append(QString("<td>%1</td>").arg(ui.tableWidget_result->item(row, 9)->text()));
        }

    }
    text.append("</table></body></html>");
    return text;
}

void search::printResult(){
    QPrinter printerA(QPrinterInfo::defaultPrinter());
    printerA.setPageSize(QPrinter::A4);
    printerA.setPageMargins(10.00, 5.00, 10.00, 15.00, QPrinter::Millimeter);
    QPrintPreviewDialog preDialog(&printerA, this);
    connect(&preDialog, SIGNAL(paintRequested(QPrinter*)), SLOT(previewResult(QPrinter*)));
    preDialog.exec();
}

void search::previewResult(QPrinter *p){
    QTextDocument *textDoc = new QTextDocument();
    textDoc->setHtml(makeDocResult());
    textDoc->print(p);
}

void search::toBook(){
    int cRow = ui.tableWidget_result->currentRow();
    QList<int> _l;
    for (int row = 0; row < ui.tableWidget_result->rowCount(); row++){
        _l << ui.tableWidget_result->item(row, 0)->text().toInt();
    }
    b_card *bc = new b_card(_l, cRow, false, this);
    bc->exec();
}

void search::saveResult(){
    QString fileName(QFileDialog::getSaveFileName(this, tr("Save as..."), "/home", tr("HTML (*.html)")));
    QFile out(fileName);
    out.open(QIODevice::WriteOnly);
    out.write(makeDocResult().toUtf8());
    out.close();
}

