#include <WiFi.h>        // Para conectarse a WiFi
#include <HTTPClient.h>  // Para hacer peticiones HTTP
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

#define boton 17  // Push button
#define rojo 21   // LED rojo
#define verde 19  // LED verde
#define azul 18   // LED azul

Adafruit_BME280 bme;                 // El sensor que usamos
LiquidCrystal_I2C lcd(0x27, 16, 2);  // La pantallita LCD

// Estadps iniciales
bool estadoBotonPrevio = LOW;
bool estadoAlarma = LOW;

// Valores de la alarma
const float TEMP_MIN = 18.0;
const float TEMP_MAX = 28.0;
const float HUM_MIN = 30.0;
const float HUM_MAX = 70.0;

float temp;
float hum;

// Esto es para el "delay" para imprimir en la pantallita
unsigned long ultimaLectura = 0;
const unsigned long intervalo = 1500;

// Configuración del WiFi
const char* ssid = "INFINITUM627A_2.4";
const char* password = "weYTYkFDmo";

// URI de la API REST
const char* apiEndpoint = "http://192.168.1.69:9090/lecturas/registrar";

void setup() {
  Serial.begin(115200);
  // Configuramos el botón como pulldown
  pinMode(boton, INPUT_PULLDOWN);
  // Configuramos los LEDs como salidas
  pinMode(rojo, OUTPUT);
  pinMode(verde, OUTPUT);
  pinMode(azul, OUTPUT);

  // Los apagamos todos (con HIGH porque son de ánodo común)
  digitalWrite(rojo, HIGH);
  digitalWrite(verde, HIGH);
  digitalWrite(azul, HIGH);

  // Inicia I2C con SDA en GPIO 4 y SCL en GPIO 2 tanto para la
  // pantalla LCD como para el sensor, el cual es un BME280z
  Wire.begin(4, 2);

  // Prendemos la pantalla
  lcd.init();
  lcd.backlight();

  // Prendemos el sensor
  bool status = bme.begin(0x76);  // o 0x77, dependiendo de tu sensor
  if (!status) {
    Serial.println("No se encontró el BME280.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error BME280");
    while (1)
      ;
  }

  // Esto es para prender el WiFi
  WiFi.begin(ssid, password);
  lcd.setCursor(0, 0);
  lcd.print("Conectando WiFi");  // Lo imprimimos en la pantalla
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi conectado");  // Ya nos conectamos!!!
  delay(1000);
}

void loop() {
  // Leemos el botón
  bool estadoBotonActual = digitalRead(boton);
  // Si antes estaba HIGH y ahora está LOW, significa que se presionó
  // porque está en pulldown.
  if (estadoBotonPrevio == HIGH && estadoBotonActual == LOW) {
    delay(50);  // Para el debounce
    // Si en efecto se presionó, cambiamos el estado de la alarma
    if (digitalRead(boton) == LOW) {
      estadoAlarma = !estadoAlarma;
    }
  }
  // Actualizamos el estado previo del botón
  estadoBotonPrevio = estadoBotonActual;

  // Obtenemos las lecturas del sensor
  temp = bme.readTemperature();
  hum = bme.readHumidity();

  // Esto es como un delay, pero no detiene todo el programa
  // permitiendo que se haga esto cada 2 segundos pero el resto
  // siga funcionando
  unsigned long tiempoActual = millis();
  if (tiempoActual - ultimaLectura >= intervalo) {
    imprimirLecturas();
    enviarLecturaSeparada("temperatura", temp);
    enviarLecturaSeparada("humedad", hum);
    // Se actualiza el tiempo en que se capturaron las últimas lecturas
    ultimaLectura = tiempoActual;
  }

  // Si la alarma está activada, se verifica si se debe disparar o no.
  if (estadoAlarma) {
    verificarAlarma();
  } else {
    prenderAzul();
  }
}

void verificarAlarma() {
  bool fueraDeLimites = (temp < TEMP_MIN || temp > TEMP_MAX || hum < HUM_MIN || hum > HUM_MAX);
  // Si las lecturas están fuera de los límites, se prende rojo el LED
  if (fueraDeLimites) {
    prenderRojo();
  } else {
    prenderVerde(); // Si no están fuera, se prende verde
  }
}

void imprimirLecturas() {
  lcd.clear(); // Se limpia la pantalla
  if (estadoAlarma) {
    verificarAlarma(); // Se verifica la alarma
    lcd.setCursor(0, 0); // Esto indica que escribirá en la fila de arriba
    lcd.print("ALARMA PRENDIDA"); // Se imprime en la pantallita
  } else {
    lcd.setCursor(0, 0);
    lcd.print("ALARMA APAGADA "); // Se imprime en la pantallita
  }
  lcd.setCursor(0, 1); // Esto indica que escribirá en la fila de abajo
  lcd.print("T:");
  lcd.print(temp, 1);
  lcd.print("C H:");
  lcd.print(hum, 1);
  lcd.print("%");
}

// Prende el LED rojo y apaga los demás
void prenderRojo() {
  digitalWrite(verde, HIGH);
  digitalWrite(azul, HIGH);
  digitalWrite(rojo, LOW);
}

// Prende el LED verde y apaga los demás
void prenderVerde() {
  digitalWrite(azul, HIGH);
  digitalWrite(rojo, HIGH);
  digitalWrite(verde, LOW);
}

// Prende el LED azul y apaga los demás
void prenderAzul() {
  digitalWrite(verde, HIGH);
  digitalWrite(rojo, HIGH);
  digitalWrite(azul, LOW);
}

// Envía las lecturas (temperatura y humedad) por separado
void enviarLecturaSeparada(const String& tipo, float valor) {}
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
