#include "dbexport.h"
#include <QApplication>
#include <QWidget>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  QWidget *widget=new QWidget();
  dbexport *db_export=new dbexport();
  db_export->setupUi(widget);
  db_export->init();
  widget->show();
  qDebug() << "Start";
  return app.exec();
}
