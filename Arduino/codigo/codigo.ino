#include <WiFi.h>               // Para conectarse a WiFi
#include <HTTPClient.h>         // Para hacer peticiones HTTP
#include <LiquidCrystal_I2C.h>  // Pantalla LCD con interfaz I2C
#include <Wire.h>
#include <Adafruit_BME280.h>  // Sensor ambiental

// Pines
#define PIN_BOTON 17
#define PIN_LED_ROJO 21
#define PIN_LED_VERDE 19
#define PIN_LED_AZUL 18

// Objetos
Adafruit_BME280 bme;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Estados
bool estadoBotonPrevio = LOW;
bool estadoAlarma = LOW;

// Límites de la alarma
const float TEMP_MIN = 18.0;
const float TEMP_MAX = 28.0;
const float HUM_MIN = 30.0;
const float HUM_MAX = 70.0;

// Valores de sensores
float temp;
float hum;

// Temporizador para lecturas periódicas
unsigned long ultimaLectura = 0;
const unsigned long INTERVALO_LECTURA = 1500;

// Valores de la red WiFi
const char* ssid = "INFINITUM627A_2.4";
const char* password = "weYTYkFDmo";

// Endpoint de la API REST
const char* apiEndpoint = "http://192.168.1.69:9090/lecturas/registrar";

void setup() {
  Serial.begin(115200);

  pinMode(PIN_BOTON, INPUT_PULLDOWN);
  pinMode(PIN_LED_ROJO, OUTPUT);
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_AZUL, OUTPUT);

  // Apagar todos los LEDs (ánodo común)
  apagarTodosLosLEDs();

  // Inicia I2C con SDA en GPIO 4 y SCL en GPIO 2 tanto para la
  // pantalla LCD como para el sensor, el cual es un BME280
  Wire.begin(4, 2);

  // Se prende la pantalla LCD
  lcd.init();
  lcd.backlight();

  // Inicializar sensor BME280
  if (!bme.begin(0x76)) {
    Serial.println("No se encontró el BME280.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error BME280");
    while (true)
      ;  // Se detiene el programa
  }

  // Conexión WiFi
  WiFi.begin(ssid, password);
  lcd.setCursor(0, 0);
  lcd.print("Conectando WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi conectado");  // Ya nos conectamos
  delay(1000);
}

void loop() {
  leerBoton();
  leerSensor();

  // Mostrar datos periódicamente
  if (millis() - ultimaLectura >= INTERVALO_LECTURA) {
    mostrarLecturas();
    enviarLectura("Temperatura", temp);
    enviarLectura("Humedad", hum);
    ultimaLectura = millis();
  }

  // Si la alarma está activada, se verifica si se debe disparar o no.
  if (estadoAlarma) {
    verificarAlarma();
  } else {
    prenderAzul();
  }
}

void leerBoton() {
  // Se lee el estado del botón
  bool estadoActual = digitalRead(PIN_BOTON);

  // Si antes estaba HIGH y ahora está LOW, significa que se presionó
  if (estadoBotonPrevio == HIGH && estadoActual == LOW) {
    delay(50);  // Para el debounce
    if (digitalRead(PIN_BOTON) == LOW) {
      // Cambiar estado de la alarma
      estadoAlarma = !estadoAlarma;
    }
  }
  // Se actualiza el estado previo del botón
  estadoBotonPrevio = estadoActual;
}

void leerSensor() {
  temp = bme.readTemperature();
  hum = bme.readHumidity();
}

void verificarAlarma() {
  bool fueraDeLimites = (temp < TEMP_MIN || temp > TEMP_MAX || hum < HUM_MIN || hum > HUM_MAX);
  // Si las lecturas están fuera de los límites, se prende rojo el LED
  if (fueraDeLimites) {
    prenderRojo();
  } else {
    prenderVerde();  // Si no están fuera, se prende verde
  }
}

void mostrarLecturas() {
  lcd.clear(); // Se limpia la pantalla

  lcd.setCursor(0, 0); // Esto indica que escribirá en la fila de arriba
  // Dependiendo del estado se imprime una o la otra
  lcd.print(estadoAlarma ? "ALARMA PRENDIDA" : "ALARMA APAGADA ");

  lcd.setCursor(0, 1); // Esto indica que escribirá en la fila de abajo
  lcd.print("T:");
  lcd.print(temp, 1);
  lcd.print("C H:");
  lcd.print(hum, 1);
  lcd.print("%");
}

void prenderRojo() {
  digitalWrite(PIN_LED_VERDE, HIGH);
  digitalWrite(PIN_LED_AZUL, HIGH);
  digitalWrite(PIN_LED_ROJO, LOW);
}

void prenderVerde() {
  digitalWrite(PIN_LED_AZUL, HIGH);
  digitalWrite(PIN_LED_ROJO, HIGH);
  digitalWrite(PIN_LED_VERDE, LOW);
}

void prenderAzul() {
  digitalWrite(PIN_LED_VERDE, HIGH);
  digitalWrite(PIN_LED_ROJO, HIGH);
  digitalWrite(PIN_LED_AZUL, LOW);
}

void apagarTodosLosLEDs() {
  digitalWrite(PIN_LED_ROJO, HIGH);
  digitalWrite(PIN_LED_VERDE, HIGH);
  digitalWrite(PIN_LED_AZUL, HIGH);
}

// Envía las lecturas (temperatura y humedad) por separado
void enviarLectura(const String& tipo, float valor) {
  // Si el WiFi está prendido
  if (WiFi.status() == WL_CONNECTED) {
    // Creamos un cliente HTTP
    HTTPClient http;

    // Le decimos que vamos a enviar una petición a la URL de la API
    http.begin(apiEndpoint);
    // Decimos que vamos a enviar un JSON
    http.addHeader("Content-Type", "application/json");
    // Formateamos lo que vamos a enviar
    String json = "{\"tipo\":\"" + tipo + "\",\"valor\":" + String(valor, 2) + "}";
    // Enviamos la petición en un POST con el JSON en el body
    http.POST(json);
    // Terminamos la petición
    http.end();
  } else {
    Serial.println("WiFi no conectado");
  }
}
