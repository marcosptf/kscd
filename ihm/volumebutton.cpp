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
#include "volumebutton.h"

VolumeButton::VolumeButton(QWidget * parent,QString sName,qreal value):KscdWidget(sName,parent)
{
	m_bounds = new QRegion((m_renderer->boundsOnElement(getId())).toRect(),QRegion::Ellipse);
	move((m_bounds->boundingRect()).x(),(m_bounds->boundingRect()).y());
	m_vValue = value;
	m_angle = valueToAngle(m_vValue);
	m_centerX = width()/2;
	m_centerY = height()/2;
	m_move = false;
}

VolumeButton::~VolumeButton()
{
	delete m_bounds ;
}

/* change skin path and refresh */
void VolumeButton::changeSkin(QString newPathSkin)
{
	QString newId = m_baseName + "_default";
			
	m_path=newPathSkin;

	m_renderer->load(m_path);
	if (m_renderer->elementExists(m_id))
	{
	//	loadPicture(getName(),"default");
		setFixedSize(m_renderer->boundsOnElement(newId).width(),
					m_renderer->boundsOnElement(newId).height());
		
		m_bounds = new QRegion((m_renderer->boundsOnElement(newId)).toRect(),QRegion::Ellipse);
		
		move(m_renderer->boundsOnElement(newId).x(),
			m_renderer->boundsOnElement(newId).y());
		
		emit(changePicture());
		emit(needRepaint());
		m_centerX = width()/2;
		m_centerY = height()/2;
	}
}

void VolumeButton :: mousePressEvent(QMouseEvent *event)
{
	if(m_bounds->contains(event->pos()+(m_bounds->boundingRect()).topLeft()))
	{
		event->accept();
		m_posX = event->x();
		m_posY = event->y();
		m_move =true;
		grabMouse(Qt::ClosedHandCursor);
	}
	else
	{
		event->ignore();
	}
}
// 
void VolumeButton :: mouseMoveEvent(QMouseEvent *event)
{
      if(m_bounds->contains(event->pos()+(m_bounds->boundingRect()).topLeft()) && m_move == true){
	    event->accept();

	    //find the angle
	    
		//find three triangle side's length	
	    qreal b=(qreal)sqrt((m_centerX - m_posX)*(m_centerX - m_posX) + (m_centerY - m_posY)*(m_centerY - m_posY));
	    qreal a=(qreal)sqrt((event->x() - m_posX)*(event->x() - m_posX) + (event->y()- m_posY)*(event->y()- m_posY));
	    qreal c=(qreal)sqrt((m_centerX - event->x())*(m_centerX - event->x()) + (m_centerY - event->y())*(m_centerY - event->y()));
	    	//find angle in radian between last point, center point and the new point
		qreal A=(qreal)acos((b*b+c*c-a*a)/(2*b*c));
		
	//sound must be higher
	if(event->x()>m_centerX && event->y()< m_posY 
		|| event->x()< m_centerX && event->y()>m_posY 
		|| event->y()<m_centerY && event->y()==m_posY && event->x()<m_posX
		|| event->y()>m_centerY && event->y()==m_posY && event->x()>m_posX){
		
		if((m_angle - (180 * (A))/3.1415)>=0){
			rotation(m_angle - (180 * (A))/3.1415); //change m_angle
			m_vValue=angleToValue(m_angle);		//change m_vValue
			emit(volumeChange(m_vValue));		// change sound volume
		}
	//sound must be lower
	}else if(event->x()<=m_centerX && event->y()< m_posY 
		|| event->x()>= m_centerX && event->y()>m_posY 
		|| event->y()<m_centerY && event->y()==m_posY && event->x()>m_posX
		|| event->y()>m_centerY && event->y()==m_posY && event->x()<m_posX){


		if((m_angle + (180 * (A))/3.1415)<=250){
			rotation(m_angle + (180 * (A))/3.1415);
			m_vValue=angleToValue(m_angle);
			emit(volumeChange(m_vValue));
		}

	}

	    //new position
	    m_posX = event->x();
	    m_posY = event->y();kDebug()<<"Quit Shortcut";

      }else{
	    event->ignore();
      }
}


void VolumeButton :: mouseReleaseEvent(QMouseEvent *event)
{
	releaseMouse();
	m_move = false;
}

void VolumeButton :: wheelEvent(QWheelEvent *event)
{
	qreal step = valueToAngle((event->delta()/120)*wheelStep);

	if((m_angle + step)>=0 && (m_angle + step)<=250)
	{	
//		event->accept();
		m_angle += step;
		m_vValue += (event->delta()/120)*wheelStep;
		emit(volumeChange(m_vValue));
		rotation(m_angle);
	}
	else
	{
		event->ignore();
	}
}


void VolumeButton::rotation (qreal angle)
{
	m_angle = angle;
	emit(update());
}

qreal VolumeButton::angleToValue(qreal angle)
{
	return angle/wheelStep ;
}

qreal VolumeButton::valueToAngle(qreal value)
{
	return value*wheelStep ;
}
void VolumeButton :: paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.translate((m_renderer->boundsOnElement(m_id).width())/2,
				(m_renderer->boundsOnElement(m_id).height())/2);
	painter.rotate(m_angle);
	painter.translate(-(m_renderer->boundsOnElement(m_id).width())/2,
				-(m_renderer->boundsOnElement(m_id).height())/2);
	m_renderer->render(&painter,m_id);
}

void VolumeButton :: enterEvent (QEvent * event )
{
	event->accept();
	setToolTip(m_name);
}

void VolumeButton :: leaveEvent (QEvent * event )
{
	event->accept();
}

void VolumeButton::volumeShortcut(qreal value)
{
	if (m_vValue<=100 && m_vValue>=0)
	{
		qreal angle = valueToAngle(value);
		rotation (m_angle + angle);
		m_vValue+=value;
		emit(volumeChange(m_vValue));
	}
}
