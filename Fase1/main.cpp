#include <iostream>
#include <pthread.h>

using namespace std;

struct Tienda {
    int id;
    int distancia;
    int pedidos[4];
};

struct Camion {
    int id;
    string tipoCarga;
    Tienda ruta[5];
    long carga;
    int TiempoCarga;
    int TiempoRuta;
};

int main() {
    cout << "Simulacion de Entrega" << endl;









    return 0;
}
