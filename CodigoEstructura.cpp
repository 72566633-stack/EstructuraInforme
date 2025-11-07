#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <direct.h>  // Para crear directorios en Windows
using namespace std;

// ============================================
// PARTE 1: TDA (TIPO DE DATO ABSTRACTO)
// ============================================
// Un TDA es una estructura que encapsula datos y operaciones

// Registro para almacenar datos de un paciente
struct Paciente {
    string nombre;
    string dni;
    int edad;
    string motivo;
    string estado; // "En espera", "En atencion", "Atendido"
    int prioridad; // 1=urgente, 2=normal, 3=baja
    string rutaReceta; // Ruta de la imagen de receta medica
    bool tieneReceta; // Indica si tiene receta asociada
};

// ============================================
// PARTE 2: NODO PARA LISTA ENLAZADA
// ============================================
// Los punteros permiten crear estructuras dinamicas
struct Nodo {
    Paciente datos;      // Datos del paciente
    Nodo* siguiente;     // Puntero al siguiente nodo
};

// ============================================
// PARTE 3: ARRAYS UNIDIMENSIONALES
// ============================================
// Constantes para arrays
const int MAX_HISTORIAL = 50;
const int MAX_MEDICAMENTOS = 20;

// Arrays para almacenar datos temporales
string historialAtenciones[MAX_HISTORIAL];
int contadorHistorial = 0;

string listaMedicamentos[MAX_MEDICAMENTOS] = {
    "Paracetamol", "Ibuprofeno", "Amoxicilina", "Omeprazol", 
    "Loratadina", "Salbutamol", "Metformina", "Enalapril",
    "Atorvastatina", "Losartan"
};
int cantidadMedicamentos = 10;

// ============================================
// PARTE 4: ARRAYS BIDIMENSIONALES (MATRICES)
// ============================================
const int MAX_DIAS = 7;
const int MAX_HORAS = 10;

// Matriz para horarios de atencion (filas=dias, columnas=horas)
int horariosDisponibles[MAX_DIAS][MAX_HORAS];
string diasSemana[MAX_DIAS] = {"Lun", "Mar", "Mie", "Jue", "Vie", "Sab", "Dom"};

// Inicializar matriz de horarios
void inicializarHorarios() {
    for(int i = 0; i < MAX_DIAS; i++) {
        for(int j = 0; j < MAX_HORAS; j++) {
            horariosDisponibles[i][j] = 5; // 5 citas disponibles por hora
        }
    }
}

// Mostrar matriz de horarios
void mostrarHorarios() {
    cout << "\n=== HORARIOS DISPONIBLES (8:00-17:00) ===\n";
    cout << "Dia\\Hora ";
    for(int j = 0; j < MAX_HORAS; j++) {
        cout << (8+j) << "h ";
    }
    cout << endl;
    
    for(int i = 0; i < MAX_DIAS; i++) {
        cout << diasSemana[i] << "     ";
        for(int j = 0; j < MAX_HORAS; j++) {
            cout << " " << horariosDisponibles[i][j] << "  ";
        }
        cout << endl;
    }
}

// ============================================
// FUNCIONES PARA MANEJO DE RECETAS MEDICAS
// ============================================

// Crear directorio para almacenar recetas si no existe
void crearDirectorioRecetas() {
    _mkdir("Recetas_Medicas"); // Crea carpeta en Windows
}

// Copiar archivo de imagen a la carpeta del sistema
bool copiarReceta(string rutaOrigen, string dni) {
    // Abrir archivo origen en modo binario
    ifstream archivoOrigen(rutaOrigen.c_str(), ios::binary);
    
    if(!archivoOrigen) {
        cout << "\nError: No se pudo abrir el archivo de imagen.\n";
        return false;
    }
    
    // Crear nombre de archivo destino
    string nombreDestino = "Recetas_Medicas/receta_" + dni + ".jpg";
    ofstream archivoDestino(nombreDestino.c_str(), ios::binary);
    
    if(!archivoDestino) {
        cout << "\nError: No se pudo crear el archivo destino.\n";
        archivoOrigen.close();
        return false;
    }
    
    // Copiar byte por byte
    char buffer;
    while(archivoOrigen.get(buffer)) {
        archivoDestino.put(buffer);
    }
    
    archivoOrigen.close();
    archivoDestino.close();
    
    cout << "\nReceta guardada exitosamente en: " << nombreDestino << endl;
    return true;
}

// Verificar si existe una receta para un paciente
bool existeReceta(string dni) {
    string ruta = "Recetas_Medicas/receta_" + dni + ".jpg";
    ifstream archivo(ruta.c_str());
    bool existe = archivo.good();
    archivo.close();
    return existe;
}

// Abrir imagen de receta con visor predeterminado
void abrirReceta(string dni) {
    string ruta = "Recetas_Medicas/receta_" + dni + ".jpg";
    
    if(!existeReceta(dni)) {
        cout << "\nNo existe receta medica para el DNI: " << dni << endl;
        return;
    }
    
    cout << "\nAbriendo receta medica...\n";
    // Comando para abrir con visor predeterminado en Windows
    string comando = "start " + ruta;
    system(comando.c_str());
}
