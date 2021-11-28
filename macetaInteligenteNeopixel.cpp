#include <Adafruit_NeoPixel.h>

#define PIN_RELE      9
#define PIN_NEOPIXEL  10
#define PIN_BOTON     11
#define SENSOR        A0

#define VALOR_HUMEDAD 700
#define NUMPIXELS     64
#define DEBOUNCE_REP  40

#define ESTADO_PRESIONADO    0
#define ESTADO_NO_PRESIONADO 1

#define ESTADO_APAGADO 0
#define ESTADO_COLOR_1 1
#define ESTADO_COLOR_2 2
#define ESTADO_COLOR_3 3

Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

unsigned long interval_Motor = 5000;
unsigned long interval_Sensor = 20000; ///2 horas = 7.200.000 milisegundos
unsigned long interval_Button = 1000;
unsigned long previousMillis_sensor;
unsigned long previousMillis_motor;
unsigned long previousMillis_button;
int lecturaSensor, estadoNeopixels;
int flagButton; int buttonFSM;

void setup() {
  pinMode(PIN_BOTON, INPUT_PULLUP);
  pinMode(PIN_RELE, OUTPUT);

  previousMillis_sensor = millis();
  previousMillis_motor = millis();
  previousMillis_button = millis();

  estadoNeopixels = ESTADO_APAGADO;
  digitalWrite(PIN_RELE, HIGH);

  Serial.begin(9600);
  pixels.begin();
  pixels.clear();

  flagButton = 0;
  buttonFSM = ESTADO_NO_PRESIONADO ;
}


void loop() {
 /// int estadoBoton = debounce(PIN_BOTON);
  maceta();
  machineButton();
  maquina_Neopixels();
  delay(100);
}


void maceta() {
  lecturaSensor = analogRead(SENSOR);
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis_sensor) >= interval_Sensor) {
    Serial.println("Lee humedad");
    if (lecturaSensor > 500) {
      digitalWrite(PIN_RELE, LOW);
      Serial.println("Relé prendido");
      previousMillis_motor = millis();
    }
    previousMillis_sensor = millis();
  }

  if ((currentMillis - previousMillis_motor) >= interval_Motor) {
    Serial.println("Relé apagado");
    digitalWrite(PIN_RELE, HIGH);
  }

 
}


void maquina_Neopixels() {
  switch (estadoNeopixels) {
    case ESTADO_APAGADO:
      if (flagButton == 1){
        Serial.println("Apagado");
        flagButton = 0;
        estadoNeopixels = ESTADO_COLOR_1;
        for (int i = 0; i <= NUMPIXELS; i++) {
         
          pixels.setPixelColor(i, pixels.Color(0, 0, 0));
          pixels.show();
        }
      }

      break;

    case ESTADO_COLOR_1:
      if (flagButton == 1) {
        Serial.println("Color 1");
        flagButton = 0;
        estadoNeopixels = ESTADO_COLOR_2;
        for (int i = 0; i < NUMPIXELS; i++) {
        i++;
        pixels.setPixelColor(i, pixels.Color(255, 0, 0));
          pixels.show();
        }
      }
      break;

    case ESTADO_COLOR_2:
      if (flagButton == 1) {
        Serial.println("Color 2");
        
        flagButton = 0;
        estadoNeopixels = ESTADO_COLOR_3;
        for (int i = 0; i < NUMPIXELS; i++) {
        i++;
          pixels.setPixelColor(i, pixels.Color(0, 255, 0));
          pixels.show();
        }
      }
      break;

    case ESTADO_COLOR_3:
      if (flagButton == 1) {
        Serial.println("Color 3");
        flagButton = 0;
        estadoNeopixels = ESTADO_APAGADO;
        for (int i = 0; i < NUMPIXELS; i++) {
        i++;
          pixels.setPixelColor(i, pixels.Color(0, 0, 255));
          pixels.show();
        }
      }
      break;
  }
}
void machineButton(void) {
  int dato;
  switch (buttonFSM) {
    case ESTADO_NO_PRESIONADO:
      dato = debounce(PIN_BOTON);
      if (dato == 1) {
        buttonFSM = ESTADO_PRESIONADO;
      }
      break;

    case ESTADO_PRESIONADO:
      dato = debounce(PIN_BOTON);
      if (dato == 0) {
        buttonFSM = ESTADO_NO_PRESIONADO;
        flagButton = 1;
      }
      break;
  }
}

int debounce(int pin) {
  int state, previous_state;
  previous_state = digitalRead(pin);
  for (int i = 0; i < DEBOUNCE_REP; i++) {
    delay(1);
    state = digitalRead(pin);
    if (state != previous_state) {
      i = 0;
      previous_state = state;
    }
  }
  return state;
}