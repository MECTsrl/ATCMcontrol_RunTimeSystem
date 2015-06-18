/*
 * Copyright 2011 Mect s.r.l
 *
 * This file is part of FarosPLC.
 *
 * FarosPLC is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * FarosPLC is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * FarosPLC. If not, see http://www.gnu.org/licenses/.
*/

/*
 * Filename: mainthread.h
 */

#ifndef MAIN_TREAD_H
#define MAIN_TREAD_H
#include <QThread>
#include <QLineEdit>
#include <QListWidget>
#include <QLCDNumber>
#include <QObject>
#include <QLabel>
#include "fc_comunication.h"


#define HMI_PERIOD_MS 500
#define HOLD_SAMPLE 5
//#define REMOTE_ADDRESS "192.168.0.220"
//#define REMOTE_ADDRESS "192.168.0.214"
//#define REMOTE_ADDRESS "192.168.0.220"
//#define REMOTE_ADDRESS "192.168.0.24"
#define REMOTE_ADDRESS "127.0.0.1"
#define REMOTE_PORT 17290

class ComThread : public QThread
{
	Q_OBJECT
	public:
		ComThread( char * RemoteAddress = REMOTE_ADDRESS, int RemotePort = REMOTE_PORT );
		~ComThread();
		bool readVar(int index, char * variable);
		bool readIP(void);
	private:
		bool done;
		FC_comunication comm;
		virtual void run();
		bool getVariableList();
		QLCDNumber ** _R_array;
		char * _RemoteAddress;
		int _RemotePort;
	signals:
		void switch_led(int Id, bool status);
		void update_lcd(int Id, float value);
};

#endif

