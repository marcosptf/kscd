/*   
   Kscd - A simple cd player for the KDE Project

   $Id$
 
   Copyright (c) 1997 Bernd Johannes Wuebben math.cornell.edu

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


 */

/*
 * SVR4 specific.  Much of this is similar to plat_hpux.c.
 */



#include "config.h"

#if defined(SVR4) && !defined(sun) && !defined(__sun__) || defined(_UNIXWARE)




#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mkdev.h>
#include <sys/stat.h>
#ifdef _UNIXWARE
#include <sys/scsi.h>
#include <sys/sdi_edt.h>
#endif
#include <sys/sdi.h>
#ifdef SVR4
#include <sys/sdi_edt.h>
#include <sys/scsi.h>
#endif
#include <errno.h>

#include "struct.h"


void *malloc();
char *strchr();

int	min_volume = 0;
int	max_volume = 255;

extern char	*cd_device;

/*
 * find_cdrom
 *
 * Determine the name of the CD-ROM device.
 *
 * If the "CDROM" environment variable is set, use that instead.
 */                                

int               
find_cdrom()
{          
    if ((cd_device = getenv("CDROM")) != NULL)
                return 1;
    if (access("/dev/cdrom/cdrom1", F_OK) == 0)
        {
                cd_device = "/dev/cdrom/cdrom1";
                return 1;
        }
    else if (access("/dev/cdrom/cdrom2", F_OK) == 0){
                cd_device = "/dev/cdrom/cdrom2";
                return 1;
        }
    else
        {
                fprintf(stderr, "Couldn't find a CD device!\n");
                return 0;
        }
}

/*
 * Initialize the drive.  A no-op for the generic driver.
 */
int
gen_init(d)
	struct wm_drive	*d;
{
	return (0);
}

/*
 * Get the number of tracks on the CD.
 */
int
gen_get_trackcount(d, tracks)
	struct wm_drive	*d;
	int		*tracks;
{
	return (wm_scsi2_get_trackcount(d, tracks));
}


int
gen_get_trackinfocddb(d, track, min, sec, frm)
	struct wm_drive	*d;
	int    *min,*sec,*frm;
{
	return (wm_scsi2_get_trackinfo(d, track, min,sec,frm));

}

/*
 * Get the start time and mode (data or audio) of a track.
 */
int
gen_get_trackinfo(d, track, data, startframe)
	struct wm_drive	*d;
	int		track, *data, *startframe;
{
	return (wm_scsi2_get_trackinfo(d, track, data, startframe));
}

/*
 * Get the number of frames on the CD.
 */
int
gen_get_cdlen(d, frames)
	struct wm_drive	*d;
	int		*frames;
{
	int		tmp;

	return (wm_scsi2_get_cdlen(d, frames));
}

/*
 * Get the current status of the drive: the current play mode, the absolute
 * position from start of disc (in frames), and the current track and index
 * numbers if the CD is playing or paused.
 */
int
gen_get_drive_status(d, oldmode, mode, pos, track, index)
	struct wm_drive	*d;
	enum cd_modes	oldmode, *mode;
	int		*pos, *track, *index;
{
	return (wm_scsi2_get_drive_status(d, oldmode, mode, pos, track, index));
}

/*
 * Set the volume level for the left and right channels.  Their values
 * range from 0 to 100.
 */
int
gen_set_volume(d, left, right)
	struct wm_drive	*d;
	int		left, right;
{
	return (wm_scsi2_set_volume(d, left, right));
}

/*
 * Read the initial volume from the drive, if available.  Each channel
 * ranges from 0 to 100, with -1 indicating data not available.
 */
int
gen_get_volume(d, left, right)
	struct wm_drive	*d;
	int		*left, *right;
{
	return (wm_scsi2_get_volume(d, left, right));
}

/*
 * Pause the CD.
 */
int
gen_pause(d)
	struct wm_drive	*d;
{
	return (wm_scsi2_pause(d));
}

