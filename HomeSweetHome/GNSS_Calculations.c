/*
 * GNSS_Calculations.c
 *
 * Created: 15/09/2022 09:42:14
 *  Author: higgy
 */ 

#include "GNSS_calculations.h"



/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::                                                                         :*/
/*::  This routine calculates the distance between two points (given the     :*/
/*::  latitude/longitude of those points). It is being used to calculate     :*/
/*::  the distance between two locations using GeoDataSource(TM) products.   :*/
/*::                                                                         :*/
/*::  Definitions:                                                           :*/
/*::    South latitudes are negative, east longitudes are positive           :*/
/*::                                                                         :*/
/*::  Passed to function:                                                    :*/
/*::    lat1, lon1 = Latitude and Longitude of point 1 (in decimal degrees)  :*/
/*::    lat2, lon2 = Latitude and Longitude of point 2 (in decimal degrees)  :*/
/*::    unit = the unit you desire for results                               :*/
/*::           where: 'M' is statute miles (default)                         :*/
/*::                  'K' is kilometers                                      :*/
/*::                  'N' is nautical miles                                  :*/
/*::  Worldwide cities and other features databases with latitude longitude  :*/
/*::  are available at https://www.geodatasource.com                         :*/
/*::                                                                         :*/
/*::  For inquiries, please contact sales@geodatasource.com                  :*/
/*::                                                                         :*/
/*::  Official Web site: https://www.geodatasource.com                       :*/
/*::                                                                         :*/
/*::           GeoDataSource.com (C) All Rights Reserved 2022                :*/
/*::                                                                         :*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#include <math.h>

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  Function prototypes                                           :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double);
double rad2deg(double);

double distance(double lat1, double lon1, double lat2, double lon2, char unit);
int32_t get_GNSS_bearing(double lat,double lon,double lat2,double lon2);


const char gngll_header_data[] = "$GNGLL";
const char test_gll[] = "$GNGLL,,,,,,V,A*60";

const char string_delimiter[] = ",";
const char checksum_delimiter[] = "*";
const char end_delimiter[] = "\r";


void populate_GNGLL_data(char *GNGLL_buffer)
{
	// clear the previous struct
	memset(&gnss_data, 0, ARRAY_BYTES(gnss_data));
	
	// $GNGLL string
	char *token = zstring_strtok(GNGLL_buffer, string_delimiter);
	memcpy(gnss_data.type, token, ARRAY_SIZE(gnss_data.type));
	
	// Latitude string
	token = zstring_strtok(NULL, string_delimiter); // Next token
	memcpy(gnss_data.latitude, token, ARRAY_SIZE(gnss_data.latitude));
	
	// North South string
	token = zstring_strtok(NULL, string_delimiter); // Next token
	memcpy(gnss_data.ns, token, ARRAY_SIZE(gnss_data.ns));
	
	// Longitude string
	token = zstring_strtok(NULL, string_delimiter); // Next token
	memcpy(gnss_data.longitude, token, ARRAY_SIZE(gnss_data.longitude));
	
	// East West string
	token = zstring_strtok(NULL, string_delimiter); // Next token
	memcpy(gnss_data.ew, token, ARRAY_SIZE(gnss_data.ew));
	
	// time string
	token = zstring_strtok(NULL, string_delimiter); // Next token
	memcpy(gnss_data.time, token, ARRAY_SIZE(gnss_data.time));
	
	// Status string
	token = zstring_strtok(NULL, string_delimiter); // Next token
	memcpy(gnss_data.status, token, ARRAY_SIZE(gnss_data.status));
	
	// position mode string
	token = zstring_strtok(NULL, checksum_delimiter); // Next token
	memcpy(gnss_data.posMode, token, ARRAY_SIZE(gnss_data.posMode));
	
	// checksum string
	token = zstring_strtok(NULL, end_delimiter); // Next token
	token[strcspn(token, "\n")] = '\0'; // Remove trailing new lines
	token[strcspn(token, "\r")] = '\0'; // Remove trailing new lines
	memcpy(gnss_data.checksum, token, ARRAY_SIZE(gnss_data.checksum));
	
}

double get_distance_to_base()
{
	double distance_between = INVALID_DISTANCE;
	
	double current_lat = convert_latitude_GNSS_to_decimal();
	double current_long = convert_longitude_GNSS_to_decimal();
	
	// valid GPS
	if (current_lat < 360 || current_long <360)
	{
		distance_between = distance(LAT_BASE, LONG_BASE, current_lat, current_long, DISTANCE_UNIT);
	}
	
	return distance_between;
}

/*
* Distance between two points
*
*/
double distance(double lat1, double lon1, double lat2, double lon2, char unit)
{
	double theta, dist;
	
	if ((lat1 == lat2) && (lon1 == lon2)) 
	{
		return 0;
	}
	else 
	{
		theta = lon1 - lon2;
		dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
		dist = acos(dist);
		dist = rad2deg(dist);
		dist = dist * 60 * 1.1515;
		switch(unit) {
			case 'M':
			break;
			case 'K':
			dist = dist * 1.609344;
			break;
			case 'N':
			dist = dist * 0.8684;
			break;
		}
		return (dist);
	}
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts decimal degrees to radians             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double deg) {
	return (deg * M_PI / 180);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts radians to decimal degrees             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double rad2deg(double rad) {
	return (rad * 180 / M_PI);
}


/* based on wiki as math.h does not have atan2 for this IC */
double atan2_approx(double y, double x)
{
	if(x > 0.0)
	{
		return atan(y/x);
	}
	else if (y > 0.0)
	{
		return M_PI_2 - atan(x/y);
	}
	else if (y < 0.0)
	{
		return  -M_PI_2 - atan(x/y);
	}
	else
	{
		return atan(y/x) + M_PI;
	}
}

int32_t get_GNSS_bearing_all()
{
	int32_t bearing_between = INVALID_BEARING;
	
	double start_lat = convert_latitude_GNSS_to_decimal();
	double start_long = convert_longitude_GNSS_to_decimal();
	
	// valid GPS
	if (start_lat < 360 || start_long <360)
	{
		bearing_between = get_GNSS_bearing(start_lat, start_long, LAT_BASE, LONG_BASE);
	}
	
	return bearing_between;
}

int32_t get_GNSS_bearing(double lat,double lon,double lat2,double lon2)
{
	/*
		Formula:	? = atan2( sin ?? ? cos ?2 , cos ?1 ? sin ?2 ? sin ?1 ? cos ?2 ? cos ?? )
		where	?1,?1 is the start point, ?2,?2 the end point (?? is the difference in longitude)
	*/

	double teta1 = deg2rad(lat);
	double teta2 = deg2rad(lat2);
	double delta2 = deg2rad(lon2-lon);

	//==================Heading Formula Calculation================//

	double y = sin(delta2) * cos(teta2);
	double x = cos(teta1)*sin(teta2) - sin(teta1)*cos(teta2)*cos(delta2);
	double brng = atan2_approx(y,x);
	brng = rad2deg(brng);// radians to degrees
	brng = ( ((int)brng + 360) % 360 );
	//brng = (brng + 360.0) % 360.0;
	
	return (int32_t) brng;
}

double convert_latitude_GNSS_to_decimal(void)
{
	// latitude value that is not possible
	double lat = 360.0;
	
	if (gnss_data.latitude[0] != ',')
	{
		// uint part of the coordinate
		uint8_t l_deg;
		
		// minutes part of coordinate
		double mins;
		
		// decimal minutes converted from degree minutes
		double decimal_minutes;
		
		lat = strtod(gnss_data.latitude, NULL);
		// latitude is 1234.56789
		// should be 12.3456789 deg.minutes
		lat /=100;
		
		// get the degree part of the coordinate
		l_deg = (uint8_t)lat;
		
		// get the minutes part
		mins = lat - l_deg;
		
		// convert minutes to decimal by /0.60
		decimal_minutes = mins/0.6;
		
		// combine degree and decimal minutes
		lat = (double)l_deg + decimal_minutes;
		
		
		// if south then is -'ve
		if(gnss_data.ns[0] == 'S')
		{
			lat *=-1;
		}
	}
	
	return lat;		
}

double convert_longitude_GNSS_to_decimal(void)
{
	// Longitude value that is not possible
	double longitude = 360.0;
	
	if (gnss_data.longitude[0] != ',')
	{
		// uint part of the coordinate
		uint8_t l_deg;
		
		// minutes part of coordinate
		double mins;
		
		// decimal minutes converted from degree minutes
		double decimal_minutes;
		
		longitude = strtod(gnss_data.longitude, NULL);
		// longitude is 123.4567890
		// should be 1.234567890
		longitude /=100;
		
		// get the degree part of the coordinate
		l_deg = (uint8_t)longitude;
		// get the minutes part
		mins = longitude - l_deg;
		// convert minutes to decimal by /0.60
		decimal_minutes = mins/0.6;
		// combine degree and decimal minutes
		longitude = (double)l_deg + decimal_minutes;
		
		// if west then is -'ve
		if(gnss_data.ew[0] == 'W')
		{
			longitude *=-1;
		}
	}
	
	return longitude;
}

uint8_t get_hours(void)
{
	
	uint8_t hours = NO_TIME;	
	
	if (gnss_data.time[0] != ',')		
	{
		unsigned long hours_l;
		char hours_ascii[2];
		
		memcpy(hours_ascii, &gnss_data.time[0], 2);
		
		hours_l = strtoul(hours_ascii, NULL, 10);
		
		hours = (uint8_t)hours_l;
		
		uint8_t offset_hours = get_time_offset();
		
		if (offset_hours > 23)
		{
			offset_hours = 0;
		}
		
		hours += offset_hours;
		
		if (hours > 23)
		{
			hours -= 24;
		}
	}
	
	return hours;
}

uint8_t get_minutes(void)
{
	uint8_t mins = NO_TIME;
	
	if (gnss_data.time[0] != ',')
	{
		unsigned long mins_l;
		
		char mins_ascii[2];

		memcpy(mins_ascii, &gnss_data.time[2], 2);

		mins_l = strtoul(mins_ascii, NULL, 10);

		mins = (uint8_t)mins_l;
	}
	return mins;
}

uint8_t get_seconds(void)
{
	uint8_t seconds = NO_TIME;
	if (gnss_data.time[0] != ',')
	{
		unsigned long seconds_l;
		
		char seconds_ascii[2];
		
		memcpy(seconds_ascii, &gnss_data.time[4], 2);

		seconds_l = strtoul(seconds_ascii, NULL, 10);

		seconds = (uint8_t)seconds_l;
	}
	return seconds;
}