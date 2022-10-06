#include <iostream>
#include <pthread.h>

using namespace std;

struct Tienda {
    int id;
    int distancia;
    int pedidosTienda[4];
};

struct Camion {
    int id;
    string tipoCarga;
    bool Ruta[5];
    long carga;
    int TiempoCarga;
    int TiempoRuta;
};

int KmH;
int Tcarga;
int Tdescarga;

void creacionTienda(Tienda t){

}

void *asignacionTienda(void * args){

}

void *CargaDescarga(void * args){

}

void *RutaCamion(void * args){

}



int main() {
    Tienda T[5];
    Camion C[4];

    cout << "Simulacion de Entrega" << endl;

    cout << "\nIngrese la velocidad promedio de los camiones: " << endl;
    cin >> KmH;
    cout << "\nIngrese la velodidad de carga: " << endl;
    cin >> Tcarga;
    cout << "\nIngrese la velocidad de descarga: " << endl;
    cin >> Tdescarga;









    return 0;
}
