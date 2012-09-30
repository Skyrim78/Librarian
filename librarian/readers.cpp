#include "readers.h"
#include "readerCard.h"

readers::readers(QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);

    ui.groupBox_columns->hide();
    readSetting();
    selectColumns();

    updateReaders();

    connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui.toolButton_col, SIGNAL(clicked(bool)), ui.groupBox_columns, SLOT(setVisible(bool)));

    connect (ui.checkBox_fam, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_ima, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_otc, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_data_r, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_num, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_phone, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_address, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_doc, SIGNAL(clicked()), this, SLOT(selectColumns()));

    connect(ui.actionAdd, SIGNAL(triggered()), this, SLOT(addReaders()));
    connect(ui.tableWidget_readers, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openReaders()));
}

void readers::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("readers");
        int _w = node.item(0).attributes().namedItem("width").nodeValue().toInt();
        int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
        int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
        int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
        ui.checkBox_fam->setChecked(node.item(0).attributes().namedItem("fam").nodeValue().toInt());
        ui.checkBox_ima->setChecked(node.item(0).attributes().namedItem("ima").nodeValue().toInt());
        ui.checkBox_otc->setChecked(node.item(0).attributes().namedItem("otc").nodeValue().toInt());
        ui.checkBox_data_r->setChecked(node.item(0).attributes().namedItem("date_r").nodeValue().toInt());
        ui.checkBox_num->setChecked(node.item(0).attributes().namedItem("num").nodeValue().toInt());
        ui.checkBox_phone->setChecked(node.item(0).attributes().namedItem("phone").nodeValue().toInt());
        ui.checkBox_address->setChecked(node.item(0).attributes().namedItem("address").nodeValue().toInt());
        ui.checkBox_doc->setChecked(node.item(0).attributes().namedItem("doc").nodeValue().toInt());
        setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void readers::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();
    QDomNodeList node = doc.elementsByTagName("readers");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));
    node.item(0).attributes().namedItem("fam").setNodeValue(QString("%1").arg(ui.checkBox_fam->isChecked()));
    node.item(0).attributes().namedItem("ima").setNodeValue(QString("%1").arg(ui.checkBox_ima->isChecked()));
    node.item(0).attributes().namedItem("otc").setNodeValue(QString("%1").arg(ui.checkBox_otc->isChecked()));
    node.item(0).attributes().namedItem("date_r").setNodeValue(QString("%1").arg(ui.checkBox_data_r->isChecked()));
    node.item(0).attributes().namedItem("num").setNodeValue(QString("%1").arg(ui.checkBox_num->isChecked()));
    node.item(0).attributes().namedItem("phone").setNodeValue(QString("%1").arg(ui.checkBox_phone->isChecked()));
    node.item(0).attributes().namedItem("address").setNodeValue(QString("%1").arg(ui.checkBox_address->isChecked()));
    node.item(0).attributes().namedItem("doc").setNodeValue(QString("%1").arg(ui.checkBox_doc->isChecked()));
    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void readers::closeEvent(QCloseEvent *){
    writeSetting();
}

void readers::selectColumns(){
    ui.tableWidget_readers->setColumnHidden(0, true);
    ui.tableWidget_readers->setColumnHidden(1, !ui.checkBox_fam->checkState());
    ui.tableWidget_readers->setColumnHidden(2, !ui.checkBox_ima->checkState());
    ui.tableWidget_readers->setColumnHidden(3, !ui.checkBox_otc->checkState());
    ui.tableWidget_readers->setColumnHidden(4, !ui.checkBox_data_r->checkState());
    ui.tableWidget_readers->setColumnHidden(5, !ui.checkBox_num->checkState());
    ui.tableWidget_readers->setColumnHidden(6, !ui.checkBox_phone->checkState());
    ui.tableWidget_readers->setColumnHidden(7, !ui.checkBox_address->checkState());
    ui.tableWidget_readers->setColumnHidden(8, !ui.checkBox_doc->checkState());
    ui.tableWidget_readers->resizeColumnsToContents();
    ui.tableWidget_readers->horizontalHeader()->setStretchLastSection(true);
}

void readers::updateReaders(){
    for (int a = ui.tableWidget_readers->rowCount(); a >= 0; a--){
        ui.tableWidget_readers->removeRow(a);
    }
    QSqlQuery _r("select readers.id, readers.fam, readers.ima, readers.otc, readers.date_r, readers.num, readers.phone, "
                 "readers.address, readers.doc from readers order by readers.fam ");
    int row = 0;
    while (_r.next()){
        ui.tableWidget_readers->insertRow(row);
        for (int col = 0; col < 9; col++){
            QTableWidgetItem *item = new QTableWidgetItem(_r.value(col).toString());
            ui.tableWidget_readers->setItem(row, col, item);
        }
        row++;
    }
}

void readers::addReaders(){
    QList<int> list;
    readCard *rc = new readCard(list, 0, this);
    rc->exec();
    updateReaders();
}

void readers::openReaders(){
    QList<int> list;
    for (int row = 0; row < ui.tableWidget_readers->rowCount(); row++){
        list << ui.tableWidget_readers->item(row, 0)->text().toInt();
    }
    readCard *rc = new readCard(list, ui.tableWidget_readers->currentRow(), this);
    rc->exec();
    updateReaders();
}
