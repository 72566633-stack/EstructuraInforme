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

struct Fecha {
    int dia;
    int mes;
    int anio;
};

// Registro para almacenar datos de un paciente
struct Paciente {
    int id;                 // ID unico autogenerado
    string nombre;
    string dni;
    int edad;
    char genero;            // 'M' o 'F'
    string telefono;
    string motivo;
    string diagnostico;
    string estado;          // "En espera", "En atencion", "Atendido"
    int prioridad;          // 1=urgente, 2=normal, 3=baja
    Fecha fechaIngreso;
    string rutaReceta;      // Ruta de la imagen de receta medica
    bool tieneReceta;       // Indica si tiene receta asociada
};

// ============================================
// PARTE 2: NODO PARA LISTA ENLAZADA
// ============================================
struct Nodo {
    Paciente datos;      // Datos del paciente
    Nodo* siguiente;     // Puntero al siguiente nodo (memoria dinamica)
};

// ============================================
// PARTE 3: ARRAYS UNIDIMENSIONALES
// ============================================
const int MAX_HISTORIAL = 100;
const int MAX_MEDICAMENTOS = 30;
const int MAX_ESPECIALIDADES = 5;

// Array para historial de atenciones
string historialAtenciones[MAX_HISTORIAL];
int contadorHistorial = 0;

// Array de medicamentos disponibles
string listaMedicamentos[MAX_MEDICAMENTOS] = {
    "Paracetamol", "Ibuprofeno", "Amoxicilina", "Omeprazol", 
    "Loratadina", "Salbutamol", "Metformina", "Enalapril",
    "Atorvastatina", "Losartan", "Aspirina", "Diclofenaco",
    "Cetirizina", "Ranitidina", "Ciprofloxacino"
};
int cantidadMedicamentos = 15;

// Array de especialidades medicas
string especialidadesMedicas[MAX_ESPECIALIDADES] = {
    "Medicina General", "Cardiologia", "Pediatria", 
    "Traumatologia", "Ginecologia"
};

// Array para contar pacientes por especialidad
int pacientesPorEspecialidad[MAX_ESPECIALIDADES] = {0};

// Array para almacenar IDs de pacientes atendidos
int idsAtendidos[MAX_HISTORIAL];
int totalAtendidos = 0;

// ============================================
// PARTE 4: ARRAYS BIDIMENSIONALES (MATRICES)
// ============================================
const int MAX_DIAS = 7;
const int MAX_HORAS = 10;

// Matriz para horarios de atencion (filas=dias, columnas=horas)
int horariosDisponibles[MAX_DIAS][MAX_HORAS];
string diasSemana[MAX_DIAS] = {"Lun", "Mar", "Mie", "Jue", "Vie", "Sab", "Dom"};

// Matriz de estadisticas: especialidades x rangos de edad
// Columnas: 0-18, 19-35, 36-60, 60+
int estadisticasEdadEspecialidad[MAX_ESPECIALIDADES][4] = {0};

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
    cout << "\n========== HORARIOS DISPONIBLES (8:00-17:00) ==========\n";
    cout << "Dia\\Hora ";
    for(int j = 0; j < MAX_HORAS; j++) {
        cout << (8+j) << "h ";
    }
    cout << endl;
    cout << "--------------------------------------------------------\n";
    
    for(int i = 0; i < MAX_DIAS; i++) {
        cout << diasSemana[i] << "      ";
        for(int j = 0; j < MAX_HORAS; j++) {
            cout << " " << horariosDisponibles[i][j] << "  ";
        }
        cout << endl;
    }
}

// Reservar horario (operacion con matriz)
bool reservarHorario(int dia, int hora) {
    if(dia >= 0 && dia < MAX_DIAS && hora >= 0 && hora < MAX_HORAS) {
        if(horariosDisponibles[dia][hora] > 0) {
            horariosDisponibles[dia][hora]--;
            return true;
        }
    }
    return false;
}

// ============================================
// PARTE 5: TABLA DE DISPERSION (HASH TABLE)
// ============================================
const int TAMANO_HASH = 50;

struct NodoHash {
    int idPaciente;
    string dni;
    string nombreCompleto;
    NodoHash* siguiente; // Para manejar colisiones
};

NodoHash* tablaHash[TAMANO_HASH] = {NULL};

// Funcion hash simple
int funcionHash(string dni) {
    int suma = 0;
    for(int i = 0; i < dni.length(); i++) {
        suma += dni[i];
    }
    return suma % TAMANO_HASH;
}

// Insertar en tabla hash
void insertarHash(int id, string dni, string nombre) {
    int indice = funcionHash(dni);
    
    NodoHash* nuevo = new NodoHash;
    nuevo->idPaciente = id;
    nuevo->dni = dni;
    nuevo->nombreCompleto = nombre;
    nuevo->siguiente = tablaHash[indice];
    tablaHash[indice] = nuevo;
}

// Buscar en tabla hash - Complejidad O(1) promedio
bool buscarEnHash(string dni, string& nombreEncontrado, int& idEncontrado) {
    int indice = funcionHash(dni);
    NodoHash* actual = tablaHash[indice];
    
    while(actual != NULL) {
        if(actual->dni == dni) {
            nombreEncontrado = actual->nombreCompleto;
            idEncontrado = actual->idPaciente;
            return true;
        }
        actual = actual->siguiente;
    }
    return false;
}

