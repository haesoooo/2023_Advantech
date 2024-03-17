#include <DHT11.h>
#include <MySQL_Generic.h>

#define relay D6
#define DHTPIN D5
DHT11 dht11(DHTPIN);

String tempjudge = "";
String lightjudge = "";

#if ! (ESP8266 || ESP32 )
  #error This code is intended to run on the ESP8266/ESP32 platform! Please check your Tools->Board setting
#endif

char ssid[] = "Haesoooo";             // your network SSID (name)
char pass[] = "04189087";         // your network password

char user[]         = "test";          // MySQL user login username
char password[]     = "1111";          // MySQL user login password

#define MYSQL_DEBUG_PORT      Serial
// Debug Level from 0 to 4
#define _MYSQL_LOGLEVEL_      1


#define USING_HOST_NAME     false


// Optional using hostname, and Ethernet built-in DNS lookup
char SQL_server[] = "118.46.197.161"; // change to your server's hostname/URL
uint16_t server_port = 3306;    //3306;
char default_database[] = "data";           //"test_arduino";
char default_table[]    = "judge";          //"test_arduino";

MySQL_Connection conn((Client *)&client);
MySQL_Query *query_mem;
MySQL_Query sql_query = MySQL_Query(&conn);

void setup()
{
  Serial.begin(115200);
  
  MYSQL_DISPLAY1("\nStarting Basic_Insert_ESP on", ARDUINO_BOARD);
  MYSQL_DISPLAY(MYSQL_MARIADB_GENERIC_VERSION);

  // Begin WiFi section
  MYSQL_DISPLAY1("Connecting to", ssid);
  
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    MYSQL_DISPLAY0(".");
  }

  // print out info about the connection:
  MYSQL_DISPLAY1("Connected to network. My IP address is:", WiFi.localIP());

  MYSQL_DISPLAY3("Connecting to SQL Server @", SQL_server, ", Port =", server_port);
  MYSQL_DISPLAY5("User =", user, ", PW =", password, ", DB =", default_database);

  pinMode(relay, OUTPUT);
}


String query = "";
String INSERT_SQL = "";

void loop()
{
  String text = "";
  query = "select temperature_judge, light_judge from data.judge order by id desc limit 1";

  if (conn.connectNonBlocking(SQL_server, server_port, user, password) != RESULT_FAIL)
  {
    delay(500);
    
    runQuery();
    conn.close();                     // close the connection
  } 
  else 
  {
    MYSQL_DISPLAY("\nConnect failed. Trying again on next iteration.");
  }
  
  while(1)
  {
    delay(20);
  }
  //text string split
  int index = text.indexOf(",");
  String tempjudge = text.substring(0, index);
  String lightjudge = text.substring(index+1, text.length());

  int temp = dht11.readTemperature();
  int hum = dht11.readHumidity();

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %");
  
  if (tempjudge = "Inppropriate Temperature") {
      digitalWrite(relay, LOW);
      Serial.println("Heater turned ON");
  }
  else {
    digitalWrite(relay, HIGH);
    Serial.println("Heater turned OFF");
  }

  delay(5000);
}


void runQuery()
{
  MYSQL_DISPLAY("====================================================");
  MYSQL_DISPLAY("> Running SELECT with dynamically supplied parameter");
  MySQL_Query query_mem = MySQL_Query(&conn);
  String text = "";
  // Execute the query
  // KH, check if valid before fetching
  // 쿼리 실행
  if ( !query_mem.execute(query.c_str()) )
  {
    MYSQL_DISPLAY("Querying error");
    return;
  }
  
  // Fetch the columns and print them
  column_names *cols = query_mem.get_columns();
  // 쿼리 조건에 해당하는 데이터 열 가져옴
  MYSQL_DISPLAY();

  
  // Read the rows and print them
  row_values *row = NULL;

  //row 가 NULL 이 될때까지 반복
  do 
  {
    row = query_mem.get_next_row();
    
    if (row != NULL) 
    {
      for (int f = 0; f < cols->num_fields; f++) 
      {
        MYSQL_DISPLAY(row->values[f]);
        text += row->values[f];
        if (f < cols->num_fields - 1) 
        {
          MYSQL_DISPLAY0(",");
          text += ",";
        }
      }
      Serial.println();
      Serial.print("SQL read : ");
      Serial.println(text);
      
      MYSQL_DISPLAY();
    }
  } while (row != NULL);
}