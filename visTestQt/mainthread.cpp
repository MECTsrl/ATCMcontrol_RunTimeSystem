#include "mainwindow.h"
#include "mainthread.h"
#include <QPicture> 

ComThread::ComThread( char * RemoteAddress, int RemotePort )
{
	done = true;
	_RemoteAddress = RemoteAddress;
	_RemotePort = RemotePort;
}

ComThread::~ComThread()
{
	done = false;
}

bool ComThread::readVar(int index, char * variable)
{
	char StrData[100];
	float FloatData;
	bool ret;

	if (comm.readValue(variable, StrData))
	{

		FloatData=atof(StrData);
		ret = true;
	}
	else
	{
		FloatData=-1;
		strcpy(StrData, "ERR");
		printf("@@@@@@@@%s\n", "ERR");
		ret = false;
	}
	printf("@@@@@@@@%d - %s - %f\n",index, StrData, FloatData);
	emit update_lcd(index, FloatData);
	return ret;
}

#define BYTENUM 4
bool ComThread::readIP(void)
{
	char StrData[100];
	int index, bit;

	if (comm.readValue("IP", StrData))
	{
		sprintf (StrData, "%d", atoi(StrData));
		//printf("%s %s\n",  __func__, StrData);
		for (index = 0; index < BYTENUM; index++)
		{
			for (bit = 0; bit < 4 && bit + (4 * index) < 13; bit++)
			{
				/* ON */
				if ((((StrData[index] - '0') >> bit) & 0x1) == 0x1)
				{
					//printf("%d ON\n",  bit + (4 * index));
					emit switch_led(bit + (4 * index), true);
				}
				/* OFF */
				else
				{
					//printf("%d OFF\n",  bit + (4 * index));
					emit switch_led(bit + (4 * index), false);
				}
			}
		}
		//printf("%s EXIT\n", __func__ );
		return true;
	}
	else
	{
		for (index = 0; index < 13; index++)
		{
			//printf("%d ERR\n",  index);
			emit switch_led(index, false);
		}
		return false;
	}
}

void ComThread::run()
{
	done = true;

	if (comm.connect(_RemoteAddress, _RemotePort))
	{

		while (done)
		{
			readVar(0, "R0");
			readVar(1, "R1");
			readVar(2, "R2");
			readVar(3, "R3");
			readVar(4, "R4");
			readVar(5, "R5");
			readVar(6, "R6");
			readVar(7, "R7");
			readIP();

			usleep(HMI_PERIOD_MS * 1000);
		}

		comm.disconnect();
	}
	else
	{
		done = false;
	}
}

