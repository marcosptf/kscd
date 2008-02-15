/*
 * Kscd - A simple cd player for the KDE Project
 *
 * Copyright (c) 1997 Bernd Johannes wuebben@math.cornell.edu
 * Copyright (c) 2002-2003 Aaron J. Seigo <aseigo@kde.org>
 * Copyright (c) 2004 Alexander Kern <alex.kern@gmx.de>
 * Copyright (c) 2003-2006 Richard Lärkäng <nouseforaname@home.se>
 *
 * --------------
 * ISI KsCD Team :
 * --------------
 * Stanislas KRZYWDA <stanislas.krzywda@gmail.com>
 * Sovanramy Var <mastasushi@gmail.com>
 * Bouchikhi Mohamed-Amine <bouchikhi.amine@gmail.com>
 * Gastellu Sylvain<sylvain.gastellu@gmail.com>
 * -----------------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#include "kscdwindow.h"
#include "titlePopUp.h"
#include <QString>
#include <QTextEdit>
#include <klocalizedstring.h>

#include "panel.h"
#include "configwindow.h"

KscdWindow::KscdWindow(QWidget *parent):QWidget(parent)
{
 	setFixedSize ( 600,400 );
 	m_layout = new QGridLayout;
	m_layout->setSizeConstraint(QLayout::SetFixedSize);

	m_stopB = new StopButton(this);
	m_playB = new PlayButton(this);
	m_nextB = new NextButton(this);
	m_prevB = new PreviousButton(this);
	m_ejectB = new EjectButton(this);
	m_muteB = new MuteButton(this);
	m_randB = new RandomButton(this);
	m_loopB = new LoopButton(this);
	m_trackB = new TrackListButton(this);
	m_volumeB = new VolumeButton(this);
	
	
	m_stateTrackDialog = false;
	m_trackDlgCreated = false;
 	m_trackDlg = new TrackListDlg(parent);
// 	createTrackWindow();

	// Panel
	m_panel = new Panel(this);

//  	instantiating of kscd title popup
	m_titlePopUp = new TitlePopUp();

	// Configuration windows
	ConfigWindow *m_config = new ConfigWindow(this);

	QGridLayout* panelLayout = new QGridLayout;
	m_layout->addLayout(panelLayout, 0, 3, 2, 1);
	m_time = new QLCDNumber();
	m_time->display("00:00:00");
// 	QTextEdit * t = new QTextEdit("<b>Welcome to KsCD !</b>");
	m_artistLabel = new QLabel(i18n("<qt><font face=Impact>Welcome to KsCD !</font></qt>"));
	m_artistLabel->setFixedWidth(250);
	m_artistLabel->setAlignment(Qt::AlignCenter);
	
	m_trackinfoLabel = new QLabel;
	m_trackinfoLabel->setFixedWidth(250);
	m_trackinfoLabel->setAlignment(Qt::AlignCenter);

	// Items positionment
	m_layout->addWidget(m_ejectB, 0, 1);
	m_layout->addWidget(m_prevB, 1, 0);
	m_layout->addWidget(m_playB, 1, 1);
	m_layout->addWidget(m_nextB, 1, 2);
	m_layout->addWidget(m_stopB, 2, 1);
	m_layout->addWidget(m_volumeB, 0,5,3,1);
	m_layout->addWidget(m_time,1,3,1,2);
	m_layout->addWidget(m_panel, 0,3,2,2);


//    	m_layout->addWidget(m_volumeB, 1,5,Qt::AlignCenter);
	m_layout->addWidget(m_randB, 3, 0, Qt::AlignCenter);
	m_layout->addWidget(m_loopB, 3, 2, Qt::AlignCenter);
	m_layout->addWidget(m_muteB,0, 6, Qt::AlignCenter);
	m_layout->addWidget(m_trackB, 3, 6, Qt::AlignCenter);

	setLayout(m_layout);
	show();

	connect(m_stopB,SIGNAL(buttonClicked(QString)),SLOT(catchButton(QString)));
	connect(m_playB,SIGNAL(buttonClicked(QString)),SLOT(catchButton(QString)));
	connect(m_prevB,SIGNAL(buttonClicked(QString)),SLOT(catchButton(QString)));
	connect(m_nextB,SIGNAL(buttonClicked(QString)),SLOT(catchButton(QString)));
	connect(m_ejectB,SIGNAL(buttonClicked(QString)),SLOT(catchButton(QString)));
	connect(m_muteB,SIGNAL(buttonClicked(QString)),SLOT(catchButton(QString)));
	connect(m_randB,SIGNAL(buttonClicked(QString)),SLOT(catchButton(QString)));
	connect(m_loopB,SIGNAL(buttonClicked(QString)),SLOT(catchButton(QString)));
	connect(m_trackB,SIGNAL(buttonClicked(QString)),SLOT(catchButton(QString)));
	connect(m_volumeB,SIGNAL(buttonClicked(QString)),SLOT(catchButton(QString)));

	connect(m_volumeB,SIGNAL(volumeChange(qreal)),SLOT(catchVolume(qreal)));

	connect(m_trackDlg,SIGNAL(itemClicked(int))
		,this,SLOT(doubleClickedEvent(int)));

}

KscdWindow::~KscdWindow()
{
	delete m_playB;
	delete m_stopB;
	delete m_nextB;
	delete m_prevB;
	delete m_ejectB;
	delete m_muteB;
	delete m_trackB;

	delete m_titlePopUp;

	delete m_layout;
	

	delete m_trackDlg;
}

void KscdWindow :: closeTrackDialog()
{
	kDebug()<<"Close Track Dialog";
	m_stateTrackDialog = false;
	m_trackDlg->hide();
}

void KscdWindow :: createTrackDialog(QList<CDDBTrack> trackList,QString albumTitle)
{
	QList<CDDBTrack>::iterator it;
	m_trackDlg->removeRowsTrackTable(trackList.size());

	m_stateTrackDialog = true;
	m_trackDlg->setAlbumLbl(albumTitle);
 	int trackNumber = 1;
	for(it = trackList.begin();it != trackList.end();it++)
	{
		m_trackDlg->addRowTrackTable(trackNumber-1);
		m_trackDlg->addItemTrackTable(trackNumber-1,0,QString::number(trackNumber));
		m_trackDlg->addItemTrackTable(trackNumber-1,1,(*it).Title);
		m_trackDlg->addItemTrackTable(trackNumber-1,2,(*it).Length);
		m_trackDlg->setYearLbl((*it).Year);
		trackNumber++;
	}
	m_trackDlg->moveTrackDialog(x(),y()+frameGeometry().height());
	m_trackDlg->show();
}

void KscdWindow :: doubleClickedEvent(int pos)
{
	kDebug()<<"signal recu\n"<<"pos clicked:"<<pos;
 	emit(trackClicked(pos));
}

void KscdWindow::addSeekSlider(Phonon::SeekSlider *ss)
{
	m_layout->addWidget((QWidget*)ss, 2, 3,1,2);
}
void KscdWindow::addVolumeSlider(Phonon::VolumeSlider *vs)
{
	m_layout->addWidget((QWidget*)vs, 1, 4);

}


void KscdWindow::setTime(qint64 pos)
{
	qint64 md = ((pos/1000)/60)/10;
	qint64 mu = ((pos/1000)/60)%10;
	qint64 sd = ((pos/1000)%60)/10;
	qint64 su = ((pos/1000)%60)%10;

	QString result;
	QTextStream(&result) << md << mu << ":" << sd << su;
	m_time->display(result);
}

/**
 * Manages the Artist label
 */