/*
 * Resume playing the CD (assuming it was paused.)
 */
int
gen_resume(d)
	struct wm_drive	*d;
{
	return (wm_scsi2_resume(d));
}

/*
 * Stop the CD.
 */
int
gen_stop(d)
	struct wm_drive	*d;
{
	return (wm_scsi2_stop(d));
}

/*
 * Play the CD from one position to another (both in frames.)
 */
int
gen_play(d, start, end)
	struct wm_drive	*d;
	int		start, end;
{
	return (wm_scsi2_play(d, start, end));
}

/*
 * Eject the current CD, if there is one.
 */
int
gen_eject(d)
	struct wm_drive	*d;
{
	return (wm_scsi2_eject(d));
}

gen_closetray(struct wm_drive *d)
{
  return(wm_scsi2_closetray(d));
} /* gen_closetray() */

static int
create_cdrom_node(char *dev_name)
{
	char pass_through[100];
	int file_des;
	dev_t pass_thru_device;
	int err;
	int ccode;


	strcpy(pass_through, dev_name);
	strcat(pass_through, "p" );

	if (setreuid(-1,0) < 0)
	{
		perror("setregid/setreuid/access");
		exit(1);
	}

	ccode = access(pass_through, F_OK);
	
	if (ccode < 0)
	{
		if ((file_des = open(dev_name, O_RDONLY)) < 0)
		{
			perror("open cdrom devices failed");
			return -1;
		}

		if (ioctl(file_des, B_GETDEV, &pass_thru_device) < 0)
		{
			perror("Call to get pass-through device number failed");
			return -1;
		}

		(void)close(file_des);

		if (mknod(pass_through, (S_IFCHR | S_IREAD | S_IWRITE),
			pass_thru_device) < 0)
		{
			perror("Unable to make pass-through node");
			return -1;
		}

        	if (chown(pass_through, 0 , 0) < 0)
		{
			perror("chown");
			return -1;
		}

		if (chmod(pass_through, 0660 ) < 0)
		{
			perror("chmod");
			return -1;
		}
	}
	
	file_des = open( pass_through, O_RDWR);
	err = errno;

	if ( (setreuid(-1,getuid()) < 0) || (setregid(-1,getgid()) < 0) )
	{
		perror("setreuid/setregid");
		exit(1);
	}
	errno = err;
	return file_des;
}

/*
 * Open the CD and figure out which kind of drive is attached.
 */
int
wmcd_open(d)
	struct wm_drive	*d;
{
	int		fd, flag = 1;
	static int	warned = 0;
	char		vendor[9], model[17], rev[5];

        if (cd_device == NULL){
          if(!find_cdrom())
            return -1;
        }

	if (d->fd >= 0)		/* Device already open? */
		return (0);
	
	if (cd_device == NULL){
	  fprintf(stderr,"cd_device string empty\n");
	  return (-1);
	}

	d->fd = create_cdrom_node(cd_device); /* this will do open */

	if (d->fd < 0)
	{
		if (errno == EACCES)
		{
			if (! warned)
			{
				fprintf(stderr,"Cannot access %s\n",cd_device);
				warned++;
			}
		}
		else if (errno != EINTR)
		{
			perror(cd_device);
		}

		/* can not acces CDROM device */
		return (-1);
	}

	if (warned)
	{
		warned = 0;
		fprintf(stderr, "Thank you.\n");
	}

	/* Now fill in the relevant parts of the wm_drive structure. */

	fd = d->fd;

	if (wm_scsi_get_drive_type(d, vendor, model, rev) < 0)
	{
		perror("Cannot inquiry drive for it's type");
		return (-1);
	}
	*d = *(find_drive_struct(vendor, model, rev));
	/*	about_set_drivetype(d->vendor, d->model, rev);*/

	d->fd = fd;

	return (0);
}


/*
 * Re-Open the device
 */
