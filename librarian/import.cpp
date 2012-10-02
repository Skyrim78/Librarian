#include "import.h"
#include <QFileDialog>

import::import(QWidget *parent):QDialog(parent){
    ui.setupUi(this);
    ui.comboBox_object->setCurrentIndex(-1);

    connect(ui.comboBox_object, SIGNAL(currentIndexChanged(int)), this, SLOT(selectObject()));
    connect(ui.toolButton_file, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui.pushButton_import, SIGNAL(clicked()), this, SLOT(importData()));

}

void import::selectObject(){
    for (int a = ui.tableWidget->rowCount(); a >= 0; a--){
        ui.tableWidget->removeRow(a);
    }
    QStringList col;
    if (ui.comboBox_object->currentIndex() == 0){
        col << "Фамилия" << "Имя" <<"Отчество";
    } else if (ui.comboBox_object->currentIndex() == 1){
        col << "Название" << "Описание";
    }
    int row = 0;
    for (int x = 0; x < col.size(); x++){
        ui.tableWidget->insertRow(row);
        QTableWidgetItem *item = new QTableWidgetItem(col.at(x));
        ui.tableWidget->setItem(row, 0, item);
        row++;
    }
}

void import::openFile(){
    for (int a = ui.tableWidget_csv->rowCount(); a >= 0; a--){
        ui.tableWidget_csv->removeRow(a);
    }
    QString fileName = QFileDialog::getOpenFileName(this,
         "Выберите файл", "/home", tr("CSV Files (*.csv)"));
    ui.lineEdit_file_name->setText(fileName);

    QFile file(fileName);
    QStringList listA;
    int row = 0;
    if (file.open(QIODevice::ReadOnly)){
        while(!file.atEnd()){
            QString line = file.readLine();
            listA = line.split(QString("%1").arg(ui.comboBox_splin->currentText()));
            ui.tableWidget_csv->setColumnCount(listA.size());
            ui.tableWidget_csv->insertRow(row);
            for (int x = 0; x < listA.size(); x++){
                QTableWidgetItem *item = new QTableWidgetItem(listA.at(x));
                ui.tableWidget_csv->setItem(row, x, item);
            }
            row++;
        }
    }
    file.close();
   ui.spinBox_f_line->setMaximum(ui.tableWidget_csv->rowCount());
}

void import::importData(){
    if (ui.comboBox_object->currentIndex() == 0){
        int c_fam = ui.tableWidget->item(0, 1)->text().toInt() - 1;
        int c_ima = ui.tableWidget->item(1, 1)->text().toInt() - 1;
        int c_otc = ui.tableWidget->item(2, 1)->text().toInt() - 1;
        for (int row = ui.spinBox_f_line->value()-1; row < ui.tableWidget->rowCount(); row++ ){
            QString fam = ui.tableWidget_csv->item(row, c_fam)->text();
            QString ima =  ui.tableWidget_csv->item(row, c_ima)->text();
            QString otc =  ui.tableWidget_csv->item(row, c_otc)->text();
            QString name_k = fam;
            if (ima.size() > 1 and ima != " "){
                name_k.append(QString(" %1.").arg(ima.left(1)));
            }
            if (otc.size() > 1 and otc != " "){
                name_k.append(QString(" %1.").arg(otc.left(1)));
            }
            QSqlQuery testAuth(QString("select authors.id from authors "
                                       "where authors.fam = \'%1\' and authors.ima = \'%2\'  ")
                               .arg(fam).arg(ima));
            testAuth.next();
            if (!testAuth.value(0).isValid()){
                QSqlQuery q_auth("insert into authors (fam, ima, otc, name_k) values (:fam, :ima, :otc, :name_k)");
                q_auth.bindValue(0, fam);
                q_auth.bindValue(1, ima);
                q_auth.bindValue(2, otc);
                q_auth.bindValue(3, name_k);
                q_auth.exec();
            }
        }
    }
}
