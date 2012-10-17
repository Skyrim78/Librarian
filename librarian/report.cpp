#include "reports.h"

reports::reports(int r, QWidget *parent):QDialog(parent){
    ui.setupUi(this);

    ui.listWidget->setCurrentRow(r);
    readSetting();
    selectReport();

    connect(ui.pushButton_close, SIGNAL(clicked()), SLOT(close()));
    connect(ui.listWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(selectReport()));
    //-----report 6
    connect(ui.spinBox_days, SIGNAL(valueChanged(int)), ui.horizontalSlider_days, SLOT(setValue(int)));
    connect(ui.horizontalSlider_days, SIGNAL(valueChanged(int)), ui.spinBox_days, SLOT(setValue(int)));
    //-----report 0, 1
    ui.comboBox_org->setEnabled(false);
    QSqlQuery org("select organizations.name from organizations");
    while (org.next()){
        ui.comboBox_org->addItem(org.value(0).toString());
    }
    connect(ui.checkBox_org, SIGNAL(clicked(bool)), ui.comboBox_org, SLOT(setEnabled(bool)));
    ui.dateEdit_form->setDate(QDate::fromJulianDay(QDate::currentDate().toJulianDay() - 365));
    ui.dateEdit_form->setEnabled(false);
    ui.dateEdit_to->setDate(QDate::currentDate());
    ui.dateEdit_to->setEnabled(false);
    ui.label_2->setEnabled(false);
    connect(ui.checkBox_period, SIGNAL(clicked(bool)), ui.dateEdit_form, SLOT(setEnabled(bool)));
    connect(ui.checkBox_period, SIGNAL(clicked(bool)), ui.dateEdit_to, SLOT(setEnabled(bool)));
    connect(ui.checkBox_period, SIGNAL(clicked(bool)), ui.label_2, SLOT(setEnabled(bool)));

}

void reports::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("reports");
         int _w =  node.item(0).attributes().namedItem("width").nodeValue().toInt();
         int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
         int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
         int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
         setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void reports::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();
    QDomNodeList node = doc.elementsByTagName("reports");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));
    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void reports::closeEvent(QCloseEvent *){
    writeSetting();
}

void reports::selectReport(){
    report = ui.listWidget->currentRow();
    if (report != 5){
        ui.label_3->hide();
        ui.label_4->hide();
        ui.spinBox_days->hide();
        ui.horizontalSlider_days->hide();
    } else {
        ui.label_3->setVisible(true);
        ui.label_4->setVisible(true);
        ui.spinBox_days->setVisible(true);
        ui.horizontalSlider_days->setVisible(true);
    }
    if (report == 0 or report == 1 or report == 5){
        ui.groupBox_terms->setVisible(true);
    } else {
        ui.groupBox_terms->hide();
    }
    if (report == 0 or report == 1){
        ui.checkBox_org->setVisible(true);
        ui.comboBox_org->setVisible(true);
        ui.checkBox_period->setVisible(true);
        ui.dateEdit_form->setVisible(true);
        ui.label_2->setVisible(true);
        ui.dateEdit_to->setVisible(true);
    } else {
        ui.checkBox_org->setVisible(false);
        ui.comboBox_org->setVisible(false);
        ui.checkBox_period->setVisible(false);
        ui.dateEdit_form->setVisible(false);
        ui.label_2->setVisible(false);
        ui.dateEdit_to->setVisible(false);
    }
}

void reports::makeReport(){
    QString query;
    if (report == 0){
        query.append("select docs.id, docs.name, docs.num, docs.date_d, organizations.name, "
                     "(select Sum(doc_item.coun) from doc_item where doc_item.doc = docs.id) "
                     "from docs, organizations where docs.org = organizations.id and docs.vid = \'1\' ");
        if (ui.checkBox_org->isChecked()){
            query.append(QString("and organizations.name = \'%1\' ").arg(ui.comboBox_org->currentText()));
        }
        if (ui.checkBox_period->isChecked()){
            query.append(QString("and (docs.date_d between \'%1\' and \'%2\') ")
                         .arg(ui.dateEdit_form->date().toString("yyyy-MM-dd"))
                         .arg(ui.dateEdit_to->date().toString("yyyy-MM-dd")));
        }
    } else if (report == 1){
        query.append("select docs.id, docs.name, docs.num, docs.date_d, organizations.name, "
                     "(select Sum(doc_item.coun) from doc_item where doc_item.doc = docs.id) "
                     "from docs, organizations where docs.org = organizations.id and docs.vid = \'2\' ");
        if (ui.checkBox_org->isChecked()){
            query.append(QString("and organizations.name = \'%1\' ").arg(ui.comboBox_org->currentText()));
        }
        if (ui.checkBox_period->isChecked()){
            query.append(QString("and (docs.date_d between \'%1\' and \'%2\') ")
                         .arg(ui.dateEdit_form->date().toString("yyyy-MM-dd"))
                         .arg(ui.dateEdit_to->date().toString("yyyy-MM-dd")));
        }
    } else if (report == 2){

    }

}
