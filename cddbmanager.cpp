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
#include "cddbmanager.h"

class KSCD;

CDDBManager::CDDBManager()
{
	// CDDB initialization
	m_cddbInfo.clear(); // The first freedb revision is "0"
	m_cddbClient = new KCDDB::Client();
//	m_cddialog = 0L;
	
	connect(m_cddbClient, SIGNAL(finished(KCDDB::Result)), this, SLOT(lookupCDDBDone(KCDDB::Result)));

// TODO inactivate CDDB options if no disc
// TODO erase that !
	for (int i=0; i<20; i++)
	{
		m_cddbInfo.track(i).set(KCDDB::Title, QString("unknown") ) ;
	}

	//Create CDDB Window
	m_cddialog = new QDialog(0, Qt::Window);
}

CDDBManager::~CDDBManager()
{
	delete m_cddialog;
	delete m_cddbClient;
}

// Fonction to be called whenever a new Disc is inserted
void CDDBManager::setupCDDB(int nbTrack, KCDDB::TrackOffsetList signature)
{
	m_cdSignature = signature;
	
	for (int i=0; i<nbTrack; i++)
	{
		m_cddbInfo.track(i).set(KCDDB::Title, QString("unknown") ) ;
	}
}

void CDDBManager::CDDialogSelected()
{	
	kDebug() << "CDDialogSelected1" ;

	//m_cddialog->raise();// Puts the window on top

	
	m_cddialog->setFixedSize ( 300,500 );

	albumTitleLabel = new QLabel(tr("Album Title :"));
	albumTitlelineEdit = new QLineEdit;
	//albumTitleLabel->setBuddy(albumTitlelineEdit);

	albumArtistLabel = new QLabel(tr("Album Artist :"));
	albumArtistlineEdit = new QLineEdit;
	//albumArtistLabel->setBuddy(albumArtistlineEdit);

	albumCommentLabel = new QLabel(tr("Album Comment :"));
	albumCommentlineEdit = new QLineEdit;
	//albumCommentLabel->setBuddy(albumCommentlineEdit);

	albumGenreLabel = new QLabel(tr("Album Genre :"));
	albumGenrelineEdit = new QLineEdit;
	//albumGenreLabel->setBuddy(albumGenrelineEdit);

	albumYearLabel = new QLabel(tr("Album Year :"));
	albumYearlineEdit = new QLineEdit;
	//albumYearLabel->setBuddy(albumYearlineEdit);

	albumCategoryLabel = new QLabel(tr("Album Category :"));
	albumCategorylineEdit = new QLineEdit;
	//albumCategoryLabel->setBuddy(albumCategorylineEdit);

	albumLengthLabel = new QLabel(tr("Album Length :"));
	albumLengthlineEdit = new QLineEdit;
	//albumLengthLabel->setBuddy(albumLengthlineEdit);

	QGridLayout * mainLayout = new QGridLayout;
	//mainLayout->setSizeConstraint(QLayout::SetFixedSize);
	
	mainLayout->addWidget(albumTitleLabel, 0, 0);
	mainLayout->addWidget(albumTitlelineEdit, 0, 1);

	mainLayout->addWidget(albumArtistLabel, 1, 0);
	mainLayout->addWidget(albumArtistlineEdit, 1, 1);

	mainLayout->addWidget(albumCommentLabel, 2, 0);
	mainLayout->addWidget(albumCommentlineEdit, 2, 1);

	mainLayout->addWidget(albumGenreLabel, 3, 0);
	mainLayout->addWidget(albumGenrelineEdit, 3, 1);

	mainLayout->addWidget(albumYearLabel, 4, 0);
	mainLayout->addWidget(albumYearlineEdit, 4, 1);

	mainLayout->addWidget(albumCategoryLabel, 5, 0);
	mainLayout->addWidget(albumCategorylineEdit, 5, 1);

	mainLayout->addWidget(albumLengthLabel, 6, 0);
	mainLayout->addWidget(albumLengthlineEdit, 6, 1);	

	m_cddialog->setLayout(mainLayout);

	m_cddialog->show();
/*	table = new QTableWidget(20, 8, m_cddialog);

	// Association entre le manipulateur de model et le model de la table
	QAbstractItemModel * model = table->model ();


	// Columns
	model->setData(model->index(0,1),"Title");
	model->setData(model->index(0,2),"Artist");
	model->setData(model->index(0,3),"Genre");
	model->setData(model->index(0,4),"Year");
	model->setData(model->index(0,5),"Length");
	model->setData(model->index(0,6),"Category");
	model->setData(model->index(0,7),"Comment");
	
	// CD Info
	model->setData(model->index(1,0),"CD");
	model->setData(model->index(1,1),m_cddbInfo.get(Title).toString());
	model->setData(model->index(1,2),m_cddbInfo.get(Artist).toString());
	model->setData(model->index(1,3),m_cddbInfo.get(Genre).toString());
	model->setData(model->index(1,4),m_cddbInfo.get(Year).toString());
	model->setData(model->index(1,5),m_cddbInfo.get(Length).toString());
	model->setData(model->index(1,6),m_cddbInfo.get(Category).toString());
	model->setData(model->index(1,7),m_cddbInfo.get(Comment).toString());
	
	// Track Info
	for (int i=0; i<25; i++) {
		model->setData(model->index(i+2,0),QString("Track %1").arg(i+1));
		model->setData(model->index(i+2,1),m_cddbInfo.track(i).get(Title).toString());
		model->setData(model->index(i+2,2),m_cddbInfo.track(i).get(Artist).toString());
		model->setData(model->index(i+2,3),m_cddbInfo.track(i).get(Genre).toString());
		model->setData(model->index(i+2,4),m_cddbInfo.track(i).get(Year).toString());
		model->setData(model->index(i+2,5),m_cddbInfo.track(i).get(Length).toString());
		model->setData(model->index(i+2,6),m_cddbInfo.track(i).get(Category).toString());
		model->setData(model->index(i+2,7),m_cddbInfo.track(i).get(Comment).toString());
	}
	
	table->show();*/



/*
// 	if (!m_cddialog)
// 	{
		kDebug() << "Create a CDDB Dialog" ;
		// CDDB Dialog initialization
		m_cddialog = new CDDBDlg(this);
		connect(m_cddialog,SIGNAL(newCDInfoStored(KCDDB::CDInfo)), SLOT(setCDInfo(KCDDB::CDInfo)));
		connect(m_cddialog,SIGNAL(finished()),SLOT(CDDialogDone()));
 		connect(cddialog,SIGNAL(play(int)),cddialog,SLOT(slotTrackSelected(int)));
// 	}
	kDebug() << "CDDialogSelected" ;
	m_cddialog->show();
	kDebug() << "CDDialogSelected" ;
	m_cddialog->raise(); // Puts the window on top*/

}

