////////////////////////////////////////////////////////
// Problema del Barbero Durmiente.
// Álvaro Fernández García.
////////////////////////////////////////////////////////

import monitor.*;
import java.util.Random;

// Monitor barbería:

class Barberia extends AbstractMonitor {
  private int numClientesSala = 0;
  private boolean sillaOcupada = false;
  private Condition pelandose = makeCondition();
  private Condition colaClientes = makeCondition();
  private Condition duerme = makeCondition();

  // invcado por los clientes para cortarse el pelo
  public void cortarPelo() throws InterruptedException {
    enter();
	 
    if(sillaOcupada){
      numClientesSala++;
      System.out.println("Llega un cliente a la sala de espera. Clientes esperando " + numClientesSala);
      colaClientes.await();  
    }
	 else{
      System.out.println("El cliente despierta al barbero.");
	   duerme.signal();
    }
    		
    sillaOcupada = true;

    if(numClientesSala > 0)
	 	numClientesSala--;

    System.out.println("El barbero comienza a cortarle el pelo a un cliente. Clientes esperando: "+numClientesSala);
    pelandose.await();

    leave();
  }

  // invocado por el barbero para esperar (si procede) a un nuevo cliente y sentarlo para el corte
  public void siguienteCliente() throws InterruptedException {
    enter();

    if(colaClientes.isEmpty()){
      System.out.println("El barbero duerme.");
		duerme.await();
	 }
	 if(!sillaOcupada){
    	System.out.println("El barbero le dice a un cliente que puede pasar.");
	 	colaClientes.signal();				
	 }			
    leave();
  }

  // invocado por el barbero para indicar que ha terminado de cortar el pelo
  public void finCliente() throws InterruptedException{
    enter();
	 System.out.println("El barbero ha terminado de pelar a un cliente.");
    sillaOcupada = false;
    pelandose.signal();
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

// Hebra cliente:

class Cliente implements Runnable {
  private Barberia barberia;
  private int numCliente;
  public Thread thr;

  public Cliente(String nombre, Barberia p_barberia, int p_numCliente){
    numCliente = p_numCliente;
    barberia = p_barberia;
    thr = new Thread(this, nombre);
  }

  public void run () {
    try{
      while (true) {
        barberia.cortarPelo (); // el cliente espera (si procede) y se corta el pelo
        aux.dormir_max( 2000 ); // el cliente está fuera de la barberia un tiempo
      }
    }
    catch(Exception e){
      System.err.println("Excepcion en main: " + e);
    }
  }
}

// Hebra Barbero:

class Barbero implements Runnable {
  private Barberia barberia;
  public Thread thr;

  public Barbero(String nombre, Barberia p_barberia){
    barberia = p_barberia;
    thr = new Thread(this, nombre);
  }

  public void run () {
    try{
      while (true) {
        barberia.siguienteCliente ();
        aux.dormir_max( 2500 ); // el barbero está cortando el pelo
        barberia.finCliente ();
      }
    }
    catch(Exception e){
      	System.err.println("Excepcion en main: " + e);
    }
  }
}

// Main

class MainBarbero {
		public static void main( String[] args ){

			// Creacion de hebras:
			Barberia barberia = new Barberia();
			Barbero barbero = new Barbero("Barbero", barberia);

			Cliente[] clientes = new Cliente[5];
			for(int i = 0; i<5; ++i)
			 		clientes[i] = new Cliente("Cliente "+i, barberia, i);

		   // Poner en marcha las hebras
         barbero.thr.start();		  
			for(int i = 0; i < 3; i++)
		    clientes[i].thr.start();
		 	}
}
