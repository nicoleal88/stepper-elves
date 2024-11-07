#include <AccelStepper.h>
#include <DHT.h>
#include <Servo.h>

// Variables
long currentPosition = 0; // Variable para almacenar la posición actual en grados
long currentSteps = 0;    // Variable para almacenar la posición actual en pasos
long angleDifference = 0;
long Flag = 0;

// Pines
const int analogPin = A0;  // Pin analógico para la lectura de batería
const int pinServo = 6;    // Pin del servo
const int dhtPin = 7;      // Pin de datos del sensor DHT22
const int stepPin = 8;     // Pin del step del driver A4988
const int dirPin = 9;      // Pin del step del driver A4988
const int endstopPin = 10; // Pin del final de carrera

// Constantes
const int MAX_ANGLE = 120;          // Límite de seguridad en grados
long int park_angle = -75;          // ángulo de park en grados
const float factorVoltaje = 0.0202; // Factor de conversión ADC->voltaje
const int servoOnAngle = 25;        // Angulo para servo ON
const int servoOffAngle = 0;        // Angulo para servo OFF

// Objetos
AccelStepper stepper(1, stepPin, dirPin); // Paso a paso en el pin 8, dirección en el pin 9
DHT dht(dhtPin, DHT22);
Servo miServo;

void setup()
{
  // put your setup code here, to run once:
  pinMode(endstopPin, INPUT_PULLUP);

  Serial.begin(9600);

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);

  miServo.attach(pinServo); // El pin del servo
  miServo.write(0);
  dht.begin();

  Serial.println("Inicialización completada. Esperando comandos...");
  Serial.println("#####");
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('\n');
    Serial.print("Comando recibido: ");
    Serial.println(command);
    handle_command(command);
  }
}