void CDDBManager::CDDialogDone()
{
	kDebug() << "test CDDB" ;
	/*
	m_cddialog->delayedDestruct();
	m_cddialog = 0L;*/
}

void CDDBManager::lookupCDDBDone(Result result)
{
	if (result != KCDDB::Success)
	{
		if(result == KCDDB::NoRecordFound)
			emit showArtistLabel(i18n("No matching freedb entry found."));
		else
			emit showArtistLabel(i18n("Error getting freedb entry."));

		QTimer::singleShot(3000, this, SIGNAL(restoreArtistLabel()));
		return;
	}

	KCDDB::CDInfo info = m_cddbClient->lookupResponse().first();

	// If there is more than 1 result display a choice window
	if(m_cddbClient->lookupResponse().count() > 1) {
		CDInfoList cddb_info = m_cddbClient->lookupResponse();
		QStringList list;
		CDInfoList::iterator it;
		for ( it = cddb_info.begin(); it != cddb_info.end(); ++it  )
		{
			list.append( QString("%1, %2, %3, %4").arg((*it).get(Artist).toString()).arg((*it).get(Title).toString()).arg((*it).get(Genre).toString()).arg((*it).get(KCDDB::Category).toString()));
		}
	
		bool ok(false);
		QString res = KInputDialog::getItem(i18n("Select CDDB Entry"),
				i18n("Select a CDDB entry:"), list, 0, false, &ok);
		if(ok)
		{
			// The user selected and item and pressed OK
			int c = 0;
			for(QStringList::Iterator it = list.begin(); it != list.end(); ++it )
			{
				if( *it == res) break;
				c++;
			}
			// Ensure that the selected item is within the range of the number of CDDB entries found
			if( c < cddb_info.size() )
			{
				info = cddb_info[c];
			}
		}
		else
		{
			return;
			// user pressed Cancel
		}
	}
	setCDInfo(info);
}

