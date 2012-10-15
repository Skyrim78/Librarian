#include "search.h"

search::search(QWidget *parent):QDialog(parent){
    ui.setupUi(this);
    ui.groupBox_search->hide();
    QButtonGroup *groupA = new QButtonGroup();
    groupA->addButton(ui.radioButton_ravno);
    groupA->addButton(ui.radioButton_bol);
    //groupA->addButton(ui.radioButton_diap);
    groupA->addButton(ui.radioButton_men);
    groupA->addButton(ui.radioButton_null);
    ui.radioButton_ravno->setChecked(true);
    QButtonGroup *groupB = new QButtonGroup();
    groupB->addButton(ui.radioButton_and);
    groupB->addButton(ui.radioButton_or);
    ui.radioButton_and->setChecked(true);
    queryString.append("select distinct books.id, books.isbn, books.title, sections.name, category.name, authors.name_k, publish.name, "
                       "books.year, languages.name, books.place "
                       "from books, sections, category, authors, book_auth, publish, languages "
                       "where sections.id = books.sect and category.id = books.cat and book_auth.book = books.id and "
                       "book_auth.auth = authors.id and publish.id = books.pub and languages.id = books.lang ");

    readSetting();
    connect(ui.listWidget_tables, SIGNAL(currentRowChanged(int)), this, SLOT(selectPosition()));
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));

    connect(ui.pushButton_add, SIGNAL(clicked()), this, SLOT(makeQuery()));
    connect(ui.pushButton_run, SIGNAL(clicked()), this, SLOT(runQuery()));
    connect(ui.pushButton_clear, SIGNAL(clicked()), this, SLOT(clearQuery()));
    connect(ui.pushButton_del, SIGNAL(clicked()), this, SLOT(delItemQuery()));
    //
    connect(ui.tableWidget_items, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(delItemQuery()));
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

    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void search::closeEvent(QCloseEvent *){
    writeSetting();
}

void search::selectColumn(){

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
    if (ui.radioButton_and->isChecked()){
        preQuery.append("and ");
    } else if (ui.radioButton_or->isChecked()){
        preQuery.append("or ");
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

    ui.tableWidget_items->insertRow(ui.tableWidget_items->rowCount());
    QTableWidgetItem *item = new QTableWidgetItem(preQuery);
    ui.tableWidget_items->setItem(ui.tableWidget_items->rowCount()-1, 0, item);
}

void search::runQuery(){
    for (int a = ui.tableWidget_result->rowCount(); a >= 0; a--){
        ui.tableWidget_result->removeRow(a);
    }
    for (int row = 0; row < ui.tableWidget_items->rowCount(); row++){
        queryString.append(ui.tableWidget_items->item(row, 0)->text());
    }
    qDebug() << queryString;
    QSqlQuery mainQuery(queryString);

    int row = 0;
    while (mainQuery.next()){
        ui.tableWidget_result->insertRow(row);
        for (int col = 0; col < 4; col++){
            QTableWidgetItem *item = new QTableWidgetItem(mainQuery.value(col).toString());
            ui.tableWidget_result->setItem(row, col, item);
        }
        row++;
    }
    ui.tabWidget->setCurrentIndex(1);



}

void search::clearQuery(){
    for (int a = ui.tableWidget_items->rowCount(); a >=0; a--){
        ui.tableWidget_items->removeRow(a);
    }
}

void search::delItemQuery(){
    int row = ui.tableWidget_items->currentRow();
    ui.tableWidget_items->removeRow(row);
}


