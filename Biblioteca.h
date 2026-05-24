#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <string>
#include <map>
#include "Areas.h"

using namespace std;

class Biblioteca {
private:
    string nombre;
    string direccion;
    Areas conjunto_areas; 
    map<string, string> tabla_areas; // El único contenedor STL permitido por el profesor

public:
    Biblioteca(string _nombre, string _direccion);

    string getNombre() const;
    string getDireccion() const;

    // Metodos de areas
    bool agregarArea(string codigo, string descripcion);
    bool eliminarArea(string codigo);
    bool existeArea(string codigo);
    bool modificarDescripcionArea(string codigo, string nuevaDesc);
    string getDescripcionArea(string codigo); 
    void mostrarTablaAreas() const;

    // Metodos de libros
    bool agregarLibro(Libro nuevo);
    bool eliminarLibroTotal(ClaveLibro clave);
    Libro* buscarLibroEnBiblio(ClaveLibro clave);

    // Para poder iterar las areas desde el main
    const Areas& getConjuntoAreas() const;
    Areas& getConjuntoAreasModificable();
    const map<string, string>& getTablaAreas() const;
};

#endif