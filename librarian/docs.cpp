#include "docs.h"
#include "doc_card.h"

docs::docs(int _v, QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);
    vid = _v;
    ui.groupBox_column->hide();
    ui.groupBox_doc->hide();
    readDocs();
    readSetting();
    selectColumn();

    if (vid == 1){
        setWindowTitle("Приходные документы");
    } else if (vid == 2){
        setWindowTitle("Расходные документы");
    }

    QSqlQuery org("select organizations.name from organizations");
    while (org.next()){
        ui.comboBox_org->addItem(org.value(0).toString());
    }



    connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui.actionAdd, SIGNAL(triggered()), this, SLOT(addDoc()));

    connect(ui.pushButton_save, SIGNAL(clicked()), this, SLOT(saveDoc()));
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(closeDoc()));

    connect(ui.toolButton_col, SIGNAL(clicked(bool)), ui.groupBox_column, SLOT(setVisible(bool)));
    connect(ui.checkBox_name, SIGNAL(clicked(bool)), this, SLOT(selectColumn()));
    connect(ui.checkBox_num, SIGNAL(clicked(bool)), this, SLOT(selectColumn()));
    connect(ui.checkBox_date, SIGNAL(clicked(bool)), this, SLOT(selectColumn()));
    connect(ui.checkBox_org, SIGNAL(clicked(bool)), this, SLOT(selectColumn()));
    connect(ui.checkBox_note, SIGNAL(clicked(bool)), this, SLOT(selectColumn()));
    connect(ui.checkBox_books, SIGNAL(clicked(bool)), this, SLOT(selectColumn()));

    connect(ui.tableWidget_docs, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openDoc()));
}

void docs::closeEvent(QCloseEvent *){
    writeSetting();
}

void docs::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("docs");
         int _w =  node.item(0).attributes().namedItem("width").nodeValue().toInt();
         int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
         int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
         int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
         ui.checkBox_name->setChecked(node.item(0).attributes().namedItem("name").nodeValue().toInt());
         ui.checkBox_date->setChecked(node.item(0).attributes().namedItem("date_d").nodeValue().toInt());
         ui.checkBox_num->setChecked(node.item(0).attributes().namedItem("num").nodeValue().toInt());
         ui.checkBox_org->setChecked(node.item(0).attributes().namedItem("org").nodeValue().toInt());
         ui.checkBox_note->setChecked(node.item(0).attributes().namedItem("note").nodeValue().toInt());
         ui.checkBox_books->setChecked(node.item(0).attributes().namedItem("books").nodeValue().toInt());
         setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void docs::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();
    QDomNodeList node = doc.elementsByTagName("docs");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));
    node.item(0).attributes().namedItem("name").setNodeValue(QString("%1").arg(ui.checkBox_name->isChecked()));
    node.item(0).attributes().namedItem("date_d").setNodeValue(QString("%1").arg(ui.checkBox_date->isChecked()));
    node.item(0).attributes().namedItem("num").setNodeValue(QString("%1").arg(ui.checkBox_num->isChecked()));
    node.item(0).attributes().namedItem("org").setNodeValue(QString("%1").arg(ui.checkBox_org->isChecked()));
    node.item(0).attributes().namedItem("note").setNodeValue(QString("%1").arg(ui.checkBox_note->isChecked()));
    node.item(0).attributes().namedItem("books").setNodeValue(QString("%1").arg(ui.checkBox_books->isChecked()));
    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();

}

void docs::selectColumn(){
    ui.tableWidget_docs->setColumnHidden(0, true);
    ui.tableWidget_docs->setColumnHidden(1, !ui.checkBox_name->isChecked());
    ui.tableWidget_docs->setColumnHidden(3, !ui.checkBox_date->isChecked());
    ui.tableWidget_docs->setColumnHidden(2, !ui.checkBox_num->isChecked());
    ui.tableWidget_docs->setColumnHidden(4, !ui.checkBox_org->isChecked());
    ui.tableWidget_docs->setColumnHidden(5, !ui.checkBox_note->isChecked());
    ui.tableWidget_docs->setColumnHidden(6, !ui.checkBox_books->isChecked());
    ui.tableWidget_docs->resizeColumnsToContents();
    ui.tableWidget_docs->horizontalHeader()->setStretchLastSection(true);
}

void docs::readDocs(){
    for (int a = ui.tableWidget_docs->rowCount(); a >= 0; a--){
        ui.tableWidget_docs->removeRow(a);
    }
    QSqlQuery query(QString("select docs.id, docs.name, docs.num, docs.date_d, organizations.name, docs.note, "
                            "(select Count(doc_item.id) from doc_item where doc_item.doc = docs.id) "
                            "from docs, organizations where organizations.id = docs.org and docs.vid = \'%1\' "
                            "order by docs.date_d").arg(vid));
    int row = 0;
    while (query.next()){
        ui.tableWidget_docs->insertRow(row);
        for (int col = 0; col < 7; col ++){
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui.tableWidget_docs->setItem(row, col, item);
        }
        row++;
    }
}

void docs::addDoc(){
    ui.groupBox_doc->setVisible(true);
    ui.dateEdit_doc->setDate(QDate::currentDate());
    ui.comboBox_org->setCurrentIndex(-1);
}

void docs::closeDoc(){
    ui.lineEdit_name->clear();
    ui.lineEdit_num->clear();
    ui.dateEdit_doc->setDate(QDate::currentDate());
    ui.comboBox_org->setCurrentIndex(-1);
    ui.groupBox_doc->hide();
}

void docs::saveDoc(){
    QSqlQuery queryOrg(QString("select organizations.id from organizations where organizations.name = \'%1\' ")
                       .arg(ui.comboBox_org->currentText()));
    queryOrg.next();
    QSqlQuery query(QString("insert into docs (name, num, date_d, org, vid) "
                            "values (:name, :num, :date_d, :org, :vid)"));
    query.bindValue(0, ui.lineEdit_name->text());
    query.bindValue(1, ui.lineEdit_num->text());
    query.bindValue(2, ui.dateEdit_doc->date().toString("yyyy-MM-dd"));
    query.bindValue(3, queryOrg.value(0).toString());
    query.bindValue(4, QString("%1").arg(vid));
    query.exec();

    QSqlQuery queryID("select Max(docs.id) from docs");
    queryID.next();
    QList<int> listDocs;
    listDocs << queryID.value(0).toInt();

    closeDoc();
    docCard *dc = new docCard(listDocs, 0, this);
    dc->exec();
    readDocs();
}

void docs::openDoc(){
    QList<int> listDocs;
    for (int a = 0; a < ui.tableWidget_docs->rowCount(); a++){
        listDocs << ui.tableWidget_docs->item(a, 0)->text().toInt();
    }
    docCard *dc = new docCard(listDocs, ui.tableWidget_docs->currentRow(), this);
    dc->exec();
    readDocs();
}




