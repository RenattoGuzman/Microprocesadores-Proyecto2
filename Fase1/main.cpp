#include <iostream>
#include <pthread.h>
#include <random>

using namespace std;

struct Tienda {
    int id;
    int distancia;
    vector<int> pedidosTienda;
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
int hId=0;
Tienda T[5];
Camion C[4];


Tienda creacionTienda(int id){
    cout << "Creacion Tienda"<< endl;

    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(0.0,10.0);

    vector<int> arr;

    for (int i =0; i< 4;i++){
        int num =(int) (dist(mt)*10);
        if(num <= 25)
            arr.push_back(0);
        else
            arr.push_back(num);
    }
    Tienda t = {id+1,25*(id+1),arr};
    return t;

}

void *asignacionTienda(void * args){
    int id = hId++;
    cout << "Asignacion"<< endl;

    T[id]= creacionTienda(id);
}

void *CargaDescarga(void * args){

    cout << "Carga Descarga"<< endl;

}

void *RutaCamion(void * args){

    cout << "Ruta"<< endl;

}



int main() {


    cout << "Simulacion de Entrega" << endl;

    //cout << "\nIngrese la velocidad promedio de los camiones: " << endl;
    //cin >> KmH;
    //cout << "\nIngrese la velodidad de carga: " << endl;
    //cin >> Tcarga;
    //cout << "\nIngrese la velocidad de descarga: " << endl;
    // cin >> Tdescarga;


    pthread_t hilo;

    pthread_create(&hilo, NULL,asignacionTienda,NULL);
    //pthread_create(&hilo, NULL,CargaDescarga,&C[0]);
    //pthread_create(&hilo, NULL,RutaCamion,&C[0]);

    pthread_join(hilo,NULL);

    cout<<"Id: "<< T[0].id<<endl;
    cout<<"Distancia: "<< T[0].distancia<<endl;
    cout<<"Pedidos: "<<endl;
    for (int i = 0; i<4;i++)
        cout <<"Camion " <<i <<" pedido: "<< T[0].pedidosTienda[i]<<endl;









    return 0;
}

