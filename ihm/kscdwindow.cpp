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
#include <string.h>
#include <stdio.h>

KscdWindow::KscdWindow(QString skinPath):QWidget()
{
 	setMaximumSize ( 600,400 );
 	m_layout = new QGridLayout;
	m_layout->setSizeConstraint(QLayout::SetMaximumSize);

	m_stopB = new StopButton(this);
	m_playB = new PlayButton(this);
	m_nextB = new NextButton(this);
	m_prevB = new PreviousButton(this);
	m_ejectB = new EjectButton(this);
	m_muteB = new MuteButton(this);
	m_randB = new RandomButton(this);
	m_loopB = new LoopButton(this);
	m_trackB = new TrackListButton(this);
	m_artistLabel = new QLabel("Welcome !");
	m_artistLabel->setFixedWidth(200);

	time = new QLabel(" 0 0 : 0 0 ");

 	m_layout->addWidget(m_ejectB, 0, 1);
 	m_layout->addWidget(m_prevB, 1, 0);
 	m_layout->addWidget(m_playB, 1, 1);
 	m_layout->addWidget(m_nextB, 1, 2);
 	m_layout->addWidget(m_stopB, 2, 1);
	m_layout->addWidget(m_randB, 3, 0,Qt::AlignCenter);
	m_layout->addWidget(m_loopB, 3, 2,Qt::AlignCenter);
	m_layout->addWidget(m_muteB, 3, 1,Qt::AlignCenter);
	m_layout->addWidget(m_trackB, 3, 3,Qt::AlignCenter);
	m_layout->addWidget(time,0,3,Qt::AlignCenter);
	m_layout->addWidget(m_artistLabel, 1, 3,Qt::AlignCenter);
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
	delete m_layout;
}

void KscdWindow::addSeekSlider(Phonon::SeekSlider *ss)
{
	m_layout->addWidget((QWidget*)ss, 2, 3);
}

QString KscdWindow::getSkinPath()
{
	return m_skinPath;
}

void KscdWindow::setTime(qint64 pos)
{
	qint64 md = ((pos/1000)/60)/10;
	qint64 mu = ((pos/1000)/60)%10;
	qint64 sd = ((pos/1000)%60)/10;
	qint64 su = ((pos/1000)%60)%10;

QString result;
     QTextStream(&result) << " " << md << " " << mu << " : " << sd << " " << su << " ";
	time->setText(result);
}

void KscdWindow::setSkinPath(QString sPath)
{
	m_skinPath = sPath;
}

QLabel* KscdWindow::getArtistLabel()
{
	return (m_artistLabel);
}
void KscdWindow::setArtistLabel(QString artist)
{
	m_artistLabel->setText(artist);
}

void KscdWindow::catchButton(QString name)
{
	kDebug()<<"Catch :" << name;
	emit(actionClicked(name));
}

void KscdWindow::changePicture(QString name,StateButton state)
{
	if(name == "play")
	{
		m_playB->loadPicture(m_playB->findFile("pause",state));
	}
	if(name == "pause")
	{
		m_playB->loadPicture(m_playB->findFile("play",state));
	}
	if(name == "stop")
	{
		m_stopB->loadPicture(m_stopB->findFile(name,state));
		m_playB->setName("play");
		m_playB->loadPicture(m_playB->findFile(m_playB->getName(),Default));
	}
	if(name == "eject")
	{
		m_ejectB->loadPicture(m_ejectB->findFile(name,state));
	}
	if(name == "next")
	{
		m_nextB->loadPicture(m_nextB->findFile(name,state));
	}
	if(name == "previous")
	{
		m_prevB->loadPicture(m_prevB->findFile(name,state));
	}
	if(name == "mute")
	{
		m_muteB->loadPicture(m_muteB->findFile(name,state));
	}
	if(name == "unmute")
	{
		m_muteB->loadPicture(m_muteB->findFile(name,state));
	}
	if(name == "random")
	{
		m_randB->loadPicture(m_randB->findFile(name,state));
	}
	if(name == "p_random")
	{
		m_randB->loadPicture(m_randB->findFile("random",Pressed));
	}
	if(name == "loop")
	{
		m_loopB->loadPicture(m_loopB->findFile(name,state));
	}
	if(name == "looptrack")
	{
		m_loopB->loadPicture(m_loopB->findFile(name,state));
	}
	if(name == "loopdisc")
	{
		m_loopB->loadPicture(m_loopB->findFile(name,state));
	}
	if(name == "tracklist")
	{
		m_trackB->loadPicture(m_trackB->findFile(name,state));
	}
}