wmcd_reopen( struct wm_drive *d )
{
  int status;
  int tries = 0;
  do {
    if (d->fd >= 0) /* Device really open? */
      {
	if( (close(d->fd )) < 0 ) /* ..then close it */
	  d->fd = -1; /* closed */
      }
    susleep( 1000 );
    status = wmcd_open( d );
    susleep( 1000 );
    tries++;
  } while ( (status != 0) && (tries < 10) );
  return status;
} /* wmcd_reopen() */

void
keep_cd_open() { }

/*
 * Send a SCSI command out the bus.
 */
int 
wm_scsi(d, xcdb, cdblen, retbuf, retbuflen, getreply)
	struct wm_drive *d;
	unsigned char *xcdb;
	int cdblen;
	int getreply;
	char *retbuf;
	int retbuflen;
{
	int ccode;
	int file_des = d->fd;
	int i,j;
	unsigned char sense_buffer[ SENSE_SZ ];
	int errno_save;

	/* getreply == 1 is read, == 0 is write */
	
	struct sb sb;
	struct scs scs;

        sb.sb_type = ISCB_TYPE;

	sb.SCB.sc_comp_code = SDI_PROGRES;
	sb.SCB.sc_int = NULL;
	sb.SCB.sc_wd = 0;
#ifndef _UNIXWARE
	sb.SCB.sc_dev.sa_major = 0;
	sb.SCB.sc_dev.sa_minor = 0;
#endif
	sb.SCB.sc_dev.sa_lun = 0;
#ifdef _UNIXWARE
	sb.SCB.sc_dev.sa_bus = 0;
#else
	sb.SCB.sc_dev.sa_exlun = 0;
#endif
	sb.SCB.sc_status = 0;
	sb.SCB.sc_link = (struct sb *) NULL;
	sb.SCB.sc_resid = 0;

	sb.SCB.sc_cmdpt = (void *)xcdb;
	sb.SCB.sc_cmdsz = cdblen;

	sb.SCB.sc_datapt = retbuf ;
	sb.SCB.sc_datasz = retbuflen ;

	if (getreply == 1)
		sb.SCB.sc_mode = SCB_READ;
	else
		sb.SCB.sc_mode = SCB_WRITE;

	sb.SCB.sc_time = 500;

	ccode =  ioctl(file_des, SDI_SEND,  &sb);

	if ( (sb.SCB.sc_comp_code != 0xd000000e ) ||
				( sb.SCB.sc_status != 02) )
		return ccode;

	errno_save = errno;

        sb.SCB.sc_comp_code = SDI_PROGRES;
        sb.SCB.sc_int = NULL;
        sb.SCB.sc_wd = 0;
#ifndef _UNIXWARE
        sb.SCB.sc_dev.sa_major = 0;
        sb.SCB.sc_dev.sa_minor = 0;
#endif
        sb.SCB.sc_dev.sa_lun = 0;
#ifdef _UNIXWARE
        sb.SCB.sc_dev.sa_bus = 0;
#else
        sb.SCB.sc_dev.sa_exlun = 0;
#endif
        sb.SCB.sc_status = 0;
        sb.SCB.sc_link = (struct sb *) NULL;
        sb.SCB.sc_resid = 0;

	scs.ss_op	=	SS_REQSEN;
	scs.ss_lun	=	0;
	scs.ss_addr1	=	0;
	scs.ss_addr	=	0;
	scs.ss_len	=	SENSE_SZ;
	scs.ss_cont	=	0;

	sb.SCB.sc_cmdpt = SCS_AD(&scs);
	sb.SCB.sc_cmdsz = SCS_SZ;
	sb.SCB.sc_datapt = sense_buffer;
	sb.SCB.sc_datasz = 18;
	sb.SCB.sc_mode = SCB_READ;
	sb.SCB.sc_time = 5000;

	if (ioctl(file_des, SDI_SEND,  &sb) < 0)
	{
		fprintf(stderr,"Cannot read sense.\n");
		exit(-1);
	}

	errno=errno_save;
	return -1;
}

#endif
