#include <iostream>
#include <pthread.h>
#include <random>
#include "unistd.h"

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
    bool enTienda;
    long carga;
    long cargatotal;
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
        if(t.pedidosTienda[i] != 0){
            C[i].Ruta[id] = true;
            C[i].cargatotal += t.pedidosTienda[i];
        }
    }
}

void *asignacionTienda(void * args){
    int id = hId++;
    asignarRuta(id,creacionTienda(id));
}

void *CargaDescarga(void * args){
    Camion * camion;
    camion =(Camion *) args;

    //Carga
    int contC = camion->cargatotal/Tcarga;
    int contC2 = camion->cargatotal%Tcarga;
    for(int i =0;i<contC;i++){
        camion->carga += Tcarga;
        camion->TiempoCarga++;
        //cout<<"Se han cargado "<<Tcarga<<" en Camion "<<camion->id<<", faltan "<< (camion->cargatotal)-(camion->carga)<< endl;
        printf("Se han cargado %d en Camion %d, faltan %ld \n",Tcarga,camion->id,(camion->cargatotal)-(camion->carga));
        sleep(1);
    }
    if(contC2>0){
        camion->carga += contC2;
        camion->TiempoCarga++;
        //cout<<"Se han cargado "<<contC2<<" en Camion "<<camion->id<<", faltan "<< (camion->cargatotal)-(camion->carga)<< endl;
        printf("Se han cargado %d en Camion %d, faltan %ld \n",contC2,camion->id,(camion->cargatotal)-(camion->carga));
        sleep(1);
    }
    cout<<"Camion "<<camion->id<<", faltan ha terminado de ser cargado" << endl;
    camion->enTienda = false;

    //Descarga
    int i = 0;
    while(camion->carga>0){
        if(camion->enTienda){
            int contD = 0;
            int contD2 = 0;
            while(i<camion->Ruta.size()){
                if(camion->Ruta[i]){
                    contD=T[i].pedidosTienda[(camion->id)-1]/Tdescarga;
                    contD2=T[i].pedidosTienda[(camion->id)-1]%Tdescarga;
                    break;
                }
                i++;
            }

            for(int j =0;j<contD;j++){
                camion->carga -= Tdescarga;
                camion->TiempoCarga++;
                T[i].pedidosTienda[(camion->id)-1] -= Tdescarga;
                //cout<<"Se han descargado "<<Tdescarga<<" de Camion "<<camion->id<<" en Tienda"<<i+1 <<", faltan "<< T[i].pedidosTienda[(camion->id)-1]<< endl;
                printf("Se han descargado %d de Camion %d en Tienda %d, faltan %d \n",Tdescarga,camion->id,i+1,T[i].pedidosTienda[(camion->id)-1]);
                sleep(1);
            }
            if(contD2>0){
                camion->carga -= contD2;
                camion->TiempoCarga++;
                T[i].pedidosTienda[(camion->id)-1] -= contD2;
                //cout<<"Se han descargado "<<contD2<<" en Camion "<<camion->id<<" en Tienda"<<i+1 <<", faltan "<< T[i].pedidosTienda[(camion->id)-1]<< endl;
                printf("Se han descargado %d de Camion %d en Tienda %d, faltan %d \n",contD2,camion->id,i+1,T[i].pedidosTienda[(camion->id)-1]);
                sleep(1);
            }
            cout<<"Camion "<<camion->id<<", ha terminado de descargar en Tienda "<<i+1 << endl;
            camion->enTienda = false;
        } else
            sleep(1);
    }
}

