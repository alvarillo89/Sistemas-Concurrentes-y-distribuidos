///////////////////////////////////////////////////////////////////////////////
//
// Prácticas de SCD. Práctica 1.
// Ejercicio de los fumadores y el estanquero
// Álvaro Fernández García, Grupo A1
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>      // incluye "time(....)"
#include <unistd.h>    // incluye "usleep(...)"
#include <stdlib.h>    // incluye "rand(...)" y "srand"

using namespace std ;

// Variables globales, comunes a todas las hebras.

const unsigned N_FUMADORES = 3;
sem_t s_fumadores[N_FUMADORES], mostrador, MUTEX;

// ---------------------------------------------------------------------
// introduce un retraso aleatorio de duración comprendida entre
// 'smin' y 'smax' (dados en segundos)

void retraso_aleatorio( const float smin, const float smax )
{
  static bool primera = true ;
  if ( primera )        // si es la primera vez:
  {  srand(time(NULL)); //   inicializar la semilla del generador
     primera = false ;  //   no repetir la inicialización
  }
  // calcular un número de segundos aleatorio, entre {\ttbf smin} y {\ttbf smax}
  const float tsec = smin+(smax-smin)*((float)random()/(float)RAND_MAX);
  // dormir la hebra (los segundos se pasan a microsegundos, multiplicándos por 1 millón)
  usleep( (useconds_t) (tsec*1000000.0)  );
}

// ----------------------------------------------------------------------------
// función que simula la acción de fumar, como un retardo aleatorio de la hebra.
// recibe como parámetro el numero de fumador
// el tiempo que tarda en fumar está entre dos y ocho décimas de segundo.

void fumar( int num_fumador )
{
   sem_wait(&MUTEX);	
   cout << "Fumador número " << num_fumador << ": comienza a fumar." << endl << flush ;
   sem_post(&MUTEX);
   
   retraso_aleatorio( 0.2, 0.8 );
   
   sem_wait(&MUTEX);
   cout << "Fumador número " << num_fumador << ": termina de fumar." << endl << flush ;
   sem_post(&MUTEX);
}
// ----------------------------------------------------------------------------

void * funcion_estanquero( void * )
{ 
  int recurso_producido;
  
  while(true)
  {
    sem_wait(&mostrador);
    recurso_producido = rand() % 3;

    if(recurso_producido == 0){
      sem_wait(&MUTEX);
      cout << "Estanquero produce tabaco." << endl;
      sem_post(&MUTEX);
    }
    else if(recurso_producido == 1){
      sem_wait(&MUTEX);	
      cout << "Estanquero produce papel." << endl;
      sem_post(&MUTEX);
    }
    else{
      sem_wait(&MUTEX);
      cout << "Estanquero produce cerillas." << endl;
      sem_post(&MUTEX);
    }

    sem_post(&(s_fumadores[recurso_producido]));

  }

  return NULL;
}

void * funcion_fumador( void * num_fum_void )
{
  unsigned long num_fum = (unsigned long) num_fum_void;

  while(true)
  {
    sem_wait(&(s_fumadores[num_fum]));
    
    if(num_fum == 0){
      sem_wait(&MUTEX);
      cout << "Fumador 0 recoge tabaco." << endl;
      sem_post(&MUTEX);
    }
    else if(num_fum == 1){
      sem_wait(&MUTEX);
      cout << "Fumador 1 recoge papel." << endl;
	  sem_post(&MUTEX);    
    }
    else{
      sem_wait(&MUTEX);
      cout << "Fumador 2 recoge cerillas." << endl;
      sem_post(&MUTEX);
    }

    sem_post(&mostrador);

    fumar(num_fum);    

  }

  return NULL;
}

// ----------------------------------------------------------------------------

int main()
{
  srand( time(NULL) ); // inicializa semilla aleatoria para selección aleatoria de fumador
  
  sem_init(&mostrador, 0, 1);
  sem_init(&MUTEX, 0, 1);

  for(int i = 0; i < N_FUMADORES; i++)
    sem_init(&(s_fumadores[i]), 0, 0);

  pthread_t estanquero, fumadores[N_FUMADORES];

  pthread_create(&estanquero, NULL, funcion_estanquero, NULL);

  for(unsigned long i = 0; i < N_FUMADORES; i++)
  {
    void* n_fumador = (void*) i;
    pthread_create(&(fumadores[i]), NULL, funcion_fumador, n_fumador);
  }

  pthread_join(estanquero, NULL);

  for(int i = 0; i< N_FUMADORES; i++)
    pthread_join(fumadores[i], NULL);

  sem_destroy(&mostrador);
  sem_destroy(&MUTEX);

  for (int i = 0; i < N_FUMADORES; ++i) {
    sem_destroy(&(s_fumadores[i]));
  }

  return 0 ;
}
