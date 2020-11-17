#include "dbexport.h"
#include <iostream>
#include <fstream>
#include <QFileDialog>
#include <ActiveQt/QAxObject>
#include  <QDesktopServices>
#include "OptDlg.h"
dbexport::dbexport()
{
	model = new QSqlQueryModel();
	db = QSqlDatabase::addDatabase("QSQLITE");
}
 dbexport::~dbexport()
 {
	 if (db.isOpen()) db.close();
 }
 void dbexport::init()
 {
	 connect(pb_OpenDB, SIGNAL(clicked()),this, SLOT(opendb()));
	 connect(pb_Export, SIGNAL(clicked()),this, SLOT(exportex()));
 }
bool dbexport::createConnection(QString dbfile)
{
  db.setDatabaseName(dbfile);
  if(!db.open()){
    qDebug() << "Database Open Error!"; return false;
  }
  QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
  QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
  return true;
}
void dbexport::opendb()
{
	std::cout<<"opendb"<<std::endl;
	QString fileName = QFileDialog::getOpenFileName(0,
		tr("Open DB"), "./", tr("db Files (*.db3)"));
	//QString fileName="data-057.db3";
	qDebug()<<fileName;
	if (fileName.isEmpty())return;
	if (db.isOpen())db.close();
	if(!createConnection(fileName)) return;
	le_db->setText(fileName);
	OptDlg dlg(Optwidget);
	int res = dlg.exec();
	QString name,postision;
	if(res == QDialog::Accepted)
	{
		b_sgns=dlg.buttonGroup->checkedId();
	}
	if (b_sgns==0)
	{
		model->setQuery(QString("select patient.NAME,screws.NAME as SNAME,screws.LEN,screws.D,screws.PLOTX,screws.PLOTY from screws left join surgerys on surgerys.ID=screws.SURGERY left join patient on surgerys.PID=patient.ID"));
		model->setHeaderData(0, Qt::Horizontal, QObject::tr("姓名"));
		model->setHeaderData(1, Qt::Horizontal, QObject::tr("钉子位置"));
		model->setHeaderData(2, Qt::Horizontal, QObject::tr("钉子长度"));
		model->setHeaderData(3, Qt::Horizontal, QObject::tr("钉子直径"));
		tableView->setModel(model);
		tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
		tableView->horizontalHeader()->setStretchLastSection(true);
		tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
		tableView->setColumnHidden(4,true);
		tableView->setColumnHidden(5,true);
	}else
	{
		model->setQuery(QString("select NAME,POS,TIME,PLOTY from gsns"));
		model->setHeaderData(0, Qt::Horizontal, QObject::tr("姓名"));
		model->setHeaderData(1, Qt::Horizontal, QObject::tr("钉子位置"));
		tableView->setModel(model);
		tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
		tableView->horizontalHeader()->setStretchLastSection(true);
		tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	}

}
void dbexport::exporttxt()
{
	QString sexcel=le_excel->text();
	QSqlRecord record;
	QByteArray plotx,ploty;
	QString patienName,dingName;
	std::ofstream ofile;
	ofile.open((sexcel+"11.txt").toStdString().c_str(), std::ios::out);
	if(!ofile) return ;
	for(int i=0; i<this->model->rowCount(); i++){
		xs.clear();
		y1.clear();
		record = model->record(i);
		//qDebug()<<record.fieldName(0)<<record.fieldName(1)<<record.fieldName(2);
		patienName=record.value("NAME").toString();
		dingName=record.value("SNAME").toString();
		plotx=record.value("PLOTX").toByteArray();
		ploty=record.value("PLOTY").toByteArray();
		QDataStream streamx(plotx);
		streamx >> xs;
		QDataStream streamy(ploty);
		streamy >> y1;
		int vsize=xs.size();
		std::cout<<"vsize:"<<vsize<<std::endl;
		if(vsize<1)continue;
		ofile<<patienName.toStdString().c_str()<<" "<<dingName.toStdString().c_str()<<" "<<"行程";
		for (int j=0;j<vsize;j++) 
		{
			if (j!=0&&j%500==0) 
			{
				ofile<<"\n";
				ofile<<patienName.toStdString().c_str()<<" "<<dingName.toStdString().c_str()<<" "<<"行程";
			}
			ofile<<" "<<xs[j];
		}
		ofile<<"\n";
		ofile<<patienName.toStdString().c_str()<<" "<<dingName.toStdString().c_str()<<" "<<"压力";
		for (int j=0;j<vsize;j++) 
		{
			if (j!=0&&j%500==0)
			{
				ofile<<"\n";
				ofile<<patienName.toStdString().c_str()<<" "<<dingName.toStdString().c_str()<<" "<<"压力";
			}
			ofile<<" "<<y1[j];
		}
		ofile<<"\n";
	}
	ofile.close();
}
QString to26AlphabetString(int data)
{
	QChar ch = data + 0x40;//A对应0x41
	return QString(ch);
}
void convertToColName(int data, QString &res)//有bug，采用第二种方式计算excel列号excelColIndexToStr
{
	//qDebug()<<"covertdata:"<<data;
	Q_ASSERT(data>0 && data<65535);
	
	//Q_ASSERT(data>0);

	int tempData = data / 26;
	//std::cout<<"tempData:"<<tempData<<std::endl;
	//qDebug()<<"tempData:"<<tempData;
	if(tempData > 1)
	{
		int mode = data % 26;
		//std::cout<<"mode:"<<mode<<std::endl;
		if(mode==0) 
		{
			res=(to26AlphabetString(26)+res);
			convertToColName(tempData-1,res);
		}
		else
		{
			convertToColName(mode,res);
			convertToColName(tempData,res);
		}
	}
	else
	{
		res=(to26AlphabetString(data)+res);
	}
}