void *RutaCamion(void * args){
    Camion * camion;
    camion =(Camion *) args;

    while(camion->enTienda) {
        sleep(1);
    }
    for(int i=0;i<camion->Ruta.size();i++){
        if(camion->Ruta[i]){
            int contC = (T[i].distancia-((camion->TiempoRuta)*KmH))/KmH;
            int contC2 = (T[i].distancia-((camion->TiempoRuta)*KmH))%KmH;
            for(int j = 0;j<contC;j++){
                camion->TiempoRuta++;
                //cout<<"Camion "<<camion->id<<" ha viajado " <<(j+1)*KmH<<" hacia Tienda "<< i+1 << endl;
                printf("Camion %d ha viajado %d hacia Tienda %d \n",camion->id,(j+1)*KmH,i+1 );
                sleep(1);
            }
            if(contC2>0){
                camion->carga += contC2;
                camion->TiempoCarga++;
                //cout<<"Camion "<<camion->id<<" ha viajado " <<(contC*KmH)+contC2<<" hacia Tienda "<< i+1 << endl;
                printf("Camion %d ha viajado %d hacia Tienda %d \n",camion->id,(contC*KmH)+contC2*KmH,i+1 );
                sleep(1);
            }
            cout<<"Camion "<<camion->id<<" ha llegado a Tienda "<< i+1 << endl;
            camion->enTienda = true;
            while(camion->enTienda) {
                sleep(1);
            }
        }
    }

    cout<<"Camion "<<camion->id<<" ha terminado su ruta" << endl;
}

int main() {
    cout << "Simulacion de Entrega" << endl;

    int cantTiendas = 0;

    cout << "\nIngrese la cantidad de tiendas en ruta: " << endl;
    cin >> cantTiendas;
    cout << "\nIngrese la velocidad promedio de los camiones: " << endl;
    cin >> KmH;
    cout << "\nIngrese la velocidad de carga: " << endl;
    cin >> Tcarga;
    cout << "\nIngrese la velocidad de descarga: " << endl;
    cin >> Tdescarga;

    cout <<"------------------------------------------------------------------- " <<endl;

    vector<bool> ruta0;
    T = vector<Tienda> (cantTiendas);
    for(int i = 0; i<cantTiendas;i++){
        ruta0.push_back(false);
    }

    C.push_back({1,"Barras de Chocolate",ruta0, true,0,0,0,0});
    C.push_back({2,"Goma de Mascar de Tres Platos",ruta0, true,0,0,0,0});
    C.push_back({3,"Fizzy Lifting Drink",ruta0, true,0,0,0,0});
    C.push_back({4,"Everlasting Gobstopper",ruta0,true,0,0,0,0});


    pthread_t hilo;

    pthread_create(&hilo, NULL,asignacionTienda,NULL);

    pthread_join(hilo,NULL);

    cout <<"------------------------------------------------------------------- " <<endl;

    cout<<"Tienda: "<< T[0].id<<endl;
    cout<<"Distancia: "<< T[0].distancia<<" km de la fabrica"<<endl;
    cout<<"Pedidos: "<<endl;
    for (int i = 0; i<4;i++)
        cout <<"Se realizo un pedido al Camion " <<i+1 <<" de: "<< T[0].pedidosTienda[i]<<endl;

    cout <<"------------------------------------------------------------------- " <<endl;

    cout <<"Pedidos a cada camion por Tienda 1" <<endl;

    for (int j =0; j<4;j++){
        if(C[j].Ruta[0])
            cout <<"Tienda 1 tiene un pedido para camion "<< j+1<< endl;
        else
            cout <<"Tienda 1 NO tiene un pedido para camion "<< j+1 << endl;
    }

    cout <<"------------------------------------------------------------------- " <<endl;


    pthread_create(&hilo, NULL,CargaDescarga,&C[0]);
    pthread_create(&hilo, NULL,RutaCamion,&C[0]);

    pthread_join(hilo,NULL);


    cout <<"------------------------------------------------------------------- " <<endl;
    cout<<"El Camion 1 tuvo un tiempo de carga de "<< C[0].TiempoCarga<<endl;
    cout<<"El Camion 1 tuvo un tiempo en ruta de "<< C[0].TiempoRuta<<endl;
    cout<<"El Camion 1 tuvo un tiempo total de "<< C[0].TiempoCarga+C[0].TiempoCarga<<endl;


    return 0;
}
