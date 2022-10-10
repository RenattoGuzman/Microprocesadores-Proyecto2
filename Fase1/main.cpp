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
    vector<bool> Ruta;
    long carga;
    int TiempoCarga;
    int TiempoRuta;
};


vector<Camion> C;
vector<Tienda> T;


int KmH;
int Tcarga;
int Tdescarga;
int hId=0;


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


    T[id] = {id+1,25*(id+1),arr};
    return T[id];
}
void asignarRuta(int id,Tienda t){
    for(int i =0;i<4;i++){
        if(t.pedidosTienda[i] != 0)
            C[i].Ruta[id] = true;
    }
}

void *asignacionTienda(void * args){
    int id = hId++;
    cout << "Asignacion"<< endl;

    asignarRuta(id,creacionTienda(id));
}

void *CargaDescarga(void * args){

    cout << "Carga Descarga"<< endl;

}

void *RutaCamion(void * args){

    cout << "Ruta"<< endl;

}



int main() {
    cout << "Simulacion de Entrega" << endl;

    int cantTiendas = 0;

    cout << "\nIngrese la cantidad de tiendas en ruta: " << endl;
    cin >> cantTiendas;
    cout << "\nIngrese la velocidad promedio de los camiones: " << endl;
    cin >> KmH;
    cout << "\nIngrese la velodidad de carga: " << endl;
    cin >> Tcarga;
    cout << "\nIngrese la velocidad de descarga: " << endl;
    cin >> Tdescarga;

    vector<bool> ruta0;
    T = vector<Tienda> (cantTiendas);
    for(int i = 0; i<cantTiendas;i++){
        ruta0.push_back(false);
    }

    C.push_back({1,"Barras de Chocolate",ruta0,0,0,0});
    C.push_back({2,"Goma de Mascar de Tres Platos",ruta0,0,0,0});
    C.push_back({3,"Fizzy Lifting Drink",ruta0,0,0,0});
    C.push_back({4,"Everlasting Gobstopper",ruta0,0,0,0});


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

    for(int i = 0; i<cantTiendas;i++){
        if(C[0].Ruta[i])
            cout <<"Tienda "<<i<<" tiene un pedido "<< endl;
        else
            cout <<"Tienda "<<i<<" NO tiene un pedido "<< endl;
    }

    return 0;
}