void  excelColIndexToStr(int columnIndex,QString &columnStr) {
	     if (columnIndex <= 0) columnStr="";
	     columnIndex--;
	     do {
	             if (columnStr.length() > 0) 
	                 columnIndex--;
	             columnStr = ((char) (columnIndex % 26 + (int) 'A')) + columnStr;
	             columnIndex = (int) ((columnIndex - columnIndex % 26) / 26);
	         } while (columnIndex > 0);
}

void Excel_SetCell(QAxObject *worksheet,int column,int row,QColor color,QString text)
{
	QAxObject *cell = worksheet->querySubObject("Cells(int,int)", row, column);
	cell->setProperty("Value", text);
	QAxObject *font = cell->querySubObject("Font");
	font->setProperty("Color", color);
}
QString saveas()
{
	QString file;
	QString filter;


	//如果版本低于QT5,则需要将:
	//  QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
	//改为:QDesktopServices::storageLocation(QDesktopServices::DesktopLocation),
	file = QFileDialog::getSaveFileName (
		NULL,                               //父组件
		"另存为",                              //标题
		//QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),                 //设置路径, .表示当前路径,./表示更目录
		QDesktopServices::storageLocation(QDesktopServices::DesktopLocation),
		"Excel(*.xlsx)",     //过滤器
		&filter  );

	return file;
}
//把QList<QList<QVariant> > 转为QVariant,用于快速写入的
void castListListVariant2Variant(const QList<QList<QVariant> > &cells, QVariant &res)
{
	QVariantList vars;
	const int rows = cells.size();
	for(int i=0;i<rows;++i)
	{
		vars.append(QVariant(cells[i]));
	}
	res = QVariant(vars);
}

