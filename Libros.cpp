#include "Libros.h"

using namespace std;

// Constructor:dñ Inicializa la lista vacía
Libros::Libros() {
    cabeza = nullptr;
    cantidad = 0;
}

// Destructor: Se llama automaticamente cuando el objeto Libros se destruye.
// Para evitar memory leaks.
Libros::~Libros() {
    vaciarLista();
}

// Vacia la lista liberando la memoria de cada nodo
void Libros::vaciarLista() {
    NodoLibro* actual = cabeza;
    while (actual != nullptr) {
        NodoLibro* aBorrar = actual;
        actual = actual->sig;
        delete aBorrar; 
    }
    cabeza = nullptr;
    cantidad = 0;
}

// Utilidades basicas
int Libros::getCantidad() const { return cantidad; }
bool Libros::estaVacio() const { return cabeza == nullptr; }

// --------------------------------------------------------------

// Inserta un libro manteniendo el orden alfabetico por su ClaveLibro
bool Libros::insertarOrdenado(Libro nuevoLibro) {
    // 1. Verificamos si el libro ya existe; si existe, sumamos stock en lugar de duplicarlo
    Libro* existente = buscar(nuevoLibro.getClave());
    if (existente != nullptr) {
        existente->agregarEjemplares(nuevoLibro.getEjemplares());
        return true;
    }

    NodoLibro* nuevoNodo = new NodoLibro(nuevoLibro);

    // Caso A: La lista está vacia o el nuevo libro va de primero (es menor que la cabeza)
    if (cabeza == nullptr || nuevoLibro.getClave() < cabeza->dato.getClave()) {
        nuevoNodo->sig = cabeza;
        cabeza = nuevoNodo;
        cantidad++;
        return true;
    }

    // Caso B: Inserción en el medio o al final
    NodoLibro* actual = cabeza;
    // Avanzamos mientras haya un siguiente nodo 
    while (actual->sig != nullptr && actual->sig->dato.getClave() < nuevoLibro.getClave()) {
        actual = actual->sig;
    }

    // Insertamos el nuevo nodo entre 'actual' y 'actual->sig'
    nuevoNodo->sig = actual->sig;
    actual->sig = nuevoNodo;
    
    cantidad++;
    return true;
}

// Elimina un libro específico basándose en su clave única
bool Libros::eliminar(ClaveLibro clave) {
    if (estaVacio()) return false; // Si lista vacía

    // Caso particular2: El libro a eliminar es el primero (cabeza)
    if (cabeza->dato.getClave() == clave) {
        NodoLibro* aBorrar = cabeza;
        cabeza = cabeza->sig;
        delete aBorrar;
        cantidad--;
        return true;
    }

    // Busqueda del libro en el resto de la lista
    NodoLibro* actual = cabeza;
    while (actual->sig != nullptr && !(actual->sig->dato.getClave() == clave)) {
        actual = actual->sig;
    }

    // Si salimos del ciclo y sig es nullptr, significa que no lo encontró
    if (actual->sig == nullptr) {
        return false;
    }

    // Si lo encontro, lo desenlazamos y lo borramos
    NodoLibro* aBorrar = actual->sig;
    actual->sig = aBorrar->sig; // Saltamos el nodo
    delete aBorrar; // Lo destruimos
    
    cantidad--;
    return true;
}

// Busca un libro y devuelve su puntero o direccion
Libro* Libros::buscar(ClaveLibro clave) const {
    NodoLibro* actual = cabeza;
    while (actual != nullptr) {
        if (actual->dato.getClave() == clave) {
            return &(actual->dato); // Retorna la direccion de memoria de ese libro
        }
        actual = actual->sig;
    }
    return nullptr; // Retorna nulo si no existe
}


// Retorna un iterador apuntando al primer elemento
IteradorLibros Libros::begin() const {
    return IteradorLibros(cabeza);
}

// Retorna un iterador apuntando al "final" (nullptr)
IteradorLibros Libros::end() const {
    return IteradorLibros(nullptr);
}