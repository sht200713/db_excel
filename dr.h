#ifndef __DR_H__
#define __DR_H__

#include<QDebug>
#include<QtNetwork/QTcpSocket>
#include<QFile>


#define SWAP(n)	(((n>>8)&0xFF) | ((n<<8)&0xFF00))


class DrLink : public QTcpSocket
{
	Q_OBJECT

public:
	DrLink();

	QString host;
	int port;
	int payload;

	void connectToBot(void);
	void query();

    char drbuff[100];
    char device[100];

public slots:
    void connected();
    void disconnected();
	void displayError(QAbstractSocket::SocketError socketError);
	void disconnectFromBot(void);
};

class DrCtrl : public DrLink
{
    Q_OBJECT

public:
	DrCtrl();

	int state;
	int port;
	float data[10];

	short DrillValue[11];
	void SaveDrillValue();
  int drPULSE;
signals:
	void linked();
public slots:
    void connected();

public:
	void test();
    bool update(void);
    void setRegs(short *cmds,short count);

	void START(short speed,short drill);
	void PAUSE();
	bool PULSE();
    void setForce(short v);
    void setLimit(short v);
    void setDelta(short v);
    void setDrill(short v);
    void setLen(float v);
    void setPos(float v);
    void setState(short v);
    void clrState(short v);
    void setPort(short v);
    void clrPort(short v);

    void setLED(short v);

private:
	enum{	//请跟下位机同步
		REG_STATE=0,
		REG_POS,
		REG_LEN,
		REG_SPEED,
		REG_DRILL,
		REG_FORCE,
		REG_DELTA,
		REG_PORT,
		REG_TEST,
		REG_LIMIT,
		REG_DRILL_VALUE1=100,
		REG_DRILL_VALUE2,
		REG_DRILL_VALUE3,
		REG_DRILL_VALUE4,
		REG_DRILL_VALUE5,
		REG_DRILL_VALUE6,
		REG_DRILL_VALUE7,
		REG_DRILL_VALUE8,
		REG_DRILL_VALUE9,
		REG_DRILL_VALUE10,
		REG_END
	};
};


#endif//__DR_H__