void dbexport::exportex()
{
	QString templatePath = "./db.xlsx";
	QFileInfo info(templatePath);
	if(!info.exists())
	{
		qDebug()<<"db.xlsx is NULL";
		return ;
	}
	templatePath = info.absoluteFilePath();                   //获取模板的绝地路径
	templatePath = QDir::toNativeSeparators(templatePath);   //转换一下路径,让windows能够识别

	QString ExcelFile = QDir::toNativeSeparators(saveas());  //打开文件保存对话框,找到要保存的位置
	if(ExcelFile=="") return;
	le_excel->setText(ExcelFile);
	QFile::copy(templatePath, ExcelFile);                   //将模板文件复制到要保存的位置去

	info.setFile(ExcelFile);
	info.setFile(info.dir().path()+"/~$"+info.fileName());

	if(info.exists())          //判断一下,有没有"~$XXX.xlsx"文件存在,是不是为只读
	{
		qDebug()<<"报表属性为只读,请检查文件是否已打开!";
		return   ;
	}

	QAxObject *excel = new QAxObject();//建立excel操作对象
	excel->setControl("Excel.Application");//连接Excel控件
	excel->setProperty("Visible", true);//显示窗体看效果
	excel->setProperty("DisplayAlerts", false);//显示警告看效果
	QAxObject *workbooks = excel->querySubObject("WorkBooks");
	QString sexcel=le_excel->text()+"db.xlsx";
	QSqlRecord record;
	QByteArray plotx,ploty;
	QString patienName,dingName;
	QAxObject* workbook = workbooks->querySubObject("Open(const QString&)",QDir::toNativeSeparators(ExcelFile));
	excel->setProperty("Caption", "db_Excel");      //标题为Qt Excel
	QAxObject *work_book = excel->querySubObject("ActiveWorkBook");
	QAxObject *worksheet = work_book->querySubObject("Sheets(int)",1); 
	int nn=2;
	for(int i=0; i<this->model->rowCount(); i++){
	//for(int i=0; i<1; i++){
		xs.clear();
		y1.clear();
		record = model->record(i);
		//qDebug()<<record.fieldName(0)<<record.fieldName(1)<<record.fieldName(2);
		if (b_sgns==0)
		{
			patienName=record.value("NAME").toString();
			dingName=record.value("SNAME").toString();
			plotx=record.value("PLOTX").toByteArray();
			ploty=record.value("PLOTY").toByteArray();
			QDataStream streamx(plotx);
			streamx >> xs;
			QDataStream streamy(ploty);
			streamy >> y1;
			int vsize=xs.size();
			std::cout<<"vsize:"<<vsize<<std::endl;
			Excel_SetCell(worksheet,1,i+nn,QColor(0,0,255),patienName);
			Excel_SetCell(worksheet,2,i+nn,QColor(0,0,255),dingName);
			Excel_SetCell(worksheet,3,i+nn,QColor(0,0,255),"行程");
			nn++;
			Excel_SetCell(worksheet,3,i+nn,QColor(0,0,255),"压力");
			if(vsize<1)continue;
		}else
		{
			patienName=record.value("NAME").toString();
			dingName=record.value("POS").toString();
			plotx=record.value("TIME").toByteArray();
			ploty=record.value("PLOTY").toByteArray();
			QDataStream streamx(plotx);
			streamx >> xs;
			QDataStream streamy(ploty);
			streamy >> y1;
			int vsize=xs.size();
			std::cout<<"vsize:"<<vsize<<std::endl;
			Excel_SetCell(worksheet,1,i+nn,QColor(0,0,255),patienName);
			Excel_SetCell(worksheet,2,i+nn,QColor(0,0,255),dingName);
			Excel_SetCell(worksheet,3,i+nn,QColor(0,0,255),"时间");
			nn++;
			Excel_SetCell(worksheet,3,i+nn,QColor(0,0,255),"压力");
			if(vsize<1)continue;
		}

		//单个单元格存储
#if 0	
		for (int j=0;j<vsize;j++) 
		{
			char buffer[20];
			sprintf_s(buffer,10,"%f",xs[j]);
			QString str=buffer;
			Excel_SetCell(worksheet,4+j,i+nn,QColor(0,0,0),str);
		}
		nn++;
		Excel_SetCell(worksheet,3,i+nn,QColor(255,0,0),"压力");
		for (int j=0;j<vsize;j++) 
		{
			char buffer[20];
			sprintf_s(buffer,10,"%f",y1[j]);
			QString str=buffer;
			Excel_SetCell(worksheet,4+j,i+nn,QColor(0,0,0),str);
		}
#else 
		//批量存储
		//QAxObject *user_range = worksheet->querySubObject("Range(const QString&)","A4:I106");

		QList<QList<QVariant>> cells;
		QList<QVariant> clodata1,clodata2;
		int xssize=xs.size();
		for (int jj=0;jj<xssize;jj++)
		{
			clodata1.push_back(xs[jj]);
			clodata2.push_back(y1[jj]);
		}	
		cells.append(clodata1);
		cells.append(clodata2);
		
		int row = cells.size();
		int col = cells.at(0).size()+3;
		//qDebug()<<col;
		QString rangStr;
		//convertToColName(col,rangStr);//列号
		excelColIndexToStr(col,rangStr);
		rangStr += QString::number(2+i*2+1);//行号
		rangStr = "D"+QString::number(2+i*2)+":"+ rangStr;
		qDebug()<<rangStr;
		QAxObject *range = worksheet->querySubObject("Range(const QString&)",rangStr);
		if(NULL == range || range->isNull()) return;
		QVariant var;
		castListListVariant2Variant(cells,var);
		range->setProperty("Value", var);
		delete range;
#endif
	}
	workbook->dynamicCall("Save()" );
	workbook->dynamicCall("Close()");  //关闭文件
	excel->dynamicCall("Quit()");//关闭excel
	delete worksheet;
	delete work_book;
	delete workbook;
	delete workbooks;
	delete excel;
	qDebug()<<"Convert Success!";
}

