/* Autor : Santiago Ventura Gomez . 2017
Curso Experto Universitario en Robótica , Programación e Impresión 3D
Asignatura : Robótica
Profesor : Alberto Valero
Actividad 4 : Robot CorreCaminos I
*/

/*
Objetivo : 

Se debe construir un robot que recorra un circuito delimitado por dos líneas negras

Acerca del circuito:

Debe ser un circuito cerrado, como los de Fórmula 1, donde una vez dentro no haya salida.
El circuito debe tener al menos 1 metro de longitud
El circuito debe estar delimitado por líneas negras.
Las líneas negras pueden ser pintadas, realizadas con cinta aislante negra, etc.

Acerca del robot:

La construcción del robot es totalmente libre
El robot debe recorrer el circuito sin salirse, puede pisar las líneas negras pero no sobrepasarlas.

*/

/* Para definir los lados Izquierda y Derecha del Robot se hace visto desde la parte trasera y mirando en la dirección
    de avance del mismo */
    
/* Elementos de construcción del Robot :

    * Para las piezas del Chasis , me he descargado los archivos STL del Printbot Evolution , y he utilizado la base de 3 mm
      Todas las piezas están impresas en una impresora Hephestos 2 
    * Movimiento de las Ruedas : Robot diferencial de tres puntos de apoyo , que cuenta con dos servomotores de movimiento continuo para las ruedas
      y un apoyo fijo trasero
    * La detección de los límites de la pista se hace mediante 2 sensores IR ,usando las dos interrupciones con las que cuenta
       la placa BT Zum en los pines 2 y 3 , de manera que :
    
          Color Blanco : Lectura IR = 1
          Color Negro  : Lectura IR = 0
    * Boton pulsador que nos permitirá controlar el momento de inicio del movimiento de los motores de las ruedas y definicion de interrupciones
 */
 
 /* Asignación de pines :
 
   * Sensor IRizqda     :  Pin 2
   * Sensor IRdrcha     :  Pin 3
   * Servo RuedaDrcha   :  Pin 4
   * Servo RuedaIzqda   :  Pin 5
   * PulsadorInicio     :  Pin 8
*/
/*  Valores de velocidades y giros :

   * Avance Robot :
       Rueda Derecha : 180º
       Rueda Izquierda : 0º
  
   * Giro Derecha Robot :
       Rueda Derecha : 90º
       Rueda Izquierda : 0º
   
   * Giro Izquierda Robot :
       Rueda Derecha : 180º
       Rueda Izquierda : 90º
  */

/********************************* DEFINICION DE LIBRERIAS Y VARIABLES GLOBALES **********************************************/

#include <Servo.h>                            // Libreria Arduino que controla funciones especiales de los servomotores

Servo RuedaIzqda;                             // Declaracion de variable tipo Servo para el motor que mueve la rueda Izquierda
Servo RuedaDrcha;                             // Declaracion de variable tipo Servo para el motor que mueve la rueda Derecha


int PulsadorInicio = 8;                       // Declaracion del pin asignado al boton que iniciará el programa


/********************************* DEFINICION DE FUNCIONES LIGADAS A LAS INTERRUPCIONES **************************************/


void giroDrcha(){                              // Funcion de Interrupcion lanzada al pasar el sensor IR izquierdo de 1 a 0 ( blanco a negro )
        RuedaDrcha.write(90);
        RuedaIzqda.write(0);                   // Los servos provocan un giro a la derecha del robot
        detachInterrupt(0);                    // Desconectamos la interrupcion definida en el pin 2 según FALLING ( paso de 1 a 0 )....
        attachInterrupt(0,avanzar,RISING);     /* ... y la sustituimos por una nueva interrupcion provocada por el mismo 
                                                  sensor IR izquierdo pero segun RISING al pasar de 0 a 1 , para que ahora sea capaz 
                                                  de detectar la vuelta a la pista blanca y provocar una nueva reaccion mediante la funcion "avanzar" */
}

void giroIzqda(){                              // Funcion de Interrupcion lanzada al pasar el sensor IR derecho de 1 a 0 ( blanco a negro )
        RuedaDrcha.write(180);
        RuedaIzqda.write(90);                  // Los servos provocan un giro a la izquierda del robot
        detachInterrupt(1);                    // Desconectamos la interrupcion definida en el pin 3 según FALLING ( paso de 1 a 0 )....
        attachInterrupt(1,avanzar,RISING);     /* ... y la sustituimos por una nueva interrupcion provocada por el mismo 
                                                  sensor IR derecho pero segun RISING al pasar de 0 a 1 , para que ahora sea capaz 
                                                  de detectar la vuelta a la pista blanca y provocar una nueva reaccion mediante la funcion "avanzar" */
}
void avanzar(){                                // Funcion de Interrupcion lanzada por cualquiera de los dos sensores IR al pasar de 0 a 1 ( negro a blanco )
        RuedaDrcha.write(180);
        RuedaIzqda.write(0);                   // Los servos provocan el avance del robot
        detachInterrupt(0);                       // Desconectamos la interrupcion definida en el pin 2 según RISING ( paso de 0 a 1 )....
        attachInterrupt(0,giroDrcha,FALLING);     /*... Y recuperamos la interrupcion inicial sensible al flanco descendente para detectar cuando
                                                    el sensor IR izquierdo toque la linea negra ( pase de 1 a 0 ) */
        detachInterrupt(1);                       // Desconectamos la interrupcion definida en el pin 3 según RISING ( paso de 0 a 1 )....
        attachInterrupt(1,giroIzqda,FALLING);     /*... Y recuperamos la interrupcion inicial sensible al flanco descendente para detectar cuando
                                                    el sensor IR derecho toque la linea negra ( pase de 1 a 0 ) */    
}

void setup() {

  
  RuedaDrcha.attach(4);                       // Declaracion del pin al que está conectado el servomotor de la rueda derecha
  RuedaIzqda.attach(5);                       // Declaracion del pin al que está conectado el servomotor de la rueda izquierda
    
  pinMode(PulsadorInicio,INPUT);              // Declaramos el pin del pulsador como entrada
  
  RuedaDrcha.write(90);
  RuedaIzqda.write(90);                       // Inicialmente las dos ruedas paradas
  
  while(digitalRead(PulsadorInicio) == 0){    // Bucle que espera a que se pulse el boton PulsadorInicio
           delay(10);
  }
  while(digitalRead(PulsadorInicio) == 1){    // Bucle que espera a que soltemos el boton PulsadorInicio
           delay(10); 
  } 
  
  /* Definicion inicial de interrupciones :
      Robot dentro de la pista blanca , con los dos sensores sobre color blanco , 
      interrupciones sensibles al flanco DESCENDENTE */
      
  attachInterrupt(0,giroDrcha,FALLING);      // Interrupcion provocada por el sensor IR izquierdo al pasar de 1 a 0
  attachInterrupt(1,giroIzqda,FALLING);      // Interrupcion provocada por el sensor IR derecho  al pasar de 1 a 0
  
  /* Una vez pulsado el boton el robot comienza el circuito AVANZANDO RECTO y se mantiene así salvo que exista
      una interrupcion desde uno de los dos sensores IR */
  
  RuedaDrcha.write(180);
  RuedaIzqda.write(0);                       

}



void loop() {

  /****************************************                  INICIO PROGRAMA PRINCIPAL              ********************************/
  
  
  
  /****************************************                       FIN DEL PROGRAMA                   ********************************/

}
