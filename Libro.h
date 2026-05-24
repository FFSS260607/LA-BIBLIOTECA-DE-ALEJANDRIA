#ifndef LIBRO_H
#define LIBRO_H

#include <string>
#include "ClaveLibro.h" 

using namespace std;

class Libro {
private:
    ClaveLibro clave; 
    int anio;
    string area;       
    string editorial;
    int ejemplares;

public:
    // Constructores
    Libro();
    Libro(ClaveLibro _clave, int _anio, string _area, string _editorial, int _ejemplares);

    // Getters
    ClaveLibro getClave() const;
    int getAnio() const;
    string getArea() const;
    string getEditorial() const;
    int getEjemplares() const;

    // Setters
    void setClave(ClaveLibro _clave);
    void setAnio(int _anio);
    void setArea(string _area);
    void setEditorial(string _editorial);
    void setEjemplares(int _ejemplares);

    // Metodos de negocio para el sistema de biblioteca
    bool prestarEjemplar(); 
    void devolverEjemplar();
    bool agregarEjemplares(int cantidad); // Para cuando llega nuevo inventario
    
    // Metodo para imprimir la información en consola
    void imprimirInfo() const;
};

#endif