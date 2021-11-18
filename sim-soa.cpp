#include <iostream>
#include <random>
#include <fstream>
#include <omp.h>
#include <iomanip> /*Librería para ajustar la precision*/
#include "sim-soa.hpp"
#include "calculations.hpp"
using namespace std;





// HACEMOS PASO POR REFERENCIA EN EL MÉTODO COMPROBACIONCOLISIONES PARA QUE CAMBIE EL VALOR DE NUM_OBJETOS TAMBIÉN EN EL MAIN
void comprobacionColisiones (int& num_objetos, object objetos) {

    int num_eliminados; //Para ir viendo el numero de eliminados
    double norma;

    //COMPROBAMOS EL OBJETO ACTUAL CON EL RESTO DE OBJETOS. PARA ELLO SE QUEDA EN UNA POSICION J Y COMPRUEBA CON EL RESTO DE OBJETOS EN EL SIGUIENTE
    //FOR ITERANDO EN LA POSICION K
    for (int j = 0 ; j < num_objetos; j++){

        num_eliminados=0;

        for (int k = j+1; k < num_objetos+1; k++) {

            //ESTO SE HACE PARA QUE EL "COMPROBADOR DE COLISIONES" PRUEBE LA POSICIÓN NUEVA A LA QUE SE HAN MOVIDO LOS ELEMENTOS QUE NO
            // ERAN "NULOS" (masa == -1)
            if(j!=k-1) {
                norma = std::sqrt(((objetos.position_x[k-1] - objetos.position_x[j]) * (objetos.position_x[k-1] - objetos.position_x[j])) + ((objetos.position_y[k-1] - objetos.position_y[j]) * (objetos.position_y[k-1] - objetos.position_y[j])) + ((objetos.position_z[k-1] - objetos.position_z[j]) * (objetos.position_z[k-1] - objetos.position_z[j])));

                if (norma<1) {
                    k=k-1;
                }
            }
            if (objetos.masa[k]==0) {
                break;
            }

            norma = std::sqrt(((objetos.position_x[k] - objetos.position_x[j]) * (objetos.position_x[k] - objetos.position_x[j])) + ((objetos.position_y[k] - objetos.position_y[j]) * (objetos.position_y[k] - objetos.position_y[j])) + ((objetos.position_z[k] - objetos.position_z[j]) * (objetos.position_z[k] - objetos.position_z[j])));

            if (norma < 1) {//si su distancia es menor que la unidad

                //ESCRIBIMOS LAS COLISIONES EN EL TXT DE POSITIONS
                positionsFile << "Collapsing objects " << j << " and " << k+num_eliminados<< endl;
                positionsFile << "Body " << j << ": " << objetos.position_x[j] << " " << objetos.position_y[j] << " " << objetos.position_z[j] << " " << objetos.speed_x[j] << " " << objetos.speed_y[j] << " " << objetos.speed_z[j] << " " << objetos.masa[j] << endl;
                positionsFile << "Body " << k+num_eliminados << ": " << objetos.position_x[k] << " " << objetos.position_y[k] << " " << objetos.position_z[k] << " " << objetos.speed_x[k] << " " << objetos.speed_y[k] << " " << objetos.speed_z[k] << " " << objetos.masa[k] << endl;

                //SUMAMOS LAS MASAS Y VELOCIDADES DE LOS OBJETOS J Y K Y ELIMINAMOS EL SEGUNDO OBJETO (ASIGNANDO VALORES -1)
                objetos.masa[j] = objetos.masa[j] + objetos.masa[k];
                objetos.speed_x[j] = objetos.speed_x[j] + objetos.speed_x[k];
                objetos.speed_y[j] = objetos.speed_y[j] + objetos.speed_y[k];
                objetos.speed_z[j] = objetos.speed_z[j] + objetos.speed_z[k];

                #pragma omp parallel
                {
                    objetos.masa[k] = -1;
                    objetos.speed_x[k] = -1;
                    objetos.speed_y[k] = -1;
                    objetos.speed_z[k] = -1;
                    objetos.position_x[k] = -1;
                    objetos.position_y[k] = -1;
                    objetos.position_z[k] = -1;
                }

                positionsFile << "Body " << k+num_eliminados << " removed" <<endl;


                //ELIMINAMOS LOS OBJETOS MOVIÉNDOLOS AL FINAL DEL ARRAY, PARA QUE AL PRINCIPIO SOLO ESTÉN LOS NO COLISIONADOS
                //PARA ELLO, COMPROBAMOS EL OBJETO ELIMINADO CON EL SIGUIENTE. SI EL SIGUIENTE TIENE MASA DISTINTA A -1 INTERCAMBIAN SUS POSICIONES.
                //ASI HASTA QUE LLEGA A LA ULTIMA POSICION, QUE SERÁ CUANDO EL SIGUIENTE OBJETO TAMBIÉN TENGA MASA -1 O HAYA LLEGADO A NUM_OBJETOS

                for (int a = k; a < num_objetos; a++) {
                    if (objetos.masa[a] == -1) {
                        if (objetos.masa[a + 1] != -1) {

                            objetos.masa[a] = objetos.masa[a + 1];
                            objetos.speed_x[a] = objetos.speed_x[a + 1];
                            objetos.speed_y[a] = objetos.speed_y[a + 1];
                            objetos.speed_z[a] = objetos.speed_z[a + 1];
                            objetos.position_x[a] = objetos.position_x[a + 1];
                            objetos.position_y[a] = objetos.position_y[a + 1];
                            objetos.position_z[a] = objetos.position_z[a + 1];
                            objetos.masa[a + 1] = -1;
                            objetos.speed_x[a + 1] = -1;
                            objetos.speed_y[a + 1] = -1;
                            objetos.speed_z[a + 1] = -1;
                            objetos.position_x[a + 1] = -1;
                            objetos.position_y[a + 1] = -1;
                            objetos.position_z[a + 1] = -1;

                        }
                    }



                }
                num_eliminados++;

                positionsFile << "Body " << j << " after collapse" <<endl;
                positionsFile << "Body " << j << ": " << objetos.position_x[j] << " " << objetos.position_y[j] << " " << objetos.position_z[j] << " " << objetos.speed_x[j] << " " << objetos.speed_y[j] << " " << objetos.speed_z[j] << " " << objetos.masa[j] << endl << endl;


                num_objetos = num_objetos - 1;

            }

        }

    }

}



