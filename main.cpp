#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <clocale> 
#include "Biblioteca.h"

using namespace std;

// Constante con la ruta relativa de la carpeta de textos
const string RUTA_TEXTOS = "TEXTOS/"; 

void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

void reemplazarSubcadena(string& str, const string& buscar, const string& reemplazar) {
    size_t pos = 0;
    while ((pos = str.find(buscar, pos)) != string::npos) {
        str.replace(pos, buscar.length(), reemplazar);
        pos += reemplazar.length();
    }
}

string normalizar(string cad) {
    string aux = trim(cad); 
    transform(aux.begin(), aux.end(), aux.begin(), ::tolower);
    reemplazarSubcadena(aux, "á", "a");
    reemplazarSubcadena(aux, "é", "e");
    reemplazarSubcadena(aux, "í", "i");
    reemplazarSubcadena(aux, "ó", "o");
    reemplazarSubcadena(aux, "ú", "u");
    return aux;
}

bool validarFormatoAutores(string autores) {
    if (autores.empty()) return false;
    if (autores.find(',') == string::npos) return false;
    if (autores.find('.') == string::npos) return false;
    return true;
}

Libro parsearLineaLibro(string linea) {
    size_t posAutores = linea.find("Autores:");
    size_t posTitulo = linea.find("Titulo:");
    size_t posFecha = linea.find("Fecha:");
    size_t posArea = linea.find("Area:");
    size_t posEditorial = linea.find("Editorial:");
    size_t posEjemplares = linea.find("Ejemplares:");

    string autores = "Desconocido", titulo = "Sin titulo", area = "GEN", editorial = "N/A";
    string fechaStr = "", ejemplaresStr = "";

    if(posAutores != string::npos && posTitulo != string::npos)
        autores = trim(linea.substr(posAutores + 8, posTitulo - (posAutores + 8)));
    if(posTitulo != string::npos && posFecha != string::npos)
        titulo = trim(linea.substr(posTitulo + 7, posFecha - (posTitulo + 7)));
    if(posFecha != string::npos && posArea != string::npos)
        fechaStr = trim(linea.substr(posFecha + 6, posArea - (posFecha + 6)));
    if(posArea != string::npos && posEditorial != string::npos)
        area = trim(linea.substr(posArea + 5, posEditorial - (posArea + 5)));
    if(posEditorial != string::npos && posEjemplares != string::npos)
        editorial = trim(linea.substr(posEditorial + 10, posEjemplares - (posEditorial + 10)));
    if(posEjemplares != string::npos)
        ejemplaresStr = trim(linea.substr(posEjemplares + 11));

    int anio = 0, ejem = 0;
    try { if(!fechaStr.empty()) anio = stoi(fechaStr); } catch(...) {}
    try { if(!ejemplaresStr.empty()) ejem = stoi(ejemplaresStr); } catch(...) {}

    ClaveLibro clave(autores, titulo);
    return Libro(clave, anio, area, editorial, ejem);
}

void guardarTodoEnArchivo(Biblioteca& bib, int version) {
    string nombreBib = bib.getNombre();
    for (size_t i = 0; i < nombreBib.length(); i++) {
        if (nombreBib[i] == ' ') nombreBib[i] = '_';
    }
    
    string rutaCompleta = RUTA_TEXTOS + nombreBib + "_" + to_string(version) + ".txt";
    ofstream archivo(rutaCompleta);
    
    if (!archivo.is_open()) {
        cout << "Error: No se pudo guardar el archivo final en: " << rutaCompleta << endl;
        return;
    }
    
    archivo << "Nombre: " << bib.getNombre() << endl;
    archivo << "Direccion: " << bib.getDireccion() << endl;
    
    archivo << "Areas:" << endl;
    auto mapa = bib.getTablaAreas();
    for (auto const& [cod, desc] : mapa) {
        archivo << cod << " " << desc << endl;
    }
    
    archivo << "Libros:" << endl;
    Areas conjAreas = bib.getConjuntoAreas();
    for (auto itA = conjAreas.begin(); itA != conjAreas.end(); ++itA) {
        Area areaActual = *itA;
        Libros misLib = areaActual.getLibros();
        for (auto itL = misLib.begin(); itL != misLib.end(); ++itL) {
            Libro lib = *itL;
            archivo << "Autores: " << lib.getClave().getAutores() 
                    << " Titulo: " << lib.getClave().getTitulo() 
                    << " Fecha: " << lib.getAnio() 
                    << " Area: " << lib.getArea() 
                    << " Editorial: " << lib.getEditorial() 
                    << " Ejemplares: " << lib.getEjemplares() << endl;
        }
    }
    
    archivo.close();
    cout << "\nBiblioteca guardada con exito en TEXTOS como: " << nombreBib + "_" + to_string(version) + ".txt" << endl;
}

