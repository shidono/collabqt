#include "chedli.h"
#include "ui_chedli.h"
#include "mailing/SmtpMime"
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QDebug>
#include "chedli/modifierchedli.h"
chedli::chedli(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chedli){
    int ret;
    ui->setupUi(this);
    ui->tableinfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->perctable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableinfo->setModel(tmpspon.afficher());
    ui->perctable->setModel(tmpspon.AfficherPerc());
    mSystemTrayIcon =new QSystemTrayIcon(this);
    mSystemTrayIcon->setVisible(true);
    ret=A.connect_arduino();
    switch(ret){
        case(0):
            qDebug() << "arduino is available and connected to : " << A.getarduino_port_name();
        break;
        case(1):
            qDebug() << "arduino is available but not connected to :" << A.getarduino_port_name();
        break;
        case(-1):
            qDebug() << "arduino is not availabe";
        break;
    }
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(myfunction()));
    //timer->start(5000);
}

chedli::~chedli()
{
    delete ui;
}




void chedli::on_ajouterbutt_clicked()
{
    QString nom = ui->nomedit_4->text();
    if(nom.length() > 20){
        QMessageBox::information(nullptr, QObject::tr("Erreur dans l ajout"),
                QObject::tr("le champ nom a dépasser la longeur maximale.\n"
                "Click Cancel to exit."), QMessageBox::Cancel);
    }
    QString localisation = ui->localedit->text();
    if(localisation.length() > 20){
        QMessageBox::information(nullptr, QObject::tr("Erreur dans l ajout"),
                QObject::tr("le champ localisation a dépasser la longeur maximale.\n"
                "Click Cancel to exit."), QMessageBox::Cancel);
    }
    QString pays = ui->paysedit->text();
    if(pays.length() > 20){
        QMessageBox::information(nullptr, QObject::tr("Erreur dans l ajout"),
                QObject::tr("le champ pays a dépasser la longeur maximale.\n"
                "Click Cancel to exit."), QMessageBox::Cancel);
    }
    int contrat = ui->contratedit->text().toInt();
    Sponsor e(nom,localisation,pays,contrat);
    bool test=e.ajouterbase();

    if(test){
        mSystemTrayIcon->showMessage(tr("Affiliee ajoutee"),tr("votre affiliee est ajoutée"));
        ui->tableinfo->setModel(tmpspon.afficher());//refresh
        /*QMessageBox::information(nullptr, QObject::tr("Ajouter un étudiant"),
        QObject::tr("Affiliée ajouté.\n"
        "Click Cancel to exit."), QMessageBox::Cancel);*/
    }else{
        QMessageBox::critical(nullptr, QObject::tr("Ajouter un étudiant"),
        QObject::tr("Erreur !.\n"
        "Click Cancel to exit."), QMessageBox::Cancel);
    }
}

void chedli::on_pushButton_clicked(){
    QString q;
    modifierchedli  m;
    int a ;
    QString col ;
    QModelIndex index;
    QModelIndexList selection = ui->tableinfo->selectionModel()->selectedIndexes();
    for(int i=0;i<selection.count();i++){
       index = selection.at(i);
       q = QVariant(ui->tableinfo->model()->index(index.row(),0).data()).toString();
       col = ui->tableinfo->model()->headerData(index.column(),Qt::Horizontal).toString();
       m.setcol(col);
       m.setnom(q);
       a=m.exec();
       if (a==QDialog::Accepted){
           ui->tableinfo->setModel(tmpspon.afficher());
           ui->perctable->setModel(tmpspon.AfficherPerc());
        }
    }
}

void chedli::on_pushButton_2_clicked(){
    QString nom ;
    QModelIndex index;
    QModelIndexList selection = ui->tableinfo->selectionModel()->selectedRows();
    for(int i=0;i<selection.count();i++){
        index = selection.at(i);
    nom =QVariant(ui->tableinfo->model()->index(index.row(),0).data()).toString();
    if (tmpspon.supprimer(nom)){
        QMessageBox :: information(this,"info","succes");
        ui->tableinfo->setModel(tmpspon.afficher());
        ui->perctable->setModel(tmpspon.AfficherPerc());

    }
  }
}

