/**
 * @file main.c
 * @author Jose Luis Cruz (jlcruz@ipn.mx)
 * @brief
 * @version 0.1
 * @date 2024-06-03
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "miniwin.h"
#include "serial.h"
#include <stdlib.h> // Para srand() y rand()
#include <time.h>   // Para time()

#define GRAVEDAD 1

#define AJUSTEX 2780
#define AJUSTEY 2820
#define VELXMAX 20
#define VELYMAX 20
#define VALMAX 4095

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CIRCLE_RADIUS 30
#define PATO_WIDTH 50 // Ancho del pato
#define PATO_HEIGHT 50 // Alto del pato

int ajusteX(int x) {
    x -= AJUSTEX;
    if (x > 0) {
        x = (int)((double)x * ((double)VELXMAX / (double)(VALMAX - AJUSTEX)));
    } else {
        x = (int)((double)x * ((double)VELXMAX / (double)(AJUSTEX)));
    }
    return x;
}

int ajusteY(int x) {
    x -= AJUSTEY;
    if (x > 0) {
        x = (int)((double)x * ((double)VELYMAX / (double)(VALMAX - AJUSTEY)));
    } else {
        x = (int)((double)x * ((double)VELYMAX / (double)(AJUSTEY)));
    }
    return x;
}

// Función para verificar si la mira está sobre el pato
bool isMiraSobrePato(int cx, int cy, int patoX, int patoY) {
    // Verificar si la mira está dentro del área del pato
    return (cx >= patoX && cx <= patoX + PATO_WIDTH) && (cy >= patoY && cy <= patoY + PATO_HEIGHT);
}

// Función para generar una posición Y aleatoria para el pato
int generarPosYAleatoria() {
    return rand() % 101 + 100; // Devuelve un valor entre 0 y 100
}

int main()
{
    srand(time(NULL)); // Inicializar el generador de números aleatorios

    int t, puntuacion = 0;
    float x = 0, y = 0;
    bool clic = false;
    int cx = 100, cy = 100, acel = 1;
    MWImage pato = creaImagenYMascaraBMP("patoN.bmp", "patoM.bmp");
    MWImage fondo = creaImagenBMP("fondo.bmp");
    SerialPort esp32 = initSerialPort("COM4", B115200);
    char cmd[MAX_DATA_LENGTH];

    // Inicializar posición del pato
    pato.pos_x = -25;
    pato.pos_y = generarPosYAleatoria();

    if (!isConnected(&esp32)) {
        cierra();
        return 1;
    }

    titulo("Progra Avanzada Energia");
    ventana(WINDOW_WIDTH, WINDOW_HEIGHT);
    color_fondo(BLANCO);

    t = tecla();
    while (t != ESCAPE)
    {
        sprintf(cmd, "dame_datos\n");
        writeSerialPort(cmd, strlen(cmd), &esp32);
        Sleep(1);
        if (readSerialPort(cmd, MAX_DATA_LENGTH, &esp32)) {
            // Obtención de los valores del joystick
            int jx = atoi(cmd);
            int jy = atoi(&cmd[5]);
            int jb = atoi(&cmd[10]);

            // Ajustar las coordenadas según los datos del joystick
            cx += ajusteX(jx);
            cy += ajusteY(jy);

            // Aplicar hitbox de la mira dentro de los límites de la ventana
            if (cx - CIRCLE_RADIUS < 0) {
                cx = CIRCLE_RADIUS;
            }
            if (cx + CIRCLE_RADIUS > WINDOW_WIDTH) {
                cx = WINDOW_WIDTH - CIRCLE_RADIUS;
            }
            if (cy - CIRCLE_RADIUS < 0) {
                cy = CIRCLE_RADIUS;
            }
            if (cy + CIRCLE_RADIUS > WINDOW_HEIGHT) {
                cy = WINDOW_HEIGHT - CIRCLE_RADIUS;
            }

            // Manejar el clic del botón
            clic = !jb;
            if (clic) {
                sprintf(cmd, "vibra\n");
                writeSerialPort(cmd, strlen(cmd), &esp32);
                Sleep(1);
                sprintf(cmd, "que_chille\n");
                writeSerialPort(cmd, strlen(cmd), &esp32);
            } else {
                sprintf(cmd, "!vibra\n");
                writeSerialPort(cmd, strlen(cmd), &esp32);
            }

            // Verificar si la mira está sobre el pato y si se hizo clic
            if (clic && isMiraSobrePato(cx, cy, pato.pos_x, pato.pos_y)) {
                // Reaparecer el pato fuera de la ventana a la izquierda con una nueva posición Y
                puntuacion += 100;
                pato.pos_x = -15;
                pato.pos_y = generarPosYAleatoria();

                // Enviar la puntuación al ESP32
                sprintf(cmd, "puntuacion %d\n", puntuacion);
                writeSerialPort(cmd, strlen(cmd), &esp32);
            }
        }

        // Mover el pato hacia la derecha
        pato.pos_x += acel;

        // Si el pato se sale de la ventana por la derecha, reaparecerlo a la izquierda
        if (pato.pos_x > WINDOW_WIDTH) {
            pato.pos_x = -15;
            pato.pos_y = generarPosYAleatoria();
        }

        // Dibujo fondo
        borra();
        color(BLANCO);
        muestraImagen(&fondo);

        // Texto de instrucción
        textoExt(10, 10, "Presione ESC para salir", 22, false, false, false, "Arial");

        // Mostrar la puntuación
        char textoPuntuacion[50];
        sprintf(textoPuntuacion, "Puntuacion: %d", puntuacion);
        textoExt(10, 40, textoPuntuacion, 22, false, false, false, "Arial");

        // Dibujo del pato
        muestraImagen(&pato);

        // Dibujo de la mira
        color(BLANCO);
        circulo(cx, cy, CIRCLE_RADIUS);
        circulo(cx, cy, 20);
        linea(cx, cy - 10, cx, cy - 25);
        linea(cx, cy + 10, cx, cy + 25);
        linea(cx - 10, cy, cx - 25, cy);
        linea(cx + 10, cy, cx + 25, cy);

        refresca();

        t = tecla();
    }

    cierra();
    closeSerial(&esp32);
    return 0;
}
