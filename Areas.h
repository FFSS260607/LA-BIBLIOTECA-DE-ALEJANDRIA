#ifndef AREAS_H
#define AREAS_H

#include "Area.h"

using namespace std;

// Estructura del nodo para la lista simple de las áreas
struct NodoArea {
    Area dato;
    NodoArea* sig;
    
    NodoArea(string _codigo) : dato(_codigo), sig(nullptr) {}
};

// Iterador para soporte de recorridos externos
class IteradorAreas {
private:
    NodoArea* actual;

public:
    IteradorAreas(NodoArea* nodo) : actual(nodo) {}

    IteradorAreas& operator++() {
        if (actual != nullptr) {
            actual = actual->sig;
        }
        return *this;
    }

    Area& operator*() {
        return actual->dato;
    }

    bool operator!=(const IteradorAreas& otro) const {
        return actual != otro.actual;
    }
};

class Areas {
private:
    NodoArea* cabeza;
    int cant; 

public:
    Areas();
    ~Areas(); 

    // Métodos estructurales de la lista
    bool estaVacio() const;
    void vaciar();
    int getCantidad() const;

    // Lógica de negocio del área
    bool insertar(string codigo_area);
    Area* buscar(string cod) const;
    bool eliminar(string cod);

    // OPERACIONES NATIVAS EXIGIDAS POR EL ENUNCIADO PARA LOGÍSITCA DE LIBROS
    Libro* buscarLibro(ClaveLibro clave) const;
    bool eliminarLibro(ClaveLibro clave);

    // Soporte para iteración estructurada
    IteradorAreas begin() const { return IteradorAreas(cabeza); }
    IteradorAreas end() const { return IteradorAreas(nullptr); }
};

#endif