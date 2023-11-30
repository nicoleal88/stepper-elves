# Control de Motor Paso a Paso con Arduino

Este proyecto permite controlar un motor paso a paso usando un Arduino y una librería AccelStepper. Puedes enviar comandos a través del puerto serie para mover el motor a posiciones específicas, realizar calibraciones y más.

## Requisitos

- Arduino
- Motor paso a paso bipolar
- Driver A4988 o similar
- Sensor de final de carrera
- Sensor DHT22 para la medición de temperatura y humedad
- Librería AccelStepper (instalable desde el Administrador de bibliotecas de Arduino IDE)
- Librería DHT (instalable desde el Administrador de bibliotecas de Arduino IDE)

## Conexiones

- Conecta el motor al pin 8 (paso) y 9 (dirección) del Arduino.
- Conecta una pata del sensor de final de carrera al pin 10 del Arduino, y la otra a GND (Usando el modo Normal Abierto).
- Conecta el sensor DHT22 (Pata 1 a VCC, pata 2 a pin 7, pata 3 desconectada, pata 4 a GND)
- Asegúrate de tener la alimentación adecuada para el motor y los elementos de control.

## Uso

1. Carga el código en el Arduino utilizando el Arduino IDE.
2. Abre el monitor serie en el Arduino IDE para enviar comandos.
3. Utiliza los siguientes comandos:

   - `*ANGULO-VELOCIDAD*`: Mueve el motor al ángulo especificado a la velocidad indicada.
   - `*SETZERO*`: Establece la posición actual como cero.
   - `*GETPOS*`: Obtiene la posición actual del motor.
   - `*CALIBRATE*`: Inicia el proceso de calibración moviendo el motor hacia atrás hasta tocar el final de carrera.
   - `*READBATTERY*`: Lee el nivel de batería del pin analógico.
   - `*READTEMP*`: Lee la temperatura y humedad del sensor DHT22.

### Ejemplos de comandos:

- `*45-50*`: Mueve el motor al ángulo 45 a una velocidad de 50.
- `*SETZERO*`: Establece la posición actual como cero.
- `*GETPOS*`: Obtiene la posición actual del motor.
- `*CALIBRATE*`: Inicia el proceso de calibración.
- `*READBATTERY*`: Lee el nivel de batería.
- `*READTEMP*`: Lee la temperatura y humedad.

**Importante**: Asegúrate de tener cuidado con las conexiones eléctricas y la manipulación del motor.

## Límite de Seguridad

El código incluye un límite de seguridad `MAX_ANGLE` para evitar movimientos que excedan este límite. Si un comando especifica un ángulo mayor a `MAX_ANGLE`, se imprimirá un error y el movimiento será rechazado.
