#ifndef CLAVELIBRO_H
#define CLAVELIBRO_H

#include <string>

using namespace std; 

class ClaveLibro {

private:
    string autores;
    string titulo;

public:
    // Constructores
    ClaveLibro(); 
    ClaveLibro(string _autores, string _titulo);

    // Getters
    string getAutores() const;
    string getTitulo() const;

    // Setters
    void setAutores(string _autores);
    void setTitulo(string _titulo);

    // Sobrecarga de operadores para facilitar comparaciones y ordenamiento
    bool operator==(const ClaveLibro& otraClave) const;
    bool operator<(const ClaveLibro& otraClave) const;
    bool operator>(const ClaveLibro& otraClave) const;
};

#endif