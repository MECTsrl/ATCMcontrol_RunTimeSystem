
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
 * Filename: mainwindow.h
 */

// checksum 0x9a77 version 0x30001

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "mainwindow.h"
#include "mainthread.h"
#include "ui_mainwindow.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	public:
		enum ScreenOrientation {
			ScreenOrientationLockPortrait,
			ScreenOrientationLockLandscape,
			ScreenOrientationAuto
		};

		explicit MainWindow(QWidget *parent = 0, char * RemoteAddress = REMOTE_ADDRESS, int RemotePort = REMOTE_PORT );
		virtual ~MainWindow();

		void setOrientation(ScreenOrientation orientation);
		void showExpanded();

	private:
		Ui::MainWindow *ui;
		QLCDNumber * R_array[8];
		QLabel * ledIP[13];
		ComThread * comm_th;
		/* pixmap led */
		QPixmap red_on;
		QPixmap red_off;
		QPixmap green_on;
		QPixmap green_off;
		char * _RemoteAddress;
		int _RemotePort;
	public slots:
		void Close(void);
		void switchLed(int Id, bool status);
		void updateLCD(int Id, float value);
#if 0
	private slots:
		void mousePressEvent(QMouseEvent*);
#endif
};

#endif // MAINWINDOW_H
