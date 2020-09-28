#ifndef _DB_INIT_H
#define _DB_INIT_H

#include <QtSql/QSqlDatabase>
#include <QTextCodec>
#include <QtSql>


void initDatabase()
{
    QSqlQuery query;

    qDebug() << query.exec(
        QString("CREATE TABLE IF NOT EXISTS surgerys (") +
        QString("   ID      INTEGER     PRIMARY KEY AUTOINCREMENT NOT NULL,") +
        QString("   DESC    VCHAR       NOT NULL,") +
        QString("   NAME    VCHAR       NOT NULL,") +
        QString("   PID     INTEGER     NOT NULL,") +
        QString("   CTDATA  VCHAR,") +
        QString("   IMA     BLOB,") +
        QString("   IMB     BLOB,") +
        QString("   MARKERA VCHAR,") +
        QString("   MARKERB VCHAR,") +
        QString("   SHOTTIME INTEGER") +
        QString(");")
    );

    qDebug() << query.exec(
        QString("CREATE TABLE IF NOT EXISTS screws (") +
        QString("   ID      INTEGER     PRIMARY KEY AUTOINCREMENT NOT NULL,") +
        QString("   SURGERY INTEGER     NOT NULL,") +
        QString("   NAME    VCHAR       NOT NULL,") +
        QString("   TYPE    VCHAR       NOT NULL,") +
        QString("   X       INTEGER,") +
        QString("   Y       INTEGER,") +
        QString("   Z       INTEGER,") +
        QString("   RX      INTEGER,") +
        QString("   RY      INTEGER,") +
        QString("   TF      BLOB,") +
        QString("   LEN     INTEGER,") +
        QString("   D       INTEGER,") +
        QString("   PLOTX	BLOB,") +
        QString("   PLOTY	BLOB,") +
        QString("   EDITTIME INTEGER") +
        QString(");")
    );

    qDebug() << query.exec(
        QString("CREATE TABLE IF NOT EXISTS users (") +
        QString("   ID      INTEGER     PRIMARY KEY AUTOINCREMENT NOT NULL,") +
        QString("   NAME    TEXT        UNIQUE NOT NULL,") +
        QString("   PSWD    TEXT        NOT NULL,") +
        QString("   canVIEW     BOOLEAN,") +
        QString("   canEDIT     BOOLEAN,") +
        QString("   canRUN      BOOLEAN,") +
        QString("   canADMIN    BOOLEAN") +
        QString(");")
    );
    // tel/info/last-login/...

    qDebug() << query.exec(
        QString("CREATE TABLE IF NOT EXISTS patient (") +
        QString("   ID      INTEGER     PRIMARY KEY AUTOINCREMENT NOT NULL,") +
        QString("   NAME    TEXT          NOT NULL,") +
        QString("   SEX    BOOLEAN            NOT NULL,") +
        QString("   AGE     INT            NOT NULL,") +
        QString("   HID    INT            NOT NULL,") +
        QString("   DESC    VCHAR         ,") +
        QString("   Unit   VCHAR,") +
        QString("   Profession	VCHAR,") +
        QString("   Address		VCHAR,") +
        QString("   Married		VCHAR,") +
        QString("   NativePlace		VCHAR,") +
        QString("   BloodType		VCHAR,") +
        QString("   Admintime		VCHAR,") +
        QString("   Nation		VCHAR,") +
        QString("   Recordtime		VCHAR,") +
        QString("   Office	VCHAR,") +
        QString("   BedNum	VCHAR,") +
        QString("   AdminNum	VCHAR,") +
        QString("   MainDoc	VCHAR") +
        QString(");")
    );

    qDebug() << query.exec(
        QString("INSERT OR IGNORE INTO users (ID,NAME,PSWD,canVIEW,canEDIT,canRUN,canADMIN) VALUES (1,\"root\",\"toor\",1,1,1,1)")
    );//need change

    qDebug() << query.exec(
        QString("INSERT OR IGNORE INTO surgerys (ID,DESC,NAME,PID) VALUES (1,\"test\",\"test\",1)")
    );//for plan
    qDebug() << query.exec(
        QString("INSERT OR IGNORE INTO patient (ID,NAME,SEX,AGE,HID) VALUES (1,\"xxx\",1,20,1)")
    );//for plan
}


void createConnection()
{
  QFileInfo fi("data.db3");
  bool initflag = ! fi.isFile();

  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName("data.db3");
  if(!db.open()){
    qDebug() << "Database Open Error!"; ::exit(0);
  }
  QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
  QTextCodec::setCodecForTr(QTextCodec::codecForLocale());

  if(initflag) initDatabase();
}


#endif//_DB_INIT_H
