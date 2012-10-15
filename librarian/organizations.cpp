#include "organizations.h"

org::org(QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);
    id = 0;
    ui.groupBox_card->hide();
    ui.groupBox_column->hide();

    readSettingColumns();
    updateOrg();

    connect(ui.toolButton_column, SIGNAL(clicked(bool)), ui.groupBox_column, SLOT(setVisible(bool)));
    connect (ui.checkBox_address, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect (ui.checkBox_email, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect (ui.checkBox_name, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect (ui.checkBox_note, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect (ui.checkBox_phone, SIGNAL(clicked()), this, SLOT(selectColumn()));
    connect (ui.checkBox_face, SIGNAL(clicked()), this, SLOT(selectColumn()));

    connect(ui.actionAdd, SIGNAL(triggered()), this, SLOT(addOrg()));
    connect(ui.pushButton_save, SIGNAL(clicked()), this, SLOT(saveOrg()));
    connect(ui.pushButton_del, SIGNAL(clicked()), this, SLOT(deleteOrg()));
    connect(ui.tableWidget_org, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editOrg()));

    connect(ui.pushButton_close, SIGNAL(clicked()), ui.groupBox_card, SLOT(hide()));
    connect(ui.actionClose, SIGNAL(triggered()), this , SLOT(close()));    
}

void org::selectColumn(){
    ui.tableWidget_org->setColumnHidden(0, true);
    ui.tableWidget_org->setColumnHidden(1, !ui.checkBox_name->isChecked());
    ui.tableWidget_org->setColumnHidden(2, !ui.checkBox_address->isChecked());
    ui.tableWidget_org->setColumnHidden(3, !ui.checkBox_phone->isChecked());
    ui.tableWidget_org->setColumnHidden(4, !ui.checkBox_email->isChecked());
    ui.tableWidget_org->setColumnHidden(5, !ui.checkBox_face->isChecked());
    ui.tableWidget_org->setColumnHidden(6, !ui.checkBox_note->isChecked());
    ui.tableWidget_org->resizeColumnsToContents();
    ui.tableWidget_org->horizontalHeader()->setStretchLastSection(true);    
}

void org::updateOrg(){
    for (int a = ui.tableWidget_org->rowCount(); a >= 0; a--){
        ui.tableWidget_org->removeRow(a);
    }
    QSqlQuery  q("select organizations.id, organizations.name, organizations.address, organizations.phone, "
                 "organizations.email, organizations.face, organizations.note "
                 "from organizations  where organizations.id > 1");
    int row = 0;
    while (q.next()){
        ui.tableWidget_org->insertRow(row);
        for (int col = 0; col < 7; col++){
            QTableWidgetItem *item = new QTableWidgetItem(q.value(col).toString());
            ui.tableWidget_org->setItem(row, col, item);
        }
        row++;
    }
    selectColumn();
}

void org::addOrg(){
    id = 0;
    ui.groupBox_card->setVisible(true);
    ui.pushButton_del->setEnabled(false);
    ui.lineEdit_address->clear();
    ui.lineEdit_email->clear();
    ui.lineEdit_face->clear();
    ui.lineEdit_name->clear();
    ui.lineEdit_note->clear();
    ui.lineEdit_phone->clear();
    ui.lineEdit_name->setFocus();
}

void org::saveOrg(){
    if (!ui.lineEdit_name->text().isEmpty()){
        if (id == 0){
            QSqlQuery _a("insert into organizations (name, address, phone, email, face, note) "
                         "values (:name, :address, :phone, :email, :face, :note)");
            _a.bindValue(0, ui.lineEdit_name->text());
            _a.bindValue(1, ui.lineEdit_address->text());
            _a.bindValue(2, ui.lineEdit_phone->text());
            _a.bindValue(3, ui.lineEdit_email->text());
            _a.bindValue(4, ui.lineEdit_face->text());
            _a.bindValue(5, ui.lineEdit_note->text());
            _a.exec();
            addOrg();
        } else if (id > 0){
            QSqlQuery _u(QString("update organizations set name = \'%1\', address = \'%2\', phone = \'%3\', email = \'%4\',  "
                                 "face = \'%5\', note = \'%6\' where organizations.id = \'%7\' ")
                         .arg(ui.lineEdit_name->text())
                         .arg(ui.lineEdit_address->text())
                         .arg(ui.lineEdit_phone->text())
                         .arg(ui.lineEdit_email->text())
                         .arg(ui.lineEdit_face->text())
                         .arg(ui.lineEdit_note->text())
                         .arg(id));
            _u.exec();
        }
    } else {
        QMessageBox messa;
        messa.setText("Укажите название организации...");
        messa.exec();
    }
    updateOrg();
}

void org::editOrg(){
    ui.groupBox_card->setVisible(true);
    id = ui.tableWidget_org->item(ui.tableWidget_org->currentRow(), 0)->text().toInt();
    QSqlQuery _e(QString("select t.name, t.address, t.phone, t.email, t.face, t.note "
                         "from organizations t where t.id = \'%1\' ").arg(id));
    _e.next();
    ui.lineEdit_name->setText(_e.value(0).toString());
    ui.lineEdit_address->setText(_e.value(1).toString());
    ui.lineEdit_phone->setText(_e.value(2).toString());
    ui.lineEdit_email->setText(_e.value(3).toString());
    ui.lineEdit_face->setText(_e.value(4).toString());
    ui.lineEdit_note->setText(_e.value(5).toString());
    ui.lineEdit_name->setFocus();

    QSqlQuery dTest(QString("select Count(docs.id) from docs where docs.org = \'%1\' ").arg(id));
    dTest.next();
    if (dTest.value(0).toInt() == 0){
        ui.pushButton_del->setEnabled(true);
    } else {
        ui.pushButton_del->setEnabled(false);
    }
}

void org::deleteOrg(){
    QSqlQuery _d(QString("delete from organizations where organizations.id = \'%1\' ").arg(id));
    _d.exec();
    updateOrg();
    ui.groupBox_card->hide();
}

void org::readSettingColumns(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("org");
        ui.checkBox_name->setChecked(node.item(0).attributes().namedItem("name").nodeValue().toInt());
        ui.checkBox_address->setChecked(node.item(0).attributes().namedItem("address").nodeValue().toInt());
        ui.checkBox_phone->setChecked(node.item(0).attributes().namedItem("phone").nodeValue().toInt());
        ui.checkBox_email->setChecked(node.item(0).attributes().namedItem("email").nodeValue().toInt());
        ui.checkBox_face->setChecked(node.item(0).attributes().namedItem("face").nodeValue().toInt());
        ui.checkBox_note->setChecked(node.item(0).attributes().namedItem("note").nodeValue().toInt());
        int _w = node.item(0).attributes().namedItem("width").nodeValue().toInt();
        int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
        int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
        int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
        setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void org::writeSettingColumns(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();
    QDomNodeList node = doc.elementsByTagName("org");
    node.item(0).attributes().namedItem("name").setNodeValue(QString("%1").arg(ui.checkBox_name->isChecked()));
    node.item(0).attributes().namedItem("address").setNodeValue(QString("%1").arg(ui.checkBox_address->isChecked()));
    node.item(0).attributes().namedItem("phone").setNodeValue(QString("%1").arg(ui.checkBox_phone->isChecked()));
    node.item(0).attributes().namedItem("email").setNodeValue(QString("%1").arg(ui.checkBox_email->isChecked()));
    node.item(0).attributes().namedItem("face").setNodeValue(QString("%1").arg(ui.checkBox_face->isChecked()));
    node.item(0).attributes().namedItem("note").setNodeValue(QString("%1").arg(ui.checkBox_note->isChecked()));
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));

    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void org::closeEvent(QCloseEvent *){
    writeSettingColumns();
}
