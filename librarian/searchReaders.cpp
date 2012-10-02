#include "searchReaders.h"
#include "readerCard.h"

sReaders::sReaders(QWidget *parent):QDialog(parent){
    ui.setupUi(this);
    ui.groupBox_column->hide();
    ui.tableWidget_result->setColumnHidden(0, true);
    ui.lineEdit_search->setFocus();

    readSetting();
    selectColumn();

    connect(ui.lineEdit_search, SIGNAL(textEdited(QString)), this, SLOT(search(QString)));
    connect(ui.tableWidget_result, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(toReader()));
    connect(ui.toolButton_col, SIGNAL(clicked(bool)), ui.groupBox_column, SLOT(setVisible(bool)));
}

void sReaders::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("search_readers");
        int _w = node.item(0).attributes().namedItem("width").nodeValue().toInt();
        int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
        int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
        int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
        ui.checkBox_fam->setChecked(node.item(0).attributes().namedItem("fam").nodeValue().toInt());
        ui.checkBox_ima->setChecked(node.item(0).attributes().namedItem("ima").nodeValue().toInt());
        ui.checkBox_otc->setChecked(node.item(0).attributes().namedItem("otc").nodeValue().toInt());
        ui.checkBox_date_r->setChecked(node.item(0).attributes().namedItem("date_r").nodeValue().toInt());
        ui.checkBox_num->setChecked(node.item(0).attributes().namedItem("num").nodeValue().toInt());
        ui.checkBox_address->setChecked(node.item(0).attributes().namedItem("address").nodeValue().toInt());
        ui.checkBox_phone->setChecked(node.item(0).attributes().namedItem("phone").nodeValue().toInt());
        ui.checkBox_readed->setChecked(node.item(0).attributes().namedItem("readed").nodeValue().toInt());
        ui.checkBox_reading->setChecked(node.item(0).attributes().namedItem("reading").nodeValue().toInt());
        setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void sReaders::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();
    QDomNodeList node = doc.elementsByTagName("search_readers");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));
    node.item(0).attributes().namedItem("fam").setNodeValue(QString("%1").arg(ui.checkBox_fam->isChecked()));
    node.item(0).attributes().namedItem("ima").setNodeValue(QString("%1").arg(ui.checkBox_ima->isChecked()));
    node.item(0).attributes().namedItem("otc").setNodeValue(QString("%1").arg(ui.checkBox_otc->isChecked()));
    node.item(0).attributes().namedItem("date_r").setNodeValue(QString("%1").arg(ui.checkBox_date_r->isChecked()));
    node.item(0).attributes().namedItem("num").setNodeValue(QString("%1").arg(ui.checkBox_num->isChecked()));
    node.item(0).attributes().namedItem("address").setNodeValue(QString("%1").arg(ui.checkBox_address->isChecked()));
    node.item(0).attributes().namedItem("phone").setNodeValue(QString("%1").arg(ui.checkBox_phone->isChecked()));
    node.item(0).attributes().namedItem("readed").setNodeValue(QString("%1").arg(ui.checkBox_readed->isChecked()));
    node.item(0).attributes().namedItem("reading").setNodeValue(QString("%1").arg(ui.checkBox_reading->isChecked()));

    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void sReaders::closeEvent(QCloseEvent *){
    writeSetting();
}

void sReaders::selectColumn(){
    ui.tableWidget_result->setColumnHidden(0, true);
    ui.tableWidget_result->setColumnHidden(1, !ui.checkBox_fam->isChecked());
    ui.tableWidget_result->setColumnHidden(2, !ui.checkBox_ima->isChecked());
    ui.tableWidget_result->setColumnHidden(3, !ui.checkBox_otc->isChecked());
    ui.tableWidget_result->setColumnHidden(4, !ui.checkBox_date_r->isChecked());
    ui.tableWidget_result->setColumnHidden(5, !ui.checkBox_num->isChecked());
    ui.tableWidget_result->setColumnHidden(6, !ui.checkBox_address->isChecked());
    ui.tableWidget_result->setColumnHidden(7, !ui.checkBox_phone->isChecked());
    ui.tableWidget_result->setColumnHidden(8, !ui.checkBox_readed->isChecked());
    ui.tableWidget_result->setColumnHidden(9, !ui.checkBox_reading->isChecked());
    ui.tableWidget_result->resizeColumnsToContents();
    ui.tableWidget_result->horizontalHeader()->setStretchLastSection(true);
}

void sReaders::search(const QString text){
    qDebug() << text;
    for (int a = ui.tableWidget_result->rowCount(); a >= 0; a--){
        ui.tableWidget_result->removeRow(a);
    }
    QString s_text("select readers.id, readers.fam, readers.ima, readers.otc, readers.date_r, readers.num, readers.address, readers.phone, "
                   "(select Count(card_read.id) from card_read where card_read.date_e is null and card_read.read = readers.id), "
                   "(select Count(card_read.id) from card_read where card_read.date_e is not null and card_read.read = readers.id) from readers where ");
    if (ui.radioButton_fam->isChecked()){
        s_text.append(QString("readers.fam like \'%1%\' ").arg(text));
    } else if (ui.radioButton_num->isChecked()){
        s_text.append(QString("readers.num like \'%1%\' ").arg(text));
    }
    s_text.append("order by readers.fam, readers.ima ");
    QSqlQuery query(s_text);
    int row = 0;
    while (query.next()){
        ui.tableWidget_result->insertRow(row);
        for (int col = 0; col < 10; col++){
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui.tableWidget_result->setItem(row, col, item);
        }
        row++;
    }
}

void sReaders::toReader(){
    QList<int> list;
    list << ui.tableWidget_result->item(ui.tableWidget_result->currentRow(), 0)->text().toInt();

    readCard *rc = new readCard(list, 0, this);
    rc->exec();
    ui.lineEdit_search->clear();
    ui.lineEdit_search->setFocus();
}