Libro* paginacionDeLibros(Libro* arreglo[], int total) {
    if (total == 0) {
        cout << "No se encontraron libros con esos criterios." << endl;
        return nullptr;
    }
    
    int paginaActual = 0;
    int totalPaginas = (total - 1) / 10;
    
    while (true) {
        limpiarPantalla();
        cout << "=== RESULTADOS DE BUSQUEDA (Pag " << (paginaActual + 1) << " de " << (totalPaginas + 1) << ") ===" << endl;
        int inicio = paginaActual * 10;
        int fin = min(inicio + 10, total);
        
        for (int i = inicio; i < fin; i++) {
            cout << (i % 10) + 1 << ") " << arreglo[i]->getClave().getTitulo() << " - " << arreglo[i]->getClave().getAutores() << " (Ejem: " << arreglo[i]->getEjemplares() << ")" << endl;
        }
        
        cout << "------------------------------------------" << endl;
        cout << "Opciones:" << endl;
        if (paginaActual > 0) cout << " 'a' -> Pagina Anterior" << endl;
        if (paginaActual < totalPaginas) cout << " 's' -> Pagina Siguiente" << endl;
        cout << " 1-10 -> Seleccionar libro de esta pagina" << endl;
        cout << " '0' -> Cancelar y volver al menu" << endl;
        cout << "Elija una opcion: ";
        
        string opc;
        cin >> opc;
        
        if (opc == "0") return nullptr;
        if (opc == "a" || opc == "A") {
            if (paginaActual > 0) paginaActual--;
        } else if (opc == "s" || opc == "S") {
            if (paginaActual < totalPaginas) paginaActual++;
        } else {
            int num = 0;
            try { num = stoi(opc); } catch(...) {}
            if (num >= 1 && num <= (fin - inicio)) {
                return arreglo[inicio + (num - 1)];
            } else {
                cout << "Numero invalido. Presione Enter...";
                cin.ignore();
                cin.get();
            }
        }
    }
}

int buscarLibrosParcial(Biblioteca& bib, string apeBusq, string titBusq, Libro* arreglo[], int maxArreglo) {
    string nApe = normalizar(apeBusq);
    string nTit = normalizar(titBusq);
    int encontrados = 0;
    
    Areas conj = bib.getConjuntoAreasModificable();
    for (auto itA = conj.begin(); itA != conj.end(); ++itA) {
        Area& areaActual = *itA;
        Libros& misLib = areaActual.getLibros();
        for (auto itL = misLib.begin(); itL != misLib.end(); ++itL) {
            Libro& lib = *itL;
            string nAutLib = normalizar(lib.getClave().getAutores());
            string nTitLib = normalizar(lib.getClave().getTitulo());
            
            bool coincideAut = (nApe.empty()) ? true : (nAutLib.find(nApe) != string::npos);
            bool coincideTit = (nTit.empty()) ? true : (nTitLib.find(nTit) != string::npos);
            
            if (coincideAut && coincideTit) {
                if (encontrados < maxArreglo) {
                    arreglo[encontrados] = &lib;
                    encontrados++;
                }
            }
        }
    }
    return encontrados;
}

