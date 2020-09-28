#include "OptDlg.h"
#include <iostream>
#include <QDebug>


OptDlg::OptDlg(QWidget *parent) : QDialog(parent)
{
	setupUi(this);
	buttonGroup = new QButtonGroup;
	buttonGroup->setObjectName(QString::fromUtf8("buttonGroup"));
	buttonGroup->addButton(radioButton,0);
	buttonGroup->addButton(radioButton_2,1);
}
OptDlg::~OptDlg()
{
}
