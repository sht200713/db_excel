/*=============================================================================

  Library: CTK

  Copyright (c) xjt.com

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "robotPlugin.h"
#include <QtPlugin>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QMessageBox>
#include <iostream>
#include <math.h>


robotPlugin::robotPlugin()
{
    std::cout << "robotPlugin constructor" << std::endl;
    moniter = new UrMoniter();
    ur = new UrCtrl();
    dr = new DrCtrl();

    urUiWidget = new QWidget();
    uiUR = new Ui::UR();
    uiUR->setupUi(urUiWidget);

    urCfgWidget = new QWidget();
    urCfg = new UrCfg();
    urCfg->setupUi(urCfgWidget);

    urCfg->load(ur->lock,ur->locked);
    if(ur->locked) uiUR->pbLOCK->setText(tr("锁定 !"));
	connect(urCfg->pbStand, SIGNAL(clicked()), this, SLOT(on_Stand()));
	connect(urCfg->pbPack, SIGNAL(clicked()), this, SLOT(on_Pack()));
    robotSelfChk = new RobotSelfChk(ur, dr);
    robotSelfChkWidget = new QWidget();
    robotSelfChk->setupUi(robotSelfChkWidget);
    connect(robotSelfChk->pbStartSelfInspection, SIGNAL(clicked(bool)), robotSelfChk, SLOT(onSelfInspection(bool)));
    connect(robotSelfChk->pbDrillBack, SIGNAL(pressed()), robotSelfChk, SLOT(drillBack()));
    connect(robotSelfChk->pbDrillBack, SIGNAL(released()), robotSelfChk, SLOT(stop()));
    connect(robotSelfChk->pbDrillForward, SIGNAL(pressed()), robotSelfChk, SLOT(drillForward()));
    connect(robotSelfChk->pbDrillForward, SIGNAL(released()), robotSelfChk, SLOT(stop()));
    connect(robotSelfChk->startMove, SIGNAL(clicked()), robotSelfChk, SLOT(onStartMove()));
    connect(robotSelfChk->startDemo, SIGNAL(clicked()), robotSelfChk, SLOT(onStartDemo()));
    connect(robotSelfChk->pbPreVision, SIGNAL(clicked()), robotSelfChk, SLOT(onPreVision()));
    connect(uiUR->pbLOCK, SIGNAL(clicked()),this, SLOT(LOCK()));
	uiUR->pbLOCK->hide();
    connect(uiUR->pb_UR_POWERON, SIGNAL(clicked()),this, SLOT(POWERON()));
    connect(uiUR->pb_UR_POWEROFF, SIGNAL(clicked()),this, SLOT(POWEROFF()));

    connect(uiUR->pb_UR_UP, SIGNAL(pressed()),this, SLOT(on_URUP_pressed()));
    connect(uiUR->pb_UR_DOWN, SIGNAL(pressed()),this, SLOT(on_URDOWN_pressed()));
    connect(uiUR->pb_UR_FORW, SIGNAL(pressed()),this, SLOT(on_URFORW_pressed()));
    connect(uiUR->pb_UR_BACK, SIGNAL(pressed()),this, SLOT(on_URBACK_pressed()));
    connect(uiUR->pb_UR_LEFT, SIGNAL(pressed()),this, SLOT(on_URLEFT_pressed()));
    connect(uiUR->pb_UR_RIGHT, SIGNAL(pressed()),this, SLOT(on_URRIGHT_pressed()));
    connect(uiUR->pb_UR_UP, SIGNAL(released()),this, SLOT(on_UR_released()));
    connect(uiUR->pb_UR_DOWN, SIGNAL(released()),this, SLOT(on_UR_released()));
    connect(uiUR->pb_UR_FORW, SIGNAL(released()),this, SLOT(on_UR_released()));
    connect(uiUR->pb_UR_BACK, SIGNAL(released()),this, SLOT(on_UR_released()));
    connect(uiUR->pb_UR_LEFT, SIGNAL(released()),this, SLOT(on_UR_released()));
    connect(uiUR->pb_UR_RIGHT, SIGNAL(released()),this, SLOT(on_UR_released()));
    connect(uiUR->pb_UR_L, SIGNAL(pressed()),this, SLOT(on_XL_pressed()));
    connect(uiUR->pb_UR_R, SIGNAL(pressed()),this, SLOT(on_XR_pressed()));
    connect(uiUR->pb_UR_N, SIGNAL(pressed()),this, SLOT(on_YL_pressed()));
    connect(uiUR->pb_UR_S, SIGNAL(pressed()),this, SLOT(on_YR_pressed()));
    connect(uiUR->pb_UR_E, SIGNAL(pressed()),this, SLOT(on_ZR_pressed()));
    connect(uiUR->pb_UR_W, SIGNAL(pressed()),this, SLOT(on_ZL_pressed()));
    connect(uiUR->pb_UR_L, SIGNAL(released()),this, SLOT(on_UR_released()));
    connect(uiUR->pb_UR_R, SIGNAL(released()),this, SLOT(on_UR_released()));
    connect(uiUR->pb_UR_N, SIGNAL(released()),this, SLOT(on_UR_released()));
    connect(uiUR->pb_UR_S, SIGNAL(released()),this, SLOT(on_UR_released()));
    connect(uiUR->pb_UR_E, SIGNAL(released()),this, SLOT(on_UR_released()));
    connect(uiUR->pb_UR_W, SIGNAL(released()),this, SLOT(on_UR_released()));
    connect(uiUR->pb_UR_FREE, SIGNAL(pressed()),this, SLOT(on_URFREE_pressed()));
    connect(uiUR->pb_UR_FREE, SIGNAL(released()),this, SLOT(on_URFREE_released()));
    connect(uiUR->hs_UR_SPEED, SIGNAL(valueChanged(int)),this, SLOT(on_UR_speed(int)));
    connect(urCfg->cbFreeMode, SIGNAL(stateChanged(int)),this, SLOT(on_FreeMode(int)));
    QFile file(":/ur/res/ur.qss");
    file.open(QFile::ReadOnly);
    QString s = file.readAll();
    urUiWidget->setStyleSheet(s);

    for(int i=0; i<6; i++) urPrePose[i] = 0;

    drUiWidget = new QWidget();
    uiDR = new Ui::DR();
    uiDR->setupUi(drUiWidget);
	uiUR->pb_UR_FREE->setEnabled(false);
    connect(uiDR->lenSpinBox, SIGNAL(valueChanged(double)),this, SLOT(on_set_len(double)));
    connect(uiDR->posSpinBox, SIGNAL(valueChanged(double)),this, SLOT(on_set_pos(double)));
    connect(uiDR->limitSpinBox, SIGNAL(valueChanged(int)),this, SLOT(on_set_limit(int)));
    connect(uiDR->hsLimit, SIGNAL(valueChanged(int)),this, SLOT(on_set_limit(int)));
    connect(uiDR->limitSpinBox, SIGNAL(valueChanged(int)),uiDR->hsLimit, SLOT(setValue(int)));
    connect(uiDR->hsLimit, SIGNAL(valueChanged(int)),uiDR->limitSpinBox, SLOT(setValue(int)));
    connect(uiDR->pb_DR_BACK, SIGNAL(clicked()),this, SLOT(on_set_back()));
    connect(uiDR->pb_DR_REV, SIGNAL(clicked()),this, SLOT(on_set_rev()));
    connect(uiDR->cbMode, SIGNAL(currentIndexChanged(int)),this, SLOT(on_set_kir(int)));
    //connect(uiDR->pb_DR_START, SIGNAL(clicked()),this, SLOT(START()));
    //connect(uiDR->pb_DR_START, SIGNAL(released()),this, SLOT(PAUSE()));
    connect(uiDR->pbZeroForce, SIGNAL(clicked()),this, SLOT(onZeroForce()));
    connect(uiDR->sliderRPM, SIGNAL(valueChanged(int)),this, SIGNAL(drSpeedChanged(int)));	//just for Calib.dll

    drCfgWidget = new QWidget();
    drCfg = new DrCfg();
    drCfg->setupUi(drCfgWidget);
    drCfg->load();
	connect(drCfg->pbDrReset, SIGNAL(clicked()), dr, SLOT(disconnectFromBot()));
    connect(drCfg->pb_DR_ACFAIL, SIGNAL(clicked()),this, SLOT(on_set_acfail()));

	drPlotWidget = new QWidget();
	drPlot = new DrPlot();
    drPlot->setupUi(drPlotWidget);
	drPlot->init();
	drPlot->box->setVisible(true);

	QTimer *timer = new QTimer( this );
	connect(timer, SIGNAL(timeout()), this, SLOT(onPulse()));
	timer->start(1000);
	QTimer *timer2 = new QTimer( this );
	connect(timer2, SIGNAL(timeout()), this, SLOT(onLED()));
	timer2->start(10);

	drUiWidget->setEnabled(false);
	drCfgWidget->setEnabled(false);
	drPlotWidget->setEnabled(false);
	urUiWidget->setEnabled(false);
}

robotPlugin::~robotPlugin()
{
}

void robotPlugin::start()
{
  std::cout << "robotPlugin start" << std::endl;
  connect(ur, SIGNAL(readyRead()), this, SLOT(urUpdate()));
  connect(dr, SIGNAL(readyRead()), this, SLOT(drUpdate()));
  connect(dr, SIGNAL(linked()), this, SLOT(drDevice()));
}
void robotPlugin::stop()
{
  std::cout << "robotPlugin stop" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
void robotPlugin::onPulse()
{
  //std::cout << "robotPlugin pluse" << std::endl;
  bool state;
  state = dr->PULSE();
  dr->drPULSE = (state) ? 1 : 0;
  drUiWidget->setEnabled(state);
  drCfgWidget->setEnabled(state);
  drPlotWidget->setEnabled(state);
  state = ur->PULSE() && moniter->PULSE();
  urUiWidget->setEnabled(state);
  ur->urPULSE = (state) ? 1 : 0;
  
}

void robotPlugin::onLED()
{
	//std::cout << "led " <<ur->busy<< "   port " <<dr->state<< std::endl;
	//if(!dr->linked) return;

	static bool switchflag;

	int led = 0;
	if(ur->busy) led |= 1;
	if(dr->state & 1) led |= 2;

	switch(led)
	{
		case 1:		//UR
			if(switchflag){
				switchflag = false;
				if(!(dr->port&4)) dr->setLED(4);
			}else{
				switchflag = true;
				if(!(dr->port&2)) dr->setLED(2);
			}
			break;
		case 2:		//电钻
			if(switchflag){
				switchflag = false;
				if(!(dr->port&4)) dr->setLED(4);
			}else{
				switchflag = true;
				if((dr->port&4)) dr->setLED(0);
			}
			break;
		default:	//蓝灯
			if(!(dr->port&4)) dr->setLED(4);
			break;
	}
}

void robotPlugin::urUpdate()
{
  ur->update();

  uiUR->lPose0->setText(QString::number(ur->pose[0]*1000.,'f',1));
  uiUR->lPose1->setText(QString::number(ur->pose[1]*1000.,'f',1));
  uiUR->lPose2->setText(QString::number(ur->pose[2]*1000.,'f',1));
  uiUR->lPose3->setText(QString::number(ur->pose[3],'f',2));
  uiUR->lPose4->setText(QString::number(ur->pose[4],'f',2));
  uiUR->lPose5->setText(QString::number(ur->pose[5],'f',2));

  uiUR->leJoint0->setText(QString::number(R2D(ur->joint[0]),'f',2));
  uiUR->leJoint1->setText(QString::number(R2D(ur->joint[1]),'f',2));
  uiUR->leJoint2->setText(QString::number(R2D(ur->joint[2]),'f',2));
  uiUR->leJoint3->setText(QString::number(R2D(ur->joint[3]),'f',2));
  uiUR->leJoint4->setText(QString::number(R2D(ur->joint[4]),'f',2));
  uiUR->leJoint5->setText(QString::number(R2D(ur->joint[5]),'f',2));

  uiUR->hsJoint0->setValue(R2D(ur->joint[0]));
  uiUR->hsJoint1->setValue(R2D(ur->joint[1]));
  uiUR->hsJoint2->setValue(R2D(ur->joint[2]));
  uiUR->hsJoint3->setValue(R2D(ur->joint[3]));
  uiUR->hsJoint4->setValue(R2D(ur->joint[4]));
  uiUR->hsJoint5->setValue(R2D(ur->joint[5]));
	
  QString s,btn;
  switch(ur->code){
	  case 3: s="关闭"; btn="开机"; break;
	  case 5: s="空闲"; btn="启动"; break;
	  case 7: s="正常"; btn="启动"; break;
	  default: s="待机"; btn="启动";  break;
  }
  uiUR->lbCode->setText(s);
  uiUR->pb_UR_POWERON->setText(btn);
  showMovement();
}

void robotPlugin::move(float *pose){
  ur->move(pose);
}
void robotPlugin::trans(float *pose,bool pre){
  float plusz = pre ? (urCfg->getPlusZ()):0;
  ur->trans(pose,plusz);
}
int robotPlugin::busy(void){
  return ur->busy;
}

void robotPlugin::STOP(void){
  ur->STOP();
  dr->PAUSE();
}
void robotPlugin::BACK(void){
  ur->TUI();
}
void robotPlugin::LOCK(void){
  ur->LOCK();
  urCfg->save(ur->lock,ur->locked);
  uiUR->pbLOCK->setText(QString::fromLocal8Bit("锁定 !"));
}

void robotPlugin::POWEROFF()
{
	QMessageBox message(QMessageBox::Warning,"确认","确定要关机吗?",QMessageBox::Yes|QMessageBox::No,NULL);  
    if(message.exec()==QMessageBox::Yes) ur->PowerOFF();
}
void robotPlugin::POWERON()             { ur->PowerON(); on_UR_speed(3); }
void robotPlugin::on_URUP_pressed()     { ur->URUP(); }
void robotPlugin::on_URDOWN_pressed()   { ur->URDOWN(); }
void robotPlugin::on_URFORW_pressed()   { ur->URFORW(); }
void robotPlugin::on_URBACK_pressed()   { ur->URBACK(); }
void robotPlugin::on_URLEFT_pressed()   { ur->URLEFT(); }
void robotPlugin::on_URRIGHT_pressed()  { ur->URRIGHT(); }
void robotPlugin::on_XL_pressed()       { ur->XL(); }
void robotPlugin::on_XR_pressed()       { ur->XR(); }
void robotPlugin::on_YL_pressed()       { ur->YL(); }
void robotPlugin::on_YR_pressed()       { ur->YR(); }
void robotPlugin::on_ZL_pressed()       { ur->ZL(); }
void robotPlugin::on_ZR_pressed()       { ur->ZR(); }
void robotPlugin::on_UR_released()      { ur->STOP(); }
void robotPlugin::on_URFREE_pressed()   { ur->URFREE(); }
void robotPlugin::on_URFREE_released()  { ur->ENDFREE(); }
void robotPlugin::on_FreeMode(int val)  { uiUR->pb_UR_FREE->setEnabled(val); }
void robotPlugin::on_UR_speed(int val)  { ur->SPEED(val/10.0); }

void robotPlugin::setPrePose()
{
    for(int i=0; i<6; i++) urPrePose[i] = ur->pose[i];
    std::cout << "setPrePose " << std::endl;
}
void robotPlugin::clrPrePose()
{
    for(int i=0; i<6; i++) urPrePose[i] = 9999;
    std::cout << "clrPrePose " << std::endl;
}
float robotPlugin::getMovement()
{
    float x = ur->pose[0] - urPrePose[0];
    float y = ur->pose[1] - urPrePose[1];
    float z = ur->pose[2] - urPrePose[2];
    return sqrt(x*x+y*y+z*z);
}
void robotPlugin::showMovement()
{
  float v = 100.0-getMovement()*1000;
  if(fabs(v) < 111) uiUR->lbDistance->setText(QString::number(v,'f',1)+" mm");
  else uiUR->lbDistance->setText("");
}

void robotPlugin::setUrSpeed(float speed)
{
	uiUR->hs_UR_SPEED->setValue(speed*10);
}


///////////////////////////////////////////////////////////////////////////////
void robotPlugin::drUpdate()
{
  if(!dr->update()) return;

  uiDR->pb_DR_BACK->setChecked(dr->state&(1<<2));
  uiDR->pb_DR_REV->setChecked(dr->state&(1<<3));
  drCfg->pb_DR_ACFAIL->setChecked(dr->state&(1<<4));
  uiDR->cbMode->setCurrentIndex((dr->state&(1<<6)) ? 1:0);
  if(dr->state & 1){
    uiDR->posSpinBox->setEnabled(0);
    uiDR->lenSpinBox->setEnabled(0);
    uiDR->posSpinBox->setValue(dr->data[0]);
    uiDR->lenSpinBox->setValue(dr->data[1]);
	drPlot->update(dr->data[4], dr->data[0], dr->data[5]);
	//emit calibUpdate(dr->data[4]);
  }else{
    uiDR->posSpinBox->setEnabled(1);
    uiDR->lenSpinBox->setEnabled(1);
  }

  uiDR->lbForce->setText(QString::number(dr->data[4],'f',1));
  uiDR->lbSpeed->setText(QString::number(dr->data[2],'f',1)+" mm/s");
  uiDR->lbRPM->setText(QString::number(dr->data[3],'f',1)+" rpm");
 
  QString s;
  switch(dr->state & 0x03){
	  case 0: s="停止"; break;
	  case 1: s="工作"; break;
	  case 3: s="暂停"; break;
	  default: s="err"; break;
  }
  uiDR->lbState->setText(s);
  emit calibUpdate(dr->data[4]);
  emit drUpdated();
}

void robotPlugin::drDevice()
{
	_DEV_INFO *p = (_DEV_INFO*)dr->device;
	qDebug() <<"DEVSN::::::::::::::::::::::::"<< p->devsn;

	//SN/VER
	QString sn = QString::number(p->devsn,16).toUpper();
	QString ver = QString("v%1.%2").arg(p->ver>>8).arg(p->ver&0xFF);
  	drCfg->lDevice->setText(sn + "/" + ver);

	//转速控制参数
	QString s = "0";
	for(int i=0;i<10;i++){
		dr->DrillValue[1+i] = p->drill[i];
		s += ","+QString::number(p->drill[i]);
	}
  	drCfg->lDrSpeed->setText(s);

	//压力校准参数
	s = QString::number(p->coeff[0]) + "," + QString::number(p->coeff[1]);
  	drCfg->lCoeff->setText(s);

	//初始化一些工作参数
	dr->setDelta(0);
	dr->setPos(0);
	uiDR->hsLimit->setValue(12);
}


int robotPlugin::STATE(void){
  return dr->state;
}
void robotPlugin::PAUSE(void){
  dr->PAUSE();
}
void robotPlugin::START(){
  dr->START(uiDR->sliderSpeed->value(),
      uiDR->sliderRPM->value());
}
void robotPlugin::PARAM(int speed,int drill,float len, bool dir){
  uiDR->sliderSpeed->setValue(speed),
  uiDR->sliderRPM->setValue(drill);
  uiDR->lenSpinBox->setValue(len);

  if(dir) dr->clrState(1<<2);
  else dr->setState(1<<2);
  dr->setLen(len);
}

void robotPlugin::setGoBack()
{
	dr->setState(1<<2);
	uiDR->lenSpinBox->setValue(dr->data[0]);
	dr->setLen(dr->data[0]);
}

void robotPlugin::setForce(float val)
{
	dr->setForce(val * 1000.0);
}
void robotPlugin::setDrill(short val)
{
	dr->setDrill(val);
}
int robotPlugin::getDrillLevel()
{
	return uiDR->sliderRPM->value();
}
short *robotPlugin::getDrillValue()
{
	return dr->DrillValue;
}
void robotPlugin::saveDrillValue()
{
	dr->SaveDrillValue();
}

float robotPlugin::getDrPos()
{
	return dr->data[0];
}

void robotPlugin::clrPlot()
{
	drPlot->clear();
}

void robotPlugin::showPlot(bool state)
{
	drPlot->box->setVisible(state);
}
void robotPlugin::fillPlot(QByteArray x,QByteArray y)
{
	drPlot->fill(x,y);
}
void robotPlugin::pollPlot(QByteArray &x,QByteArray &y)
{
	drPlot->poll(x,y);
}


void robotPlugin::on_set_len(double v){
  if(uiDR->lenSpinBox->hasFocus()) dr->setLen(v);
}
void robotPlugin::on_set_pos(double v){
  if(uiDR->posSpinBox->hasFocus()) dr->setPos(v);
}
void robotPlugin::on_set_back(void){
  if(dr->state&(1<<2)) dr->clrState(1<<2);
  else dr->setState(1<<2);
}
void robotPlugin::on_set_rev(void){
  if(dr->state&(1<<3)) dr->clrState(1<<3);
  else dr->setState(1<<3);
}
void robotPlugin::on_set_acfail(){
  if(dr->state&(1<<4)) dr->clrState(1<<4);
  else dr->setState(1<<4);
}
void robotPlugin::on_set_kir(int v){
  if(v==0) dr->clrState(1<<6);
  else     dr->setState(1<<6);
}
void robotPlugin::onZeroForce(){
	dr->setForce(0);
	std::cout << "robotPlugin zero" << std::endl;
}

void robotPlugin::on_set_limit(int v)
{
	std::cout << "on_set_limit::" << v << std::endl;
	dr->setLimit(v*1000);
}


///////////////////////////////////////
void robotPlugin::on_Stand()
{
	std::cout << "on_Stand" << std::endl;
	int r=QMessageBox::question(urCfgWidget,tr("Warning"),tr("请确保足够空间"),QMessageBox::Yes|QMessageBox::Default,QMessageBox::No|QMessageBox::Escape);
	if(r==QMessageBox::Yes){
		ur->Stand();
		robotSelfChk->setParams(60000, 1000);
		connect(robotSelfChk, SIGNAL(finishedChk(bool)), this, SLOT(chkStand(bool)));
		robotSelfChk->waitForSuccess(&ur->busy, 0);
	}
}

void robotPlugin::on_Pack()
{
	std::cout << "on_Pack" << std::endl;
	int r=QMessageBox::question(urCfgWidget,tr("Warning"),tr("请确保足够空间"),QMessageBox::Yes|QMessageBox::Default,QMessageBox::No|QMessageBox::Escape);
	if(r==QMessageBox::Yes){
		ur->Pack();
		robotSelfChk->setParams(60000, 1000);
		connect(robotSelfChk, SIGNAL(finishedChk(bool)), this, SLOT(chkPack(bool)));
		robotSelfChk->waitForSuccess(&ur->busy, 0);
	}
}

void robotPlugin::chkStand(bool chk)
{
	if(chk == true)
	{
		ur->isStand = 1;
		std::cout << "Stand success" << std::endl;
	}
	else
		std::cout << "Stand failed" << std::endl;
	disconnect(robotSelfChk, SIGNAL(finishedChk(bool)), this, SLOT(chkStand(bool)));
}

void robotPlugin::chkPack(bool chk)
{
	if(chk == true)
	{
		ur->isStand = 0;
		std::cout << "Pack success" << std::endl;
	}
	else
		std::cout << "Pack failed" << std::endl;
	disconnect(robotSelfChk, SIGNAL(finishedChk(bool)), this, SLOT(chkPack(bool)));
}

Q_EXPORT_PLUGIN2(RobotPlugin, robotPlugin);
