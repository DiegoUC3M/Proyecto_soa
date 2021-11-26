//
// Created by diego on 22/10/21.
//

#ifndef PROYECTO_SOA_CALCULATIONS_HPP
#define PROYECTO_SOA_CALCULATIONS_HPP
#include <fstream>

extern ofstream forcesFile;

vec calcForces(int num_objetos, object objetos, vec forces, int iteration);

vec calcAccelerations(int num_objetos,object objetos, vec forces, vec acelerations);

void calcVelocities (int num_objetos, object objetos, vec accelerations, double incr_tiempo);

void calcPositions (int num_objetos, object objetos, double incr_tiempo, double lado);


#endif //PROYECTO_SOA_CALCULATIONS_HPP
