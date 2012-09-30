#include "publish.h"

pub::pub(QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);
    id = 0;

    ui.groupBox_card->hide();
    ui.tableWidget_pub->setColumnHidden(0, true);

    readSetting();
    updatePub();
    connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui.actionAdd, SIGNAL(triggered()), this, SLOT(addPub()));
    connect(ui.tableWidget_pub, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editPub()));
    connect(ui.pushButton_save, SIGNAL(clicked()), this, SLOT(savePub()));
    connect(ui.pushButton_del, SIGNAL(clicked()), this, SLOT(deletePub()));
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(closePub()));
}

void pub::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("pub");
        int _w = node.item(0).attributes().namedItem("width").nodeValue().toInt();
        int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
        int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
        int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
        setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void pub::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();
    QDomNodeList node = doc.elementsByTagName("pub");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));

    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void pub::closeEvent(QCloseEvent *){
    writeSetting();
}

void pub::updatePub(){
    for (int a = ui.tableWidget_pub->rowCount(); a >= 0; a--){
        ui.tableWidget_pub->removeRow(a);
    }
    QSqlQuery q("select publish.id, publish.name, "
                "(select Sum(doc_item.coun) from doc_item, books where doc_item.book = books.id and books.pub = publish.id) "
                "from publish ");
    int row = 0;
    while (q.next()){
        ui.tableWidget_pub->insertRow(row);
        for (int col = 0; col < 3; col++){
            QTableWidgetItem *item = new QTableWidgetItem(q.value(col).toString());
            ui.tableWidget_pub->setItem(row, col, item);
        }
        row++;
    }
    ui.tableWidget_pub->resizeColumnsToContents();
    ui.tableWidget_pub->horizontalHeader()->setStretchLastSection(true);
}

void pub::addPub(){
    id = 0;
    ui.groupBox_card->setVisible(true);
    ui.lineEdit_name->clear();
    ui.textEdit_note->clear();
    ui.pushButton_del->setEnabled(true);
    ui.lineEdit_name->setFocus();
}

void pub::editPub(){
    int cRow = ui.tableWidget_pub->currentRow();
    id = ui.tableWidget_pub->item(cRow, 0)->text().toInt();
    ui.groupBox_card->setVisible(true);
    QSqlQuery q(QString("select  publish.name, publish.note from publish where publish.id = \'%1\' ").arg(id));
    q.next();
    ui.lineEdit_name->setText(q.value(0).toString());
    ui.textEdit_note->setPlainText(q.value(1).toString());
    if (ui.tableWidget_pub->item(cRow, 2)->text().toInt() == 0){
        ui.pushButton_del->setEnabled(true);
    } else {
        ui.pushButton_del->setEnabled(false);
    }
    ui.lineEdit_name->setFocus();
}

void pub::savePub(){
    if (!ui.lineEdit_name->text().isEmpty()){
        if (id == 0){
            QSqlQuery _a("insert into publish (name, note) values (:name, :note) ");
            _a.bindValue(0, ui.lineEdit_name->text());
            _a.bindValue(1, ui.textEdit_note->toPlainText());
            _a.exec();
            addPub();
        } else if (id > 0){
            QSqlQuery _u(QString("update publish set name = \'%1\', note = \'%2\' where publish.id = \'%3\' ")
                         .arg(ui.lineEdit_name->text())
                         .arg(ui.textEdit_note->toPlainText())
                         .arg(id));
            _u.exec();
        }
    } else {
        QMessageBox messa;
        messa.setText("Укажите название издательства...");
        messa.exec();
    }
    updatePub();
}

void pub::deletePub(){
    QSqlQuery _d(QString("delete from publish where publish.id = \'%1\' ").arg(id));
    _d.exec();
    updatePub();
    closePub();
}

void pub::closePub(){
    ui.groupBox_card->hide();
}
