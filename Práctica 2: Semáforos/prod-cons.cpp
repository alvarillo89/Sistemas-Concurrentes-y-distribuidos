///////////////////////////////////////////////////////////////////////////////
//
// Prácticas de SCD. Práctica 1.
// Ejercicio del productor-consumidor con buffer intermedio.
// Álvaro Fernández García, Grupo A1
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // necesario para {\ttbf usleep()}
#include <stdlib.h> // necesario para {\ttbf random()}, {\ttbf srandom()}
#include <time.h>   // necesario para {\ttbf time()}

using namespace std ;

// ---------------------------------------------------------------------
// constantes configurables:

const unsigned num_items = 40, tam_vector = 10;    // numero total de items que se producen o consumen
                                                   // tamaño del vector, debe ser menor que el número de items
 
unsigned buffer[tam_vector];   				   	         // Buffer intermedio.
int primera_libre = 0; 						   	             // Implementación LIFO (Pila acotada)
sem_t puede_leer, puede_escribir, MUTEX;		       // Semáforos.

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

// ---------------------------------------------------------------------
// función que simula la producción de un dato

unsigned producir_dato()
{
  static int contador = 0 ;
  contador = contador + 1 ;
  retraso_aleatorio( 0.1, 0.5 );
  cout << "Productor : dato producido: " << contador << endl << flush ;
  return contador ;
}
// ---------------------------------------------------------------------
// función que simula la consumición de un dato

void consumir_dato( int dato )
{
   retraso_aleatorio( 0.1, 1.5 );
   cout << "Consumidor: dato consumido: " << dato << endl << flush ;
}
// ---------------------------------------------------------------------
// función que ejecuta la hebra del productor

void * funcion_productor( void * )
{
  for( unsigned i = 0 ; i < num_items ; i++ )
  {
    int dato = producir_dato() ;
    
    sem_wait(&puede_escribir);
    sem_wait(&MUTEX);
    
    buffer[primera_libre++] = dato; 
    cout << "Productor : dato insertado: " << dato << endl << flush ;
    
    sem_post(&MUTEX);
    sem_post(&puede_leer);  
}
  return NULL ;
}
// ---------------------------------------------------------------------
// función que ejecuta la hebra del consumidor

void * funcion_consumidor( void * )
{
  for( unsigned i = 0 ; i < num_items ; i++ )
  {
    int dato;
    
    sem_wait(&puede_leer);
    sem_wait(&MUTEX);
    
    dato = buffer[primera_libre-1];
    primera_libre--;
    cout << "Consumidor: dato extraído : " << dato << endl << flush ;
    
    sem_post(&MUTEX);
    sem_post(&puede_escribir);
    consumir_dato( dato ) ;
  }
  return NULL ;
}
//----------------------------------------------------------------------

int main()
{
   sem_init(&puede_escribir, 0, tam_vector);
   sem_init(&puede_leer, 0, 0);
   sem_init(&MUTEX, 0, 1);	
   
   pthread_t hebra_productora, hebra_consumidora;
 
   pthread_create(&hebra_productora, NULL, funcion_productor, NULL);
   pthread_create(&hebra_consumidora, NULL, funcion_consumidor, NULL);

   pthread_join( hebra_productora, NULL);
   pthread_join( hebra_consumidora, NULL);

   sem_destroy( &puede_escribir );
   sem_destroy( &puede_leer );
   sem_destroy( &MUTEX );
    
   return 0 ;
}
