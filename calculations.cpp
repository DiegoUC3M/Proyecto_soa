//
// Created by diego on 22/10/21.
//
#include <iostream>
#include <cmath>
#include <iomanip>
#include <omp.h>
#include <vector>
#include "sim-soa.hpp"
#include "calculations.hpp"


//DECLARAMOS LAS VARIABLES FILE
ofstream forcesFile;




//HACEMOS EL CALCULO DE FUERZAS
vec calcForces(int num_objetos, object objetos, vec forces, int iteration) {

    forcesFile << "iteración " << iteration << ":" << endl; //Escribimos en el txt en que iteracion esta antes de entrar en el bucle

    //EN ESTAS VARIABLES SE ALMACENAN LOS CALCULOS DE LA FUERZA GRAVITATORIA ENTRE DOS OBJETOS. LOS INICIALIZAMOS
    double fx = 0;
    double fy = 0;
    double fz = 0;

    //HACE EL CALCULO DE UN SOLO OBJETO CON EL DEL RESTO DE OBJETOS. POR ELLO SE QUEDA EN UN OBJETO CON EL PRIMER BUCLE (POSICION J) E ITERA
    //SOBRE EL RESTO DE OBJETOS (POSICION K)
    for (int j = 0; j < num_objetos; j++) {

        for (int k = j + 1; k < num_objetos; k++) {

            //FUNCIÓN FUERZA
            double norma;
            norma = std::sqrt(((objetos.position_x[k] - objetos.position_x[j]) *
                               (objetos.position_x[k] - objetos.position_x[j])) +
                              ((objetos.position_y[k] - objetos.position_y[j]) *
                               (objetos.position_y[k] - objetos.position_y[j])) +
                              ((objetos.position_z[k] - objetos.position_z[j]) *
                               (objetos.position_z[k] - objetos.position_z[j])));

            fx = (((G * objetos.masa[k]) * objetos.masa[j]) * (objetos.position_x[k] - objetos.position_x[j])) / (norma * norma * norma);//fuerza eje x
            fy = (((G * objetos.masa[k]) * objetos.masa[j]) * (objetos.position_y[k] - objetos.position_y[j])) / (norma * norma * norma);//fuerza eje y
            fz = (((G * objetos.masa[k]) * objetos.masa[j]) * (objetos.position_z[k] - objetos.position_z[j])) / (norma * norma * norma);//fuerza eje z

            //COMO SOBRE UN OBJETO INFLUYE LA FUERZA DEL RESTO DE OBJETOS, VAMOS SUMANDO CONTINUAMENTE LAS FUERZAS QUE SE EJERCEN SOBRE EL OBJETO QUE
            //ESTA SIENDO ITERADO (J), POR ESO SE SUMA EL ANTIGUO VALOR DE LA VARIABLE.
            forces.x[j] = forces.x[j] + fx;
            forces.y[j] = forces.y[j] + fy;
            forces.z[j] = forces.z[j] + fz;
            forces.x[k] = forces.x[k] - fx;
            forces.y[k] = forces.y[k] - fy;
            forces.z[k] = forces.z[k] - fz;

        }


        forcesFile << "fuerza[" << j << "]: " << forces.x[j] << " "  << forces.y[j] << " "  << forces.z[j] <<endl;

    }

    return forces;
}






//HACEMOS EL CALCULO DE LAS ACELERACIONES
vec calcAccelerations(int num_objetos,object objetos, vec forces, vec accelerations, int iteration) {

    //CALCULAMOS TODAS LAS ACELERACION ITERANDO SOBRE TODOS LOS OBJETOS


#pragma omp parallel for
    for (int j = 0; j < num_objetos; j++) {
        accelerations.x[j] = forces.x[j] / objetos.masa[j];//aceleración eje x
        accelerations.y[j] = forces.y[j] / objetos.masa[j];//aceleración eje y
        accelerations.z[j] = forces.z[j] / objetos.masa[j];//aceleración eje z


    }

    return accelerations;
}






//HACEMOS EL CALCULO DE LAS VELOCIDADES
void calcVelocities (int num_objetos, object objetos, vec accelerations, int iteration, double incr_tiempo) {



    //CALCULAMOS TODAS LAS VELOCIDADES ITERANDO SOBRE TODOS LOS OBJETOS


#pragma omp parallel for
    for (int j = 0; j < num_objetos; j++) {

        objetos.speed_x[j] = objetos.speed_x[j] + (accelerations.x[j] * (incr_tiempo));
        objetos.speed_y[j] = objetos.speed_y[j] + (accelerations.y[j] * (incr_tiempo));
        objetos.speed_z[j] = objetos.speed_z[j] + (accelerations.z[j] * (incr_tiempo));


    }

}




//HACEMOS EL CALCULO DE LAS POSICIONES
void calcPositions (int num_objetos, object objetos, int iteration, double incr_tiempo, double lado) {



#pragma omp parallel for
    for (int j = 0; j < num_objetos; j++) {
        objetos.position_x[j] = objetos.position_x[j] + (objetos.speed_x[j] * (incr_tiempo));
        objetos.position_y[j] = objetos.position_y[j] + (objetos.speed_y[j] * (incr_tiempo));
        objetos.position_z[j] = objetos.position_z[j] + (objetos.speed_z[j] * (incr_tiempo));



        //SI SE SALE UN OBJETO POR EL EJE X
        if (objetos.position_x[j] <= 0) {
            objetos.speed_x[j] = -objetos.speed_x[j]; //CAMBIAMOS LA DIRECCION DEL VECTOR VELOCIDAD
            objetos.position_x[j] = 0;
        } else if (objetos.position_x[j] >= lado) {
            objetos.speed_x[j] = -objetos.speed_x[j];
            objetos.position_x[j] = lado;
        }


        //SI SE SALE UN OBJETO POR EL EJE Y
        if (objetos.position_y[j] <= 0) {
            objetos.speed_y[j] = -objetos.speed_y[j];
            objetos.position_y[j] = 0;
        } else if (objetos.position_y[j] >= lado) {
            objetos.speed_y[j] = -objetos.speed_y[j];
            objetos.position_y[j] = lado;
        }


        //SI SE SALE UN OBJETO POR EL EJE Z
        if (objetos.position_z[j] <= 0) {
            objetos.speed_z[j] = -objetos.speed_z[j];
            objetos.position_z[j] = 0;
        } else if (objetos.position_z[j] >= lado) {
            objetos.speed_z[j] = -objetos.speed_z[j];
            objetos.position_z[j] = lado;
        }


    }

}
