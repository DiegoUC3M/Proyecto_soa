#ifndef PROYECTO_SIM_SOA_HPP
#define PROYECTO_SIM_SOA_HPP
using namespace std;


const double G = 6.674*10E-12;



struct object {
    //DESPUÉS CON MALLOC, RESERVAREMOS LA MEMORIA QUE NECESITAMOS (NUM_OBJETOS)
    double* position_x;
    double* position_y;
    double* position_z;
    double* speed_x;
    double* speed_y;
    double* speed_z;
    double* masa;

};

//LOS STRUCT VEC SON VECTORES, CON SUS RESPECTIVAS POSICIONES X,Y,Z
struct vec {
    double* x;
    double* y;
    double* z;
};

//FUNCIONES UTILIZADAS EN SIM-SOA.CPP:*********************/
void comprobacionColisiones (int& num_objetos, object objetos);



#endif //PROYECTO_SIM_SOA_HPP
