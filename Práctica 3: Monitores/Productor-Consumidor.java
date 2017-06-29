//////////////////////////////////////////////////////////////////////
//Nombre: Álvaro Fernández García.
//Implementación del ejercicio Productor-Consumidor con monitores Hoare
//////////////////////////////////////////////////////////////////////

import monitor.*;

// Monitor:

class MonitorPC extends AbstractMonitor  {
	
	// Variables del Monitor:
	
	private int numSlots = 0, cont = 0;
	private double[] buffer   = null;
	private Condition producir = makeCondition();
	private Condition consumir = makeCondition();

	// Constructor:

	public MonitorPC(int nSlots){
		numSlots = nSlots;
		buffer = new double[numSlots];
	}

	// Procedimientos del monitor:

	public void depositar(double item) throws InterruptedException {
		enter();
		
		if(cont == numSlots)
			producir.await();
		
		buffer[cont] = item;
		cont++;
	
		consumir.signal();
			
		leave();
	}

	public double extraer () throws InterruptedException {
		enter();
		
		double valor;
		
		if(cont == 0)
			consumir.await();
	
		valor = buffer[cont-1];
		cont --;

		producir.signal();		
		leave();
		return valor;
	}
}

// Hebra Productor:

class Productor implements Runnable {
	private MonitorPC monitorpc;
	private int veces, numP;
	public  Thread thr;

	public Productor( MonitorPC monitor, int pveces, int pnumP ) {
		monitorpc = monitor;
		veces = pveces;
		numP  = pnumP ;
  		thr   = new Thread(this,"productor "+numP);
  	}

	public void run(){
   	try{
      	double item = 100*numP ;

      	for( int i=0 ; i < veces ; i++ ){
        		System.out.println(thr.getName()+", produciendo " + item);
        		monitorpc.depositar( item++ );
      	}
    	}
    	catch( Exception e ){
      	System.err.println("Excepcion en main: " + e);
    	}
  	}
}

// Hebra Consumidor

class Consumidor implements Runnable {
	private MonitorPC  monitorpc;
  	private int veces, numC;
  	public  Thread  thr;

  	public Consumidor( MonitorPC monitor, int pveces, int pnumC ){
    	monitorpc = monitor;
    	veces = pveces;
    	numC  = pnumC ;
    	thr   = new Thread(this,"consumidor "+numC);
  	}
  	public void run() {
    	try{
      	for( int i=0 ; i<veces ; i++ ){
        		double item = monitorpc.extraer ();
        		System.out.println(thr.getName()+", consumiendo "+item);
      	}
    	}
    	catch( Exception e ){
      	System.err.println("Excepcion en main: " + e);
    	}
  	}
}


// Main:

class MainProductorConsumidor {
  	public static void main( String[] args ){
    	if ( args.length != 5 ){
      System.err.println("Necesita 5 argumentos: (1) núm.prod.  (2) núm.iter.prod.  (3) núm.cons.  (4)     núm.iter.cons.  (5) tam.buf.");
      return ;
    	}

	// leer parametros, crear vectores y buffer intermedio
   Productor[] prod = new Productor[Integer.parseInt(args[0])] ;
   int iter_prod = Integer.parseInt(args[1]);
   Consumidor[] cons = new Consumidor[Integer.parseInt(args[2])] ;
   int iter_cons = Integer.parseInt(args[3]);
   MonitorPC monitorpc = new MonitorPC(Integer.parseInt(args[4]));

   if ( prod.length*iter_prod != cons.length*iter_cons ){
      System.err.println("no coinciden número de items a producir con a cosumir");
      return ;
    }

   // crear hebras de consumidores
   for(int i = 0; i < cons.length; i++)
     cons[i] = new Consumidor(monitorpc,iter_cons,i) ;

   // crear hebras de productores
   for(int i = 0; i < prod.length; i++)
     prod[i] = new Productor(monitorpc,iter_prod,i) ;

   // poner en marcha las hebras
   for(int i = 0; i < prod.length; i++)
     prod[i].thr.start();
   for(int i = 0; i < cons.length; i++)
     cons[i].thr.start();
 	}
}
