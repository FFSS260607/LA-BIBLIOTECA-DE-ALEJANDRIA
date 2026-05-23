#include "Areas.h"

using namespace std;

Areas::Areas() {
    cabeza = nullptr;
    cant = 0;
}

Areas::~Areas() {
    vaciar();
}

void Areas::vaciar() {
    NodoArea* aux = cabeza;
    while (aux != nullptr) {
        NodoArea* borrar = aux;
        aux = aux->sig;
        delete borrar; 
    }
    cabeza = nullptr;
    cant = 0;
}

bool Areas::estaVacio() const {
    return cabeza == nullptr;
}

int Areas::getCantidad() const {
    return cant;
}

bool Areas::insertar(string codigo_area) {
    if (buscar(codigo_area) != nullptr) {
        return false; 
    }
    
    NodoArea* nuevo = new NodoArea(codigo_area);
    
    if (cabeza == nullptr || codigo_area < cabeza->dato.getCodigo()) {
        nuevo->sig = cabeza;
        cabeza = nuevo;
        cant++;
        return true;
    }

    NodoArea* aux = cabeza;
    while (aux->sig != nullptr && aux->sig->dato.getCodigo() < codigo_area) {
        aux = aux->sig;
    }

    nuevo->sig = aux->sig;
    aux->sig = nuevo;
    cant++;
    return true;
}

Area* Areas::buscar(string cod) const {
    NodoArea* aux = cabeza;
    while(aux != nullptr) {
        if (aux->dato.getCodigo() == cod) {
            return &(aux->dato); 
        }
        aux = aux->sig;
    }
    return nullptr; 
}

bool Areas::eliminar(string cod) {
    if (estaVacio()) return false;

    if (cabeza->dato.getCodigo() == cod) {
        NodoArea* borrar = cabeza;
        cabeza = cabeza->sig;
        delete borrar;
        cant--;
        return true;
    }

    NodoArea* aux = cabeza;
    while (aux->sig != nullptr && aux->sig->dato.getCodigo() != cod) {
        aux = aux->sig;
    }

    if (aux->sig == nullptr) {
        return false; 
    }

    NodoArea* borrar = aux->sig;
    aux->sig = borrar->sig;
    delete borrar;
    cant--;
    return true;
}

// IMPLEMENTACIÓN DE LAS OPERACIONES NATIVAS DEL ENUNCIADO
Libro* Areas::buscarLibro(ClaveLibro clave) const {
    NodoArea* aux = cabeza;
    while (aux != nullptr) {
        Libro* encontrado = aux->dato.getLibros().buscar(clave);
        if (encontrado != nullptr) {
            return encontrado;
        }
        aux = aux->sig;
    }
    return nullptr;
}

bool Areas::eliminarLibro(ClaveLibro clave) {
    NodoArea* aux = cabeza;
    while (aux != nullptr) {
        if (aux->dato.eliminarLibro(clave)) {
            return true;
        }
        aux = aux->sig;
    }
    return false;
}