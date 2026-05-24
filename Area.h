#ifndef AREA_H
#define AREA_H

#include <string>
#include "Libros.h"

using namespace std;

class Area {
private:
    string codigo;
    Libros misLibros; // el tda de tu compañero, la lista enlazada de libros

public:
    // constructores
    Area();
    Area(string _codigo);

    // getters y setters basicos
    string getCodigo() const;
    void setCodigo(string _codigo);

    // pasamos por referencia para poder modificar el inventario directamente
    Libros& getLibros(); 
    
    // utilidades
    bool esVacia() const;
    void insertarLibro(Libro nuevo);
    bool eliminarLibro(ClaveLibro clave);
};

#endif