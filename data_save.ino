#include <ESP8266WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <DHT.h>

#define DHTPIN 2     
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);

IPAddress server_addr(192,168,0,3);  // MySQL 서버의 IP 주소
char user[] = "test";               // MySQL 사용자명
char password[] = "1111";           // MySQL 암호
char db[] = "data";                 // MySQL 데이터베이스명

WiFiClient client;
MySQL_Connection conn((Client*)&client);

void setup() {
  Serial.begin(115200);
  WiFi.begin("iptime", ""); // Wi-Fi 연결

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  if (conn.connect(server_addr, 3306, user, password, db)) {
    Serial.println("Connected to MySQL server");
  } else {
    Serial.println("Connection failed.");
  }

  dht.begin();
}

void loop() {

  // 온습도 읽기
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  delay(2000);
  
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print("%, Temperature: ");
  Serial.print(temp);
  Serial.println("°C");

  // MySQL 서버로 데이터 전송
  if (conn.connected()) {
    char query[128];
    sprintf(query, "INSERT INTO testdata (humidity, temperature) VALUES ('%.2f', '%.2f')", hum, temp);
    
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    cur_mem->execute(query);
    delete cur_mem;
  } 
  else {
    Serial.println("MySQL Server Connection Lost");
  }

  delay(5000); // 
}