void CDDBManager::setCDInfo(KCDDB::CDInfo info)
{
	// Some sanity provisions to ensure that the number of records matches what
	// the CD actually contains.
	// TODO Q_ASSERT(info.numberOfTracks() == cd->tracks());

	m_cddbInfo = info;
	
	kDebug() << m_cddbInfo.get(Title).toString() ;
	kDebug() << m_cddbInfo.get(Comment).toString() ;
	kDebug() << m_cddbInfo.get(Artist).toString() ;
	kDebug() << m_cddbInfo.get(Genre).toString() ;
	kDebug() << m_cddbInfo.get(Year).toString() ;
	kDebug() << m_cddbInfo.get(Length).toString() ;
	kDebug() << m_cddbInfo.get(Category).toString() ;
	
	// Trace
	kDebug() << m_cddbInfo.toString() ;
	
	for (int i=0; i<m_cddbInfo.numberOfTracks(); i++)
	{
		kDebug() << i << " title " << m_cddbInfo.track(i).get(KCDDB::Title).toString() ;
		kDebug() << i << " artist " << m_cddbInfo.track(i).get(KCDDB::Artist).toString() ;
		kDebug() << i << " length " << m_cddbInfo.track(i).get(KCDDB::Length).toString() ;
	}
//	populateSongList();
	emit restoreArtistLabel();
	emit restoreTrackinfoLabel();
}

QList <CDDBTrack> CDDBManager::getTrackList()
{
	QList<CDDBTrack> list;
	CDDBTrack track;
	for (int i=0; i<m_cddbInfo.numberOfTracks(); i++)
	{
		track.Title = m_cddbInfo.track(i).get(KCDDB::Title).toString();
		track.Comment = m_cddbInfo.track(i).get(KCDDB::Comment).toString();
		track.Artist = m_cddbInfo.track(i).get(KCDDB::Artist).toString();
		track.Genre = m_cddbInfo.track(i).get(KCDDB::Genre).toString();
		track.Year = m_cddbInfo.track(i).get(KCDDB::Year).toString();
		track.Length = m_cddbInfo.track(i).get(KCDDB::Length).toString();
		track.Category = m_cddbInfo.track(i).get(KCDDB::Category).toString();
		
		list << track;
	}
	return list;
}

void CDDBManager::populateSongList()
{
/*
	songListCB->clear();
	for (uint i = 1; i <= m_cd->tracks(); ++i)
	{
	unsigned tmp = m_cd->trackLength(i);
	unsigned mymin;
	unsigned mysec;
	mymin = tmp / 60;
	mysec = (tmp % 60);
	QString str1;
	str1.sprintf("%02u: ", i);

	QString str2;
	str2.sprintf(" (%02u:%02u) ", mymin,  mysec);
	// see comment before restoreArtistLabel()
	if (cddbInfo.isValid() && cddbInfo.numberOfTracks() == m_cd->tracks()) {
	const KCDDB::TrackInfo& t = cddbInfo.track(i - 1);

	if (cddbInfo.get(KCDDB::Artist).toString() != t.get(KCDDB::Artist).toString())
	str1.append(t.get(KCDDB::Artist).toString()).append(" - ");
	str1.append(t.get(KCDDB::Title).toString());
} else {
	if (m_cd->discArtist() != m_cd->trackArtist(i))
	str1.append(m_cd->trackArtist(i)).append(" - ");
	str1.append(m_cd->trackTitle(i));
}
	str1.append(str2);
	songListCB->addItem(str1);
}
*/
}
