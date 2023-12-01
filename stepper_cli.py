import serial

# Configuración del puerto serie
port = "/dev/ttyACM0"  # Cambia esto al puerto serie correcto
baudrate = 9600
# inicio = True
# Abre la conexión al puerto serie
ser = serial.Serial(port, baudrate, timeout=1)

def enviar_comando(comando):
    ser.write(comando.encode())
    ser.write(b"\n")  # Añade una nueva línea al final del comando para indicar el final
    ser.flush()  # Asegura que los datos se envíen

def recibir_resultado():
    while ser.in_waiting:
        resultado = ser.readline().decode().strip()
        print("Resultado:", resultado)

# Bucle principal
while True:
    # if inicio:
    #     print("Loop inicial")
    #     # Limpia el buffer serial antes de enviar el comando
    #     ser.reset_input_buffer()
    #     ser.reset_output_buffer()
    #     inicio = False
    # Entrada del usuario
    comando = input("Introduce el comando (o 'exit' para salir): ")

    if comando.lower() == 'exit':
        break  # Sal del bucle si el usuario escribe 'exit'

    # Envía el comando al Arduino
    enviar_comando(comando)

    # Espera y recibe el resultado del Arduino
    recibir_resultado()

# Cierra la conexión al puerto serie
ser.close()
