/*********************************************************************************************************************************
  Basic_Insert_ESP.ino

  Library for communicating with a MySQL or MariaDB Server

  Based on and modified from Dr. Charles A. Bell's MySQL_Connector_Arduino Library https://github.com/ChuckBell/MySQL_Connector_Arduino
  to support nRF52, SAMD21/SAMD51, SAM DUE, STM32F/L/H/G/WB/MP1, ESP8266, ESP32, etc. boards using W5x00, ENC28J60, LAM8742A Ethernet,
  WiFiNINA, ESP-AT, built-in ESP8266/ESP32 WiFi.

  The library provides simple and easy Client interface to MySQL or MariaDB Server.

  Built by Khoi Hoang https://github.com/khoih-prog/MySQL_MariaDB_Generic
  Licensed under MIT license
  Version: 1.0.3

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      13/08/2020 Initial coding/porting to support nRF52, SAM DUE and SAMD21/SAMD51 boards using W5x00 Ethernet
                                  (Ethernet, EthernetLarge, Ethernet2, Ethernet3 library), WiFiNINA and ESP8266/ESP32-AT shields
  1.0.1   K Hoang      18/08/2020 Add support to Ethernet ENC28J60. Fix bug, optimize code.
  1.0.2   K Hoang      20/08/2020 Fix crashing bug when timeout. Make code more error-proof. Drop support to ESP8266_AT_Webserver.
  1.0.3   K Hoang      02/10/2020 Add support to Ethernet ENC28J60 using new EthernetENC library.
 **********************************************************************************************************************************/
/*
  MySQL Connector/Arduino Example : connect by wifi

  This example demonstrates how to connect to a MySQL server from an
  Arduino using an Arduino-compatible Wifi shield. Note that "compatible"
  means it must conform to the Ethernet class library or be a derivative
  with the same classes and methods.

  For more information and documentation, visit the wiki:
  https://github.com/ChuckBell/MySQL_Connector_Arduino/wiki.

  INSTRUCTIONS FOR USE

  1) Change the address of the server to the IP address of the MySQL server
  2) Change the user and password to a valid MySQL user and password
  3) Change the SSID and pass to match your WiFi network
  4) Connect a USB cable to your Arduino
  5) Select the correct board and port
  6) Compile and upload the sketch to your Arduino
  7) Once uploaded, open Serial Monitor (use 115200 speed) and observe

  If you do not see messages indicating you have a connection, refer to the
  manual for troubleshooting tips. The most common issues are the server is
  not accessible from the network or the user name and password is incorrect.

  Created by: Dr. Charles A. Bell
*/

#if ! (ESP8266 || ESP32 )
#error This code is intended to run on the ESP8266/ESP32 platform! Please check your Tools->Board setting
#endif

#include "Credentials.h"

#define MYSQL_DEBUG_PORT      Serial

// Debug Level from 0 to 4
#define _MYSQL_LOGLEVEL_      1



#include <MySQL_Generic_WiFi.h>

IPAddress server_addr(61, 76, 218, 214);
uint16_t server_port = 3307;    //3306;

char default_database[] = "smart_factory";           //"test_arduino";
char default_table[]    = "mold_counter";          //"test_arduino";

String default_value    = "Hello, Arduino!";

// Sample query
String INSERT_SQL = "INSERT INTO smart_factory.mold_counter(record_timestamp,machine_type,mold_type,event) VALUES('2021-05-23 12:34:56', 'machine','mold','event')";
const String INSERT_PREFIX  = "INSERT INTO " + String(default_database) + "." + String(default_table) + " ";
const String COLUMN_NAMES = "(record_timestamp,machine_type,mold_type,event)";
String current_timestamp;
String current_machine;
String current_mold;
String current_event;


MySQL_Connection conn((Client *)&client);

MySQL_Query *query_mem;

/*
   OTA functions/params
*/
#include <AsyncElegantOTA.h>
AsyncWebServer server(80);

/*
   Realtime functions/params
*/

#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 32400;
const int   daylightOffset_sec = 0; //in South Korea, there is no daylight offset