void menuInventario(Biblioteca& bib, int& versionActual) {
    int opcion = 0;
    Libro* arrBuscados[2000]; 

    while (opcion != 11) {
        limpiarPantalla();
        cout << "=========================================" << endl;
        cout << "   GESTION DE INVENTARIO DE LIBROS" << endl;
        cout << "   Biblioteca: " << bib.getNombre() << endl;
        cout << "=========================================" << endl;
        cout << "1) Ver nombre y direccion de la biblioteca" << endl;
        cout << "2) Ver la TABLA AREAS de la biblioteca" << endl;
        cout << "3) Agregar una nueva area a la biblioteca" << endl;
        cout << "4) Modificar la descripcion de un area existente" << endl;
        cout << "5) Eliminar un area de la biblioteca" << endl;
        cout << "6) Agregar libros nuevos a la biblioteca" << endl;
        cout << "7) Agregar ejemplares a libros ya existentes" << endl;
        cout << "8) Eliminar definitivamente libros de la biblioteca" << endl;
        cout << "9) Eliminar ejemplares de libros" << endl;
        cout << "10) Listar libros por criterio (Filtrar)" << endl;
        cout << "11) Salir y guardar cambios" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore(); 

        switch(opcion) {
            case 1: {
                limpiarPantalla();
                cout << "--- DATOS DE LA BIBLIOTECA ---" << endl;
                cout << "Nombre: " << bib.getNombre() << endl;
                cout << "Direccion: " << bib.getDireccion() << endl;
                cout << "\nPresione Enter para volver...";
                cin.get();
                break;
            }
            case 2: {
                limpiarPantalla();
                cout << "--- TABLA DE AREAS ---" << endl;
                bib.mostrarTablaAreas();
                cout << "\nPresione Enter para volver...";
                cin.get();
                break;
            }
            case 3: {
                limpiarPantalla();
                string cod, desc;
                cout << "Ingrese el codigo de la nueva area (ej: INF): ";
                cin >> cod;
                cod = trim(cod);
                cin.ignore();
                cout << "Ingrese la descripcion: ";
                getline(cin, desc);
                desc = trim(desc);
                
                if (bib.agregarArea(cod, desc)) {
                    cout << "Area agregada con exito!" << endl;
                    cout << "Desea cargar libros desde un archivo para esta area? (s/n): ";
                    char resp;
                    cin >> resp;
                    cin.ignore();
                    if (resp == 's' || resp == 'S') {
                        string nomArch;
                        cout << "Ingrese el nombre del archivo de libros: ";
                        getline(cin, nomArch);
                        nomArch = trim(nomArch);
                        
                        string rutaCompleta = RUTA_TEXTOS + nomArch;
                        ifstream fLibros(rutaCompleta);
                        
                        if(fLibros.is_open()){
                            string l;
                            while (getline(fLibros, l)) {
                                l = trim(l);
                                if (!l.empty() && l.find("Autores:") != string::npos) {
                                    Libro lb = parsearLineaLibro(l);
                                    if(lb.getArea() == cod) {
                                        bib.agregarLibro(lb);
                                    }
                                }
                            }
                            fLibros.close();
                            cout << "Libros del archivo processed." << endl;
                        } else {
                            cout << "No se encontro el archivo en TEXTOS." << endl;
                        }
                    }
                } else {
                    cout << "Error: Ese codigo de area ya existe." << endl;
                }
                cout << "\nPresione Enter para volver...";
                cin.get();
                break;
            }
            case 4: {
                limpiarPantalla();
                cout << "--- MODIFICAR DESCRIPCION DE AREA ---" << endl;
                bib.mostrarTablaAreas();
                cout << "Ingrese codigo del area a modificar: ";
                string cod, nuevaDesc;
                cin >> cod;
                cod = trim(cod);
                cin.ignore();
                if(bib.existeArea(cod)) {
                    cout << "Nueva descripcion: ";
                    getline(cin, nuevaDesc);
                    nuevaDesc = trim(nuevaDesc);
                    bib.modificarDescripcionArea(cod, nuevaDesc);
                    cout << "Area modificada correctamente." << endl;
                } else {
                    cout << "Ese codigo no existe." << endl;
                }
                cout << "\nPresione Enter para volver...";
                cin.get();
                break;
            }
            case 5: {
                limpiarPantalla();
                cout << "--- ELIMINAR AREA ---" << endl;
                bib.mostrarTablaAreas();
                string cod;
                cout << "\nIngrese codigo de area a eliminar (ALERTA: borrara sus libros): ";
                cin >> cod;
                cod = trim(cod);
                cin.ignore();
                if(bib.existeArea(cod)) {
                    cout << "Seguro que desea eliminar el area " << cod << "? (s/n): ";
                    char resp;
                    cin >> resp;
                    cin.ignore();
                    if (resp == 's' || resp == 'S') {
                        bib.eliminarArea(cod);
                        cout << "Area eliminada." << endl;
                    }
                } else {
                    cout << "El area no existe." << endl;
                }
                cout << "\nPresione Enter para volver...";
                cin.get();
                break;
            }
            case 6: {
                limpiarPantalla();
                cout << "--- AGREGAR LIBROS NUEVOS ---" << endl;
                cout << "1) Por pantalla" << endl;
                cout << "2) Desde archivo" << endl;
                cout << "Opcion: ";
                int subOp;
                cin >> subOp;
                cin.ignore();
                
                if (subOp == 1) {
                    string aut, tit, area, edit;
                    int anio, cantEjem;
                    cout << "Autores (Ej. Garcia, B. & Lopez, C.): ";
                    getline(cin, aut);
                    
                    if (!validarFormatoAutores(aut)) {
                        cout << "\nError: Formato invalido. Asegurese de usar comas y puntos (Ej. Blanco, M.)." << endl;
                        cout << "Presione Enter para volver...";
                        cin.get();
                        break;
                    }

                    cout << "Titulo: ";
                    getline(cin, tit);
                    cout << "Anio: ";
                    cin >> anio;
                    cin.ignore();
                    
                    cout << "Area (Codigo): ";
                    getline(cin, area);
                    
                    cout << "Editorial: ";
                    getline(cin, edit);
                    cout << "Cantidad de ejemplares: ";
                    cin >> cantEjem;
                    cin.ignore();

                    ClaveLibro c(trim(aut), trim(tit));
                    Libro nuevoL(c, anio, trim(area), trim(edit), cantEjem);
                    if (bib.agregarLibro(nuevoL)) {
                        cout << "Libro agregado con exito!" << endl;
                    } else {
                        cout << "El libro ya existe o el area es incorrecta." << endl;
                    }
                } else if (subOp == 2) {
                    string path;
                    cout << "Nombre del archivo (ej. libros.txt): ";
                    getline(cin, path);
                    path = trim(path);
                    string rutaCompleta = RUTA_TEXTOS + path;
                    ifstream fLibros(rutaCompleta);
                    if (fLibros.is_open()) {
                        string l;
                        int agregados = 0;
                        while (getline(fLibros, l)) {
                            l = trim(l);
                            if (!l.empty() && l.find("Autores:") != string::npos) {
                                Libro lb = parsearLineaLibro(l);
                                if(bib.agregarLibro(lb)) agregados++;
                            }
                        }
                        fLibros.close();
                        cout << "Proceso completado. " << agregados << " libros nuevos agregados." << endl;
                    } else {
                        cout << "Archivo no encontrado." << endl;
                    }
                }
                cout << "\nPresione Enter para volver...";
                cin.get();
                break;
            }
            case 7: {
                limpiarPantalla();
                cout << "--- AGREGAR EJEMPLARES (STOCK) ---" << endl;
                cout << "1) Por pantalla (Buscar)" << endl;
                cout << "2) Desde archivo" << endl;
                cout << "Opcion: ";
                int opcStock;
                cin >> opcStock;
                cin.ignore();
                
                if(opcStock == 1) {
                    string ape, tit;
                    cout << "Apellidos del autor (o Enter para ignorar): ";
                    getline(cin, ape);
                    cout << "Palabra del titulo (o Enter para ignorar): ";
                    getline(cin, tit);
                    
                    int cantEncontrados = buscarLibrosParcial(bib, ape, tit, arrBuscados, 2000);
                    Libro* libSel = paginacionDeLibros(arrBuscados, cantEncontrados);
                    
                    if (libSel != nullptr) {
                        cout << "\nLibro seleccionado: " << libSel->getClave().getTitulo() << endl;
                        cout << "Cuantos ejemplares van a llegar?: ";
                        int num;
                        cin >> num;
                        cin.ignore();
                        libSel->agregarEjemplares(num);
                        cout << "Stock actualizado! Nuevo stock: " << libSel->getEjemplares() << endl;
                    }
                } else if (opcStock == 2) {
                    string path;
                    cout << "Nombre del archivo: ";
                    getline(cin, path);
                    path = trim(path);
                    ifstream fL(RUTA_TEXTOS + path);
                    if (fL.is_open()) {
                        string l;
                        while (getline(fL, l)) {
                            l = trim(l);
                            if (!l.empty() && l.find("Autores:") != string::npos) {
                                Libro lbParse = parsearLineaLibro(l);
                                Libro* existe = bib.buscarLibroEnBiblio(lbParse.getClave());
                                if(existe != nullptr) {
                                    existe->agregarEjemplares(lbParse.getEjemplares());
                                }
                            }
                        }
                        fL.close();
                        cout << "Stock sumado a los libros existentes." << endl;
                    }
                }
                cout << "\nPresione Enter para volver...";
                cin.get();
                break;
            }
            case 8: {
                limpiarPantalla();
                cout << "--- ELIMINAR LIBRO TOTALMENTE ---" << endl;
                cout << "1) Por pantalla (Buscar)" << endl;
                cout << "2) Desde archivo" << endl;
                cout << "Opcion: ";
                int opcDel;
                cin >> opcDel;
                cin.ignore();
                
                if(opcDel == 1) {
                    string ape, tit;
                    cout << "Apellidos del autor (o Enter para ignorar): ";
                    getline(cin, ape);
                    cout << "Palabra del titulo (o Enter para ignorar): ";
                    getline(cin, tit);
                    
                    int cantEncontrados = buscarLibrosParcial(bib, ape, tit, arrBuscados, 2000);
                    Libro* libSel = paginacionDeLibros(arrBuscados, cantEncontrados);
                    
                    if (libSel != nullptr) {
                        ClaveLibro claveBorrar = libSel->getClave();
                        cout << "Seguro que desea eliminar el libro '" << claveBorrar.getTitulo() << "' por completo? (s/n): ";
                        char r;
                        cin >> r;
                        cin.ignore();
                        if (r == 's' || r == 'S') {
                            if(bib.eliminarLibroTotal(claveBorrar)) {
                                cout << "Libro eliminado permanentemente." << endl;
                            }
                        }
                    }
                } else if (opcDel == 2) {
                    string path;
                    cout << "Nombre del archivo a leer: ";
                    getline(cin, path);
                    path = trim(path);
                    ifstream fL(RUTA_TEXTOS + path);
                    if (fL.is_open()) {
                        string l;
                        while (getline(fL, l)) {
                            l = trim(l);
                            if (!l.empty() && l.find("Autores:") != string::npos) {
                                Libro lbParse = parsearLineaLibro(l);
                                bib.eliminarLibroTotal(lbParse.getClave());
                            }
                        }
                        fL.close();
                        cout << "Libros del archivo eliminados (si existian)." << endl;
                    }
                }
                cout << "\nPresione Enter para volver...";
                cin.get();
                break;
            }
            case 9: {
                limpiarPantalla();
                cout << "--- ELIMINAR EJEMPLARES (RESTAR STOCK) ---" << endl;
                cout << "1) Por pantalla (Buscar)" << endl;
                cout << "2) Desde archivo" << endl;
                cout << "Opcion: ";
                int opcRest;
                cin >> opcRest;
                cin.ignore();
                
                if(opcRest == 1) {
                    string ape, tit;
                    cout << "Apellidos del autor (o Enter para ignorar): ";
                    getline(cin, ape);
                    cout << "Palabra del titulo (o Enter para ignorar): ";
                    getline(cin, tit);
                    
                    int cantEncontrados = buscarLibrosParcial(bib, ape, tit, arrBuscados, 2000);
                    Libro* libSel = paginacionDeLibros(arrBuscados, cantEncontrados);
                    
                    if (libSel != nullptr) {
                        cout << "\nLibro seleccionado: " << libSel->getClave().getTitulo() << " (Stock: " << libSel->getEjemplares() << ")" << endl;
                        cout << "Cuantos ejemplares se dañaron/perdieron?: ";
                        int num;
                        cin >> num;
                        cin.ignore();
                        int nuevoStk = libSel->getEjemplares() - num;
                        if(nuevoStk < 0) nuevoStk = 0;
                        libSel->setEjemplares(nuevoStk);
                        cout << "Stock actualizado! Nuevo stock: " << libSel->getEjemplares() << endl;
                    }
                } else if (opcRest == 2) {
                    string path;
                    cout << "Nombre del archivo: ";
                    getline(cin, path);
                    path = trim(path);
                    ifstream fL(RUTA_TEXTOS + path);
                    if (fL.is_open()) {
                        string l;
                        while (getline(fL, l)) {
                            l = trim(l);
                            if (!l.empty() && l.find("Autores:") != string::npos) {
                                Libro lbParse = parsearLineaLibro(l);
                                Libro* existe = bib.buscarLibroEnBiblio(lbParse.getClave());
                                if(existe != nullptr) {
                                    int nStk = existe->getEjemplares() - lbParse.getEjemplares();
                                    if(nStk < 0) nStk = 0;
                                    existe->setEjemplares(nStk);
                                }
                            }
                        }
                        fL.close();
                        cout << "Stock restado a los libros correspondientes." << endl;
                    }
                }
                cout << "\nPresione Enter para volver...";
                cin.get();
                break;
            }
            case 10: {
                limpiarPantalla();
                cout << "--- LISTADO CON MULTIPLES CRITERIOS (FILTRADO) ---" << endl;
                cout << "Deje el campo vacio (presione Enter) si no quiere filtrar por ese criterio." << endl;
                string fApe, fTit, fArea;
                string sfMin, sfMax;
                cout << "Apellidos (ej. Garcia): ";
                getline(cin, fApe);
                cout << "Titulo (ej. Lenguaje): ";
                getline(cin, fTit);
                cout << "Area (ej. INF): ";
                getline(cin, fArea);
                cout << "Desde el anio (ej. 1990): ";
                getline(cin, sfMin);
                cout << "Hasta el anio (ej. 2026): ";
                getline(cin, sfMax);
                
                int anioMin = (trim(sfMin).empty()) ? 0 : stoi(trim(sfMin));
                int anioMax = (trim(sfMax).empty()) ? 9999 : stoi(trim(sfMax));
                
                string nApe = normalizar(fApe);
                string nTit = normalizar(fTit);
                string nAre = normalizar(fArea);
                
                int totalMatch = 0;
                Areas conj = bib.getConjuntoAreasModificable();
                for (auto itA = conj.begin(); itA != conj.end(); ++itA) {
                    Area& ar = *itA;
                    Libros& mLib = ar.getLibros();
                    for (auto itL = mLib.begin(); itL != mLib.end(); ++itL) {
                        Libro& l = *itL;
                        
                        bool passApe = (nApe.empty()) ? true : (normalizar(l.getClave().getAutores()).find(nApe) != string::npos);
                        bool passTit = (nTit.empty()) ? true : (normalizar(l.getClave().getTitulo()).find(nTit) != string::npos);
                        bool passAre = (nAre.empty()) ? true : (normalizar(l.getArea()).find(nAre) != string::npos);
                        bool passFec = (l.getAnio() >= anioMin && l.getAnio() <= anioMax);
                        
                        if (passApe && passTit && passAre && passFec) {
                            if (totalMatch < 2000) {
                                arrBuscados[totalMatch] = &l;
                                totalMatch++;
                            }
                        }
                    }
                }
                
                if (totalMatch == 0) {
                    cout << "\nNingun libro cumple todos esos requisitos." << endl;
                    cout << "Presione Enter...";
                    cin.get();
                } else {
                    int pg = 0;
                    int tPg = (totalMatch - 1) / 10;
                    string o;
                    while (true) {
                        limpiarPantalla();
                        cout << "=== REPORTES DE LIBROS (" << totalMatch << " encontrados) - Pag " << (pg+1) << "/" << (tPg+1) << " ===" << endl;
                        int in = pg * 10;
                        int fn = min(in + 10, totalMatch);
                        for (int k = in; k < fn; k++) {
                            cout << k+1 << ") ";
                            arrBuscados[k]->imprimirInfo();
                        }
                        cout << "Opciones: 'a'(Anterior), 's'(Siguiente), '0'(Salir). Opcion: ";
                        cin >> o;
                        if (o == "0") break;
                        if ((o == "a" || o == "A") && pg > 0) pg--;
                        if ((o == "s" || o == "S") && pg < tPg) pg++;
                    }
                    cin.ignore();
                }
                break;
            }
            case 11: {
                versionActual++; 
                guardarTodoEnArchivo(bib, versionActual);
                cout << "Saliendo del inventario..." << endl;
                break;
            }
            default:
                cout << "Opcion invalida, intente de nuevo." << endl;
                cin.get();
        }
    }
}

