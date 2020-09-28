#ifndef NEWSCREWDLG_H
#define NEWSCREWDLG_H

#include <QDialog>
#include "ui_OptDlg.h"
#include <QtGui/QButtonGroup>    
#include <QRadioButton>
class OptDlg : public QDialog, public Ui::OptDlog
{
  Q_OBJECT

public:
	explicit OptDlg(QWidget *parent = 0);
	~OptDlg();
public:
	QButtonGroup *buttonGroup;
public slots:
};


#endif//NEWSCREWDLG_H
