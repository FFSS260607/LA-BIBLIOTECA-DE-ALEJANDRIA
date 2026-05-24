#include "Area.h"

using namespace std;

// constructor por defecto
Area::Area() {
    codigo = "SIN_CODIGO";
}

// constructor parametrizado
Area::Area(string _codigo) {
    codigo = _codigo;
}

string Area::getCodigo() const { 
    return codigo; 
}

void Area::setCodigo(string _codigo) { 
    codigo = _codigo; 
}

// retorna la lista de libros. importante el & para no copiar toda la lista
Libros& Area::getLibros() {
    return misLibros;
}

bool Area::esVacia() const {
    return misLibros.estaVacio();
}

void Area::insertarLibro(Libro nuevo) {
    // usamos la funcion que ya hizo tu pana
    misLibros.insertarOrdenado(nuevo);
}

bool Area::eliminarLibro(ClaveLibro clave) {
    // esto devuelve true si lo elimino o false si no estaba
    return misLibros.eliminar(clave);
}