// Mostrar tabla hash
void mostrarTablaHash() {
    cout << "\n========== TABLA HASH DE PACIENTES ==========\n";
    int conteo = 0;
    for(int i = 0; i < TAMANO_HASH; i++) {
        if(tablaHash[i] != NULL) {
            cout << "Indice " << i << ": ";
            NodoHash* actual = tablaHash[i];
            while(actual != NULL) {
                cout << "[" << actual->dni << "-" << actual->nombreCompleto << "] ";
                actual = actual->siguiente;
                conteo++;
            }
            cout << endl;
        }
    }
    cout << "Total de registros en hash: " << conteo << endl;
}

// ============================================
// FUNCIONES PARA MANEJO DE RECETAS MEDICAS
// ============================================

void crearDirectorioRecetas() {
    _mkdir("Recetas_Medicas");
}

// Copiar archivo de imagen usando manejo binario
bool copiarReceta(string rutaOrigen, string dni) {
    ifstream archivoOrigen(rutaOrigen.c_str(), ios::binary);
    
    if(!archivoOrigen) {
        cout << "\nError: No se pudo abrir el archivo de imagen.\n";
        return false;
    }
    
    string nombreDestino = "Recetas_Medicas/receta_" + dni + ".jpg";
    ofstream archivoDestino(nombreDestino.c_str(), ios::binary);
    
    if(!archivoDestino) {
        cout << "\nError: No se pudo crear el archivo destino.\n";
        archivoOrigen.close();
        return false;
    }
    
    // Copiar byte por byte (manejo de archivos binarios)
    char buffer;
    while(archivoOrigen.get(buffer)) {
        archivoDestino.put(buffer);
    }
    
    archivoOrigen.close();
    archivoDestino.close();
    
    cout << "\n[ARCHIVO] Receta guardada en: " << nombreDestino << endl;
    return true;
}

bool existeReceta(string dni) {
    string ruta = "Recetas_Medicas/receta_" + dni + ".jpg";
    ifstream archivo(ruta.c_str());
    bool existe = archivo.good();
    archivo.close();
    return existe;
}

void abrirReceta(string dni) {
    if(!existeReceta(dni)) {
        cout << "\nNo existe receta medica para el DNI: " << dni << endl;
        return;
    }
    
    string ruta = "Recetas_Medicas/receta_" + dni + ".jpg";
    cout << "\n[SISTEMA] Abriendo receta medica...\n";
    string comando = "start " + ruta;
    system(comando.c_str());
}

// ============================================
// PARTE 6: COLA ENLAZADA (TDA PRINCIPAL)
// ============================================
class ColaPacientes {
private:
    Nodo* frente;
    Nodo* final;
    int cantidad;
    int idContador; // Para generar IDs unicos
    
public:
    // Constructor
    ColaPacientes() {
        frente = NULL;
        final = NULL;
        cantidad = 0;
        idContador = 1000;
    }
    
    // Verificar si esta vacia
    bool estaVacia() {
        return frente == NULL;
    }
    
    // ENCOLAR (agregar al final) - Operacion principal de cola
    void encolar(Paciente p) {
        // Asignar ID unico
        p.id = ++idContador;
        
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
        
        // Agregar a tabla hash para busqueda rapida
        insertarHash(p.id, p.dni, p.nombre);
        
        // Agregar al historial
        if(contadorHistorial < MAX_HISTORIAL) {
            historialAtenciones[contadorHistorial] = p.nombre + " - " + p.motivo;
            contadorHistorial++;
        }
        
        cout << "\n[COLA] Paciente encolado con ID: " << p.id << endl;
    }
    
    // DESENCOLAR (eliminar del frente) - Operacion principal de cola
    Paciente desencolar() {
        if(estaVacia()) {
            Paciente vacio;
            vacio.nombre = "ERROR";
            vacio.id = -1;
            return vacio;
        }
        
        Nodo* temp = frente;
        Paciente p = frente->datos;
        frente = frente->siguiente;
        
        if(frente == NULL) {
            final = NULL;
        }
        
        delete temp; // Liberar memoria dinamica
        cantidad--;
        
        // Registrar en array de atendidos
        if(totalAtendidos < MAX_HISTORIAL) {
            idsAtendidos[totalAtendidos++] = p.id;
        }
        
        return p;
    }
    
    int obtenerCantidad() {
        return cantidad;
    }
    
    // Mostrar usando punteros y recorrido de lista
    void mostrarTodos() {
        if(estaVacia()) {
            cout << "\nNo hay pacientes en espera.\n";
            return;
        }
        
        cout << "\n========== LISTA DE PACIENTES EN ESPERA ==========\n";
        Nodo* actual = frente;
        int posicion = 1;
        
        while(actual != NULL) {
            cout << "\n--- Paciente #" << posicion << " ---\n";
            cout << "ID: " << actual->datos.id << endl;
            cout << "Nombre: " << actual->datos.nombre << endl;
            cout << "DNI: " << actual->datos.dni << endl;
            cout << "Edad: " << actual->datos.edad << " años | Genero: " 
                 << actual->datos.genero << endl;
            cout << "Telefono: " << actual->datos.telefono << endl;
            cout << "Motivo: " << actual->datos.motivo << endl;
            cout << "Estado: " << actual->datos.estado << endl;
            
            string prioridadTexto = (actual->datos.prioridad == 1 ? "URGENTE" : 
                                     actual->datos.prioridad == 2 ? "Normal" : "Baja");
            cout << "Prioridad: " << prioridadTexto << endl;
            cout << "Receta: " << (actual->datos.tieneReceta ? "SI" : "NO") << endl;
            cout << "Fecha ingreso: " << actual->datos.fechaIngreso.dia << "/" 
                 << actual->datos.fechaIngreso.mes << "/" 
                 << actual->datos.fechaIngreso.anio << endl;
            
            actual = actual->siguiente;
            posicion++;
        }
    }
    
