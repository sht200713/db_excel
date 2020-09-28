#ifndef _DB_EXPORT_H
#define _DB_EXPORT_H

#include <QtSql/QSqlDatabase>
#include <QTextCodec>
#include <QtSql>
#include <Qtsql/QSqlQueryModel>
#include <QByteArray>
#include "ui_db_export.h"

class dbexport: public QObject,public Ui::dbexport
{
	Q_OBJECT
public:
	dbexport();
	~dbexport();
	void init();
public:
	QVector<double> xs;
	QVector<double> y1;
	QSqlDatabase db;
	QSqlQueryModel *model;
private:
	bool createConnection(QString);
	short b_sgns;
	
public slots:
	void opendb(void);
	void exporttxt(void);//导出为txt文本格式
	void exportex(void );//导出为excel格式
	//

};
#endif//_DB_INIT_H
