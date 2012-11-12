#include "owner.h"

owner::owner(QWidget *parent):QDialog(parent){
    ui.setupUi(this);

    readSetting();
    openOwner();

    connect(ui.pushButton_clear, SIGNAL(clicked()), this, SLOT(clearOwner()));
    connect(ui.pushButton_save, SIGNAL(clicked()), this, SLOT(saveOwner()));
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
}

void owner::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("owner");
        int _w = node.item(0).attributes().namedItem("width").nodeValue().toInt();
        int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
        int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
        int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
        setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void owner::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();
    QDomNodeList node = doc.elementsByTagName("owner");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));

    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void owner::closeEvent(QCloseEvent *){
    writeSetting();
}

void owner::openOwner(){
    QSqlQuery q("select organizations.name, organizations.address, organizations.phone, organizations.email, "
                "organizations.face, organizations.note from organizations where organizations.id = 1");
    q.next();
    ui.lineEdit_name->setText(q.value(0).toString());
    ui.lineEdit_address->setText(q.value(1).toString());
    ui.lineEdit_phone->setText(q.value(2).toString());
    ui.lineEdit_email->setText(q.value(3).toString());
    ui.lineEdit_face->setText(q.value(4).toString());
    ui.lineEdit_note->setText(q.value(5).toString());
    if (q.lastError().text().size() > 1){
        ui.lineEdit_status->setText(q.lastError().text());
    }
}

void owner::clearOwner(){
    ui.lineEdit_name->clear();
    ui.lineEdit_address->clear();
    ui.lineEdit_phone->clear();
    ui.lineEdit_email->clear();
    ui.lineEdit_face->clear();
    ui.lineEdit_note->clear();
    ui.lineEdit_status->setText(tr("Clear..."));
}

void owner::saveOwner(){
    if (!ui.lineEdit_name->text().isEmpty()){
        QSqlQuery up(QString("update organizations set name = \'%1\', address = \'%2\', phone=\'%3\', email=\'%4\', face=\'%5\', "
                             "note=\'%6\' where organizations.id = 1")
                     .arg(ui.lineEdit_name->text())
                     .arg(ui.lineEdit_address->text())
                     .arg(ui.lineEdit_phone->text())
                     .arg(ui.lineEdit_email->text())
                     .arg(ui.lineEdit_face->text())
                     .arg(ui.lineEdit_note->text()));
        up.exec();
        if (up.lastError().text().size() > 1){
            ui.lineEdit_status->setText(up.lastError().text());
        } else {
            ui.lineEdit_status->setText(tr("Saved ...."));
        }
    } else {
        ui.lineEdit_status->setText(tr("Enter Organization's name ..."));
    }
}