void chedli::on_pushButton_3_clicked(){

}

void chedli::on_Annuler_4_clicked(){
    accept();
}

void chedli::on_pushButton_4_clicked(){
    /*QString test;
    test = A.read_arduino();
    qDebug() << test;
    if(test[0] == 'd'){
        A.write_arduino("1");
        double val = ui->number->value();
        ui->number->display(val+1);
        val = (val/651) * 100;
        int value = round(val);
        ui->total->setNum(value);
        ui->pleasework->resize(value,41);
    }*/
    double val = ui->number->value();
    if(val < 66){
        ui->number->display(val+1);
        val = (val/65) * 100;
        int value = round(val);
        if(value == 100) ui->total->move(310,310);
        ui->total->setNum(value);
        ui->pleasework->resize(value*6.5,41);
    }

}

void chedli::on_Envoyer_clicked()
{
    SmtpClient smtp("smtp.gmail.com", 465, SmtpClient::SslConnection);
                QString ch=ui->textEdit->toPlainText();
                smtp.setUser("chedli.elloumi@esprit.tn");
                smtp.setPassword("Closers1");
                MimeMessage message;
                message.setSender(new EmailAddress("chedli.elloumi@esprit.tn", "Service d'affiliée"));
                message.addRecipient(new EmailAddress(ui->lineEdit->text(), "Affiliée"));
                message.setSubject("ALERT CLIENT!");
                MimeText text;
                text.setText(ch);
                message.addPart(&text);
                smtp.connectToHost();
                smtp.login();
                if(smtp.sendMail(message)){
                    QMessageBox::information(this, "OK", "Mail envoyé");
                }else{
                    QMessageBox::critical(this, "Erreur", "Mail non envoye");
                }
                smtp.quit();
}

void chedli::on_boxi_currentIndexChanged()
{
     ui->tableinfo->setModel(tmpspon.afficher2(ui->boxi->currentText()));
}

void chedli::on_recherchebox_currentIndexChanged()
{
    ui->perctable->setModel(tmpspon.recherchepourtrie(ui->pourboxtrie->currentText()));
}

void chedli::on_pourboxtrie_currentIndexChanged()
{
    ui->perctable->setModel(tmpspon.recherchepourtrie(ui->pourboxtrie->currentText()));
}

void chedli::on_Ajouter_clicked()
{
    QString q;
    modifierchedli m;
    int a ;
    QString col ;
    QModelIndex index;
    QModelIndexList selection = ui->perctable->selectionModel()->selectedIndexes();
    for(int i=0;i<selection.count();i++){
       index = selection.at(i);
       q = QVariant(ui->perctable->model()->index(index.row(),0).data()).toString();
       col = ui->perctable->model()->headerData(index.column(),Qt::Horizontal).toString();
       qDebug()<< col ;
       m.setcol(col);
       m.setnom(q);
       a=m.exec();
       if (a==QDialog::Accepted){
           ui->perctable->setModel(tmpspon.recherchepour(ui->pourbox->currentText(),ui->recherchepour->text()));
        }
    }
}



void chedli::on_pourbox_currentTextChanged()
{

}

void chedli::on_recherchepour_textEdited()
{
    ui->perctable->setModel(tmpspon.recherchepour(ui->pourbox->currentText(),ui->recherchepour->text()));
}

void chedli::on_recherche_textEdited()
{
    ui->tableinfo->setModel(tmpspon.recherche(ui->recherchebox->currentText(),ui->recherche->text()));
}

void chedli::myfunction()
{
    QString test;
    test = A.read_arduino();
    qDebug() << test;
    if(test[0] == 'd'){
        A.write_arduino("1");
        double val = ui->number->value();
        ui->number->display(val+1);
        val = (val/651) * 100;
        int value = round(val);
        ui->total->setNum(value);
        ui->pleasework->resize(value,41);
    }
}
