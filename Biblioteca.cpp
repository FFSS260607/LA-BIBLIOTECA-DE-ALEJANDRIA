#include "Biblioteca.h"
#include <iostream>

using namespace std;

Biblioteca::Biblioteca(string _nombre, string _direccion) {
    nombre = _nombre;
    direccion = _direccion;
}

string Biblioteca::getNombre() const { return nombre; }
string Biblioteca::getDireccion() const { return direccion; }

bool Biblioteca::existeArea(string codigo) {
    return tabla_areas.count(codigo) > 0;
}

string Biblioteca::getDescripcionArea(string codigo) {
    if (existeArea(codigo)) {
        return tabla_areas[codigo];
    }
    return "No existe";
}

bool Biblioteca::modificarDescripcionArea(string codigo, string nuevaDesc) {
    if (!existeArea(codigo)) return false;
    tabla_areas[codigo] = nuevaDesc;
    return true;
}

void Biblioteca::mostrarTablaAreas() const {
    if (tabla_areas.empty()) {
        cout << "  La tabla de areas esta vacia." << endl;
        return;
    }
    for (auto const& [clave, valor] : tabla_areas) {
        cout << "  [" << clave << "] - " << valor << endl;
    }
}

bool Biblioteca::agregarArea(string codigo, string descripcion) {
    if (existeArea(codigo)) {
        return false; 
    }
    tabla_areas[codigo] = descripcion;
    
    return conjunto_areas.insertar(codigo);
}

bool Biblioteca::eliminarArea(string codigo) {
    if (!existeArea(codigo)) {
        return false;
    }
    tabla_areas.erase(codigo);
    return conjunto_areas.eliminar(codigo);
}

bool Biblioteca::agregarLibro(Libro nuevo) {
    string cod = nuevo.getArea();
    if (!existeArea(cod)) {
        cout << "Error: El area " << cod << " no existe en TABLA_AREAS." << endl;
        return false;
    }
    Area* areaDestino = conjunto_areas.buscar(cod);
    if (areaDestino != nullptr) {
        areaDestino->insertarLibro(nuevo);
        return true;
    }
    return false;
}

// ARQUITECTURA DE RESPONSABILIDADES: Delegación directa al TDA Areas
Libro* Biblioteca::buscarLibroEnBiblio(ClaveLibro clave) {
    return conjunto_areas.buscarLibro(clave);
}

bool Biblioteca::eliminarLibroTotal(ClaveLibro clave) {
    return conjunto_areas.eliminarLibro(clave);
}

const Areas& Biblioteca::getConjuntoAreas() const {
    return conjunto_areas;
}

Areas& Biblioteca::getConjuntoAreasModificable() {
    return conjunto_areas;
}

const map<string, string>& Biblioteca::getTablaAreas() const {
    return tabla_areas;
}