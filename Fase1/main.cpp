#include <iostream>
#include <pthread.h>
#include <random>
#include "unistd.h"

using namespace std;

//TODO Encabezado

//Estructura de Tienda
struct Tienda {
    int id;
    int distancia;
    vector<int> pedidosTienda;
    float tiempoEnDescarga;
};

//Estructura de Camion
struct Camion {
    int id;
    string tipoCarga;
    vector<bool> Ruta;
    bool enTienda;
    long carga;
    long cargatotal;
    float TiempoCarga;
    int DistanciaRecorrida;
    float TiempoRuta;
};

//Vectores que almacenan estructuras de datos
vector<Camion> C;
vector<Tienda> T;

//Variables Globales
int KmH;
int Tcarga;
int Tdescarga;
int hId=0;

//Variables tipo pthread
pthread_barrier_t barrier;
vector<pthread_mutex_t> mutexes;



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

    pthread_barrier_wait(&barrier);
    sleep(1);

    printf("Tienda %d ha asignado:\n \t %d paquetes a Camion 1 \n \t %d paquetes a Camion 2 \n \t %d paquetes a Camion 3 \n \t %d paquetes a Camion 4  \n",id+1,T[id].pedidosTienda[0],T[id].pedidosTienda[1],T[id].pedidosTienda[2],T[id].pedidosTienda[3]);
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
        printf("Se han cargado %d paquetes en Camion %d, faltan %ld \n",Tcarga,camion->id,(camion->cargatotal)-(camion->carga));
        sleep(1);
    }
    if(contC2>0){
        camion->carga += contC2;
        camion->TiempoCarga+= ((float) contC2/Tcarga);
        //cout<<"Se han cargado "<<contC2<<" en Camion "<<camion->id<<", faltan "<< (camion->cargatotal)-(camion->carga)<< endl;
        printf("Se han cargado %d paquetes en Camion %d, faltan %ld \n",contC2,camion->id,(camion->cargatotal)-(camion->carga));
        sleep(1);
    }
    cout<<"Camion "<<camion->id<<", faltan ha terminado de ser cargado" << endl; //TODO Pasar cout a printf
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

            pthread_mutex_lock(&mutexes[i]);

            camion->TiempoRuta+= T[i].tiempoEnDescarga;
            T[i].tiempoEnDescarga = 0;

            for(int j =0;j<contD;j++){
                camion->carga -= Tdescarga;
                camion->TiempoCarga++;
                T[i].tiempoEnDescarga++;
                T[i].pedidosTienda[(camion->id)-1] -= Tdescarga;
                //cout<<"Se han descargado "<<Tdescarga<<" de Camion "<<camion->id<<" en Tienda"<<i+1 <<", faltan "<< T[i].pedidosTienda[(camion->id)-1]<< endl;
                printf("Se han descargado %d paquetes de Camion %d en Tienda %d, faltan %d \n",Tdescarga,camion->id,i+1,T[i].pedidosTienda[(camion->id)-1]);
                sleep(1);
            }
            if(contD2>0){
                camion->carga -= contD2;
                camion->TiempoCarga += ((float) contD2/Tcarga);
                T[i].tiempoEnDescarga += ((float) contD2/Tcarga);
                T[i].pedidosTienda[(camion->id)-1] -= contD2;
                //cout<<"Se han descargado "<<contD2<<" en Camion "<<camion->id<<" en Tienda"<<i+1 <<", faltan "<< T[i].pedidosTienda[(camion->id)-1]<< endl;
                printf("Se han descargado %d paquetes de Camion %d en Tienda %d, faltan %d \n",contD2,camion->id,i+1,T[i].pedidosTienda[(camion->id)-1]);
                sleep(1);
            }
            cout<<"Camion "<<camion->id<<", ha terminado de descargar en Tienda "<<i+1 << endl; //TODO Pasar cout a printf
            camion->enTienda = false;

            pthread_mutex_unlock(&mutexes[i]);

            i++;
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
            int contC;
            if(i==0)
                contC = (T[i].distancia)/KmH;
            else
                contC = (T[i].distancia - camion->DistanciaRecorrida)/KmH;
            int contC2 = (T[i].distancia - camion->DistanciaRecorrida)%KmH;
            for(int j = 0;j<contC;j++){
                camion->TiempoRuta++;
                camion->DistanciaRecorrida+=KmH;
                //cout<<"Camion "<<camion->id<<" ha viajado " <<(j+1)*KmH<<" hacia Tienda "<< i+1 << endl;
                printf("Camion %d ha viajado %d Km hacia Tienda %d \n",camion->id,KmH*(j+1),i+1 );
                sleep(1);
            }
            if(contC2>0){
                camion->DistanciaRecorrida += contC2;
                camion->TiempoRuta+= ((float) contC/KmH);
                //cout<<"Camion "<<camion->id<<" ha viajado " <<(contC*KmH)+contC2<<" hacia Tienda "<< i+1 << endl;
                printf("Camion %d ha viajado %d Km hacia Tienda %d \n",camion->id,(KmH*contC)+contC2,i+1 );
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
    mutexes = vector<pthread_mutex_t> (cantTiendas);
    for(int i = 0; i<cantTiendas;i++){
        ruta0.push_back(false);
    }

    C.push_back({1,"Barras de Chocolate",ruta0, true,0,0,0,0});
    C.push_back({2,"Goma de Mascar de Tres Platos",ruta0, true,0,0,0,0});
    C.push_back({3,"Fizzy Lifting Drink",ruta0, true,0,0,0,0});
    C.push_back({4,"Everlasting Gobstopper",ruta0,true,0,0,0,0});

    pthread_t hilos[cantTiendas];

    pthread_barrier_init(&barrier,NULL,cantTiendas);
    for(int i =0;i<cantTiendas;i++){
        pthread_mutex_init(&mutexes[i], NULL);
        pthread_create(&hilos[i], NULL,asignacionTienda,NULL);
    }

    for(int i =0;i<cantTiendas;i++){
        pthread_join(hilos[i],NULL);
    }
    pthread_barrier_destroy(&barrier);

    pthread_t hilosss[4];

    for(int i =0;i<4;i++){
        pthread_create(&hilosss[i], NULL,CargaDescarga,&C[i]);
        pthread_create(&hilosss[i], NULL,RutaCamion,&C[i]);
    }




    for(int i =0;i<4;i++){
        pthread_join(hilosss[i],NULL);
    }
    for(int i =0;i<cantTiendas;i++){
        pthread_mutex_destroy(&mutexes[i]);
    }




    cout <<"------------------------------------------------------------------- " <<endl;
    //cout<<"El Camion 1 tuvo un tiempo de carga y descarga de "<< C[0].TiempoCarga<< " horas"<<endl;
    cout<<"El Camion 1 tuvo un tiempo en ruta de "<< C[0].TiempoRuta<< " horas"<<endl;
    cout<<"El Camion 1 tuvo un tiempo total de "<< C[0].TiempoCarga+C[0].TiempoRuta<< " horas"<<endl;
    cout<<"El Camion 2 tuvo un tiempo en ruta de "<< C[1].TiempoRuta<< " horas"<<endl;
    cout<<"El Camion 2 tuvo un tiempo total de "<< C[1].TiempoCarga+C[1].TiempoRuta<< " horas"<<endl;
    cout<<"El Camion 3 tuvo un tiempo en ruta de "<< C[2].TiempoRuta<< " horas"<<endl;
    cout<<"El Camion 3 tuvo un tiempo total de "<< C[2].TiempoCarga+C[2].TiempoRuta<< " horas"<<endl;
    cout<<"El Camion 4 tuvo un tiempo en ruta de "<< C[3].TiempoRuta<< " horas"<<endl;
    cout<<"El Camion 4 tuvo un tiempo total de "<< C[3].TiempoCarga+C[2].TiempoRuta<< " horas"<<endl;
    //cout<<"El Camion 1 tuvo una carga total de "<< C[0].cargatotal << " paquetes"<<endl;

    //TODO
    // Hacer Prints con informacion de cada Camion y de cada Tienda

    return 0;
}