    // ============================================
    // ALGORITMOS DE BUSQUEDA
    // ============================================
    
    // Busqueda secuencial por nombre
    Nodo* buscarPorNombre(string nombre) {
        Nodo* actual = frente;
        
        while(actual != NULL) {
            if(actual->datos.nombre == nombre) {
                return actual;
            }
            actual = actual->siguiente;
        }
        return NULL;
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
    
    // Busqueda por ID
    Nodo* buscarPorID(int id) {
        Nodo* actual = frente;
        
        while(actual != NULL) {
            if(actual->datos.id == id) {
                return actual;
            }
            actual = actual->siguiente;
        }
        return NULL;
    }
    
    // ============================================
    // ALGORITMOS DE ACTUALIZACION
    // ============================================
    
    // Actualizar usando referencias
    bool actualizarEstado(string nombre, string nuevoEstado) {
        Nodo* pacienteNodo = buscarPorNombre(nombre);
        
        if(pacienteNodo != NULL) {
            // Usando referencia para modificar directamente
            Paciente& p = pacienteNodo->datos;
            p.estado = nuevoEstado;
            cout << "\n[ACTUALIZAR] Estado actualizado correctamente.\n";
            return true;
        }
        cout << "\n[ACTUALIZAR] Paciente no encontrado.\n";
        return false;
    }
    
    // Actualizar usando punteros
    bool actualizarPrioridad(string dni, int nuevaPrioridad) {
        Nodo* paciente = buscarPorDNI(dni);
        
        if(paciente != NULL) {
            // Usando puntero para acceder al dato
            paciente->datos.prioridad = nuevaPrioridad;
            cout << "\n[ACTUALIZAR] Prioridad actualizada.\n";
            return true;
        }
        return false;
    }
    
    // Actualizar diagnostico con referencia
    void actualizarDiagnostico(int id, string nuevoDiagnostico) {
        Nodo* nodo = buscarPorID(id);
        if(nodo != NULL) {
            Paciente& p = nodo->datos;
            p.diagnostico = nuevoDiagnostico;
            cout << "\n[ACTUALIZAR] Diagnostico actualizado.\n";
        }
    }
    
    // Agregar receta usando punteros
    bool agregarRecetaPaciente(string dni, string rutaReceta) {
        Nodo* paciente = buscarPorDNI(dni);
        
        if(paciente != NULL) {
            if(copiarReceta(rutaReceta, dni)) {
                paciente->datos.tieneReceta = true;
                paciente->datos.rutaReceta = "Recetas_Medicas/receta_" + dni + ".jpg";
                cout << "\n[ACTUALIZAR] Receta asociada al paciente.\n";
                return true;
            }
        } else {
            cout << "\nPaciente no encontrado.\n";
        }
        return false;
    }
    
    // Actualizacion masiva de prioridades (mayores de 70 años)
    void actualizarPrioridadesMasivas() {
        cout << "\n[ACTUALIZACION MASIVA] Revisando prioridades...\n";
        int actualizados = 0;
        Nodo* actual = frente;
        
        while(actual != NULL) {
            Paciente& p = actual->datos; // Referencia
            
            if(p.edad > 70 && p.prioridad > 1) {
                p.prioridad = 1; // Prioridad alta
                actualizados++;
            }
            actual = actual->siguiente;
        }
        
        cout << "Pacientes con prioridad actualizada: " << actualizados << endl;
    }
    
    // ============================================
    // ALGORITMOS DE ORDENACION
    // ============================================
    
    // Ordenamiento Burbuja por prioridad
    void ordenarPorPrioridad() {
        if(estaVacia() || cantidad == 1) return;
        
        cout << "\n[ORDENAMIENTO BURBUJA] Ordenando por prioridad...\n";
        
        Paciente temp[100];
        int n = 0;
        Nodo* actual = frente;
        
        // Copiar cola a array
        while(actual != NULL && n < 100) {
            temp[n] = actual->datos;
            actual = actual->siguiente;
            n++;
        }
        
        // Algoritmo Burbuja
        for(int i = 0; i < n-1; i++) {
            for(int j = 0; j < n-i-1; j++) {
                if(temp[j].prioridad > temp[j+1].prioridad) {
                    Paciente aux = temp[j];
                    temp[j] = temp[j+1];
                    temp[j+1] = aux;
                }
            }
        }
        
        // Reconstruir cola
        vaciar();
        for(int i = 0; i < n; i++) {
            Nodo* nuevo = new Nodo;
            nuevo->datos = temp[i];
            nuevo->siguiente = NULL;
            
            if(frente == NULL) {
                frente = final = nuevo;
            } else {
                final->siguiente = nuevo;
                final = nuevo;
            }
            cantidad++;
        }
        
        cout << "[ORDENAMIENTO] Cola ordenada por prioridad.\n";
    }
    
    // Ordenamiento por Seleccion - Por edad
    void ordenarPorEdad() {
        if(estaVacia() || cantidad == 1) return;
        
        cout << "\n[ORDENAMIENTO SELECCION] Ordenando por edad...\n";
        
        Paciente temp[100];
        int n = 0;
        Nodo* actual = frente;
        
        while(actual != NULL && n < 100) {
            temp[n] = actual->datos;
            actual = actual->siguiente;
            n++;
        }
        
        // Algoritmo de Seleccion
        for(int i = 0; i < n-1; i++) {
            int min_idx = i;
            for(int j = i+1; j < n; j++) {
                if(temp[j].edad < temp[min_idx].edad) {
                    min_idx = j;
                }
            }
            if(min_idx != i) {
                Paciente aux = temp[i];
                temp[i] = temp[min_idx];
                temp[min_idx] = aux;
            }
        }
        
        vaciar();
        for(int i = 0; i < n; i++) {
            Nodo* nuevo = new Nodo;
            nuevo->datos = temp[i];
            nuevo->siguiente = NULL;
            
            if(frente == NULL) {
                frente = final = nuevo;
            } else {
                final->siguiente = nuevo;
                final = nuevo;
            }
            cantidad++;
        }
        
        cout << "[ORDENAMIENTO] Cola ordenada por edad.\n";
    }
    
    // Ordenamiento por Insercion - Por nombre
    void ordenarPorNombre() {
        if(estaVacia() || cantidad == 1) return;
        
        cout << "\n[ORDENAMIENTO INSERCION] Ordenando por nombre...\n";
        
        Paciente temp[100];
        int n = 0;
        Nodo* actual = frente;
        
        while(actual != NULL && n < 100) {
            temp[n] = actual->datos;
            actual = actual->siguiente;
            n++;
        }
        
        // Algoritmo de Insercion
        for(int i = 1; i < n; i++) {
            Paciente key = temp[i];
            int j = i - 1;
            
            while(j >= 0 && temp[j].nombre > key.nombre) {
                temp[j + 1] = temp[j];
                j--;
            }
            temp[j + 1] = key;
        }
        
        vaciar();
        for(int i = 0; i < n; i++) {
            Nodo* nuevo = new Nodo;
            nuevo->datos = temp[i];
            nuevo->siguiente = NULL;
            
            if(frente == NULL) {
                frente = final = nuevo;
            } else {
                final->siguiente = nuevo;
                final = nuevo;
            }
            cantidad++;
        }
        
        cout << "[ORDENAMIENTO] Cola ordenada por nombre.\n";
    }
    
    void vaciar() {
        while(!estaVacia()) {
            desencolar();
        }
    }
    
    // Destructor - Liberar memoria
    ~ColaPacientes() {
        vaciar();
    }
};

// ============================================
// PARTE 7: PILA PARA HISTORIAL (LIFO)
// ============================================
struct NodoPila {
    Paciente datos;
    NodoPila* siguiente;
};

class PilaHistorial {
private:
    NodoPila* tope;
    int cantidad;
    
public:
    PilaHistorial() {
        tope = NULL;
        cantidad = 0;
    }
    
