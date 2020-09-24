#ifndef _ROBOTPLUGIN_H_
#define _ROBOTPLUGIN_H_

#include <QObject>
#include <QWidget>
#include "interfaces\robotInterface.h"
#include "ur.h"
#include "dr.h"
#include "ui_ur.h"
#include "ui_dr.h"
#include "urCfg.h"
#include "drCfg.h"
#include "drPlot.h"
#include "robotSelfChk.h"
#define R2D(rad)    (57.29578 * rad)


typedef struct
{
	short flag;
	short ver;
	unsigned int devsn;
	short drill[10];
	short coeff[2];
} _DEV_INFO;

class robotPlugin : public QObject, public RobotInterface
{
  Q_OBJECT
  Q_INTERFACES(xjtInterfaceBase)
  Q_INTERFACES(RobotInterface)

public:
  robotPlugin();
  ~robotPlugin();
  QObject* Object() { return this; }

  void start();
  void stop();
  void move(float *);
  void trans(float *,bool pre);
  void setPrePose(void);
  void clrPrePose(void);
  int busy(void);

  void setForce(float);
  void setDrill(short);
  int getDrillLevel();
  short *getDrillValue();
  void saveDrillValue();
  void setUrSpeed(float);
  void setGoBack();
  float getDrPos();
  void clrPlot();
  void showPlot(bool);
  void fillPlot(QByteArray x,QByteArray y);
  void pollPlot(QByteArray &x,QByteArray &y);

	int isStand;
	int urPULSE;
	int drPULSE;
	bool displaySelfInspectionMsg(QString first, QString second = NULL, bool chk = false);
public slots:
  void onPulse(void);
  void urUpdate(void);
  void STOP(void);
  void BACK(void);
  void LOCK(void);
  void POWERON();
  void POWEROFF();

  void on_URUP_pressed();
  void on_URDOWN_pressed();
  void on_URFORW_pressed();
  void on_URBACK_pressed();
  void on_URLEFT_pressed();
  void on_URRIGHT_pressed();
  void on_XL_pressed();
  void on_XR_pressed();
  void on_YL_pressed();
  void on_YR_pressed();
  void on_ZR_pressed();
  void on_ZL_pressed();
  void on_UR_released();
  void on_URFREE_pressed();
  void on_URFREE_released();
  void on_FreeMode(int);
  void on_UR_speed(int);

  void drUpdate(void);
  void drDevice(void);
  int STATE(void);
  void PAUSE(void);
  void START(void);
  void PARAM(int speed,int drill,float len, bool dir);
  void onZeroForce(void);
  void on_set_len(double);
  void on_set_pos(double);
  void on_set_back();
  void on_set_rev();
  void on_set_acfail();
  void on_set_kir(int);
  void on_set_limit(int);

  void onLED();

  void on_Stand();
  void on_Pack();
	void chkStand(bool chk);
	void chkPack(bool chk);
private:
  UrMoniter *moniter;
  UrCtrl *ur;
  DrCtrl *dr;
  Ui::UR *uiUR;
  Ui::DR *uiDR;

  UrCfg *urCfg;
  DrCfg *drCfg;
  DrPlot *drPlot;
	RobotSelfChk *robotSelfChk;
  float urPrePose[6];
  float getMovement();
  void showMovement();

signals:
  void drSpeedChanged(int);
  void drUpdated(void);
  void calibUpdate(float);
};

#endif//_ROBOTPLUGIN_H_