void KscdWindow::showArtistLabel(QString infoStatus)
{
	m_panel->setAuthor(&infoStatus);
}

/**
 * Manages the Trackinfo Label
 */
void KscdWindow::showTrackinfoLabel(QString infoStatus)
{
	m_panel->setTitle(&infoStatus);
}

/**
* hide the title popUp
*/
void KscdWindow::hideTitlePopUp()
{
	if(m_titlePopUp != NULL)
	{
		m_titlePopUp->hide();
	}
}

/**
 * Links treatments with the UI
 */
void KscdWindow::catchButton(QString name)
{
	kDebug()<<"Catch :" << name;
	emit(actionClicked(name));
}

void KscdWindow::catchVolume(qreal value)
{
	emit(actionVolume(value));
}
void KscdWindow::changePicture(QString name,QString state)
{
	if(name == "play")
	{
		m_playB->loadPicture("pause",state);
		m_playB->setName("pause");
	}
	if(name == "pause")
	{
		m_playB->loadPicture("play",state);
		m_playB->setName("play");
	}
	if(name == "stop")
	{
		m_stopB->loadPicture(name,state);
		m_playB->setName("play");
		m_playB->loadPicture(m_playB->getName(),"default");
	}
	if(name == "eject")
	{
		m_ejectB->loadPicture(name,state);
		m_playB->setName("play");
		m_playB->loadPicture(m_playB->getName(),"default");
	}
	if(name == "next")
	{
		m_nextB->loadPicture(name,state);
	}
	if(name == "previous")
	{
		m_prevB->loadPicture(name,state);
	}
	if(name == "mute")
	{
		m_muteB->loadPicture(name,state);
	}
	if(name == "unmute")
	{
		m_muteB->loadPicture(name,state);
	}
	if(name == "random")
	{
		kDebug() << 3;
		m_randB->loadPicture(name,state);
	}
	if(name == "p_random")
	{
		kDebug() << 4;
		m_randB->loadPicture("random","pressed");
		
	}
	if(name == "loop")
	{
		m_loopB->loadPicture(name,state);
	}
	if(name == "looptrack")
	{
		m_loopB->loadPicture(name,state);
	}
	if(name == "loopdisc")
	{
		m_loopB->loadPicture(name,state);
	}
	if(name == "tracklist")
	{
		m_trackB->loadPicture(name,state);
	}
}
Panel * KscdWindow::getPanel(){
	return m_panel;
}