    // APILAR - Push (agregar al tope)
    void apilar(Paciente p) {
        NodoPila* nuevo = new NodoPila;
        nuevo->datos = p;
        nuevo->siguiente = tope;
        tope = nuevo;
        cantidad++;
        cout << "[PILA] Paciente agregado al historial.\n";
    }
    
    // DESAPILAR - Pop (quitar del tope)
    Paciente desapilar() {
        if(estaVacia()) {
            Paciente vacio;
            vacio.nombre = "ERROR";
            return vacio;
        }
        
        NodoPila* temp = tope;
        Paciente p = tope->datos;
        tope = tope->siguiente;
        delete temp;
        cantidad--;
        return p;
    }
    
    bool estaVacia() {
        return tope == NULL;
    }
    
    // Mostrar ultimos atendidos
    void mostrarUltimos(int n) {
        if(estaVacia()) {
            cout << "\nNo hay historial.\n";
            return;
        }
        
        cout << "\n===== ULTIMOS " << n << " PACIENTES ATENDIDOS =====\n";
        NodoPila* actual = tope;
        int contador = 0;
        
        while(actual != NULL && contador < n) {
            cout << (contador+1) << ". " << actual->datos.nombre 
                 << " (DNI: " << actual->datos.dni << ")\n";
            cout << "   Diagnostico: " << actual->datos.diagnostico << endl;
            actual = actual->siguiente;
            contador++;
        }
    }
    
