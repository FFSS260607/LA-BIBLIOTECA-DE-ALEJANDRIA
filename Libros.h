#ifndef LIBROS_H
#define LIBROS_H

#include "Libro.h"

using namespace std;

// 1. Definicion del Nodo que compone la lista enlazada
struct NodoLibro {
    Libro dato;
    NodoLibro* sig;
    
    // Constructor del nodo para facilitar su creación y asegurar que 'sig' sea nulo
    NodoLibro(Libro _dato) : dato(_dato), sig(nullptr) {}
};

// 2. Definicion del Iterador (ocultar los punteros al main)
class IteradorLibros {
private:
    NodoLibro* actual;

public:
    // Constructor
    IteradorLibros(NodoLibro* nodo) : actual(nodo) {}

    // Sobrecarga del operador de avance (++it)
    IteradorLibros& operator++() {
        if (actual != nullptr) {
            actual = actual->sig;
        }
        return *this;
    }

    // Sobrecarga del operador de desreferencia (*it)
    // Retornamos una REFERENCIA(&) para poder modificar el libro si es necesario
    Libro& operator*() {
        return actual->dato;
    }

    // Sobrecarga del operador de desigualdad (it != end)
    bool operator!=(const IteradorLibros& otro) const {
        return actual != otro.actual;
    }
};

class Libros {
private:
    NodoLibro* cabeza;
    int cantidad; // Contador para saber el total de libros distintos rapidamente

public:
    Libros();
    ~Libros(); // Destructor para limpiar la memoria dinámica

    // Metodos de gestion de la lista
    bool insertarOrdenado(Libro nuevoLibro); // Inserta sin repetir y en orden alfabético
    bool eliminar(ClaveLibro clave);
    
    // Retorna un puntero al libro para poder modificar sus ejemplares desde afuera
    Libro* buscar(ClaveLibro clave) const; 

    // Utilidades
    int getCantidad() const;
    bool estaVacio() const;
    void vaciarLista(); // Elimina todos los nodos

    // Metodos requeridos para el iterador moderno
    IteradorLibros begin() const;
    IteradorLibros end() const;
};

#endif