void setTimeZone(char* timezone) {
  setenv("TZ", timezone, 1);
  tzset();
}

void printLocalTime()
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  //  return &timeinfo;
}

String make_query(String timestamp, String machine, String mold, String event) {
  String query = INSERT_PREFIX + COLUMN_NAMES;
  String values = "  VALUES ";
  query = query.concat(values);
  String parenthesis = "(";
  query = query.concat(parenthesis);
  String dot = "'";
  query = query.concat(dot);
  query = query + timestamp;
  query = query.concat(dot);
  String delimeter = ",";
  query = query.concat(delimeter);
  query = query.concat(dot);
  query = query + machine;
  query = query.concat(dot);
  query = query.concat(delimeter);
  query = query.concat(dot);
  query = query + mold;
  query = query.concat(dot);
  query = query.concat(delimeter);
  query = query.concat(dot);
  query = query + event;
  query = query.concat(dot);
  String right = ")";
  query = query.concat(right);
  //  query = query + "(";
  //  query  = query + "'" + timestamp + "'" + ",";
  //  query  = query + "'" + machine + "'" + ",";
  //  query  = query + "'" + mold + "'" + ",";
  //  query  = query + "'" + event + "'" ;
  //  query = query + ")";
}
#define echoPin 32
#define trigPin 33

int IRpin = 34;
int sensor_val;
float dist;

long duration;
int distance;

void setup()
{ pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nStarting Basic_Insert_ESP on " + String(ARDUINO_BOARD));

  // Begin WiFi section
  Serial.println(String("Connecting to ") + ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // print out info about the connection:
  Serial.print("Connected to network. My IP address is: ");
  Serial.println(WiFi.localIP());
  //  setTimeZone("GMT+9");
  //OTA
  AsyncElegantOTA.begin(&server);
  server.begin();

  Serial.print("Connecting to SQL Server @ ");
  Serial.print(server_addr);
  Serial.println(String(", Port = ") + server_port);
  Serial.println(String("User = ") + user + String(", PW = ") + password + String(", DB = ") + default_database);
}

void runInsert(void)
{
  // Initiate the query class instance
  MySQL_Query query_mem = MySQL_Query(&conn);

  if (conn.connected())
  {
    Serial.println(INSERT_SQL);

    // Execute the query
    // KH, check if valid before fetching
    if ( !query_mem.execute(INSERT_SQL.c_str()) )
      Serial.println("Insert error");
    else
      Serial.println("Data Inserted.");
  }
  else
  {
    Serial.println("Disconnected from Server. Can't insert.");
  }
}

void loop()
{
  //  Serial.println("OTA test");
  AsyncElegantOTA.loop();
//  WiFi.reconnect();
  Serial.println("Connecting...");
  Serial.print("Current date/time is ");
  printLocalTime();
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0343 / 2;
  Serial.print("Sonar Distance: ");
  Serial.print(distance);
  Serial.print(" cm");




  sensor_val = map(analogRead(IRpin), 0, 4096, 0, 3300);
  dist = (24.61 / (sensor_val - 0.1696)) * 1000;
  Serial.print("  laser ");
  Serial.print(dist);
  Serial.println(" cm");
  //  String make_query(String timestamp, String machine, String mold, String event) {
  String timestamp_test = "2021-05-23 12:34:56";
  String machine_test = "testmachine";
  String mold_test = "testmold";
  String test_value = "";
  String delimeter = ",";
  test_value  = test_value.concat(distance);
  test_value = test_value.concat(delimeter);
  test_value = test_value.concat(dist);

  //  INSERT_SQL = make_query(timestamp_test, machine_test, mold_test,test_value);
  //  //if (conn.connect(server_addr, server_port, user, password))
  if (conn.connectNonBlocking(server_addr, server_port, user, password) != RESULT_FAIL)
  {
    delay(500);
    runInsert();
    conn.close();                     // close the connection
  }
  else
  {
    Serial.println("\nConnect failed. Trying again on next iteration.");
  }

  //  Serial.println("\nSleeping...");
  //  Serial.println("================================================");

  delay(500);
}
