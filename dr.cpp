#include "dr.h"


#define GEAR	135.0


DrLink::DrLink()
	:host("192.168.1.123"),port(502)
{
	setSocketOption(KeepAliveOption, 1);
	//setReadBufferSize(payload);
	connect(this, SIGNAL(connected()),this, SLOT(connected()));
	connect(this, SIGNAL(disconnected()),this, SLOT(disconnected()));
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
	//connect(this, SIGNAL(readyRead()),this, SLOT(readyRead()));
}

void DrLink::connected() {qDebug() << "dr connected";}
void DrLink::disconnected() {qDebug() << "dr disconnected";}
void DrLink::displayError(QAbstractSocket::SocketError socketError) {qDebug() << socketError;}
//void UrLink::readyRead(void) {qDebug() << "readyRead";query();}


void DrLink::connectToBot(void)
{
	connectToHost(host,port);
}
void DrLink::disconnectFromBot(void)
{
	qDebug() << "disconnectFromBot";
	disconnectFromHost();
}

void DrLink::query()
{
	while(bytesAvailable() >= payload)
	{
		/*qDebug()<<*/read(drbuff,payload);
	}
}


///////////////////////////////////////////////////////////////////////////////
DrCtrl::DrCtrl(void)
{
	connect(this, SIGNAL(connected()),this, SLOT(connected()));
}
void DrCtrl::connected() { qDebug("dr reset"); payload=60; PAUSE(); }

bool DrCtrl::PULSE(void)
{
	if(QTcpSocket::state()==0){
		connectToBot();
	}
	if(QTcpSocket::state()==3){
		return true;
	}
	return false;
}

bool DrCtrl::update(void)
{
	query();

	signed short *p = (signed short *)drbuff;

	if(p[0] == -1)	//首帧标记-1，方便识别
	{
		memcpy(device,drbuff,payload);
		emit linked();
		payload = 20;
		return false;
	}
	else
	{
		state = p[0];
		port = p[7];
		data[0] = p[1] / GEAR;      //pos
		data[1] = p[2] / GEAR;      //len
		data[2] = p[3] / GEAR;		//speed
		data[3] = p[4];             //drill
		data[4] = p[5] / 1000.0;	//force
		data[5] = p[6] / 1000.0;	//delta
		//qDebug()<< p[8]<<","<<p[9];
		return true;
	}
}

void DrCtrl::setRegs(short *cmds,short count)
{
	write((const char *)cmds,count*4);
}

void DrCtrl::test()
{
  short cmds[]={99, 0};
  setRegs(cmds,1);
}

void DrCtrl::setState(short v)
{
  short cmds[]={REG_STATE, v};
  setRegs(cmds,1);
}
void DrCtrl::clrState(short v)
{
  short cmds[]={REG_STATE, -32768|v};
  setRegs(cmds,1);
}
void DrCtrl::setPort(short v)
{
  short cmds[]={REG_PORT, v};
  setRegs(cmds,1);
}
void DrCtrl::clrPort(short v)
{
  short cmds[]={REG_PORT, -32768|v};
  setRegs(cmds,1);
}

void DrCtrl::setLED(short v)
{
  clrPort(0x07);
  setPort(v);
}

void DrCtrl::setPos(float v)
{
  short cmds[]={REG_POS, v*GEAR};
  setRegs(cmds,1);
}
void DrCtrl::setLen(float v)
{
  short cmds[]={REG_LEN, v*GEAR};
  setRegs(cmds,1);
}
void DrCtrl::setForce(short v)    //mN
{
  short cmds[]={REG_FORCE, v};
  setRegs(cmds,1);
}
void DrCtrl::setLimit(short v)    //mN
{
  short cmds[]={REG_LIMIT, v};
  setRegs(cmds,1);
}
void DrCtrl::setDelta(short v)    //mN
{
  short cmds[]={REG_DELTA, v};
  setRegs(cmds,1);
}
void DrCtrl::setDrill(short v)
{
  short cmds[]={REG_DRILL, v};
  setRegs(cmds,1);
}

void DrCtrl::PAUSE()
{
	clrState(1);
}
void DrCtrl::START(short speed,short drill)
{
  short cmds[]={REG_STATE,1,  REG_SPEED,speed,  REG_DRILL,DrillValue[drill]};
  setRegs(cmds,3);
}

void DrCtrl::SaveDrillValue()
{
  short cmds[]={
	  REG_DRILL_VALUE1,  DrillValue[1],
	  REG_DRILL_VALUE2,  DrillValue[2],
	  REG_DRILL_VALUE3,  DrillValue[3],
	  REG_DRILL_VALUE4,  DrillValue[4],
	  REG_DRILL_VALUE5,  DrillValue[5],
	  REG_DRILL_VALUE6,  DrillValue[6],
	  REG_DRILL_VALUE7,  DrillValue[7],
	  REG_DRILL_VALUE8,  DrillValue[8],
	  REG_DRILL_VALUE9,  DrillValue[9],
	  REG_DRILL_VALUE10, DrillValue[10]
  };
  setRegs(cmds,10);
}
