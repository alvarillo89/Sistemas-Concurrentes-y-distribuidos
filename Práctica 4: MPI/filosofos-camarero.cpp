//////////////////////////////////////////////////
// Álvaro Fernández García.
// Cena de los filósofos con Camarero en MPI.
//////////////////////////////////////////////////

#include <iostream>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <mpi.h>

using namespace std;

void Filosofo( int id, int nprocesos ){
  int izq = (id+1) % nprocesos;
  int der = ((id+nprocesos)-1) % nprocesos;
  int peticion = 1;

  while(1){
    // Solicitar al camarero sentarse:
    cout << "Filosofo "<<id<< " solicita sentarse al camarero" << endl << flush;
    MPI_Ssend(&peticion, 1, MPI_INT, 10, 2, MPI_COMM_WORLD);
    cout << "Filosofo "<<id<< " se ha SENTADO" << endl << flush;

    // Solicita tenedor izquierdo
    cout << "Filosofo "<<id<< " solicita tenedor izquierdo " << izq << endl << flush;
    MPI_Ssend(&peticion, 1, MPI_INT, izq, 0, MPI_COMM_WORLD);

    // Solicita tenedor derecho
    cout <<"Filosofo "<<id<< " solicita tenedor derecho " << der << endl << flush;
    MPI_Ssend(&peticion, 1, MPI_INT, der, 0, MPI_COMM_WORLD);

    cout<<"Filosofo "<<id<< " COMIENDO"<<endl<<flush;
    sleep((rand() % 3)+1);  //comiendo

    // Suelta el tenedor izquierdo
    cout <<"Filosofo "<<id<< " suelta tenedor izquierdo " << izq << endl << flush;
    MPI_Ssend(&peticion, 1, MPI_INT, izq, 0, MPI_COMM_WORLD);

    // Suelta el tenedor derecho
    cout <<"Filosofo "<<id<< " suelta tenedor derecho " << der << endl << flush;
    MPI_Ssend(&peticion, 1, MPI_INT, der, 0, MPI_COMM_WORLD);

    //Solicitar al camarero levantarse:
    cout << "Filosofo "<<id<< " solicita levantarse al camarero" << endl << flush;
    MPI_Ssend(&peticion, 1, MPI_INT, 10, 1, MPI_COMM_WORLD);
    cout << "Filosofo "<<id<< " se ha LEVANTADO" << endl << flush;

    // Piensa (espera bloqueada aleatorio del proceso)
    cout << "Filosofo " << id << " PENSANDO" << endl << flush;

    // espera bloqueado durante un intervalo de tiempo aleatorio
    // (entre una décima de segundo y un segundo)
    usleep( 1000U * (100U+(rand()%900U)) );
  }
}

void Tenedor(int id, int nprocesos){
  int buf;
  MPI_Status status;
  int Filo;

  while( true ){
    // Espera un peticion desde cualquier filosofo vecino ...
    MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    Filo = status.MPI_SOURCE;

    // Recibe la peticion del filosofo ...
    MPI_Recv(&buf, 1, MPI_INT, Filo, 0, MPI_COMM_WORLD, &status);

    cout << "Tenedor " << id << " recibe petición de " << Filo << endl << flush;

    // Espera a que el filosofo suelte el tenedor...
    MPI_Recv(&buf, 1, MPI_INT, Filo, 0, MPI_COMM_WORLD, &status);
    cout << "Tenedor " << id << " recibe liberación de " << Filo << endl << flush;
  }
}

void Camarero(){
  int num_filo = 0;
  int buf, Filo, Peticion;
  MPI_Status status;

  while(true){
    if(num_filo == 4) //No pueden sentarse más filósofos.
      MPI_Probe(MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status); //Recibir solo peticiones de levantarse.
    else
      MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); //Recibir cualquier petición.

    Filo = status.MPI_SOURCE;
    Peticion = status.MPI_TAG;

    if(Peticion == 2){ // Petición de sentarse:
      MPI_Recv(&buf, 1, MPI_INT, Filo, 2, MPI_COMM_WORLD, &status);
      cout << "Camarero recibe la petición de sentarse de Filósofo " << Filo << endl << flush;
      num_filo++;
    }
    else if(Peticion == 1){ //Peticion de levantarse:
      MPI_Recv(&buf, 1, MPI_INT, Filo, 1, MPI_COMM_WORLD, &status);
      cout << "Camarero recibe la petición de levantarse de Filósofo " << Filo << endl << flush;
      num_filo--;
    }
  }
}

int main( int argc, char** argv ){
  int rank, size;

  srand(time(0));
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );

  if( size!=11){
    if( rank == 0)
      cout<<"El numero de procesos debe ser 11" << endl << flush ;
    MPI_Finalize( );
    return 0;
  }

  if(rank == 10)
    Camarero();           // El 10 es el Camarero
  else if ((rank%2) == 0)
    Filosofo(rank,size-1); // Los pares son Filosofos
  else
    Tenedor(rank,size-1);  // Los impares son Tenedores

  MPI_Finalize( );
  return 0;
}
