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

// ============================================
// PARTE 5: COLA ENLAZADA (TDA PRINCIPAL)
// ============================================
class ColaPacientes {
private:
    Nodo* frente;    // Puntero al primer elemento
    Nodo* final;     // Puntero al ultimo elemento
    int cantidad;    // Contador de pacientes
    
public:
    // Constructor
    ColaPacientes() {
        frente = NULL;
        final = NULL;
        cantidad = 0;
    }
    
    // Verificar si la cola esta vacia
    bool estaVacia() {
        return frente == NULL;
    }
    
    // OPERACION: ENCOLAR (agregar al final)
    void encolar(Paciente p) {
        // Crear nuevo nodo dinamicamente
        Nodo* nuevo = new Nodo;
        nuevo->datos = p;
        nuevo->siguiente = NULL;
        
        if(estaVacia()) {
            frente = nuevo;
            final = nuevo;
        } else {
            final->siguiente = nuevo;
            final = nuevo;
        }
        cantidad++;
        
        // Agregar al historial (array unidimensional)
        if(contadorHistorial < MAX_HISTORIAL) {
            historialAtenciones[contadorHistorial] = p.nombre + " - " + p.motivo;
            contadorHistorial++;
        }
    }
    
    // OPERACION: DESENCOLAR (eliminar del frente)
    Paciente desencolar() {
        if(estaVacia()) {
            Paciente vacio;
            vacio.nombre = "ERROR";
            return vacio;
        }
        
        Nodo* temp = frente;
        Paciente p = frente->datos;
        frente = frente->siguiente;
        
        if(frente == NULL) {
            final = NULL;
        }
        
        delete temp; // Liberar memoria
        cantidad--;
        return p;
    }
    
    // Obtener cantidad de pacientes
    int obtenerCantidad() {
        return cantidad;
    }
    
    // Mostrar todos los pacientes
    void mostrarTodos() {
        if(estaVacia()) {
            cout << "\nNo hay pacientes en espera.\n";
            return;
        }
        
        cout << "\n=== LISTA DE PACIENTES EN ESPERA ===\n";
        Nodo* actual = frente;
        int posicion = 1;
        
        while(actual != NULL) {
            cout << "\n--- Paciente #" << posicion << " ---\n";
            cout << "Nombre: " << actual->datos.nombre << endl;
            cout << "DNI: " << actual->datos.dni << endl;
            cout << "Edad: " << actual->datos.edad << " anios" << endl;
            cout << "Motivo: " << actual->datos.motivo << endl;
            cout << "Estado: " << actual->datos.estado << endl;
            cout << "Prioridad: " << (actual->datos.prioridad == 1 ? "URGENTE" : 
                                      actual->datos.prioridad == 2 ? "Normal" : "Baja") << endl;
            cout << "Receta medica: " << (actual->datos.tieneReceta ? "SI" : "NO") << endl;
            
            actual = actual->siguiente;
            posicion++;
        }
    }
    
    // ============================================
    // PARTE 6: ALGORITMOS DE BUSQUEDA
    // ============================================
    
    // Busqueda secuencial por nombre
    Nodo* buscarPorNombre(string nombre) {
        Nodo* actual = frente;
        
        while(actual != NULL) {
            // Comparacion de strings (busqueda secuencial)
            if(actual->datos.nombre == nombre) {
                return actual;
            }
            actual = actual->siguiente;
        }
        return NULL; // No encontrado
    }
    
    // Busqueda por DNI
    Nodo* buscarPorDNI(string dni) {
        Nodo* actual = frente;
        
        while(actual != NULL) {
            if(actual->datos.dni == dni) {
                return actual;
            }
            actual = actual->siguiente;
        }
        return NULL;
    }
    
    // ============================================
    // PARTE 7: ALGORITMO DE ACTUALIZACION
    // ============================================
    
    // Actualizar estado de un paciente
    bool actualizarEstado(string nombre, string nuevoEstado) {
        Nodo* paciente = buscarPorNombre(nombre);
        
        if(paciente != NULL) {
            paciente->datos.estado = nuevoEstado;
            cout << "\nEstado actualizado correctamente.\n";
            return true;
        }
        cout << "\nPaciente no encontrado.\n";
        return false;
    }
    
    // Actualizar prioridad
    bool actualizarPrioridad(string dni, int nuevaPrioridad) {
        Nodo* paciente = buscarPorDNI(dni);
        
        if(paciente != NULL) {
            paciente->datos.prioridad = nuevaPrioridad;
            cout << "\nPrioridad actualizada correctamente.\n";
            return true;
        }
        return false;
    }
    
    // Agregar receta medica a un paciente existente
    bool agregarRecetaPaciente(string dni, string rutaReceta) {
        Nodo* paciente = buscarPorDNI(dni);
        
        if(paciente != NULL) {
            if(copiarReceta(rutaReceta, dni)) {
                paciente->datos.tieneReceta = true;
                paciente->datos.rutaReceta = "Recetas_Medicas/receta_" + dni + ".jpg";
                cout << "\nReceta asociada al paciente correctamente.\n";
                return true;
            }
        } else {
            cout << "\nPaciente no encontrado.\n";
        }
        return false;
    }
    
    // ============================================
    // PARTE 8: ALGORITMOS DE ORDENACION
    // ============================================
    
    // Ordenar por prioridad (Burbuja)
    void ordenarPorPrioridad() {
        if(estaVacia() || cantidad == 1) return;
        
        // Convertir cola a array temporal para ordenar
        Paciente temp[100];
        int n = 0;
        Nodo* actual = frente;
        
        // Copiar a array
        while(actual != NULL && n < 100) {
            temp[n] = actual->datos;
            actual = actual->siguiente;
            n++;
        }
        
        // Algoritmo de ordenacion Burbuja
        for(int i = 0; i < n-1; i++) {
            for(int j = 0; j < n-i-1; j++) {
                if(temp[j].prioridad > temp[j+1].prioridad) {
                    // Intercambiar
                    Paciente aux = temp[j];
                    temp[j] = temp[j+1];
                    temp[j+1] = aux;
                }
            }
        }
        
        // Reconstruir la cola ordenada
        vaciar();
        for(int i = 0; i < n; i++) {
            encolar(temp[i]);
        }
        
        cout << "\nCola ordenada por prioridad.\n";
    }
    
    // Ordenar por edad (Seleccion)
    void ordenarPorEdad() {
        if(estaVacia() || cantidad == 1) return;
        
        Paciente temp[100];
        int n = 0;
        Nodo* actual = frente;
        
        while(actual != NULL && n < 100) {
            temp[n] = actual->datos;
            actual = actual->siguiente;
            n++;
        }
        
        // Algoritmo de ordenacion por Seleccion
        for(int i = 0; i < n-1; i++) {
            int min_idx = i;
            for(int j = i+1; j < n; j++) {
                if(temp[j].edad < temp[min_idx].edad) {
                    min_idx = j;
                }
            }
            // Intercambiar
            if(min_idx != i) {
                Paciente aux = temp[i];
                temp[i] = temp[min_idx];
                temp[min_idx] = aux;
            }
        }
        
        vaciar();
        for(int i = 0; i < n; i++) {
            encolar(temp[i]);
        }
        
        cout << "\nCola ordenada por edad.\n";
    }
    
    // Vaciar cola
    void vaciar() {
        while(!estaVacia()) {
            desencolar();
        }
    }
    
    // Destructor (liberar memoria)
    ~ColaPacientes() {
        vaciar();
    }
};
