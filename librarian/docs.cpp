#include "docs.h"

docs::docs(int _v, QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);
    vid = _v;
    ui.groupBox_column->hide();
    readSetting();
    selectColumn();

    if (vid == 1){
        setWindowTitle("Приходные документы");
    } else if (vid == 2){
        setWindowTitle("Расходные документы");
    }

    connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui.actionAdd, SIGNAL(triggered()), this, SLOT(addDoc()));

    connect(ui.toolButton_col, SIGNAL(clicked(bool)), ui.groupBox_column, SLOT(setVisible(bool)));
    connect(ui.checkBox_name, SIGNAL(clicked(bool)), this, SLOT(selectColumn()));
    connect(ui.checkBox_num, SIGNAL(clicked(bool)), this, SLOT(selectColumn()));
    connect(ui.checkBox_date, SIGNAL(clicked(bool)), this, SLOT(selectColumn()));
    connect(ui.checkBox_org, SIGNAL(clicked(bool)), this, SLOT(selectColumn()));
    connect(ui.checkBox_note, SIGNAL(clicked(bool)), this, SLOT(selectColumn()));


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
    ui.tableWidget_docs->resizeColumnsToContents();
    ui.tableWidget_docs->horizontalHeader()->setStretchLastSection(true);
}

void docs::readDocs(){
    for (int a = ui.tableWidget_docs->rowCount(); a >= 0; a--){
        ui.tableWidget_docs->removeRow(a);
    }
    QSqlQuery query(QString("select docs.id, docs.name, docs.num, docs.date_d, docs.org, docs.note "
                            "from docs where docs.vid = \'%1\' ").arg(vid));
    int row = 0;
    while (query.next()){
        ui.tableWidget_docs->insertRow(row);
        for (int col = 0; col < 5; col ++){
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui.tableWidget_docs->setItem(row, col, item);
        }
        row++;
    }
}

void docs::addDoc(){

}

void docs::openDoc(){

}




