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
 * it under the terms of the GNU General Public License as published byfor the time
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

#include "panel.h"
#include <kdebug.h>
#include <QTimer>
#include <QTextEdit>


Panel::Panel(QWidget * parent, QString sName):KscdWidget(sName,parent)
{
	m_bounds = new QRegion((m_renderer->boundsOnElement(getId())).toRect(),QRegion::Ellipse);
	move((m_bounds->boundingRect()).x(),(m_bounds->boundingRect()).y());

	setAutoFillBackground(true); 
	p_panelColor= new QPalette(Qt::transparent);
	setPalette(*p_panelColor);
	vbl_layout = new QGridLayout();
	vbl_layout->setVerticalSpacing(1);

	index=0;
	
	l_title = new QLabel("");
	vbl_layout->addWidget(l_title,4,0,1,NULL);
	l_album = new QLabel("<center>WELCOME!</center>");
	vbl_layout->addWidget(l_album,3,0);
	l_author = new QLabel("");
	vbl_layout->addWidget(l_author,2, 0);
	l_loop = new QLabel("");
	l_random = new QLabel("");
	l_info = new QLabel("");
	textSize = new QLabel("");
	vbl_layout->addWidget(l_info,1,0);
//	l_volume = new QLabel("");
//	vbl_layout->addWidget(l_volume, 4, 1);
	l_time = new QLabel("<center><font size="+textSize->text()+"><b>00 : 00</b></font></center>");
	vbl_layout->addWidget(l_time, 6, 0);
	setLayout(vbl_layout);

	QTimer * timer = new QTimer ();
	timer->setSingleShot(false);
	connect(timer,SIGNAL(timeout()),this,SLOT(update_panel_label()));
	timer->start(150);
}

void Panel::update_panel_label(){


	if(l_title->text().count()>0)
	{
		// if the size is lower than the size of the panel
		while(l_title->text().count()< 53)
		{
			//add  " " to have the same size that the panel
			l_title->setText(l_title->text()+" ");
		}
		
		//recup the first letter
		QChar c = l_title->text().data()[0];
		
		//create a new data
		QString data;
		for(int i = 1; i <l_title->text().count(); i++)
		{
			data = data+l_title->text().data()[i];
		}
		//add the last letter
		data =data+c;
		setTitle(& data);
	}
}

Panel::~Panel()
{
	delete 	timer;
	delete vbl_layout;
	delete l_title;
	delete p_panelColor;

	delete l_author;
	delete l_album;
	delete l_playing_state;
	delete l_volume;
	delete l_time;
}

QString Panel::getTitle()
{
	return l_title->text();
}
QString Panel::getAlbum()
{
	return l_album->text();

}
QString Panel::getAuthor()
{
	return l_author->text();

}
QString Panel::getVolume()
{
	return l_volume->text();

}
QString Panel::getLoop()
{
	return l_loop->text();
}
QString Panel::getRandom()
{
	return l_random->text();
}
void Panel::setTitle(QString * title)
{
	l_title->setText(* title);
}
void Panel::setAuthor(QString * author)
{
	l_author->setText(* author);
}
void Panel::setAlbum(QString * album)
{

	QString mess = * album;
	
	l_album->setText(mess);

}
void Panel::setVolume(QString * volume)
{
	l_title->setText(*volume);

}
void Panel::setTextSize(QString size){
	kDebug() << "TAILLE DE LA POLICE : " << size;
	l_author->setFont( QFont( "Times", size.toInt(), QFont::Normal ));
	l_title->setFont( QFont( "Times", size.toInt(), QFont::Normal ));
	l_album->setFont( QFont( "Times", size.toInt(), QFont::Normal ));
	l_time->setFont( QFont( "Times", size.toInt(), QFont::Normal ));
	l_info->setFont( QFont( "Times", size.toInt(), QFont::Normal ));
}


void Panel::setTextColor(QColor c){
	color = c;
	QColorGroup grp( QColor( c.red(), c.green(), c.blue() ), Qt::black, QColor( 128, 128, 128 ),
	QColor( 64, 64, 64 ), Qt::black, Qt::darkGreen, Qt::black );
	QPalette pal( grp, grp, grp );
	l_title->setPalette(pal);
	l_author->setPalette(pal);
	l_album->setPalette(pal);
	l_time->setPalette(pal);
	l_info->setPalette(pal);
}

void Panel :: mousePressEvent(QMouseEvent *event)
{
	if(m_bounds->contains(event->pos()+(m_bounds->boundingRect()).topLeft()))
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void Panel :: mouseReleaseEvent(QMouseEvent *event)
{
	if(m_bounds->contains(event->pos()+(m_bounds->boundingRect()).topLeft()))
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void Panel :: enterEvent (QEvent * event)
{
	event->ignore();
}

void Panel :: leaveEvent (QEvent * event)
{
	event->ignore();
}

void Panel::setTime(qint64 pos)
{
	qint64 md = ((pos/1000)/60)/10;
	qint64 mu = ((pos/1000)/60)%10;
	qint64 sd = ((pos/1000)%60)/10;
	qint64 su = ((pos/1000)%60)%10;
	QString result;
	QTextStream(&result) << "<center><b>" << md << mu << " : " << sd << su << "</b></center>";
	l_time->setText(result);
}

void Panel::setLoop(QString loop)
{
	l_loop->setText(loop);
}
void Panel::setRandom(QString random)
{
	l_random->setText(random);
}

//concatenation display info random and loop panel
void Panel::displayInfo(QString loop, QString random)
{
	QString info;
	QTextStream(&info)<<loop<<random;
	l_info->setText(info);
}
QString Panel::getInfo()
{
	return l_info->text();
}