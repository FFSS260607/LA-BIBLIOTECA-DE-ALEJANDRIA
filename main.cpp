#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <filesystem>
#include <cstdlib>
#include <clocale>
#include <cctype>
#include "Biblioteca.h"

using namespace std;

// Constante con la ruta de la carpeta de textos
const string RUTA_TEXTOS = "TEXTOS/"; 

void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// =========================================================================
// HELPER UNIVERSAL: Limpia espacios, \r, \n o tabuladores a los lados
// =========================================================================
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Reemplaza texto de forma segura para evitar roturas en codificación UTF-8
void reemplazarSubcadena(string& str, const string& buscar, const string& reemplazar) {
    size_t pos = 0;
    while ((pos = str.find(buscar, pos)) != string::npos) {
        str.replace(pos, buscar.length(), reemplazar);
        pos += reemplazar.length();
    }
}

// Normaliza textos para realizar búsquedas seguras sin importar tildes
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

// =========================================================================
// PARSEADOR DE LÍNEAS DE LIBROS (EXTRAE LOS CAMPOS LIMPIOS)
// =========================================================================
Libro parsearLineaLibro(string linea) {
    string tagAut = "Autores:";
    string tagTit = "Titulo:";
    string tagFec = "Fecha:";
    string tagAre = "Area:";
    string tagEdi = "Editorial:";
    string tagEje = "Ejemplares:";

    size_t pAut = linea.find(tagAut);
    size_t pTit = linea.find(tagTit);
    size_t pFec = linea.find(tagFec);
    size_t pAre = linea.find(tagAre);
    size_t pEdi = linea.find(tagEdi);
    size_t pEje = linea.find(tagEje);

    string autores = trim(linea.substr(pAut + tagAut.length(), pTit - (pAut + tagAut.length())));
    string titulo = trim(linea.substr(pTit + tagTit.length(), pFec - (pTit + tagTit.length())));
    string fechaStr = trim(linea.substr(pFec + tagFec.length(), pAre - (pFec + tagFec.length())));
    string area = trim(linea.substr(pAre + tagAre.length(), pEdi - (pAre + tagAre.length())));
    string editorial = trim(linea.substr(pEdi + tagEdi.length(), pEje - (pEdi + tagEdi.length())));
    string ejemplaresStr = trim(linea.substr(pEje + tagEje.length()));

    int anio = 0, ejemplares = 0;
    try { anio = stoi(fechaStr); } catch(...) { anio = 0; }
    try { ejemplares = stoi(ejemplaresStr); } catch(...) { ejemplares = 0; }

    ClaveLibro clave(autores, titulo);
    return Libro(clave, anio, area, editorial, ejemplares);
}

void procesarAreasDesconocidas(Biblioteca& b, vector<Libro>& librosPendientes, const set<string>& areasNoDeclaradas) {
    if (areasNoDeclaradas.empty()) return;

    cout << "\nSe detectaron las siguientes areas no declaradas: ";
    for (const auto& codigo : areasNoDeclaradas) {
        cout << codigo << " ";
    }
    cout << "\nDesea crear estas areas ahora y luego agregar los libros con estas areas? (S/N): ";
    string respuesta;
    getline(cin, respuesta);

    if (!respuesta.empty() && (respuesta[0] == 'S' || respuesta[0] == 's')) {
        for (const auto& codigo : areasNoDeclaradas) {
            if (!b.existeArea(codigo)) {
                cout << "Descripcion para el area " << codigo << " (deje vacio para usar descripcion generica): ";
                string desc;
                getline(cin, desc);
                if (desc.empty()) {
                    desc = "Area generada automaticamente";
                }
                b.agregarArea(codigo, desc);
            }
        }

        int agregados = 0;
        for (Libro& libro : librosPendientes) {
            if (b.agregarLibro(libro)) {
                agregados++;
            }
        }
        cout << "\nSe crearon " << areasNoDeclaradas.size() << " areas y se agregaron " << agregados << " libros." << endl;
    } else {
        cout << "\nSe rechazaron " << librosPendientes.size() << " libros con areas no declaradas." << endl;
    }

    cout << "\nPresione Enter para continuar...";
    cin.get();
}

