#include "Libro.h"
#include <iostream>

using namespace std;

// Constructor por defecto
Libro::Libro() {
    anio = 0;
    area = "GEN"; 
    editorial = "Desconocida";
    ejemplares = 0;
}

// Constructor parametrizado con validaciones defensivas
Libro::Libro(ClaveLibro _clave, int _anio, string _area, string _editorial, int _ejemplares) {
    clave = _clave;
    
    // Evitar años negativos o irreales. Si mandan algo ilógico se asigna 0 por defecto.
    anio = (_anio < 0) ? 0 : _anio;
    
    // Validar strings vacíos
    area = (_area.empty()) ? "GEN" : _area;
    editorial = (_editorial.empty()) ? "Desconocida" : _editorial;

    // Control de inventario: No pueden existir copias negativas
    ejemplares = (_ejemplares < 0) ? 0 : _ejemplares;
}

// Getters
ClaveLibro Libro::getClave() const { return clave; }
int Libro::getAnio() const { return anio; }
string Libro::getArea() const { return area; }
string Libro::getEditorial() const { return editorial; }
int Libro::getEjemplares() const { return ejemplares; }

// Setters con proteccion
void Libro::setClave(ClaveLibro _clave) { clave = _clave; }

void Libro::setAnio(int _anio) { 
    if (_anio >= 0) anio = _anio; 
}

void Libro::setArea(string _area) { 
    if (!_area.empty()) area = _area; 
}

void Libro::setEditorial(string _editorial) { 
    if (!_editorial.empty()) editorial = _editorial; 
}

void Libro::setEjemplares(int _ejemplares) { 
    if (_ejemplares >= 0) ejemplares = _ejemplares; 
}

//-------------------------------------------------------


// Intenta prestar un libro. Retorna false si no hay stock (evita numeros negativos).
bool Libro::prestarEjemplar() {
    if (ejemplares > 0) {
        ejemplares--;
        return true; 
    }
    return false; // El programa cliente puede usar esto para mostrar un error al usuario
}

// Devuelve un libro (suma 1 al stock)
void Libro::devolverEjemplar() {
    ejemplares++;
}

// Añade un lote de libros (solo si la cantidad es positiva)
bool Libro::agregarEjemplares(int cantidad) {
    if (cantidad > 0) {
        ejemplares += cantidad;
        return true;
    }
    return false;
}

// Muestra los datos organizados (para el menu)
void Libro::imprimirInfo() const {
    cout << "  Titulo: " << clave.getTitulo() << endl;
    cout << "  Autores: " << clave.getAutores() << endl;
    cout << "  Anio: " << anio << " | Area: " << area << " | Editorial: " << editorial << endl;
    cout << "  Ejemplares Disponibles: " << ejemplares << endl;
    cout << "---------------------------------------------------" << endl;
}