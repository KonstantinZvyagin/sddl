/*
 * This file is part of SDDL.
 *
 * SDDL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SDDL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with SDDL.  If not, see <http://www.gnu.org/licenses/>.
 */

/*----------------------------------------------------------------------------*/
/* Project:        Surveillance Data Decoder and Lister                       */
/* File:           src/list_tod.cpp                                           */
/* Contents:       List some time of day                                      */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_tod        -- List time of day                                        */
/*----------------------------------------------------------------------------*/

 void list_tod
	(t_Object_Type object, t_Pntr pntr, t_Ui16 length, char *buffer)
{
	t_Adsb *adsb_ptr;
	t_Mlat *mlat_ptr;
	t_Rsrv *rsrv_ptr;
	t_Rtgt *rtgt_ptr;
	t_Step *step_ptr;
	t_Strk *strk_ptr;
	char tmp[81];
	t_Bool tod_present;
	char tod_type[2];
	t_Secs tod_value;

                   /* Check parameters: */
	Assert (e_no_object < object && object < e_object_sentinel,
            "Invalid parameter");
	Assert (pntr != NULL, "Invalid parameter");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check whether information should be listed: */
	if (!list_time_of_day)
	{
		goto done;
	}

                   /* Preset time of day: */
	tod_present = FALSE;
	tod_type[0] = '-';
	tod_type[1] = '\0';
	tod_value = 0.0;

                   /* Extract time of day: */
	switch (object)
	{
	case e_object_adsb:
                   /* ADS-B report */

		adsb_ptr = (t_Adsb *) pntr;
		if (adsb_ptr->detection_time.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'D';
			tod_value = adsb_ptr->detection_time.value;
		}
		else if (adsb_ptr->tor_position.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'P';
			tod_value = adsb_ptr->tor_position.value;
		}
		else if (adsb_ptr->time_of_report.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'T';
			tod_value = adsb_ptr->time_of_report.value;
		}
		else if (adsb_ptr->tor_velocity.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'V';
			tod_value = adsb_ptr->tor_velocity.value;
		}
		else if (adsb_ptr->frame_time.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'F';
			tod_value = adsb_ptr->frame_time.value;
		}
		break;

	case e_object_mlat:
                   /* Multilateration report */

		mlat_ptr = (t_Mlat *) pntr;
		if (mlat_ptr->detection_time.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'D';
			tod_value = mlat_ptr->detection_time.value;
		}
		else if (mlat_ptr->frame_time.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'F';
			tod_value = mlat_ptr->frame_time.value;
		}
		break;

	case e_object_rsrv:
                   /* Radar service message */

		rsrv_ptr = (t_Rsrv *) pntr;
		if (rsrv_ptr->message_time.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'M';
			tod_value = rsrv_ptr->message_time.value;
		}
		else if (rsrv_ptr->frame_time.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'F';
			tod_value = rsrv_ptr->frame_time.value;
		}
		break;

	case e_object_rtgt:
                   /* Radar target report */

		rtgt_ptr = (t_Rtgt *) pntr;
		if (rtgt_ptr->detection_time.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'D';
			tod_value = rtgt_ptr->detection_time.value;
		}
		else if (rtgt_ptr->estimated_time.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'E';
			tod_value = rtgt_ptr->estimated_time.value;
		}
		else if (rtgt_ptr->frame_time.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'F';
			tod_value = rtgt_ptr->frame_time.value;
		}
		break;

	case e_object_step:
                   /* System picture step */

		step_ptr = (t_Step *) pntr;
		if (step_ptr->time_of_message.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'M';
			tod_value = step_ptr->time_of_message.value;
		}
		else if (step_ptr->frame_time.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'F';
			tod_value = step_ptr->frame_time.value;
		}
		break;

	case e_object_strk:
                   /* System track message */

		strk_ptr = (t_Strk *) pntr;
		if (strk_ptr->time_of_last_update.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'U';
			tod_value = strk_ptr->time_of_last_update.value;
		}
		else if (strk_ptr->time_of_message.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'M';
			tod_value = strk_ptr->time_of_message.value;
		}
		else if (strk_ptr->frame_time.present)
		{
			tod_present = TRUE;
			tod_type[0] = 'F';
			tod_value = strk_ptr->frame_time.value;
		}
		break;

	default:
		Assert (FALSE, "Unexpected object type");
		break;
	}

                   /* List time of day: */

	memset (tmp, 0, 81);

	if (tod_present)
	{
		if (time_bias_defined)
		{
			tod_value += time_bias;
		}

		if (list_time_as_utc_text)
		{
			strncat (tmp, utc_text (tod_value), 80);
		}
		else
		{
			sprintf (tmp, "%09.3f", tod_value);
		}
	}
	else
	{
		if (list_time_as_utc_text)
		{
			strncat (tmp, "--:--:--.---", 80);
		}
		else
		{
			strncat (tmp, "-----.---", 80);
		}
	}

	strncat (buffer, " ", length);
	strncat (buffer, tod_type, length);
	strncat (buffer, " ", length);
	strncat (buffer, tmp, length);

	done:          /* We are done */
	return;
}
#endif /* LISTER */
/* end-of-file */
