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
#define GRAVEDAD 1

#define AJUSTEX 2780
#define AJUSTEY 2820
#define VELXMAX 20
#define VELYMAX 20
#define VALMAX 4095


int ajusteX(int x) {
    x -= AJUSTEX;
    if(x > 0) {
        x = (int)((double)x * ((double)VELXMAX / (double)(VALMAX - AJUSTEX)));
    } else {
        x = (int)((double)x * ((double)VELXMAX / (double)(AJUSTEX)));
    }
    return x;
}

int ajusteY(int x) {
    x -= AJUSTEY;
    if(x > 0) {
        x = (int)((double)x * ((double)VELYMAX / (double)(VALMAX - AJUSTEY)));
    } else {
        x = (int)((double)x * ((double)VELYMAX / (double)(AJUSTEY)));
    }
    return x;
}

int main()
{
    int t;
    float x = 0, y = 0;
    bool fi = false, fd = false, fs = false, clic = false;
    int cx = 100, cy = 100, acel = 0, cancho = 100, calto = 100;
    MWImage ajolote = creaImagenYMascaraBMP("ajoloteN.bmp", "ajoloteM.bmp");
    MWImage fondo = creaImagenBMP("fondo.bmp");
    SerialPort esp32 = initSerialPort("COM4", B115200);
    char cmd[MAX_DATA_LENGTH];
    

    if(!isConnected(&esp32)) {
        cierra();
        return 1;
    }
    
    titulo("Progra Avanzada Energia");
    ventana(800, 600);
    color_fondo(BLANCO);

    t = tecla();
    while (t != ESCAPE)
    {
        
        sprintf(cmd, "dame_datos\n");
        writeSerialPort(cmd, strlen(cmd), &esp32);
        Sleep(1);
        if(readSerialPort(cmd, MAX_DATA_LENGTH, &esp32)) {
            int jx = atoi(cmd);
            int jy = atoi(&cmd[5]);
            int jb = atoi(&cmd[10]);
            borra();
            refresca();
            sprintf(cmd, "%i-%i-%i", ajusteX(jx), ajusteY(jy), jb);
            color(NEGRO);
            texto(10,100, cmd);
          
             // cx += ajusteX(jx);
            //cy += ajusteY(jy);
            if(jb) clic = false;
            else clic = true;
            if(clic) {
                sprintf(cmd, "vibra\n");
                writeSerialPort(cmd, strlen(cmd), &esp32);
                Sleep(1);
                sprintf(cmd, "que_chille\n");
                writeSerialPort(cmd, strlen(cmd), &esp32);
            } else {
                sprintf(cmd, "!vibra\n");
                writeSerialPort(cmd, strlen(cmd), &esp32);
            }
            
        }

        
/*
        acel += GRAVEDAD;
        cy += acel;
        if (cy + ajolote.alto > valto())
        {
            cy = valto() - ajolote.alto;
            acel = 0;
        }
        t = teclaDown();
        //raton(&x, &y);

        if (t == IZQUIERDA)
        {
            fi = true;
        }
        if (t == DERECHA)
        {
            fd = true;
        }
        //if (t == ESPACIO)
        if(clic)
        {
            acel = -20;
        }
        if( t == RETURN) {
            fs = !fs;
            fullscreen(fs);
        }

        t = teclaUp();
        if (t == IZQUIERDA)
            fi = false;
        if (t == DERECHA)
            fd = false;

        if (fi)
            cx -= 10;
        if (fd)
            cx += 10;

        if (cx + ajolote.ancho > vancho())
            cx = vancho() - ajolote.ancho;
        if (cx < 0)
            cx = 0;

        ajolote.pos_x = cx;
        ajolote.pos_y = cy;

        borra();
        color(BLANCO);
        muestraImagen(&fondo);
        
        


        
        textoExt(10,10, "Precione ESC para salir", 22, false, false, false, "Arial");
        //color(ROJO);
        //rectangulo_lleno(cx, cy, cx + cancho, cy + calto);

        muestraImagen(&ajolote);

        color(BLANCO);
        circulo(x, y, 30);
        circulo(x, y, 20);
        linea(x, y - 10, x, y - 25);
        linea(x, y + 10, x, y + 25);
        linea(x - 10, y, x - 25, y);
        linea(x + 10, y, x + 25, y);

        //if (raton_boton_izq())
        if(clic)
        {
            // texto(x+30,y, "Jedchot");
            //color(MAGENTA);
            //textoExt(x + 30, y - 30, "Jedchot", 30, false, true, false, "Comic Sans MS");
        }
        */
        

        refresca();
        //Sleep(1);
    }
    cierra();
    closeSerial(&esp32);
    return 0;
}