#include <AccelStepper.h>
#include <DHT.h>

AccelStepper stepper(1, 8, 9);  // Paso a paso en el pin 8, dirección en el pin 9
long currentPosition = 0;  // Variable para almacenar la posición actual en grados
long currentSteps = 0; // Variable para almacenar la posición actual en pasos
const int MAX_ANGLE = 120;  // Límite de seguridad en grados

const int endstopPin = 10;  // Pin del final de carrera
const int analogPin = A0;  // Pin analógico para la lectura de batería
const int dhtPin = 7;      // Pin de datos del sensor DHT22

const float factorVoltaje = 0.2;

DHT dht(dhtPin, DHT22);

void setup() {
  pinMode(endstopPin, INPUT_PULLUP);

  Serial.begin(9600);
  
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);

  dht.begin();

  // Serial.println("Inicialización completada. Esperando comandos...");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    Serial.print("Comando recibido: ");
    Serial.println(command);
    handle_command(command);
  }
}

void handle_command(String command) {
  if (command.startsWith("*")) {
    Serial.println("Formato de comando correcto.");

    // El comando sigue el formato *ANGULO-VELOCIDAD*
    if (command.startsWith("*SETZERO*")) {
      set_zero_position();
      return;
    }

    // Comando para obtener la posición actual
    if (command.startsWith("*GETPOS*")) {
      get_current_position();
      return;
    }

    // Comando de calibración
    if (command.startsWith("*CALIBRATE*")) {
      calibrate_motor();
      return;
    }

    // Comando para leer el valor de la batería
    if (command.startsWith("*READBATTERY*")) {
      read_battery();
      return;
    }

    // Comando para leer temperatura y humedad
    if (command.startsWith("*READTEMP*")) {
      read_temperature_humidity();
      return;
    }

    command.remove(0, 1);  // Elimina el primer caracter "*"
    int delimiter_index = command.indexOf('-');
    
    if (delimiter_index != -1) {
      long targetAngle = command.substring(0, delimiter_index).toInt();
      long speed = command.substring(delimiter_index + 1).toInt();

      if (targetAngle > MAX_ANGLE) {
        Serial.println("Error: Ángulo objetivo excede el límite de seguridad.");
        return;
      }

      Serial.print("Ángulo origen: ");
      Serial.println(currentPosition);
      Serial.print("Ángulo destino: ");
      Serial.println(targetAngle);
      Serial.print("Velocidad: ");
      Serial.println(speed);

      move_stepper(targetAngle, speed);
    }
  } else {
    Serial.println("Formato de comando incorrecto.");
  }
}

void move_stepper(long targetAngle, long speed) {
  long angleDifference = targetAngle - currentPosition;
  Serial.print("angleDifference: ");
  Serial.println(angleDifference);
  
  stepper.setSpeed(angleDifference > 0 ? speed : -speed);

  // Calcular pasos necesarios para mover al ángulo destino desde la posición actual
  // 64 vuentas sinfin = 1 vuelta montura
  // 64 vuentas sinfin = 360 ° montura
  // 64 vuentas motor = 360 ° montura
  // 64*200 pasos motor = 360 ° montura
  
  // Para probar el motor solo:
  // long steps = (angleDifference) * 200 / 360;  // Suponiendo motor con 200 pasos por vuelta
  // Para probar la montura
  long steps = (angleDifference) * 200 * 65 / 360;  // Suponiendo motor con 200 pasos por vuelta

  Serial.print("steps: ");
  Serial.println(steps);
  
  stepper.moveTo(steps + currentSteps);
  currentSteps = steps + currentSteps;

  Serial.print("Moviendo hacia destino a ");
  Serial.print(targetAngle);
  Serial.println(" grados.");

  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }

  currentPosition = targetAngle;

  Serial.print("Se llegó a destino. Nueva posición: ");
  Serial.println(currentPosition);
}

void set_zero_position() {
  currentPosition = 0;
  currentSteps = 0;
  
  Serial.println("Posición actual establecida como cero.");
}

void get_current_position() {
  Serial.print("Posición actual: ");
  Serial.println(currentPosition);
}

void calibrate_motor() {
  Serial.println("Iniciando calibración...");

  stepper.setSpeed(-1);  // Velocidad negativa para moverse hacia atrás
  stepper.moveTo(-4334);  // Un valor grande para asegurar que se mueva lo suficiente, equivalente a -120 grados

  while (digitalRead(endstopPin) == HIGH && stepper.distanceToGo() != 0) {
    stepper.run();
  }

  stepper.stop();  // Detener el motor cuando se toque el final de carrera
  
  Serial.println("Fin de carrera pulsado");

  stepper.setCurrentPosition(-85);
  currentPosition = -85;
  currentSteps = 0;
  
  Serial.println("Posición seteada a -85° ");

  Serial.println("Regresando a posición cero...");

  move_stepper(0, 1); // Mover a posición cero, velocidad 1

  set_zero_position(); // Establecer la posición actual como cero
  
  Serial.println("Calibración completa.");
}

void read_battery() {
  int batteryValue = analogRead(analogPin);
  Serial.print("Nivel de batería: ");
  Serial.println(batteryValue*factorVoltaje);
}

void read_temperature_humidity() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.print("°C, Humedad: ");
  Serial.print(humidity);
  Serial.println("%");
}