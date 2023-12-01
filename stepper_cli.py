import serial
import time

# Configuración del puerto serie
port = "/dev/ttyACM0"  # Cambia esto al puerto serie correcto
baudrate = 9600

# Abre la conexión al puerto serie
ser = serial.Serial(port, baudrate, timeout=1)

# Espera un breve momento para que Arduino termine de inicializarse y envíe mensajes al setup
# time.sleep(1)

# Limpia el buffer serial
ser.reset_input_buffer()
ser.reset_output_buffer()

# Lee y muestra el mensaje de bienvenida del Arduino
bienvenida = ser.readline().decode().strip()
print("\n", bienvenida)

def enviar_comando(comando):
    ser.write(comando.encode())
    ser.write(b"\n")  # Añade una nueva línea al final del comando para indicar el final
    ser.flush()  # Asegura que los datos se envíen

def recibir_resultado():
    resultado_completo = ""
    while True:
        resultado_linea = ser.readline().decode().strip()
        if resultado_linea == "#####":
            break  # Rompe el bucle cuando se detecta el delimitador
        # print("Resultado recibido:", resultado_linea)
        if len(resultado_linea) == 0:
            print("...")
        else:
            print(resultado_linea)

# Bucle principal
while True:
    # Entrada del usuario
    comando = input("\nIntroduce el comando (o 'exit' para salir): ")

    if comando.lower() == 'exit':
        break  # Sal del bucle si el usuario escribe 'exit'

    # Limpia el buffer serial antes de enviar el comando
    ser.reset_input_buffer()
    ser.reset_output_buffer()

    # Envía el comando al Arduino
    enviar_comando(comando)

    # print("Comando enviado, esperando respuesta...")

    # Espera y recibe el resultado del Arduino
    recibir_resultado()

# Cierra la conexión al puerto serie
ser.close()
