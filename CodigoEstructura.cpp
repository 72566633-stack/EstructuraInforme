/*
estructura de datos
*/
#include <iostream>
using namespace std;

struct Paciente {
    string nombre;
    string dni;
    int edad;
    string motivo;
    int prioridad; // 1=Urgente,2=Normal,3=Baja
    string estado; // "En espera", "En atención", "Atendido"
    bool tieneReceta;
    string rutaReceta;
    Paciente() : edad(0), prioridad(2), estado("En espera"), tieneReceta(false) {}
};