int main() {
    setlocale(LC_ALL, "");
    int opcionPrincipal = 0;
    int versionActual = 0;

    while (opcionPrincipal != 3) {
        limpiarPantalla();
        cout << "========================================" << endl;
        cout << "       SISTEMA DE GESTION UCAB" << endl;
        cout << "========================================" << endl;
        cout << "1) Crear una biblioteca nueva" << endl;
        cout << "2) Cargar biblioteca desde un archivo" << endl;
        cout << "3) Salir del programa" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcionPrincipal;
        cin.ignore();

        if (opcionPrincipal == 1) {
            string nom, dir;
            cout << "\nIngrese el nombre de la biblioteca: ";
            getline(cin, nom);
            cout << "Ingrese la direccion: ";
            getline(cin, dir);

            Biblioteca miBiblio(trim(nom), trim(dir));
            versionActual = 0; 
            
            cout << "\n¿Como desea cargar la TABLA AREAS?\n1) Por teclado\n2) Desde archivo\nOpcion: ";
            int opAreas;
            cin >> opAreas;
            cin.ignore();
            
            if (opAreas == 2) {
                string pathAreas;
                cout << "Ingrese nombre de archivo (ej: TABLA_AREAS.txt): ";
                getline(cin, pathAreas);
                pathAreas = trim(pathAreas);
                
                string rutaCompleta = RUTA_TEXTOS + pathAreas;
                ifstream fAreas(rutaCompleta);
                
                if (fAreas.is_open()) {
                    string lineaArea;
                    while (getline(fAreas, lineaArea)) {
                        lineaArea = trim(lineaArea);
                        if(lineaArea.empty()) continue;
                        
                        size_t posDosPuntos = lineaArea.find(':');
                        size_t esp = lineaArea.find(' ');
                        
                        if (posDosPuntos != string::npos) {
                            string c = trim(lineaArea.substr(0, posDosPuntos));
                            string d = trim(lineaArea.substr(posDosPuntos + 1));
                            miBiblio.agregarArea(c, d);
                        } else if(esp != string::npos) {
                            string c = trim(lineaArea.substr(0, esp));
                            string d = trim(lineaArea.substr(esp + 1));
                            miBiblio.agregarArea(c, d);
                        } else {
                            miBiblio.agregarArea(lineaArea, "S/D");
                        }
                    }
                    fAreas.close();
                    cout << "Tabla de areas cargada correctamente desde TEXTOS." << endl;
                } else {
                    cout << "No se pudo abrir el archivo, empezara con areas vacias." << endl;
                }
                cout << "\nPresione Enter para continuar al inventario...";
                cin.get();
            }
            
            menuInventario(miBiblio, versionActual);

        } else if (opcionPrincipal == 2) {
            string nomArchivo;
            cout << "\nIngrese el nombre del archivo de la biblioteca (ej: BIBLIOTECA_UCAB_0.txt): ";
            getline(cin, nomArchivo);
            nomArchivo = trim(nomArchivo);
            
            string rutaCompleta = RUTA_TEXTOS + nomArchivo;
            ifstream archivoIn(rutaCompleta);
            
            if (archivoIn.is_open()) {
                string lineaName, lineaDir;
                getline(archivoIn, lineaName);
                getline(archivoIn, lineaDir);
                
                string nombreB = "";
                string direccionB = "";
                
                size_t pN = lineaName.find(":");
                if(pN != string::npos) nombreB = trim(lineaName.substr(pN + 1));
                
                size_t pD = lineaDir.find(":");
                if(pD != string::npos) direccionB = trim(lineaDir.substr(pD + 1));

                Biblioteca miBiblioCargada(nombreB, direccionB);
                
                int estadoLectura = 0; 
                string lineaArch;
                
                while (getline(archivoIn, lineaArch)) {
                    lineaArch = trim(lineaArch);
                    if (lineaArch.empty()) continue;

                    if (lineaArch.find("Areas:") != string::npos || lineaArch == "Areas") {
                        estadoLectura = 1;
                        continue;
                    } else if (lineaArch.find("Libros:") != string::npos || lineaArch == "Libros") {
                        estadoLectura = 2;
                        continue;
                    }

                    if (estadoLectura == 1) {
                        size_t posDosPuntos = lineaArch.find(':');
                        size_t esp = lineaArch.find(' ');

                        if (posDosPuntos != string::npos) {
                            string c = trim(lineaArch.substr(0, posDosPuntos));
                            string d = trim(lineaArch.substr(posDosPuntos + 1));
                            miBiblioCargada.agregarArea(c, d);
                        } else if (esp != string::npos) {
                            string c = trim(lineaArch.substr(0, esp));
                            string d = trim(lineaArch.substr(esp + 1));
                            miBiblioCargada.agregarArea(c, d);
                        }
                    } else if (estadoLectura == 2) {
                        if (lineaArch.find("Autores:") != string::npos) {
                            Libro l = parsearLineaLibro(lineaArch);
                            miBiblioCargada.agregarLibro(l);
                        }
                    }
                }
                
                size_t sub = nomArchivo.find_last_of("_");
                size_t punto = nomArchivo.find(".txt");
                if (sub != string::npos && punto != string::npos) {
                    versionActual = stoi(nomArchivo.substr(sub + 1, punto - sub - 1));
                } else {
                    versionActual = 0;
                }
                
                archivoIn.close();
                cout << "Biblioteca " << nombreB << " cargada con exito (Version " << versionActual << ")." << endl;
                cout << "\nPresione Enter para continuar...";
                cin.get();
                
                menuInventario(miBiblioCargada, versionActual);
            } else {
                cout << "Error: El archivo no existe en la carpeta TEXTOS o no se pudo abrir." << endl;
                cout << "\nPresione Enter para continuar...";
                cin.get();
            }
        }
    }

    cout << "\n¡Gracias por usar el sistema de gestion de biblioteca UCAB! Éxito en la entrega." << endl;
    return 0;
}