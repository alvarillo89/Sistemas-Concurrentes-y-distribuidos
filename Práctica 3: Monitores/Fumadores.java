																																					//////////////////////////////////////////////////////////////////////
//Nombre: Álvaro Fernández García.
//Implementación del ejercicio de los Fumadores con monitores Hoare
//////////////////////////////////////////////////////////////////////

import monitor.*;
import java.util.Random;

// Monitor Estanco:

class Estanco extends AbstractMonitor {
	private int mostrador = -1;
	private boolean mostradorOcupado = false;
	private Condition necesarioProducir = makeCondition();
	private Condition[] puedeRecoger = new Condition[3];

	public Estanco(){
		for(int i = 0; i < 3; ++i)
			puedeRecoger[i] = makeCondition();
	}

	// invocado por cada fumador, indicando su ingrediente o numero
	public void obtenerIngrediente( int miIngrediente ) throws InterruptedException {
		enter();

		if(miIngrediente != mostrador)
			puedeRecoger[miIngrediente].await();

		System.out.println("Fumador " + miIngrediente + " recoge " + miIngrediente + " y comienza a fumar");
		mostradorOcupado = false;
		necesarioProducir.signal();

		leave();
	}

	// invocado por el estanquero, indicando el ingrediente que pone
	public void ponerIngrediente( int ingrediente ) throws InterruptedException  {
		enter();

		System.out.println("Estanquero ha producido " + ingrediente);
		mostrador = ingrediente;
		mostradorOcupado = true;
		puedeRecoger[ingrediente].signal();

		leave();
	}

	// invocado por el estanquero
	public void esperarRecogidaIngrediente() throws InterruptedException {
		enter();

		if(mostradorOcupado)
			necesarioProducir.await();

		leave();
	}
}

// Clase auxiliar para dormir la hebra:

class aux {
	static Random genAlea = new Random();
	static void dormir_max(int milisecsMax){
		try{
			Thread.sleep(genAlea.nextInt(milisecsMax));
		}
		catch(InterruptedException e){
				System.err.println("sleep interrumpido en 'aux.dormir_max()'");
		}
	}
}

// Hebra estanquero:

class Estanquero implements Runnable {
	private Estanco estanco;
	public Thread thr;

	public Estanquero(String nombre, Estanco p_estanco){
		estanco = p_estanco;
		thr = new Thread(this, nombre);
	}

	public void run(){
		try{
			int ingrediente;
			while (true) {
				ingrediente = (int) (Math.random () * 3.0); // 0,1 o 2
				estanco.ponerIngrediente( ingrediente );
				estanco.esperarRecogidaIngrediente() ;
			}
		}
		catch(Exception e){
			System.err.println("Excepcion en main: " + e);
		}
	}
}

// Hebra Fumadores:

class Fumador implements Runnable {
	private Estanco estanco;
	int miIngrediente;
	public Thread thr ;

	public Fumador( int p_miIngrediente, String nombre, Estanco p_estanco) {
		miIngrediente = p_miIngrediente;
		estanco =  p_estanco;
		thr = new Thread(this, nombre);
	}

	public void run(){
		try{
			while ( true ){
				estanco.obtenerIngrediente( miIngrediente );
				aux.dormir_max( 2000 );
				System.out.println(thr.getName() +" termina de fumar");
			}
		}
		catch(Exception e){
			System.err.println("Excepcion en main: " + e);
		}
	}
}

// Main:

class MainFumadores {
		public static void main( String[] args ){

			// Creacion de hebras:
			Estanco estanco = new Estanco();
			Estanquero estanquero = new Estanquero("Estanquero", estanco);

			Fumador[] fumadores = new Fumador[3];
			for(int i = 0; i<3; ++i)
			 		fumadores[i] = new Fumador(i,"Fumador "+i, estanco);

		  // Poner en marcha las hebras
		  for(int i = 0; i < 3; i++)
		    fumadores[i].thr.start();

			estanquero.thr.start();
		 	}
}
