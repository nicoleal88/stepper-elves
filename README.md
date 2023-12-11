# Control de Motor Paso a Paso con Arduino

Este proyecto permite controlar un motor paso a paso usando un Arduino y una librería AccelStepper. Puedes enviar comandos a través del puerto serie para mover el motor a posiciones específicas, realizar calibraciones y más.

## Librerías

- [AccelStepper](https://www.arduino.cc/reference/en/libraries/accelstepper/): Para el control del motor paso a paso.
- [DHT](https://www.arduino.cc/reference/en/libraries/dht-sensor-library/): Para la lectura del sensor DHT22.
- [Servo](https://www.arduino.cc/reference/en/libraries/servo/): Para el control del servo.

## Pines y Constantes

### Pines

- `analogPin` (A0): Pin analógico para la lectura de batería
- `pinServo` (6): Pin del servo
- `dhtPin` (7): Pin de datos del sensor DHT22
- `stepPin` (8): Pin del paso del driver A4988
- `dirPin` (9): Pin de dirección del driver A4988
- `endstopPin` (10): Pin del final de carrera

## Constantes

- `MAX_ANGLE`: Límite de seguridad en grados
- `factorVoltaje`: Factor de conversión ADC->voltaje
- `servoOnAngle`: Ángulo para activar el servo
- `servoOffAngle`: Ángulo para desactivar el servo

## Comandos Serie

- `*SETZERO*`: Establece la posición actual como cero.
- `*GETPOS*`: Obtiene la posición actual.
- `*CALIBRATE*`: Inicia la calibración del motor.
- `*READBATTERY*`: Lee el nivel de la batería.
- `*READTEMP*`: Lee la temperatura y la humedad.
- `*SERVO-ON*`: Coloca el servo en posición ON.
- `*SERVO-OFF*`: Coloca el servo en posición OFF.

## Comandos para Control del Motor

Los comandos para el motor siguen el formato: `*ANGULO-VELOCIDAD*`.

Ejemplo:

- `*45-50*`: Gira a 45 grados con velocidad de 50.

### Consideraciones

- El ángulo enviado es absoluto, no relativo.
- El límite de seguridad (`MAX_ANGLE`) rechaza movimientos superiores.
- Comandos no válidos imprimen un error y rechazan el movimiento.

**Importante**: Asegúrate de tener cuidado con las conexiones eléctricas y la manipulación del motor.

## Modelos 3d en OnShape

Los modelos 3d se encuentran en el [link](https://cad.onshape.com/documents/a62f93ff7ced747f6bff1457/w/5efe87a3af445a6ccf31b3ad/e/480793982d3c0177615028ab?renderMode=0&uiState=656f6fc1e612372fc7774d9d)
