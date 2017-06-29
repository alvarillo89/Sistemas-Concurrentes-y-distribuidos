////////////////////////////////////////////////////////////////////////
// SCD. Ejemplos del seminario 1.
// Álvaro Fernández García.
// Cálculo de PI
////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <iomanip>
#include "fun_tiempo.h"

using namespace std;

// Constante y variables globales (compartidas entre hebras)

const int MAX_HEBRAS = 30;				// Número máximo de hebras.
unsigned long m; 					    // Número de muestras.
unsigned n;                             // Número de hebras
double resultado_parcial[MAX_HEBRAS];

// Implementación de la función f:

double f(double x)       
{ 
   return 4.0/(1+x*x);
}

// Cálculo secuencial:
// Precondición: m es múltiplo de n.

double calcular_integral_secuencial()
{  
   double suma = 0.0;

   for(unsigned long i = 0; i<m; i++)
      suma += f((i+0.5)/m);
   
   return suma/m;
}

// Función que ejecuta cada hebra

void* funcion_hebra(void* ih_void) 
{  
   unsigned long ih = (unsigned long) ih_void ; // Índice de la hebra
   double sumap = 0.0;
 
   for(unsigned long i = (ih*(m/n)); i<(ih*(m/n))+(m/n); i++)
     sumap += f((i+0.5)/m);
    
   resultado_parcial[ih] = sumap/m;
   return NULL ;
}

// Cálculo concurrente.
// Precondición: m es múltiplo de n.

double calcular_integral_concurrente( )
{  
   double resultado = 0.0;
   pthread_t id_hebras[MAX_HEBRAS];

   for(unsigned long i = 0; i<n; ++i) {
   		void* argumento = (void*) i;
   		pthread_create(&(id_hebras[i]), NULL, funcion_hebra, argumento);
   	}		

   for(unsigned long i = 0; i<n; ++i)
   		pthread_join(id_hebras[i], NULL);

   for(unsigned long i = 0; i<n; ++i)
   		resultado += resultado_parcial[i];
   
   return resultado;
}

int main(int argc, char* argv[])
{
   if(argc != 3) {
   		cout << "Uso: " << argv[0] << " <Número de muestras> <Número de hebras>" << endl;
   		return 1; 
   }
   else {

   	   m = (unsigned long) atoi(argv[1]);
   	   n = (unsigned long) atoi(argv[2]);	

	   cout << "Ejemplo 4 (cálculo de PI)" << endl ;
	   double pi_sec = 0.0, pi_conc = 0.0 ;
	   
	   struct timespec inicio_s = ahora();
	   pi_sec  = calcular_integral_secuencial() ;
	   struct timespec fin_s = ahora();

	   struct timespec inicio_c = ahora();
	   pi_conc = calcular_integral_concurrente() ;
	   struct timespec fin_c = ahora();
	   
	   cout << "valor real de PI = 3,1415926535..." << endl
	   		<< "valor de PI (calculado secuencialmente)  == " << pi_sec  << endl 
	        << "valor de PI (calculado concurrentemente) == " << pi_conc << endl
	        << "duración cálculo secuencial == " << fixed << setprecision(8) << duracion(&inicio_s, &fin_s) << " seg" << endl
	        << "duración cálculo concurrente == " << fixed << setprecision(8) << duracion(&inicio_c, &fin_c) << " seg" << endl;
	}
}

/* Con respecto al tiempo que tarda la ejecución secuencial respecto a la concurrente,
podemos decir que esta se realiza mucho más rápido gracias a que están disponibles 
4 núcleos en el procesador, de tal modo que cuando ejecutamos el cálculo concurrentemente,
cada una de las hebras se ejecuta en un núcleo distinto, por tanto todas calculan 
paralelamente frente al cálculo secuencial realizado por una sola CPU. */

