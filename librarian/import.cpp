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
    if (ui.comboBox_object->currentIndex() == 0){ //authors
        col << "Фамилия" << "Имя" <<"Отчество";
    } else if (ui.comboBox_object->currentIndex() == 6){ //organizations
        col << "Название" << "Адрес" <<"Телефон" <<"Контактное лицо"<< "E-mail" << "Описание";
    } else if (ui.comboBox_object->currentIndex() == 1){ //publish
        col << "Название"<< "Описание";
    } else if (ui.comboBox_object->currentIndex() == 2){//section
        col << "Название";
    } else if (ui.comboBox_object->currentIndex() == 3){//category
        col << "Название" << "Раздел";
    } else if (ui.comboBox_object->currentIndex() == 4){//books
        col << "Название" << "Раздел" << "Категория" << "Издательство" << "Год" << "ISBN"<<"Место хранения"
            <<"Язык"<<"Описание"<< "Автор 1"<<"Автор 2"<<"Автор 3";
    } else if (ui.comboBox_object->currentIndex() == 5){//readers
        col << "Фамилия" <<"Имя"<<"Отчество"<<"Дата рождения"<<"Номер"<<"Адрес"<<"Телефон"<<"Документ";
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
    QString test; // тест на наличие в бд
    QString query; // запрос на ввод в бд
    QList<int> col; // номера колонок
    int column = ui.tableWidget->rowCount(); //кол-во колонок
    for (int a = 0; a < ui.tableWidget->rowCount(); a++){
        col << ui.tableWidget->item(a, 1)->text().toInt() - 1;
    }
    for (int row = ui.spinBox_f_line->value() - 1; row < ui.tableWidget_csv->rowCount(); row++){
        QStringList content;
        for (int a = 0; a < col.size(); a++){
            content << ui.tableWidget_csv->item(row, col.at(a))->text();
        }
        if (ui.comboBox_object->currentIndex() == 0){ // name_k for authors
            QString name_k = content.at(0);
            if (content.at(1).size() > 1 and content.at(1) != " "){
                name_k.append(QString(" %1.").arg(content.at(1).left(1)));
            }
            if (content.at(2).size() > 1 and content.at(2) != " "){
                name_k.append(QString(" %1.").arg(content.at(2).left(1)));
            }
            content << name_k;
        }
        if (ui.comboBox_object->currentIndex() == 3){//section for category
            QSqlQuery qIdSection(QString("select sections.id from sections where sections.name = \'%1\'").arg(content.at(1)));
            qIdSection.next();
            if (qIdSection.value(0).isValid()){
                content.replaceInStrings(content.at(1), qIdSection.value(0).toString());
            }
        }
        if (ui.comboBox_object->currentIndex() == 4){ //id's for books
            QSqlQuery qIdSection(QString("select sections.id from sections where sections.name = \'%1\'").arg(content.at(1)));
            qIdSection.next();
            if (qIdSection.value(0).isValid()){
                content.replaceInStrings(content.at(1), qIdSection.value(0).toString());
            }
            QSqlQuery qIdCat(QString("select category.id from category where category.name = \'%1\' ").arg(content.at(2)));
            qIdCat.next();
            if (qIdCat.value(0).isValid()){
                content.replaceInStrings(content.at(2), qIdCat.value(0).toString());
            }
            QSqlQuery qIdPub(QString("select publish.id from publish where publish.name = \'%1\' ").arg(content.at(3)));
            qIdPub.next();
            if (qIdPub.value(0).isValid()){
                content.replaceInStrings(content.at(3), qIdPub.value(0).toString());
            }
        }
        if (ui.comboBox_object->currentIndex() == 0){ // authors
            column = 4;
            test = QString("select authors.id from authors where authors.fam = \'%1\' and authors.ima = \'%2\'  ")
                         .arg(content.at(0)).arg(content.at(1));
            query = QString("insert into authors (fam, ima, otc, name_k) values (:fam, :ima, :otc, :name_k)");
        } else if (ui.comboBox_object->currentIndex() == 1) { //publish
            test = QString("select public.id from publish where publish.name = \'%1\' ").arg(content.at(0));
            query = QString("insert into publish (name, note) values (:name, :note)");
        } else if (ui.comboBox_object->currentIndex() == 2){//sections
            test = QString("select sections.id from setcions where sections.name = \'%1\'").arg(content.at(0));
            query = QString("insert into sections (name) values (:name)");
        } else if (ui.comboBox_object->currentIndex() == 3){//category
            test = QString("select category.id from category where category.name = \'%1\'").arg(content.at(0));
            query = QString("insert into category (name, sect) values (:name, :sect)");
        } else if (ui.comboBox_object->currentIndex() == 4){ // books
            column = 9;
            test = QString("select books.id from books where books.title = \'%1\' and books.isbn = \'%2\' ").arg(content.at(0)).arg(content.at(5));
            query = QString("insert into books (title, sect, cat, pub, year, isbn, place, lang, note) "
                            "values (:title, :sect, :cat, :pub, :year, :isbn, :place, :lang, :note)");
        } else if (ui.comboBox_object->currentIndex() == 5){//readers
            test = QString("select readers.id from readers where readers.fam = \'%1\' and readers.ima = \'%2\' and readers.date_r=\'%3\' ")
                    .arg(content.at(0)).arg(content.at(1)).arg(content.at(3));
            query = QString("insert into readers (fam, ima, otc, date_r, num, address, phone, doc) "
                            "values (:fam, :ima, :otc, :date_r, :num, :address, :phone, :doc)");
        }else if (ui.comboBox_object->currentIndex() == 6){ //organizations
            test = QString("select organizations.id from organizations where organizations.name = \'%1\' ").arg(content.at(0));
            query = QString("insert into organizations (name, address, phone, face, email, note) "
                         "values (:name, :address, :phone, :face, :email, :note)");
        }
        QSqlQuery testImport(test);
        testImport.next();

        if (!testImport.value(0).isValid()){
            qDebug() << content;
            QSqlQuery qImport(query);
            for(int x = 0; x < column; x++){
                qImport.bindValue(x, content.at(x));
            }
            qImport.exec();
            if (ui.comboBox_object->currentIndex() == 4){ // load book's authors
                QSqlQuery qIdBook("select Max(books.id) from books");
                qIdBook.next();
                qDebug() << qIdBook.value(0).toString();
                for (int  x = 9; x < 12; x++){
                    if (content.at(x).size() > 1 and content.at(x) != " "){
                        QSqlQuery qIdAuth(QString("select authors.id from authors where authors.name_k = \'%1\'").arg(content.at(x)));
                        qIdAuth.next();
                        QSqlQuery loadAuthors("insert into book_auth (book, auth) values (:book, :auth)");
                        loadAuthors.bindValue(0, qIdBook.value(0).toString());
                        loadAuthors.bindValue(1, qIdAuth.value(0).toString());
                        loadAuthors.exec();
                    } else {
                        break;
                    }
                }
            }
        }
    }
}
