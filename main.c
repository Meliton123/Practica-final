/**
 * @file main.c
 * @authors Castelan Meliton Uriel Agustín, Duarte Arellanes Sebastian, Ojeda Flores Sebastian 
 * @brief Duck hunt pero con MiniWin
 * @version 0.1
 * @date 2024-06-13
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "miniwin.h"
#include "serial.h"
#include <stdlib.h> 
#include <time.h>   

#define GRAVEDAD 1

#define AJUSTEX 2780
#define AJUSTEY 2820
#define VELXMAX 20
#define VELYMAX 20
#define VALMAX 4095

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CIRCLE_RADIUS 30
#define PATO_WIDTH 50 
#define PATO_HEIGHT 50 

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


bool isMiraSobrePato(int cx, int cy, int patoX, int patoY) {
    
    return (cx >= patoX && cx <= patoX + PATO_WIDTH) && (cy >= patoY && cy <= patoY + PATO_HEIGHT);
}


int generarPosYAleatoria() {
    return rand() % 101 + 100; 
}

int main()
{
    srand(time(NULL)); 

    int t, puntuacion = 0;
    float x = 0, y = 0;
    bool clic = false;
    int cx = 100, cy = 100, acel = 1;
    MWImage pato = creaImagenYMascaraBMP("patoN.bmp", "patoM.bmp");
    MWImage fondo = creaImagenBMP("fondo.bmp");
    SerialPort esp32 = initSerialPort("COM4", B115200);
    char cmd[MAX_DATA_LENGTH];


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
            
            int jx = atoi(cmd);
            int jy = atoi(&cmd[5]);
            int jb = atoi(&cmd[10]);

            
            cx += ajusteX(jx);
            cy += ajusteY(jy);

            
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

        
            if (clic && isMiraSobrePato(cx, cy, pato.pos_x, pato.pos_y)) {
           
                puntuacion += 100;
                pato.pos_x = -15;
                pato.pos_y = generarPosYAleatoria();

                sprintf(cmd, "puntuacion %d\n", puntuacion);
                writeSerialPort(cmd, strlen(cmd), &esp32);
            }
        }

        pato.pos_x += acel;

       
        if (pato.pos_x > WINDOW_WIDTH) {
            pato.pos_x = -15;
            pato.pos_y = generarPosYAleatoria();
        }

        borra();
        color(BLANCO);
        muestraImagen(&fondo);

   
        textoExt(10, 10, "Presione ESC para salir", 22, false, false, false, "Arial");

     
        char textoPuntuacion[50];
        sprintf(textoPuntuacion, "Puntuacion: %d", puntuacion);
        textoExt(10, 40, textoPuntuacion, 22, false, false, false, "Arial");

     
        muestraImagen(&pato);

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
