#include "sect_cat.h"

sect_cat::sect_cat(QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);
    ui.groupBox_cat->hide();
    ui.groupBox_sect->hide();
    ui.tableWidget_cat->setColumnHidden(0, true);
    ui.tableWidget_sect->setColumnHidden(0, true);

    updateSect();
    updateCat();

    readSetting();
    connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(close()));

    connect(ui.actionAddSections, SIGNAL(triggered()), this, SLOT(addSect()));
    connect(ui.pushButton_sect_save, SIGNAL(clicked()), this, SLOT(saveSect()));
    connect(ui.pushButton_sect_del, SIGNAL(clicked()), this, SLOT(delSect()));
    connect(ui.tableWidget_sect, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editSect()));    
    connect(ui.pushButton_sect_close, SIGNAL(clicked()), this, SLOT(closeSect()));
    connect(ui.tableWidget_sect, SIGNAL(clicked(QModelIndex)), this, SLOT(selectSect()));

    connect(ui.actionAddCategory, SIGNAL(triggered()), this, SLOT(addCat()));
    connect(ui.pushButton_cat_save, SIGNAL(clicked()), this, SLOT(saveCat()));
    connect(ui.pushButton_cat_del, SIGNAL(clicked()), this, SLOT(delCat()));
    connect(ui.tableWidget_cat, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editCat()));
    connect(ui.pushButton_cat_close, SIGNAL(clicked()), this, SLOT(closeCat()));
}

void sect_cat::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("cat");
        int _w = node.item(0).attributes().namedItem("width").nodeValue().toInt();
        int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
        int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
        int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
        setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void sect_cat::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();
    QDomNodeList node = doc.elementsByTagName("cat");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));

    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}
void sect_cat::closeEvent(QCloseEvent *){
    writeSetting();
}

void sect_cat::updateSect(){
    ui.comboBox_sect->clear();
    for (int a = ui.tableWidget_sect->rowCount(); a >= 0; a--){
        ui.tableWidget_sect->removeRow(a);
    }
    QSqlQuery q("select sections.id, sections.name from sections ");
    int row = 0;
    while (q.next()){
        ui.tableWidget_sect->insertRow(row);
        for (int col = 0; col < 2; col++){
            QTableWidgetItem *item = new QTableWidgetItem(q.value(col).toString());
            ui.tableWidget_sect->setItem(row, col, item);
        }
        ui.comboBox_sect->addItem(q.value(1).toString());
        row++;
    }
}

void sect_cat::addSect(){
    ui.groupBox_sect->setVisible(true);
    sectID = 0;
    ui.pushButton_sect_del->setEnabled(false);
    ui.lineEdit_sect_name->clear();
    ui.lineEdit_sect_name->setFocus();
}

void sect_cat::editSect(){
    ui.groupBox_sect->setVisible(true);
    sectID = ui.tableWidget_sect->item(ui.tableWidget_sect->currentRow(), 0)->text().toInt();
    ui.lineEdit_sect_name->setText(ui.tableWidget_sect->item(ui.tableWidget_sect->currentRow(), 1)->text());
    QSqlQuery _d(QString("select Count(category.id) from category "
                         "where category.sect = \'%1\' ").arg(sectID));
    _d.next();
    if (_d.value(0).toInt() > 0){
        ui.pushButton_sect_del->setEnabled(false);
    } else {
        ui.pushButton_sect_del->setEnabled(true);
    }
    ui.lineEdit_sect_name->setFocus();
}

void sect_cat::saveSect(){
    if (!ui.lineEdit_sect_name->text().isEmpty()){
        if (sectID == 0){
            QSqlQuery _a("insert into sections (name) values (:name)");
            _a.bindValue(0, ui.lineEdit_sect_name->text());
            _a.exec();
            addSect();
        } else if (sectID > 0){
            QSqlQuery _u(QString("update sections set name = \'%1\' where sections.id = \'%2\' ")
                         .arg(ui.lineEdit_sect_name->text())
                         .arg(sectID));
            _u.exec();
        }
    } else {
        QMessageBox messa;
        messa.setText("Укажите название раздела...");
        messa.exec();
    }
    updateSect();
    updateCat();
}