void handle_command(String command)
{
  if (command.startsWith("*"))
  {
    // Serial.println("Formato de comando correcto.");

    // El comando sigue el formato *ANGULO-VELOCIDAD*
    if (command.startsWith("*SETZERO*"))
    {
      set_zero_position();
      return;
    }

    // Comando para obtener la posición actual
    if (command.startsWith("*GETPOS*"))
    {
      get_current_position();
      return;
    }

    // Comando de calibración
    if (command.startsWith("*CALIBRATE*"))
    {
      calibrate_motor();
      return;
    }

    // Comando para estacionar la camara
    if (command.startsWith("*PARK*"))
    {
      park_camara(50);
      return;
    }

    // Comando para leer el valor de la batería
    if (command.startsWith("*READBATTERY*"))
    {
      read_battery();
      return;
    }

    // Comando para leer temperatura y humedad
    if (command.startsWith("*READTEMP*"))
    {
      read_temperature_humidity();
      return;
    }

    // Comando para colocar servo en posición ON
    if (command.startsWith("*SERVO-ON*"))
    {
      servoOn();
      return;
    }

    // Comando para colocar servo en posición OFF
    if (command.startsWith("*SERVO-OFF*"))
    {
      servoOff();
      return;
    }

    command.remove(0, 1); // Elimina el primer caracter "*"
    int delimiter_index = command.indexOf('-');

    if (delimiter_index != -1)
    {
      long targetAngle = command.substring(0, delimiter_index).toInt();
      long speed = command.substring(delimiter_index + 1).toInt();

      if (targetAngle > MAX_ANGLE)
      {
        Serial.println("Error: Ángulo objetivo excede el límite de seguridad.");
        Serial.println("#####");
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
  }
  else
  {
    Serial.println("Formato de comando incorrecto.");
    Serial.println("#####");
  }
}

void move_stepper(long targetAngle, long speed)
{
  if (currentPosition < 0 && Flag == 1)
  {
    angleDifference = targetAngle;
  }
  else
  {
    angleDifference = targetAngle - currentPosition;
  }

  Serial.print("angleDifference: ");
  if (currentPosition < 0 && Flag == 1)
  {
    Serial.println(-currentPosition);
  }
  else
  {
    Serial.println(angleDifference);
  }

  stepper.setSpeed(angleDifference > 0 ? speed : -speed);

  // Calcular pasos necesarios para mover al ángulo destino desde la posición actual
  // 65 vuentas sinfin = 1 vuelta montura
  // 65 vuentas sinfin = 360 ° montura
  // 65 vuentas motor = 360 ° montura
  // 65*200 pasos motor = 360 ° montura

  // Para probar el motor solo:
  // long steps = (angleDifference) * 50 / 360;  // Suponiendo motor con 200 pasos por vuelta
  // long steps = (angleDifference) * 48 * 32 / 360;  // Suponiendo motor con 50 pasos por vuelta
  // Para probar la montura
  long steps = (angleDifference) * 200 * 65 / 360; // Suponiendo motor con 200 pasos por vuelta

  Serial.print("steps: ");
  if (currentPosition < 0 && Flag == 1)
  {
    long pasos = -(currentPosition) * 200 * 65 / 360; // Solo para imprimir el número de pasos
    Serial.println(pasos);
  }
  else
  {
    Serial.println(steps);
  }

  stepper.moveTo(steps + currentSteps);
  currentSteps = steps + currentSteps;

  Serial.print("Moviendo hacia destino a ");
  Serial.print(targetAngle);
  Serial.println(" grados.");

  while (stepper.distanceToGo() != 0)
  {
    stepper.run();
  }

  currentPosition = targetAngle;

  Serial.print("Se llegó a destino. Nueva posición: ");
  Serial.println(currentPosition);
  Serial.println("#####");
}

void set_zero_position()
{
  stepper.setCurrentPosition(0);
  currentPosition = 0;
  currentSteps = 0;

  Serial.println("Posición actual establecida como cero.");
  Serial.println("#####");
}

void park_camara(long speed)
{
  long currentPosition1 = currentPosition;
  currentPosition = 0;
  currentSteps = 0;
  Serial.println("Parking... ");
  long angleDifference = park_angle - currentPosition;
  long angleDifference1 = park_angle - currentPosition1;
  Serial.print("angleDifference: ");
  Serial.println(angleDifference1);
  long steps = (angleDifference) * 200 * 65 / 360; // Suponiendo motor con 200 pasos por vuelta

  stepper.setSpeed(-speed); // Velocidad negativa para moverse hacia atrás
  stepper.moveTo(steps);    // Un valor grande

  while (digitalRead(endstopPin) == HIGH && stepper.distanceToGo() != 0)
  {
    stepper.run();
  }
  currentPosition = park_angle;

  Serial.print("End parking");
  Serial.println("#####");
}

void get_current_position()
{
  Serial.print("Posición actual: ");
  Serial.println(currentPosition);
  Serial.println("#####");
}

void calibrate_motor()
{
  Serial.println("Iniciando calibración...");

  Flag = 0;

  stepper.setSpeed(-1);  // Velocidad negativa para moverse hacia atrás
  stepper.moveTo(-7584); // Un valor grande para asegurar que se mueva lo suficiente, equivalente a -210 grados

  while (digitalRead(endstopPin) == HIGH && stepper.distanceToGo() != 0)
  {
    stepper.run();
  }

  stepper.stop(); // Detener el motor cuando se toque el final de carrera

  Serial.println("Fin de carrera pulsado");
  Serial.print("current position: ");
  Serial.println(currentPosition);
  Serial.print("current steps: ");
  Serial.println(currentSteps);

  stepper.setCurrentPosition(-85);
  currentPosition = -85;
  currentSteps = 0;

  Serial.print("current position: ");
  Serial.println(currentPosition);
  Serial.print("current steps: ");
  Serial.println(currentSteps);

  Serial.println("Posición seteada a -85° ");

  Serial.println("Regresando a posición cero...");

  move_stepper(0, 1); // Mover a posición cero, velocidad 1

  set_zero_position(); // Establecer la posición actual como cero

  Serial.print("current position: ");
  Serial.println(currentPosition);
  Serial.print("current steps: ");
  Serial.println(currentSteps);

  Flag = 1;

  Serial.println("Calibración completa.");
  Serial.println("#####");
}

void read_battery()
{
  int batteryValue = analogRead(analogPin);
  Serial.print("Nivel de batería: ");
  Serial.println(batteryValue * factorVoltaje);
  Serial.println("#####");
}

void read_temperature_humidity()
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.print("°C, Humedad: ");
  Serial.print(humidity);
  Serial.println("%");
  Serial.println("#####");
}

void servoOn()
{
  miServo.write(servoOnAngle);
  Serial.println("Servo en posición ON");
  Serial.println("#####");
}

void servoOff()
{
  miServo.write(servoOffAngle);
  Serial.println("Servo en posición OFF");
  Serial.println("#####");
}