bool esCodigoAreaValido(const string& codigo) {
    if (codigo.size() < 2 || codigo.size() > 4) return false;
    for (char c : codigo) {
        if (!isalpha(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

void procesarAreaEnImportacion(Biblioteca& b, const string& codArea, const string& descArea, int& decisionDuplicados) {
    if (codArea.empty()) return;
    string areaCod = trim(codArea);
    string areaDesc = trim(descArea);
    if (!areaDesc.empty() && areaDesc.back() == '.') {
        areaDesc.pop_back();
    }
    if (b.existeArea(areaCod)) {
        // Si decisionDuplicados == -1, aun no preguntamos al usuario; preguntar una sola vez
        if (decisionDuplicados == -1) {
            cout << "\nEl area '" << areaCod << "' ya existe con descripcion: '" << b.getDescripcionArea(areaCod) << "'." << endl;
            cout << "Desea reemplazarla por la nueva descripcion '" << areaDesc << "'? (S/N) - Esta respuesta se aplicara a todas las siguientes coincidencias: ";
            string respuesta;
            getline(cin, respuesta);
            if (!respuesta.empty() && (respuesta[0] == 'S' || respuesta[0] == 's')) {
                decisionDuplicados = 1;
            } else {
                decisionDuplicados = 0;
            }
        }

        if (decisionDuplicados == 1) {
            b.modificarDescripcionArea(areaCod, areaDesc);
            cout << "Descripcion de area " << areaCod << " actualizada." << endl;
        } else {
            cout << "Se mantiene la descripcion existente de " << areaCod << "." << endl;
        }
    } else {
        b.agregarArea(areaCod, areaDesc);
    }
}

bool importarArchivoBiblioteca(Biblioteca& b, const string& nomArchivo, bool usarMetadata) {
    string rutaCompleta = RUTA_TEXTOS + nomArchivo;
    ifstream archivoIn(rutaCompleta);
    if (!archivoIn.is_open()) {
        cout << "\nError: El archivo no existe en la carpeta TEXTOS o no se pudo abrir." << endl;
        cout << "Asegurese de que este dentro de la carpeta '" << RUTA_TEXTOS << "'." << endl;
        cout << "\nPresione Enter para continuar...";
        cin.get();
        return false;
    }

    string lineaArch;
    string nombreB = "Biblioteca UCAB";
    string direccionB = "Montalban";
    int estadoLectura = 0; // 0: metadata/areas, 1: Areas, 2: Libros
    vector<Libro> librosLeidos;
    vector<Libro> librosPendientes;
    set<string> areasNoDeclaradas;
    map<string, string> areasEnArchivo; // acumula definiciones de areas encontradas en el archivo

    while (getline(archivoIn, lineaArch)) {
        lineaArch = trim(lineaArch);
        if (lineaArch.empty()) continue;

        if (lineaArch.rfind("Autores:", 0) == 0) {
            Libro l = parsearLineaLibro(lineaArch);
            librosLeidos.push_back(l);
            continue;
        }

        bool esDefArea = false;
        if (lineaArch.find(':') != string::npos && lineaArch.rfind("Nombre:", 0) != 0 && lineaArch.rfind("Direccion:", 0) != 0 && lineaArch != "Areas:" && lineaArch.find("Libros:") == string::npos) {
            size_t posDosPuntos = lineaArch.find(':');
            string codArea = trim(lineaArch.substr(0, posDosPuntos));
            if (esCodigoAreaValido(codArea)) {
                esDefArea = true;
            }
        }

        if (estadoLectura == 0) {
            if (lineaArch == "Areas:") {
                estadoLectura = 1;
            } else if (lineaArch.find("Nombre:") != string::npos) {
                nombreB = trim(lineaArch.substr(7));
                if (usarMetadata) {
                    b = Biblioteca(nombreB, direccionB);
                }
            } else if (lineaArch.find("Direccion:") != string::npos) {
                direccionB = trim(lineaArch.substr(10));
                if (usarMetadata) {
                    b = Biblioteca(nombreB, direccionB);
                }
            } else if (esDefArea) {
                size_t posDosPuntos = lineaArch.find(':');
                string codArea = trim(lineaArch.substr(0, posDosPuntos));
                string descArea = trim(lineaArch.substr(posDosPuntos + 1));
                areasEnArchivo[codArea] = descArea;
            }
        } else if (estadoLectura == 1) {
            if (lineaArch.find("Libros:") != string::npos) {
                estadoLectura = 2;
            } else if (lineaArch.find(':') != string::npos) {
                size_t posDosPuntos = lineaArch.find(':');
                string codArea = trim(lineaArch.substr(0, posDosPuntos));
                string descArea = trim(lineaArch.substr(posDosPuntos + 1));
                areasEnArchivo[codArea] = descArea;
            }
        } else if (estadoLectura == 2) {
            if (lineaArch.rfind("Autores:", 0) == 0) {
                Libro l = parsearLineaLibro(lineaArch);
                if (!b.existeArea(l.getArea())) {
                    librosPendientes.push_back(l);
                    areasNoDeclaradas.insert(l.getArea());
                } else {
                    b.agregarLibro(l);
                }
            }
        }
    }

    archivoIn.close();

    // Procesar las definiciones de areas recogidas en el archivo.
    if (!areasEnArchivo.empty()) {
        vector<string> duplicadas;
        for (auto &p : areasEnArchivo) {
            string cod = p.first;
            string desc = p.second;
            if (!desc.empty() && desc.back() == '.') desc.pop_back();
            if (b.existeArea(cod)) duplicadas.push_back(cod);
        }

        if (!duplicadas.empty()) {
            cout << "\nSe encontraron las siguientes areas ya existentes: ";
            for (const auto &c : duplicadas) cout << c << " ";
            cout << "\nDesea reemplazar sus descripciones por las nuevas del archivo? (S/N) - Esta respuesta se aplicara a todas las coincidencias: ";
            string resp; getline(cin, resp);
            bool reemplazar = (!resp.empty() && (resp[0] == 'S' || resp[0] == 's'));

            for (auto &p : areasEnArchivo) {
                string cod = p.first;
                string desc = p.second;
                if (!desc.empty() && desc.back() == '.') desc.pop_back();
                if (b.existeArea(cod)) {
                    if (reemplazar) {
                        b.modificarDescripcionArea(cod, desc);
                        cout << "Descripcion de area " << cod << " actualizada." << endl;
                    } else {
                        cout << "Se mantiene la descripcion existente de " << cod << "." << endl;
                    }
                } else {
                    b.agregarArea(cod, desc);
                }
            }
        } else {
            // Ninguna duplicada: agregar todas
            for (auto &p : areasEnArchivo) {
                string cod = p.first;
                string desc = p.second;
                if (!desc.empty() && desc.back() == '.') desc.pop_back();
                b.agregarArea(cod, desc);
            }
        }
    }

    // Agregar los libros que ahora tienen un area existente; los demas quedan pendientes.
    for (Libro &lib : librosLeidos) {
        if (b.existeArea(lib.getArea())) {
            b.agregarLibro(lib);
        } else {
            librosPendientes.push_back(lib);
            areasNoDeclaradas.insert(lib.getArea());
        }
    }

    procesarAreasDesconocidas(b, librosPendientes, areasNoDeclaradas);
    return true;
}

// Lista los archivos .txt dentro de la carpeta RUTA_TEXTOS
vector<string> listarArchivosTextos() {
    vector<string> archivos;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(RUTA_TEXTOS)) {
            if (!entry.is_regular_file()) continue;
            string nombre = entry.path().filename().string();
            if (nombre.size() >= 4 && nombre.substr(nombre.size()-4) == ".txt") {
                archivos.push_back(nombre);
            }
        }
    } catch(...) {
        // Si no existe la carpeta o no se puede leer, devolvemos vector vacio
    }
    return archivos;
}

string seleccionarArchivoTexto() {
    vector<string> archivos = listarArchivosTextos();
    if (archivos.empty()) {
        cout << "\nNo se encontraron archivos .txt en la carpeta '" << RUTA_TEXTOS << "'." << endl;
        return "";
    }

    cout << "\nSeleccione el archivo a usar:" << endl;
    for (size_t i = 0; i < archivos.size(); ++i) {
        cout << "  " << (i + 1) << ". " << archivos[i] << endl;
    }
    cout << "  0. Cancelar" << endl;
    cout << "Seleccione numero de archivo: ";

    string sel;
    getline(cin, sel);
    if (sel.empty()) return "";

    try {
        int idx = stoi(sel);
        if (idx <= 0 || idx > (int)archivos.size()) {
            cout << "\nSeleccion invalida. No se cargara ningun archivo." << endl;
            return "";
        }
        return archivos[idx - 1];
    } catch(...) {
        cout << "\nEntrada invalida. No se cargara ningun archivo." << endl;
        return "";
    }
}

// Buscar coincidencias (autor/titulo) y devolver punteros a Libros encontrados
vector<Libro*> buscarCoincidenciasLibros(Biblioteca& b, const string& autorBusq, const string& tituloBusq) {
    vector<Libro*> matches;
    Areas& areas = b.getConjuntoAreasModificable();
    string na = normalizar(autorBusq);
    string nt = normalizar(tituloBusq);

    for (auto itArea = areas.begin(); itArea != areas.end(); ++itArea) {
        Area& a = *itArea;
        Libros& libros = a.getLibros();
        for (auto itLibro = libros.begin(); itLibro != libros.end(); ++itLibro) {
            Libro& L = *itLibro;
            bool okAutor = true;
            bool okTitulo = true;
            if (!na.empty()) okAutor = (normalizar(L.getClave().getAutores()).find(na) != string::npos);
            if (!nt.empty()) okTitulo = (normalizar(L.getClave().getTitulo()).find(nt) != string::npos);
            if (okAutor && okTitulo) matches.push_back(&L);
        }
    }
    return matches;
}

// Muestra resultados paginados y permite seleccionar uno. Retorna nullptr si cancela.
Libro* mostrarResultadosPaginadosYSeleccion(vector<Libro*>& resultados) {
    if (resultados.empty()) return nullptr;
    const int TAM = 10;
    int pagina = 0;
    while (true) {
        limpiarPantalla();
        int total = resultados.size();
        int totalPag = (total + TAM - 1) / TAM; if (totalPag == 0) totalPag = 1;
        if (pagina < 0) pagina = 0; if (pagina >= totalPag) pagina = totalPag-1;
        int inicio = pagina * TAM;
        int fin = min(inicio + TAM, total);

        cout << "Resultados (Pag " << (pagina+1) << "/" << totalPag << "):" << endl;
        for (int i = inicio; i < fin; ++i) {
            Libro* l = resultados[i];
            cout << "  " << (i - inicio + 1) << ". [" << l->getArea() << "] " << l->getClave().getTitulo()
                 << " - " << l->getClave().getAutores() << " (" << l->getAnio() << ") - " << l->getEjemplares() << " disp." << endl;
        }

        cout << "\nIngrese numero para seleccionar (Enter para cancelar), N para siguiente, P para anterior: ";
        string opt; getline(cin, opt);
        if (opt.empty()) return nullptr;
        if (opt == "N" || opt == "n") { if (pagina < totalPag-1) pagina++; continue; }
        if (opt == "P" || opt == "p") { if (pagina > 0) pagina--; continue; }
        // intentar parsear numero
        try {
            int sel = stoi(opt);
            if (sel >= 1 && sel <= (fin - inicio)) {
                return resultados[inicio + sel - 1];
            }
        } catch(...) {}
        // entrada invalida -> volver a mostrar
    }
    return nullptr;
}

// =========================================================================
// MENÚ DE GESTIÓN DE INVENTARIO (CON PAGINACIÓN DE 10 EN 10)
// =========================================================================
void menuInventario(Biblioteca& b, int& version) {
    int opcion = -1;
    int paginaActual = 0;
    const int TAMANO_PAGINA = 10;

    while (opcion != 0) {
        limpiarPantalla();
        
        // Almacenamos temporalmente los libros disponibles para poder paginar
        vector<Libro*> listaTemporal;
        Areas& areasModificables = b.getConjuntoAreasModificable();
        
        for (auto itArea = areasModificables.begin(); itArea != areasModificables.end(); ++itArea) {
            Area& areaActual = *itArea;
            Libros& librosEnArea = areaActual.getLibros();
            for (auto itLibro = librosEnArea.begin(); itLibro != librosEnArea.end(); ++itLibro) {
                listaTemporal.push_back(&(*itLibro));
            }
        }

        int totalLibros = listaTemporal.size();
        int totalPaginas = (totalLibros + TAMANO_PAGINA - 1) / TAMANO_PAGINA;
        if (totalPaginas == 0) totalPaginas = 1;

        if (paginaActual >= totalPaginas) paginaActual = totalPaginas - 1;
        if (paginaActual < 0) paginaActual = 0;

        int inicio = paginaActual * TAMANO_PAGINA;
        int fin = min(inicio + TAMANO_PAGINA, totalLibros);

        cout << "=================================================================" << endl;
        cout << "   SISTEMA DE INVENTARIO - " << b.getNombre() << " (Pag. " << (paginaActual + 1) << "/" << totalPaginas << ")" << endl;
        cout << "=================================================================" << endl;

        if (totalLibros == 0) {
            cout << "  No hay libros cargados en el sistema actualmente." << endl;
        } else {
            for (int i = inicio; i < fin; i++) {
                Libro* l = listaTemporal[i];
                cout << "  " << (i - inicio + 1) << ". [" << l->getArea() << "] " 
                     << l->getClave().getTitulo() << " - " << l->getClave().getAutores() 
                     << " (" << l->getEjemplares() << " disp.)" << endl;
            }
        }

        cout << "-----------------------------------------------------------------" << endl;
        if (fin < totalLibros) cout << "  11. Siguiente pagina" << endl;
        if (paginaActual > 0)  cout << "  12. Pagina anterior" << endl;
        cout << "-----------------------------------------------------------------" << endl;
        cout << "  1. Buscar un libro por Titulo / Autor" << endl;
        cout << "  2. Prestar un ejemplar" << endl;
        cout << "  3. Devolver un ejemplar" << endl;
        cout << "  4. Insertar nuevo libro manualmente" << endl;
        cout << "  5. Eliminar un libro por completo" << endl;
        cout << "  6. Ver catalogo de Areas de conocimiento" << endl;
        cout << "  7. Modificar descripcion de un Area" << endl;
        cout << "  8. Guardar cambios (Nueva Version)" << endl;
        cout << "  9. Importar archivos (libros/areas)" << endl;
        cout << " 10. Agregar area manualmente" << endl;
        cout << "  0. Cerrar sesion de biblioteca y salir" << endl;
        cout << "-----------------------------------------------------------------" << endl;
        cout << "Seleccione una opcion: ";
        
        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n'); 

        if (opcion == 11 && fin < totalLibros) {
            paginaActual++;
        } 
        else if (opcion == 12 && paginaActual > 0) {
            paginaActual--;
        }
        else if (opcion == 1) {
            limpiarPantalla();
            cout << "=== BUSCAR LIBRO ===" << endl;
            cout << "Ingrese el termino de busqueda (Titulo o Autor): ";
            string busqueda;
            getline(cin, busqueda);
            string normalizadoBusqueda = normalizar(busqueda);

            cout << "\nResultados encontrados:" << endl;
            bool hallado = false;
            for (Libro* l : listaTemporal) {
                if (normalizar(l->getClave().getTitulo()).find(normalizadoBusqueda) != string::npos ||
                    normalizar(l->getClave().getAutores()).find(normalizadoBusqueda) != string::npos) {
                    l->imprimirInfo();
                    cout << "-----------------------------------" << endl;
                    hallado = true;
                }
            }
            if (!hallado) cout << "No se encontraron coincidencias." << endl;
            cout << "\nPresione Enter para regresar...";
            cin.get();
        }
        else if (opcion == 2) {
            limpiarPantalla();
            cout << "=== PRESTAR EJEMPLAR (BUSCAR) ===" << endl;
            cout << "Ingrese Autor (parcial, Enter para omitir): ";
            string aut; getline(cin, aut);
            cout << "Ingrese Titulo (parcial, Enter para omitir): ";
            string tit; getline(cin, tit);

            if (aut.empty() && tit.empty()) {
                cout << "\nBusqueda vacia. Volviendo..." << endl;
                cin.get();
                continue;
            }

            vector<Libro*> resultados = buscarCoincidenciasLibros(b, aut, tit);
            if (resultados.empty()) {
                cout << "\nNo se encontro ningun libro con esas especificaciones." << endl;
                cout << "\nPresione Enter para regresar...";
                cin.get();
                continue;
            }
            Libro* seleccionado = mostrarResultadosPaginadosYSeleccion(resultados);
            if (seleccionado == nullptr) continue;

            if (seleccionado->prestarEjemplar()) {
                cout << "\nPrestamo realizado. Quedan " << seleccionado->getEjemplares() << " ejemplares." << endl;
            } else {
                cout << "\nError: No hay ejemplares disponibles." << endl;
            }
            cout << "\nPresione Enter para continuar...";
            cin.get();
        }
                else if (opcion == 3) {
                    limpiarPantalla();
                    cout << "=== DEVOLVER EJEMPLAR (BUSCAR) ===" << endl;
                    cout << "Ingrese Autor (parcial, Enter para omitir): ";
                    string aut; getline(cin, aut);
                    cout << "Ingrese Titulo (parcial, Enter para omitir): ";
                    string tit; getline(cin, tit);

                    if (aut.empty() && tit.empty()) {
                        cout << "\nBusqueda vacia. Volviendo..." << endl;
                        cin.get();
                        continue;
                    }

                    vector<Libro*> resultados = buscarCoincidenciasLibros(b, aut, tit);
                    if (resultados.empty()) {
                        cout << "\nNo se encontro ningun libro con esas especificaciones." << endl;
                        cout << "\nPresione Enter para regresar...";
                        cin.get();
                        continue;
                    }
                    Libro* seleccionado = mostrarResultadosPaginadosYSeleccion(resultados);
                    if (seleccionado == nullptr) continue;

                    seleccionado->devolverEjemplar();
                    cout << "\nDevolucion registrada. Nuevo stock: " << seleccionado->getEjemplares() << " ejemplares." << endl;
                    cout << "\nPresione Enter para continuar...";
                    cin.get();
                }
                else if (opcion == 9) {
                    limpiarPantalla();
                    cout << "=== IMPORTAR ARCHIVO (TEXTOS) ===" << endl;
                    vector<string> archivos = listarArchivosTextos();
                    if (archivos.empty()) {
                        cout << "No se encontraron archivos .txt en la carpeta '" << RUTA_TEXTOS << "'." << endl;
                        cout << "Por favor agregue el archivo en esa carpeta y reintente." << endl;
                        cout << "\nPresione Enter para continuar...";
                        cin.get();
                        continue;
                    }
                    for (size_t i = 0; i < archivos.size(); ++i) {
                        cout << "  " << (i+1) << ". " << archivos[i] << endl;
                    }
                    cout << "Seleccione numero de archivo (Enter para cancelar): ";
                    string sel; getline(cin, sel);
                    if (sel.empty()) continue;
                    try {
                        int idx = stoi(sel);
                        if (idx >= 1 && idx <= (int)archivos.size()) {
                            importarArchivoBiblioteca(b, archivos[idx-1], false);
                            cout << "\nImportacion finalizada." << endl;
                        } else {
                            cout << "Seleccion invalida." << endl;
                        }
                    } catch(...) {
                        cout << "Entrada invalida." << endl;
                    }
                    cout << "\nPresione Enter para continuar...";
                    cin.get();
                }
        
        else if (opcion == 4) {
            limpiarPantalla();
            cout << "=== MANUALLY INSERT BOOK ===" << endl;
            cout << "Autores: "; string aut; getline(cin, aut);
            cout << "Titulo: "; string tit; getline(cin, tit);
            cout << "Año de publicacion: "; int anio; cin >> anio; cin.ignore(10000, '\n');
            cout << "Codigo de Area (Ej: INF, MAT): "; string area; getline(cin, area);
            cout << "Editorial: "; string edi; getline(cin, edi);
            cout << "Cantidad de ejemplares: "; int ej; cin >> ej; cin.ignore(10000, '\n');

            ClaveLibro c(aut, tit);
            Libro nuevo(c, anio, area, edi, ej);
            if (b.agregarLibro(nuevo)) {
                cout << "\nLibro guardado e indexado en su area correspondiente." << endl;
            } else {
                if (!b.existeArea(area)) {
                    cout << "\nEl area \"" << area << "\" no existe en el sistema. Desea crearla ahora? (S/N): ";
                    string resp; getline(cin, resp);
                    if (!resp.empty() && (resp[0] == 'S' || resp[0] == 's')) {
                        cout << "Descripcion para el area " << area << " (deje vacio para usar descripcion generica): ";
                        string desc; getline(cin, desc);
                        if (desc.empty()) desc = "Area creada manualmente";
                        if (b.agregarArea(area, desc)) {
                            if (b.agregarLibro(nuevo)) {
                                cout << "\nArea creada y libro guardado e indexado en su area correspondiente." << endl;
                            } else {
                                cout << "\nError: No se pudo agregar el libro aun despues de crear el area." << endl;
                            }
                        } else {
                            cout << "\nError: No se pudo crear el area (ya existe o fallo interno)." << endl;
                        }
                    } else {
                        cout << "\nOperacion abortada. Libro no guardado." << endl;
                    }
                } else {
                    cout << "\nError al agregar el libro." << endl;
                }
            }
            cout << "\nPresione Enter para continuar...";
            cin.get();
        }

        else if (opcion == 10) {
            limpiarPantalla();
            cout << "=== AGREGAR AREA MANUALMENTE ===" << endl;
            cout << "Ingrese codigo del area (3 letras, por ejemplo INF): ";
            string cod; getline(cin, cod);
            cod = trim(cod);
            bool valido = true;
            if (cod.size() != 3) valido = false;
            for (char ch : cod) if (!isalpha(static_cast<unsigned char>(ch))) valido = false;
            if (!valido) {
                cout << "\nCodigo invalido. Debe contener exactamente 3 letras." << endl;
                cout << "\nPresione Enter para continuar...";
                cin.get();
                continue;
            }
            cout << "Descripcion del area: ";
            string desc; getline(cin, desc);
            if (desc.empty()) desc = "Area creada manualmente";
            if (b.existeArea(cod)) {
                cout << "\nEl area '" << cod << "' ya existe con descripcion: '" << b.getDescripcionArea(cod) << "'." << endl;
                cout << "Desea reemplazarla por la nueva descripcion '" << desc << "'? (S/N): ";
                string resp; getline(cin, resp);
                if (!resp.empty() && (resp[0] == 'S' || resp[0] == 's')) {
                    if (b.modificarDescripcionArea(cod, desc)) {
                        cout << "\nDescripcion de area " << cod << " actualizada." << endl;
                    } else {
                        cout << "\nError: No se pudo actualizar la descripcion." << endl;
                    }
                } else {
                    cout << "\nSe mantiene la descripcion existente de " << cod << "." << endl;
                }
            } else {
                if (b.agregarArea(cod, desc)) {
                    cout << "\nArea '" << cod << "' creada con exito." << endl;
                } else {
                    cout << "\nError: No se pudo crear el area." << endl;
                }
            }
            cout << "\nPresione Enter para continuar...";
            cin.get();
        }
        else if (opcion == 5) {
            limpiarPantalla();
            cout << "=== ELIMINAR LIBRO TOTAL ===" << endl;
            cout << "Autores: "; string aut; getline(cin, aut);
            cout << "Titulo: "; string tit; getline(cin, tit);

            ClaveLibro c(aut, tit);
            if (b.eliminarLibroTotal(c)) {
                cout << "\nEl libro y todos sus registros fueron borrados de la memoria." << endl;
            } else {
                cout << "\nNo se encontro el libro especificado." << endl;
            }
            cout << "\nPresione Enter para continuar...";
            cin.get();
        }
        else if (opcion == 6) {
            limpiarPantalla();
            cout << "=== CATALOGO DE AREAS REGISTRADAS ===" << endl;
            b.mostrarTablaAreas();
            cout << "\nPresione Enter para regresar...";
            cin.get();
        }
        else if (opcion == 7) {
            limpiarPantalla();
            cout << "=== MODIFICAR DESCRIPCION DE AREA ===" << endl;
            cout << "Ingrese el codigo del area (Ej: INF): ";
            string cod; getline(cin, cod);
            if (b.existeArea(cod)) {
                cout << "Descripcion actual: " << b.getDescripcionArea(cod) << endl;
                cout << "Nueva descripcion: ";
                string nueva; getline(cin, nueva);
                b.modificarDescripcionArea(cod, nueva);
                cout << "\nArea actualizada correctamente." << endl;
            } else {
                cout << "\nEse codigo de area no existe." << endl;
            }
            cout << "\nPresione Enter para continuar...";
            cin.get();
        }
        else if (opcion == 8) {
            limpiarPantalla();
            version++;
            string nuevoNombreArchivo = RUTA_TEXTOS + "BIBLIOTECA_UCAB_" + to_string(version) + ".txt";
            ofstream archOut(nuevoNombreArchivo);
            
            if (archOut.is_open()) {
                archOut << "Nombre: " << b.getNombre() << "\n";
                archOut << "Direccion: " << b.getDireccion() << "\n";
                archOut << "Areas:\n";
                
                for (auto const& [clave, valor] : b.getTablaAreas()) {
                    archOut << clave << ": " << valor << ".\n";
                }
                
                archOut << "Libros:\n";
                for (Libro* l : listaTemporal) {
                    archOut << "Autores: " << l->getClave().getAutores() 
                            << "  Titulo: " << l->getClave().getTitulo() 
                            << "  Fecha: " << l->getAnio() 
                            << "  Area: " << l->getArea() 
                            << "  Editorial: " << l->getEditorial() 
                            << "  Ejemplares: " << l->getEjemplares() << "\n";
                }
                archOut.close();
                cout << "=== GUARDADO EXITOSO ===" << endl;
                cout << "Se ha generado el archivo de respaldo seguro: " << nuevoNombreArchivo << endl;
            } else {
                cout << "Error critico al escribir en el disco." << endl;
                version--;
            }
            cout << "\nPresione Enter para continuar...";
            cin.get();
        }
    }
}

// =========================================================================
// HILO PRINCIPAL DEL PROGRAMA
// =========================================================================
int main() {
    // Configura el idioma nativo del sistema operativo para tildes y eñes
    setlocale(LC_ALL, ""); 

    int opcionPrincipal = -1;

    while (opcionPrincipal != 0) {
        limpiarPantalla();
        cout << "======================================================" << endl;
        cout << "      SISTEMA DE GESTION BIBLIOTECARIA UCAB           " << endl;
        cout << "======================================================" << endl;
        cout << "  1. Crear biblioteca desde cero" << endl;
        cout << "  2. Cargar base de datos desde un archivo (.txt)" << endl;
        cout << "  0. Salir de la aplicacion" << endl;
        cout << "------------------------------------------------------" << endl;
        cout << "Seleccione una opcion: ";
        
        if (!(cin >> opcionPrincipal)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n');

        if (opcionPrincipal == 1) {
            limpiarPantalla();
            cout << "=== CREAR BIBLIOTECA DESDE CERO ===" << endl;
            cout << "Nombre de la biblioteca: ";
            string nombreB;
            getline(cin, nombreB);
            cout << "Direccion de la biblioteca: ";
            string direccionB;
            getline(cin, direccionB);

            if (nombreB.empty()) nombreB = "Biblioteca UCAB";
            if (direccionB.empty()) direccionB = "Montalban";

            Biblioteca miBiblioCargada(nombreB, direccionB);
            int versionActual = 0;

            cout << "\nBiblioteca creada desde cero." << endl;
            cout << "Desea importar areas y libros desde un archivo? (S/N): ";
            string respuestaImportar;
            getline(cin, respuestaImportar);
            if (!respuestaImportar.empty() && (respuestaImportar[0] == 'S' || respuestaImportar[0] == 's')) {
                string nomArchivo = seleccionarArchivoTexto();
                if (!nomArchivo.empty()) {
                    importarArchivoBiblioteca(miBiblioCargada, nomArchivo, false);
                } else {
                    cout << "\nNo se selecciono ningun archivo. Se continuara con la biblioteca vacia." << endl;
                    cout << "\nPresione Enter para continuar...";
                    cin.get();
                }
            }
            cout << "\nPresione Enter para ingresar al panel de control...";
            cin.get();
            menuInventario(miBiblioCargada, versionActual);
        }
        else if (opcionPrincipal == 2) {
            limpiarPantalla();
            cout << "=== ARCHIVO FUENTE ===" << endl;
            string nomArchivo = seleccionarArchivoTexto();

            Biblioteca miBiblioCargada("Biblioteca UCAB", "Montalban");
            int versionActual = 0;

            if (!nomArchivo.empty()) {
                if (importarArchivoBiblioteca(miBiblioCargada, nomArchivo, true)) {
                    size_t sub = nomArchivo.find_last_of("_");
                    size_t punto = nomArchivo.find(".txt");
                    if (sub != string::npos && punto != string::npos) {
                        try {
                            versionActual = stoi(nomArchivo.substr(sub + 1, punto - sub - 1));
                        } catch(...) {
                            versionActual = 0;
                        }
                    } else {
                        versionActual = 0;
                    }

                    cout << "\n>> ¡Felicidades! Base de datos sincronizada con éxito." << endl;
                    cout << "Presione Enter para ingresar al panel de control...";
                    cin.get();
                    menuInventario(miBiblioCargada, versionActual);
                }
            } else {
                cout << "\nNo se selecciono ningun archivo. Se regresara al menu principal." << endl;
                cout << "Presione Enter para continuar...";
                cin.get();
            }
        }
        
    }

    cout << "\n¡Gracias por usar el sistema de gestion de biblioteca UCAB! Exito en la entrega." << endl;
    return 0;
}