void sect_cat::delSect(){
    QSqlQuery _d(QString("delete from sections where sections.id = \'%1\' ").arg(sectID));
    _d.exec();
    sectID = 0;
    updateSect();
    updateCat();
    closeSect();
}

void sect_cat::closeSect(){
    ui.groupBox_sect->hide();
}

void sect_cat::selectSect(){
    sectID = ui.tableWidget_sect->item(ui.tableWidget_sect->currentRow(), 0)->text().toInt();
    updateCat();
}

void sect_cat::updateCat(){
    for (int a = ui.tableWidget_cat->rowCount(); a >= 0; a--){
        ui.tableWidget_cat->removeRow(a);
    }
    QString query("select category.id, sections.name, category.name, "
                  "(select Sum(doc_item.coun) from doc_item, books where doc_item.book = books.id and books.cat = category.id) "
                  "from category, sections where category.sect = sections.id ");

    if (sectID > 0){
        query.append(QString("and category.sect = \'%1\' ").arg(sectID));
    }

    QSqlQuery q(query);
    int row = 0;
    while (q.next()){
        ui.tableWidget_cat->insertRow(row);
        for (int col = 0; col < 4; col++){
            QTableWidgetItem *item = new QTableWidgetItem(q.value(col).toString());
            ui.tableWidget_cat->setItem(row, col, item);
        }
        row++;
    }
}

void sect_cat::addCat(){
    ui.groupBox_cat->setVisible(true);
    ui.pushButton_cat_del->setEnabled(false);
    catID = 0;
    ui.comboBox_sect->setCurrentIndex(-1);
    ui.lineEdit_cat_name->clear();
    ui.comboBox_sect->setFocus();
}

void sect_cat::editCat(){
    ui.groupBox_cat->setVisible(true);
    if (ui.tableWidget_cat->item(ui.tableWidget_cat->currentRow(), 3)->text().toInt() == 0){
        ui.pushButton_cat_del->setEnabled(true);
    } else {
        ui.pushButton_cat_del->setEnabled(false);
    }
    catID = ui.tableWidget_cat->item(ui.tableWidget_cat->currentRow(), 0)->text().toInt();
    ui.comboBox_sect->setCurrentIndex(
                ui.comboBox_sect->findText(
                    ui.tableWidget_cat->item(ui.tableWidget_cat->currentRow(), 1)->text()));
    ui.lineEdit_cat_name->setText(
                ui.tableWidget_cat->item(ui.tableWidget_cat->currentRow(), 2)->text());
    ui.comboBox_sect->setFocus();
}

void sect_cat::saveCat(){
    if (ui.comboBox_sect->currentIndex() > -1){
        if (!ui.lineEdit_cat_name->text().isEmpty()){
            QSqlQuery _i(QString("select sections.id from sections where sections.name = \'%1\' ")
                         .arg(ui.comboBox_sect->currentText()));
            _i.next();
            if (catID == 0){
                QSqlQuery _a("insert into category (sect, name) values (:sect, :name)");
                _a.bindValue(0, _i.value(0).toString());
                _a.bindValue(1, ui.lineEdit_cat_name->text());
                _a.exec();
            } else if (catID > 0){
                QSqlQuery _u(QString("update category set sect = \'%1\', name = \'%2\' "
                                     "where category.id = \'%3\' ")
                             .arg(_i.value(0).toInt())
                             .arg(ui.lineEdit_cat_name->text())
                             .arg(catID));
                _u.exec();
            }
        } else {
            QMessageBox messa;
            messa.setText("Укажите название категории...");
            messa.exec();
        }
    } else {
        QMessageBox messa;
        messa.setText("Укажите раздел...");
        messa.exec();
    }
    updateCat();
    updateSect();
}

void sect_cat::delCat(){
    QSqlQuery _d(QString("delete from category where category.id = \'%1\' ").arg(catID));
    _d.exec();
    catID = 0;
    updateCat();
    updateSect();
    closeCat();
}

void sect_cat::closeCat(){
    ui.groupBox_cat->hide();
}