    int obtenerCantidad() {
        return cantidad;
    }
};

// ============================================
// OPERACIONES CON CADENAS DE CARACTERES
// ============================================

// Convertir a mayusculas
string convertirMayusculas(string str) {
    for(int i = 0; i < str.length(); i++) {
        if(str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - 32;
        }
    }
    return str;
}

// Validar DNI (8 digitos)
bool validarDNI(string dni) {
    if(dni.length() != 8) return false;
    
    for(int i = 0; i < dni.length(); i++) {
        if(dni[i] < '0' || dni[i] > '9') {
            return false;
        }
    }
    return true;
}

// Validar telefono
bool validarTelefono(string tel) {
    int len = tel.length();
    if(len < 7 || len > 15) return false;
    
    for(int i = 0; i < len; i++) {
        if(tel[i] < '0' || tel[i] > '9') {
            if(tel[i] != '-' && tel[i] != ' ') {
                return false;
            }
        }
    }
    return true;
}

// ============================================
// ALGORITMOS EN ARRAYS
// ============================================

// Busqueda lineal en array de medicamentos
int buscarMedicamento(string nombre) {
    nombre = convertirMayusculas(nombre);
    
    for(int i = 0; i < cantidadMedicamentos; i++) {
        string medMayus = convertirMayusculas(listaMedicamentos[i]);
        if(medMayus == nombre) {
            return i;
        }
    }
    return -1;
}

// Ordenamiento por Insercion - Array de medicamentos
void ordenarMedicamentos() {
    cout << "\n[ORDENAMIENTO INSERCION] Ordenando medicamentos...\n";
    
    for(int i = 1; i < cantidadMedicamentos; i++) {
        string key = listaMedicamentos[i];
        int j = i - 1;
        
        while(j >= 0 && listaMedicamentos[j] > key) {
            listaMedicamentos[j + 1] = listaMedicamentos[j];
            j--;
        }
        listaMedicamentos[j + 1] = key;
    }
    cout << "Medicamentos ordenados alfabeticamente.\n";
}

void mostrarMedicamentos() {
    cout << "\n========== LISTA DE MEDICAMENTOS ==========\n";
    for(int i = 0; i < cantidadMedicamentos; i++) {
        cout << (i+1) << ". " << listaMedicamentos[i] << endl;
    }
}

// Agregar medicamento (operacion con array)
void agregarMedicamento() {
    if(cantidadMedicamentos >= MAX_MEDICAMENTOS) {
        cout << "\nLista de medicamentos llena.\n";
        return;
    }
    
    string nombre;
    cout << "\n========== AGREGAR MEDICAMENTO ==========\n";
    cout << "Nombre del medicamento: ";
    cin.ignore();
    getline(cin, nombre);
    
    listaMedicamentos[cantidadMedicamentos++] = nombre;
    cout << "\n*** Medicamento agregado correctamente ***\n";
}

// Eliminar medicamento del array
void eliminarMedicamento() {
    if(cantidadMedicamentos == 0) {
        cout << "\nNo hay medicamentos para eliminar.\n";
        return;
    }
    
    string nombre;
    cout << "\n========== ELIMINAR MEDICAMENTO ==========\n";
    cout << "Nombre del medicamento a eliminar: ";
    cin.ignore();
    getline(cin, nombre);
    
    int pos = buscarMedicamento(nombre);
    
    if(pos != -1) {
        // Desplazar elementos hacia la izquierda
        for(int i = pos; i < cantidadMedicamentos - 1; i++) {
            listaMedicamentos[i] = listaMedicamentos[i + 1];
        }
        cantidadMedicamentos--;
        cout << "\n*** Medicamento eliminado correctamente ***\n";
    } else {
        cout << "\nMedicamento no encontrado.\n";
    }
}

// Actualizar "stock" o informacion de medicamento
void actualizarMedicamento() {
    string nombre, nuevoNombre;
    cout << "\n========== ACTUALIZAR MEDICAMENTO ==========\n";
    cout << "Nombre del medicamento a actualizar: ";
    cin.ignore();
    getline(cin, nombre);
    
    int pos = buscarMedicamento(nombre);
    
    if(pos != -1) {
        cout << "Medicamento actual: " << listaMedicamentos[pos] << endl;
        cout << "Nuevo nombre del medicamento: ";
        getline(cin, nuevoNombre);
        
        listaMedicamentos[pos] = nuevoNombre;
        cout << "\n*** Medicamento actualizado correctamente ***\n";
    } else {
        cout << "\nMedicamento no encontrado.\n";
    }
}

// ============================================
// ESTADISTICAS CON MATRICES
// ============================================

void generarEstadisticasEdad(ColaPacientes& cola) {
    cout << "\n========== ESTADISTICAS POR EDAD ==========\n";
    
    // Inicializar matriz
    for(int i = 0; i < MAX_ESPECIALIDADES; i++) {
        for(int j = 0; j < 4; j++) {
            estadisticasEdadEspecialidad[i][j] = 0;
        }
    }
    
    // Esta es una simulacion - en un sistema real se tomaria
    // la especialidad de cada paciente
    
    cout << "\nRangos: 0-18 | 19-35 | 36-60 | 60+\n";
    cout << "Especialidad        | 0-18 | 19-35 | 36-60 | 60+ \n";
    cout << "-----------------------------------------------------\n";
    
    for(int i = 0; i < MAX_ESPECIALIDADES; i++) {
        cout << especialidadesMedicas[i];
        
        // Ajustar espacios
        int espacios = 20 - especialidadesMedicas[i].length();
        for(int k = 0; k < espacios; k++) cout << " ";
        
        cout << "| ";
        for(int j = 0; j < 4; j++) {
            cout << estadisticasEdadEspecialidad[i][j] << "    | ";
        }
        cout << endl;
    }
}

// ============================================
// FUNCIONES PRINCIPALES DEL SISTEMA
// ============================================

void registrarPaciente(ColaPacientes& cola) {
    Paciente nuevo;
    
    cout << "\n========== REGISTRAR NUEVO PACIENTE ==========\n";
    cin.ignore();
    
    cout << "Nombre completo: ";
    getline(cin, nuevo.nombre);
    
    do {
        cout << "DNI (8 digitos): ";
        getline(cin, nuevo.dni);
        
        if(!validarDNI(nuevo.dni)) {
            cout << "DNI invalido. Debe tener 8 digitos.\n";
        } else {
            // Verificar si ya existe en hash
            string nombreHash;
            int idHash;
            if(buscarEnHash(nuevo.dni, nombreHash, idHash)) {
                cout << "ADVERTENCIA: Ya existe un paciente con ese DNI.\n";
            }
            break;
        }
    } while(true);
    
    cout << "Edad: ";
    cin >> nuevo.edad;
    
    cout << "Genero (M/F): ";
    cin >> nuevo.genero;
    
    cin.ignore();
    do {
        cout << "Telefono: ";
        getline(cin, nuevo.telefono);
        
        if(!validarTelefono(nuevo.telefono)) {
            cout << "Telefono invalido.\n";
        } else {
            break;
        }
    } while(true);
    
    cout << "Motivo de consulta: ";
    getline(cin, nuevo.motivo);
    
    cout << "Diagnostico inicial: ";
    getline(cin, nuevo.diagnostico);
    
    cout << "Prioridad (1=Urgente, 2=Normal, 3=Baja): ";
    cin >> nuevo.prioridad;
    
    cout << "Fecha de ingreso (dia mes anio): ";
    cin >> nuevo.fechaIngreso.dia >> nuevo.fechaIngreso.mes >> nuevo.fechaIngreso.anio;
    
    nuevo.estado = "En espera";
    nuevo.tieneReceta = false;
    nuevo.rutaReceta = "";
    
    cola.encolar(nuevo);
    cout << "\n*** Paciente registrado exitosamente ***\n";
}

void atenderPaciente(ColaPacientes& cola, PilaHistorial& pila) {
    if(cola.estaVacia()) {
        cout << "\nNo hay pacientes para atender.\n";
        return;
    }
    
    Paciente p = cola.desencolar();
    
    cout << "\n========== ATENDIENDO PACIENTE ==========\n";
    cout << "ID: " << p.id << endl;
    cout << "Nombre: " << p.nombre << endl;
    cout << "DNI: " << p.dni << endl;
    cout << "Edad: " << p.edad << " años\n";
    cout << "Motivo: " << p.motivo << endl;
    cout << "Diagnostico: " << p.diagnostico << endl;
    
    p.estado = "Atendido";
    pila.apilar(p); // Guardar en historial (pila)
    
    cout << "\n*** Paciente atendido correctamente ***\n";
}

void buscarPaciente(ColaPacientes& cola) {
    int opcion;
    cout << "\n=== BUSCAR PACIENTE ===\n";
    cout << "1. Por Nombre\n";
    cout << "2. Por DNI (usando Hash - mas rapido)\n";
    cout << "3. Por ID\n";
    cout << "Opcion: ";
    cin >> opcion;
    cin.ignore();
    
    if(opcion == 1) {
        string nombre;
        cout << "Ingrese nombre: ";
        getline(cin, nombre);
        
        Nodo* encontrado = cola.buscarPorNombre(nombre);
        
        if(encontrado != NULL) {
            cout << "\n*** PACIENTE ENCONTRADO ***\n";
            cout << "ID: " << encontrado->datos.id << endl;
            cout << "Nombre: " << encontrado->datos.nombre << endl;
            cout << "DNI: " << encontrado->datos.dni << endl;
            cout << "Edad: " << encontrado->datos.edad << endl;
            cout << "Telefono: " << encontrado->datos.telefono << endl;
            cout << "Motivo: " << encontrado->datos.motivo << endl;
            cout << "Estado: " << encontrado->datos.estado << endl;
            cout << "Receta: " << (encontrado->datos.tieneReceta ? "SI" : "NO") << endl;
        } else {
            cout << "\nPaciente no encontrado.\n";
        }
    }
    else if(opcion == 2) {
        string dni;
        cout << "Ingrese DNI: ";
        getline(cin, dni);
        
        // Buscar primero en hash (O(1))
        string nombreHash;
        int idHash;
        if(buscarEnHash(dni, nombreHash, idHash)) {
            cout << "\n[HASH] Encontrado rapidamente!\n";
            cout << "ID: " << idHash << endl;
            cout << "Nombre: " << nombreHash << endl;
            cout << "DNI: " << dni << endl;
            
            // Buscar datos completos en la cola
            Nodo* encontrado = cola.buscarPorDNI(dni);
            if(encontrado != NULL) {
                cout << "Edad: " << encontrado->datos.edad << endl;
                cout << "Telefono: " << encontrado->datos.telefono << endl;
                cout << "Estado: " << encontrado->datos.estado << endl;
            }
        } else {
            cout << "\nPaciente no encontrado en el sistema.\n";
        }
    }
    else if(opcion == 3) {
        int id;
        cout << "Ingrese ID: ";
        cin >> id;
        
        Nodo* encontrado = cola.buscarPorID(id);
        
        if(encontrado != NULL) {
            cout << "\n*** PACIENTE ENCONTRADO ***\n";
            cout << "ID: " << encontrado->datos.id << endl;
            cout << "Nombre: " << encontrado->datos.nombre << endl;
            cout << "DNI: " << encontrado->datos.dni << endl;
            cout << "Estado: " << encontrado->datos.estado << endl;
        } else {
            cout << "\nPaciente no encontrado.\n";
        }
    }
}

void menuActualizaciones(ColaPacientes& cola) {
    int opcion;
    
    cout << "\n=== MENU DE ACTUALIZACIONES ===\n";
    cout << "1. Actualizar estado de paciente\n";
    cout << "2. Actualizar prioridad\n";
    cout << "3. Actualizar diagnostico\n";
    cout << "4. Actualizacion masiva de prioridades (>70 años)\n";
    cout << "Opcion: ";
    cin >> opcion;
    cin.ignore();
    
    switch(opcion) {
        case 1: {
            string nombre, estado;
            cout << "Nombre del paciente: ";
            getline(cin, nombre);
            cout << "Nuevo estado (En espera/En atencion/Atendido): ";
            getline(cin, estado);
            cola.actualizarEstado(nombre, estado);
            break;
        }
        case 2: {
            string dni;
            int prioridad;
            cout << "DNI del paciente: ";
            getline(cin, dni);
            cout << "Nueva prioridad (1-3): ";
            cin >> prioridad;
            cola.actualizarPrioridad(dni, prioridad);
            break;
        }
        case 3: {
            int id;
            string diagnostico;
            cout << "ID del paciente: ";
            cin >> id;
            cin.ignore();
            cout << "Nuevo diagnostico: ";
            getline(cin, diagnostico);
            cola.actualizarDiagnostico(id, diagnostico);
            break;
        }
        case 4:
            cola.actualizarPrioridadesMasivas();
            break;
        default:
            cout << "Opcion invalida.\n";
    }
}

void menuOrdenamiento(ColaPacientes& cola) {
    int opcion;
    
    cout << "\n=== ALGORITMOS DE ORDENAMIENTO ===\n";
    cout << "1. Ordenar por Prioridad (Burbuja)\n";
    cout << "2. Ordenar por Edad (Seleccion)\n";
    cout << "3. Ordenar por Nombre (Insercion)\n";
    cout << "Opcion: ";
    cin >> opcion;
    
    switch(opcion) {
        case 1:
            cola.ordenarPorPrioridad();
            break;
        case 2:
            cola.ordenarPorEdad();
            break;
        case 3:
            cola.ordenarPorNombre();
            break;
        default:
            cout << "Opcion invalida.\n";
    }
}

void agregarRecetaMedica(ColaPacientes& cola) {
    string dni, rutaImagen;
    
    cout << "\n========== AGREGAR RECETA MEDICA ==========\n";
    cout << "Ingrese DNI del paciente: ";
    cin.ignore();
    getline(cin, dni);
    
    cout << "Ingrese ruta completa de la imagen:\n";
    cout << "(Ejemplo: C:\\imagenes\\receta.jpg)\n";
    cout << "Ruta: ";
    getline(cin, rutaImagen);
    
    cola.agregarRecetaPaciente(dni, rutaImagen);
}

void verRecetaMedica() {
    string dni;
    
    cout << "\n========== VER RECETA MEDICA ==========\n";
    cout << "Ingrese DNI del paciente: ";
    cin.ignore();
    getline(cin, dni);
    
    abrirReceta(dni);
}

void listarRecetasMedicas() {
    cout << "\n========== RECETAS MEDICAS ALMACENADAS ==========\n";
    system("dir Recetas_Medicas\\*.jpg /B 2>nul");
    cout << "\nPara ver una receta especifica, use la opcion 15.\n";
}

void mostrarHistorial() {
    cout << "\n========== HISTORIAL DE ATENCIONES ==========\n";
    if(contadorHistorial == 0) {
        cout << "No hay atenciones registradas.\n";
        return;
    }
    
    for(int i = 0; i < contadorHistorial; i++) {
        cout << (i+1) << ". " << historialAtenciones[i] << endl;
    }
}

void reservarCita() {
    int dia, hora;
    
    cout << "\n========== RESERVAR CITA ==========\n";
    cout << "Dias: 0=Lun, 1=Mar, 2=Mie, 3=Jue, 4=Vie, 5=Sab, 6=Dom\n";
    cout << "Seleccione dia (0-6): ";
    cin >> dia;
    
    cout << "Horas disponibles: 8-17 (ingrese 0-9 para 8h-17h)\n";
    cout << "Seleccione hora (0-9): ";
    cin >> hora;
    
    if(reservarHorario(dia, hora)) {
        cout << "\n*** Cita reservada exitosamente ***\n";
        cout << "Dia: " << diasSemana[dia] << " | Hora: " << (8+hora) << ":00\n";
    } else {
        cout << "\nNo hay horarios disponibles o datos invalidos.\n";
    }
}


void mostrarEstadisticasGenerales(ColaPacientes& cola, PilaHistorial& pila) {
    cout << "\n========== ESTADISTICAS GENERALES ==========\n";
    cout << "Pacientes en espera: " << cola.obtenerCantidad() << endl;
    cout << "Pacientes atendidos (en pila): " << pila.obtenerCantidad() << endl;
    cout << "Total en historial (array): " << contadorHistorial << endl;
    cout << "Medicamentos registrados: " << cantidadMedicamentos << endl;
    
    // Mostrar IDs de ultimos atendidos
    cout << "\nUltimos IDs atendidos: ";
    for(int i = 0; i < min(5, totalAtendidos); i++) {
        cout << idsAtendidos[i] << " ";
    }
    cout << endl;
}

void demostrarPunteros(ColaPacientes& cola) {
    cout << "\n========== DEMOSTRACION DE PUNTEROS Y REFERENCIAS ==========\n";
    
    if(cola.estaVacia()) {
        cout << "No hay pacientes para demostrar.\n";
        return;
    }
    
    string dni;
    cout << "Ingrese DNI de un paciente: ";
    cin.ignore();
    getline(cin, dni);
    
    Nodo* punteroNodo = cola.buscarPorDNI(dni);
    
    if(punteroNodo != NULL) {
        cout << "\n1. Usando PUNTERO al nodo:\n";
        cout << "   Direccion de memoria del nodo: " << punteroNodo << endl;
        cout << "   Acceso con ->: " << punteroNodo->datos.nombre << endl;
        cout << "   Acceso con *: " << (*punteroNodo).datos.edad << " años\n";
        
        cout << "\n2. Usando REFERENCIA al paciente:\n";
        Paciente& ref = punteroNodo->datos;
        cout << "   Nombre: " << ref.nombre << endl;
        cout << "   DNI: " << ref.dni << endl;
        
        cout << "\n3. Modificando con referencia:\n";
        string diagnosticoAnterior = ref.diagnostico;
        ref.diagnostico = "MODIFICADO POR REFERENCIA";
        cout << "   Diagnostico anterior: " << diagnosticoAnterior << endl;
        cout << "   Diagnostico nuevo: " << ref.diagnostico << endl;
        
        // Restaurar
        ref.diagnostico = diagnosticoAnterior;
        cout << "   Restaurado a: " << ref.diagnostico << endl;
        
    } else {
        cout << "\nPaciente no encontrado.\n";
    }
}

void menuPrincipal() {
    cout << "+------------------------------------------+\n";
    cout << "|  SISTEMA DE GESTION DE PACIENTES         |\n";
    cout << "+------------------------------------------+\n";
    cout << "1.  Registrar paciente\n";
    cout << "2.  Atender paciente\n";
    cout << "3.  Mostrar todos los pacientes\n";
    cout << "4.  Buscar paciente por nombre\n";
    cout << "5.  Actualizar estado de paciente\n";
    cout << "6.  Ordenar cola por prioridad\n";
    cout << "7.  Ordenar cola por edad\n";
    cout << "8.  Mostrar medicamentos disponibles\n";
    cout << "9.  Buscar medicamento\n";
    cout << "10. Ordenar lista de medicamentos\n";
    cout << "11. Mostrar horarios disponibles\n";
    cout << "12. Mostrar historial de atenciones\n";
    cout << "13. Cantidad de pacientes en espera\n";
    cout << "14. Agregar medicamento\n";
    cout << "15. Eliminar medicamento\n";
    cout << "16. Actualizar stock de medicamento\n";
    cout << "0.  Salir\n";
    cout << "-------------------------------------------\n";
    cout << "Opcion: ";
}

// ============================================
// FUNCION PRINCIPAL
// ============================================
int main() {
    ColaPacientes cola;
    PilaHistorial historial;
    int opcion;
    
    // Inicializar
    inicializarHorarios();
    crearDirectorioRecetas();
    
    cout << "\n*** SISTEMA INICIADO ***\n";
    cout << "Carpeta 'Recetas_Medicas' creada/verificada.\n";
    
    cout << "\nPresione Enter para continuar...";
    cin.get();
    
    do {
        system("cls"); // Windows
        // system("clear"); // Linux/Mac
        
        menuPrincipal();
        cin >> opcion;
        
        switch(opcion) {
            case 1:
                registrarPaciente(cola);
                break;
                
            case 2:
                atenderPaciente(cola, historial);
                break;
                
            case 3:
                cola.mostrarTodos();
                break;
                
            case 4:
                buscarPaciente(cola);
                break;
                
            case 5: {
                string nombre, estado;
                cout << "\nNombre del paciente: ";
                cin.ignore();
                getline(cin, nombre);
                cout << "Nuevo estado: ";
                getline(cin, estado);
                cola.actualizarEstado(nombre, estado);
                break;
            }
                
            case 6:
                cola.ordenarPorPrioridad();
                break;
                
            case 7:
                cola.ordenarPorEdad();
                break;
                
            case 8:
                mostrarMedicamentos();
                break;
                
            case 9: {
                string med;
                cout << "\nNombre del medicamento: ";
                cin.ignore();
                getline(cin, med);
                int pos = buscarMedicamento(med);
                if(pos != -1) {
                    cout << "\n[BUSQUEDA] Medicamento encontrado en posicion " << (pos+1) << endl;
                } else {
                    cout << "\n[BUSQUEDA] Medicamento no encontrado.\n";
                }
                break;
            }
                
            case 10:
                ordenarMedicamentos();
                break;
                
            case 11:
                mostrarHorarios();
                break;
                
            case 12:
                mostrarHistorial();
                break;
                
            case 13:
                cout << "\nPacientes en espera: " << cola.obtenerCantidad() << endl;
                break;
                
            case 14:
                agregarMedicamento();
                break;
                
            case 15:
                eliminarMedicamento();
                break;
                
            case 16:
                actualizarMedicamento();
                break;
                
            case 0:
                cout << "\nSaliendo del sistema...\n";
                cout << "Gracias por usar el sistema.\n";
                break;
                
            default:
                cout << "\nOpcion invalida.\n";
        }
        
        if(opcion != 0) {
            cout << "\nPresione Enter para continuar...";
            cin.ignore();
            cin.get();
        }
        
    } while(opcion != 0);
    
    return 0;
}
