/***********************************
This is the Adafruit GPS library - the ultimate GPS library
for the ultimate GPS module!

Tested and works great with the Adafruit Ultimate GPS module
using MTK33x9 chipset
    ------> http://www.adafruit.com/products/746
Pick one up today at the Adafruit electronics shop 
and help support open source hardware & software! -ada

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above must be included in any redistribution
****************************************/
// Fllybob added lines 34,35 and 40,41 to add 100mHz logging capability 

#ifndef MTK3339_H
#define MTK3339_H

#include <stdint.h>

class Stream;

// different commands to set the update rate from once a second (1 Hz) to 10 times a second (10Hz)
// Note that these only control the rate at which the position is echoed, to actually speed up the
// position fix you must also send one of the position fix rate commands below too.
#define PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ  "$PMTK220,10000*2F" // Once every 10 seconds, 100 millihertz.
#define PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ  "$PMTK220,5000*1B"  // Once every 5 seconds, 200 millihertz.
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_2HZ  "$PMTK220,500*2B"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"
// Position fix update rate commands.
#define PMTK_API_SET_FIX_CTL_100_MILLIHERTZ  "$PMTK300,10000,0,0,0,0*2C" // Once every 10 seconds, 100 millihertz.
#define PMTK_API_SET_FIX_CTL_200_MILLIHERTZ  "$PMTK300,5000,0,0,0,0*18"  // Once every 5 seconds, 200 millihertz.
#define PMTK_API_SET_FIX_CTL_1HZ  "$PMTK300,1000,0,0,0,0*1C"
#define PMTK_API_SET_FIX_CTL_5HZ  "$PMTK300,200,0,0,0,0*2F"
// Can't fix position faster than 5 times a second!


#define PMTK_SET_BAUD_115200 "$PMTK251,115200*1F"
#define PMTK_SET_BAUD_57600 "$PMTK251,57600*2C"
#define PMTK_SET_BAUD_9600 "$PMTK251,9600*17"

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

// to generate your own sentences, check out the MTK command datasheet and use a checksum calculator
// such as the awesome http://www.hhhh.org/wiml/proj/nmeaxor.html

#define PMTK_LOCUS_STARTLOG  "$PMTK185,0*22"
#define PMTK_LOCUS_STOPLOG "$PMTK185,1*23"
#define PMTK_LOCUS_STARTSTOPACK "$PMTK001,185,3*3C"
#define PMTK_LOCUS_QUERY_STATUS "$PMTK183*38"
#define PMTK_LOCUS_ERASE_FLASH "$PMTK184,1*22"
#define LOCUS_OVERLAP 0
#define LOCUS_FULLSTOP 1

#define PMTK_ENABLE_SBAS "$PMTK313,1*2E"
#define PMTK_ENABLE_WAAS "$PMTK301,2*2E"

// standby command & boot successful message
#define PMTK_STANDBY "$PMTK161,0*28"
#define PMTK_STANDBY_SUCCESS "$PMTK001,161,3*36"  // Not needed currently
#define PMTK_AWAKE "$PMTK010,002*2D"

// ask for the release and version
#define PMTK_Q_RELEASE "$PMTK605*31"

// request for updates on antenna status 
#define PGCMD_ANTENNA "$PGCMD,33,1*6C" 
#define PGCMD_NOANTENNA "$PGCMD,33,0*6D" 

// how long to wait when we're looking for a response
#define MAXWAITSENTENCE 10

class MTK3339 {
 public:
  enum Antenna {
    AntennaUnknown,
    AntennaExternalProblem,
    AntennaUsingInternal,
    AntennaUsingExternal,
  };
  enum Mode {
    ModeUnknown,
    ModeNoFix,
    Mode2D,
    Mode3D,
  };

  MTK3339(Stream *serial);

  void begin();

  const char *lastNMEA();
  bool newNMEAreceived();

  void sendCommand(const char *);

  void pause(bool b);

  bool parseNMEA(char *response);
  uint8_t parseHex(char c);

  void read();
  bool parse(const char *nmea);

  bool wakeup();
  bool standby();

  // Trame: GPGGA
  uint8_t hour, minute, seconds, year, month, day;
  uint16_t milliseconds;
  // Int degree * 10000000 + floating minute * 100000 (signed).
  int32_t latitude_degree_minute, longitude_degree_minute;
  // Floating degree * 10000000 (signed).
  int32_t latitude_degree, longitude_degree;
  uint8_t fix_quality;
  uint8_t satellites_used;
  float geoidheight, altitude;
  // Trame: GPRMC
  bool fix;
  // speed in m/s
  float speed;
  float angle;
  // Trame: GPGSA
  char mode_selection; // M=Manual, forced to operate in 2D or 3D, A=Automatic, 3D/2D
  Mode mode; // 1=Fix not available, 2=2D, 3=3D
  float PDOP;
  float HDOP;
  float VDOP;
  // Trame: GPGSV
  uint8_t satellites_in_views;
  // Trame: PGTOP
  enum Antenna antenna;

  bool waitForSentence(const char *wait, uint8_t max = MAXWAITSENTENCE);
  bool LOCUS_StartLogger();
  bool LOCUS_StopLogger();
  bool LOCUS_ReadStatus();

  uint16_t LOCUS_serial, LOCUS_records;
  uint8_t LOCUS_type, LOCUS_mode, LOCUS_config, LOCUS_interval, LOCUS_distance, LOCUS_speed, LOCUS_status, LOCUS_percent;
 private:
  bool paused;

  // need to parse GPVTG (related to heading).
  bool parse_GPGGA(const char *nmea);
  bool parse_GPRMC(const char *nmea);
  bool parse_PGTOP(const char *nmea);
  bool parse_GPGSV(const char *nmea);
  bool parse_GPGSA(const char *nmea);
  bool parse_latitude_longitude(const char **nmea);

  Stream *const _gpsSerial;
};

#endif  // MTK3339_H