int main(int argc, char *argv[]) {


    mt19937_64 gen64_soa;


    //COMPROBAMOS EL NUMERO DE PARÁMETROS
    if (argc != 6) {
        cerr << "El número de parámetros no es correcto" << "\nArgumentos:\n Num_objects:" << argv[1] << "\nNum_iterations:" << argv[2] << "\nRandom_seed:" << argv[3] << "\nSize_enclosure:" << argv[4] << "\nTime_step:" << argv[5];
        return -1;
    }

    int num_objetos = stoi(argv[1]);
    int num_iteraciones = stoi(argv[2]);
    int semilla = stoi(argv[3]);
    double lado = stod(argv[4]);
    double incr_tiempo = stod(argv[5]);


    //COMPROBAMOS QUE EL NUMERO DE PARÁMETROS ES CORRECTO
    if (num_objetos <= 0 || num_iteraciones <= 0 || semilla <= 0 || lado <= 0 || incr_tiempo <= 0) {
        cerr << "El valor de alguno de los parámetros no es correcto\n" << "Argumentos:\nNum_objects:" << num_objetos << "\nNum_iterations:" << num_iteraciones << "\nRandom_seed:" << semilla << "\nSize_enclosure:" << lado << "\nTime_step:" << incr_tiempo;
        return -2;
    }

    cout << "Creating simulation:\n" << "  num_objects: " << num_objetos << "\n  num_iterations: " << num_iteraciones << "\n  random_seed: " << semilla << "\n  size_enclosure: " << lado << "\n  time_step: " << incr_tiempo << "\n";



    //CREAMOS OBJETO E INICIALIZAMOS CON MALLOC LOS ARRAYS CON SIZE = NUM_OBJETOS
    object objetos;
    objetos.position_x=(double *) malloc (num_objetos * sizeof(double));
    objetos.position_y=(double *) malloc (num_objetos * sizeof(double));
    objetos.position_z=(double *) malloc (num_objetos * sizeof(double));
    objetos.speed_x=(double *) malloc (num_objetos * sizeof(double));
    objetos.speed_y=(double *) malloc (num_objetos * sizeof(double));
    objetos.speed_z=(double *) malloc (num_objetos * sizeof(double));
    objetos.masa =(double *) malloc (num_objetos * sizeof(double));


    vec fuerza;
    fuerza.x = (double *) malloc (num_objetos * sizeof(double));
    fuerza.y = (double *) malloc (num_objetos * sizeof(double));
    fuerza.z = (double *) malloc (num_objetos * sizeof(double));


    vec aceleracion;
    aceleracion.x = (double *) malloc (num_objetos * sizeof(double));
    aceleracion.y = (double *) malloc (num_objetos * sizeof(double));
    aceleracion.z = (double *) malloc (num_objetos * sizeof(double));



    //INICIALIZACIÓN PARA LOS DATOS ALEATORIOS
    gen64_soa.seed(semilla);
    uniform_real_distribution<> distUniforme(0.0, lado);
    normal_distribution <>distNormal{10E20, 10E14};

    //INICIALIZO EL ARCHIVO: FILE_INIT
    ofstream file_init;
    file_init.open("init_config.txt");
    file_init << fixed << setprecision(3) << lado << " " << incr_tiempo << " " << num_objetos;





    //INICIALIZAMOS TODOS LOS OBJETOS
    for(int i=0; i<num_objetos; i++){
        objetos.position_x[i] = distUniforme(gen64_soa);
        objetos.position_y[i] = distUniforme(gen64_soa);
        objetos.position_z[i] = distUniforme(gen64_soa);
        #pragma omp parallel
        {
            objetos.speed_x[i] = 0;
            objetos.speed_y[i] = 0;
            objetos.speed_z[i] = 0;
        }
        objetos.masa[i] = distNormal(gen64_soa);
        file_init << fixed << setprecision(3) << "\n" << objetos.position_x[i] << " " << objetos.position_y[i] << " " << objetos.position_z[i] << " " << objetos.speed_x[i] << " " << objetos.speed_y[i] << " " << objetos.speed_z[i] << " " << objetos.masa[i];
    }
    file_init.close();//cierro el archivo





    //COMPROBACIÓN DE COLISIONES INICIAL
    comprobacionColisiones(num_objetos,objetos);





    //ABRIMOS TODOS LOS ARCHIVOS TXT PARA PROCEDER A MODIFICARLOS
    forcesFile.open("forces.txt");
    accelarationFile.open("accelaration.txt");
    velocitiesFile.open("velocities.txt");
    positionsFile.open("positions.txt");





    //********CALCULAMOS LAS FUERZAS, ACELERACIONES, VELOCIDADES Y POSICIONES DE TODOS LOS CUERPOS***********

    for (int i = 0; i < num_iteraciones; i++) {
        //ES NECESARIO INICIALIZAR LAS FUERZAS A CERO EN CADA NUEVA ITERACION, PARA QUE NO SE SUMEN LAS FUERZAS DE LAS ITERACIONES ANTERIORES EN NUEVAS ITERACIONES
#pragma omp parallel for
        for (int j = 0; j < num_objetos; j++) {
            fuerza.x[j] = 0;
            fuerza.y[j] = 0;
            fuerza.z[j] = 0;
        }

        //REALIZAMOS LOS CÁLCULOS Y LA COMPROBACIÓN DE COLISIONES LLAMANDO A LAS FUNCIONES
        fuerza = calcForces(num_objetos,objetos, fuerza, i);
        aceleracion = calcAccelerations(num_objetos,objetos,fuerza,aceleracion, i);
        calcVelocities(num_objetos,objetos,aceleracion,i, incr_tiempo);
        calcPositions(num_objetos,objetos,i,incr_tiempo,lado);

        comprobacionColisiones(num_objetos,objetos);


    }




    //CERRAMOS TODOS LOS ARCHIVOS TXT
    forcesFile.close();
    accelarationFile.close();
    velocitiesFile.close();
    positionsFile.close();


    //ESCRIBIMOS EL ARCHIVO FILE_FINAL, QUE TIENE LAS POSICIONES, VELOCIDADES Y MASAS DE LOS OBJETOS QUE QUEDAN AL FINAL AL NO HABER COLISIONADO

    ofstream file_final;
    file_final.open("final_config.txt");
    file_final << fixed << setprecision(3) << lado << " " << incr_tiempo << " " << num_objetos;
    for(int j=0; j<num_objetos; j++){
        file_final << fixed << setprecision(3) << "\n" << objetos.position_x[j] << " " << objetos.position_y[j] << " " << objetos.position_z[j] << " " << objetos.speed_x[j] << " " << objetos.speed_y[j] << " " << objetos.speed_z[j] << " " << objetos.masa[j];
    }
    file_final.close();//cierro el archivo


    //UNA VEZ TERMINADO EL PROGRAMA LIBERAMOS LA MEMORIA ASIGNADA

    free(objetos.position_x);
    free(objetos.position_y);
    free(objetos.position_z);
    free(objetos.speed_x);
    free(objetos.speed_y);
    free(objetos.speed_z);
    free(objetos.masa);
    free(fuerza.x);
    free(fuerza.y);
    free(fuerza.z);
    free(aceleracion.x);
    free(aceleracion.y);
    free(aceleracion.z);

    return 0;
}
