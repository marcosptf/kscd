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
 * $Id$
 *
 * HP/UX-specific drive control routines.
 */


#include "config.h"

#if defined(hpux) || defined(__hpux)

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>

/* this is for glibc 2.x which the ust structure in ustat.h not stat.h */
#ifdef __GLIBC__
#include <sys/ustat.h>
#endif

#include <sys/time.h>
#include <sys/scsi.h>
#include <ustat.h>

#include "struct.h"



void *malloc();
char *strchr();

int	min_volume = 0;
int	max_volume = 255;

extern char	*cd_device;

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

	return (wm_scsi2_get_trackinfocddb(d, track, min,sec,frm));

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
	struct stat	stbuf;
	struct ustat	ust;

	if (fstat(d->fd, &stbuf) != 0)
		return (-2);

	/* Is this a mounted filesystem? */
	if (ustat(stbuf.st_rdev, &ust) == 0)
		return (-3);

	return (wm_scsi2_eject(d));
}

int
gen_closetray(struct wm_drive *d)
{
	return(wm_scsi2_closetray(d));
} /* gen_closetray() */

/*
 * Send a SCSI command out the bus.
 */
int
wm_scsi(d, cdb, cdblen, retbuf, retbuflen, getreply)
	struct wm_drive	*d;
	unsigned char	*cdb;
	int		cdblen;
	void		*retbuf;
	int		retbuflen;
	int		getreply;
{
#ifdef SIOC_IO
	struct sctl_io		cmd;

	memset(&cmd, 0, sizeof(cmd));
	cmd.cdb_length = cdblen;
	cmd.data = retbuf;
	cmd.data_length = retbuflen;
	cmd.max_msecs = 1000;
	cmd.flags = getreply ? SCTL_READ : 0;
	memcpy(cmd.cdb, cdb, cdblen);

	return (ioctl(d->fd, SIOC_IO, &cmd));
#else
	/* this code, for pre-9.0, is BROKEN!  ugh. */
	char			reply_buf[12];
	struct scsi_cmd_parms	cmd;

	memset(&cmd, 0, sizeof(cmd));
	cmd.clock_ticks = 500;
	cmd.cmd_mode = 1;
	cmd.cmd_type = cdblen;
	memcpy(cmd.command, cdb, cdblen);
	if (ioctl(d->fd, SIOC_SET_CMD, &cmd) < 0)
		return (-1);

	if (! retbuf || ! retbuflen)
		(void) read(d->fd, reply_buf, sizeof(reply_buf));
	else if (getreply)
	{
		if (read(d->fd, retbuf, retbuflen) < 0)
			return (-1);
	}
	else
		if (write(d->fd, retbuf, retbuflen) < 0)
			return (-1);

	return (0);
#endif
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

	if (d->fd >= 0)		/* Device already open? */
		return (0);

	if (cd_device == NULL){
	  fprintf(stderr,"cd_device string empty\n");
	  return (-1);
	}

	d->fd = open(cd_device, O_RDWR);
	if (d->fd < 0)
	{
		if (errno == EACCES)
		{
			if (! warned)
			{
				fprintf(stderr,
		"As root, please run\n\nchmod 666 %s\n\n%s\n", cd_device,
		"to give yourself permission to access the CD-ROM device.");
				warned++;
			}
		}
		else if (errno != EINTR)
		{
			perror(cd_device);
			exit(1);
		}

		/* Can not access CDROM drive */
		return (-1);
	}

	/* Prepare the device to receive raw SCSI commands. */
	if (ioctl(d->fd, SIOC_CMD_MODE, &flag) < 0)
	{
		fprintf(stderr, "%s: SIOC_CMD_MODE: true: %s\n",
			cd_device, strerror(errno));
		/*exit(1);*/
	}

	if (warned)
	{
		warned = 0;
		fprintf(stderr, "Thank you.\n");
	}

	/* Now fill in the relevant parts of the wm_drive structure. */
	fd = d->fd;

	/* Default drive is the HP one, which might not respond to INQUIRY */
	strcpy(vendor, "TOSHIBA");
	strcpy(model, "XM-3301");
	rev[0] = '\0';
	wm_scsi_get_drive_type(d, vendor, model, rev);
	*d = *(find_drive_struct(vendor, model, rev));
	/*	about_set_drivetype(d->vendor, d->model, rev);*/
	d->fd = fd;

	(d->init)(d);

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

#endif
