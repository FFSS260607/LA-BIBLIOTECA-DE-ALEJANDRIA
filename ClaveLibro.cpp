#include "ClaveLibro.h"

using namespace std;

// Constructor por defecto: Basura en memoria
ClaveLibro::ClaveLibro() {
    autores = "Desconocido";
    titulo = "Sin Titulo";
}

// Constructor parametrizado con validación de strings vacíos
ClaveLibro::ClaveLibro(string _autores, string _titulo) {
    autores = (_autores.empty()) ? "Desconocido" : _autores;
    titulo = (_titulo.empty()) ? "Sin Titulo" : _titulo;
}

// Getters 
string ClaveLibro::getAutores() const { return autores; }
string ClaveLibro::getTitulo() const { return titulo; }

// Setters con validacion
void ClaveLibro::setAutores(string _autores) { 
    if (!_autores.empty()) autores = _autores; 
}
void ClaveLibro::setTitulo(string _titulo) { 
    if (!_titulo.empty()) titulo = _titulo; 
}

// Sobrecarga de igualdad: Buscar un libro especifico
bool ClaveLibro::operator==(const ClaveLibro& otraClave) const {
    return (this->autores == otraClave.autores && this->titulo == otraClave.titulo);
}

// Sobrecarga de menor que (<): Ordenar la lista alfabeticamente. 
// Evalua por autor, y si es el mismo, desempata por título.
bool ClaveLibro::operator<(const ClaveLibro& otraClave) const {
    if (this->autores == otraClave.autores) {
        return this->titulo < otraClave.titulo;
    }
    return this->autores < otraClave.autores;
}

// Sobrecarga de mayor que (>)
bool ClaveLibro::operator>(const ClaveLibro& otraClave) const {
    if (this->autores == otraClave.autores) {
        return this->titulo > otraClave.titulo;
    }
    return this->autores > otraClave.autores;
}

/*
 * =========================================================================
 * SOBRECARGA DEL OPERADOR MENOR QUE (<) Y MAYOR QUE (>)
 * =========================================================================
 * ¿Por qué evaluamos primero el autor y luego el título?
 * En una biblioteca real, los libros se ordenan primero por el nombre del 
 * autor alfabéticamente. Sin embargo, un mismo autor puede tener varios libros.
 * * Si (this->autores == otraClave.autores) es verdadero, significa que estamos 
 * comparando dos libros del mismo autor. En ese caso particular, el desempate 
 * lógico para saber cuál va primero en la lista es comparar sus títulos 
 * (this->titulo < otraClave.titulo).
 * * Si los autores son diferentes, simplemente se ordenan por autor ignorando 
 * el título (return this->autores < otraClave.autores).
 * * Ventaja principal:
 * Gracias a esto, cuando usemos la Lista Enlazada en la Fase 2 y hagamos 
 * (nuevoLibro < libroActual), C++ ejecutará esta lógica internamente, 
 * insertando el nodo exactamente en su posición alfabética correcta.
 * =========================================================================
 */