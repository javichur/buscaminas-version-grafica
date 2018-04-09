/*BUSCAMINAS V3.0
RATÓN HABILITADO
BY FRACAMBE 
FCO.JAVIER CAMPOS*/


/*Este programa utiliza una matriz de enteros, donde las decenas y unidades reresentan:

                       
-UNIDADES:
  -'0': indica que en esa posición no hay mina y tampoco en
   las casillas adyacentes.    
  -'9': indica que en esa posición hay una mina.
  -'1,2,...8': el nº indica cuántas minas hay cerca.

-DECENAS:

0--->casilla no está destapada.
1--->casilla está destapada.
2--->casilla está marcada
3--->casilla está con duda.
*/


#include<stdio.h>
#include<stdlib.h>
#include <SDL.h>
#include<time.h>

#ifndef srandom
#define srandom srand
#define random rand
#endif

#define N 9 /*dimensiones del tablero*/
#define M 15 /*número de bombas*/


#define DESTAPAR 1
#define MINA 9

#define MARCAR 2
#define DUDA 3
#define NADA 0

#define parte_minas %10

//modos de la cara a mostrar
#define CONTENTA 0
#define TRISTE 1
#define WIN 2
#define SUSTO 3

SDL_Surface *pantalla;
SDL_Rect rect,numero[9],casilla[3],mina[3],display[10],cara[4];
Uint32 color,color_marcado,color_transparente;
SDL_Surface *buscaminas_bmp;

int sel_x,sel_y; /*almacena coordenada x e y de la casilla seleccionada*/


int cargar_modo_grafico(void)
{
 if(SDL_Init(SDL_INIT_VIDEO)==-1)
    {
      printf("No se pudo inicar el video: %s\n",SDL_GetError());
      return(-1);
    }

  pantalla=SDL_SetVideoMode(640,480,16, SDL_ANYFORMAT);

  if(!pantalla)
    {
      printf("No se pudo iniciar el modo de pantalla: %s\n",SDL_GetError());
      SDL_Quit();
      return(-1);
    }
  return 0;
}


int cargar_imagen(void)
{
  buscaminas_bmp=SDL_LoadBMP("buscaminas.bmp");
  if (!buscaminas_bmp) {
    printf("No se pudo cargar la imagen 'buscaminas.bmp'\n");
    SDL_Quit();
    return(-1);
  }
  return 0;
}

void inicializo_matriz_a_ceros(int matriz[N][N])
{
  int i,n;
  for(i=0;i<N;i++) /*inicializa matriz*/
    for(n=0;n<N;n++)
      matriz[i][n]=NADA; //NADA=0
}

void poner_minas(int matriz[N][N])
{
  int i,pos_bomba_x,pos_bomba_y;

  for(i=0;i<M;i++) /*coloca M minas es el tablero*/
    {
      pos_bomba_x=rand()%N;
      pos_bomba_y=rand()%N;
      while (matriz[pos_bomba_x][pos_bomba_y]==MINA)
	{ /*no repite posición para 2 minas diferentes*/
	  pos_bomba_x=rand()%N;
	  pos_bomba_y=rand()%N;
	}
      matriz[pos_bomba_x][pos_bomba_y]=MINA;
    }


}

void generar_tablero_inicial(int matriz[N][N])
{

  /*Esta función rellena la matriz 'matriz' NxN una vez que en ella se han colocado
  las minas.El objetivo es que en las casillas que no contienen minas (mina equivale
  a valor '9') se guarde el número de minas que hay en sus casillas adyacentes. De no 
  haber ninguna, la casilla mantendrá su valor inicial ('0')*/

  int i,n;
  for(i=0;i<N;i++) /*2 for para recorrer todo el tablero*/
    for(n=0;n<N;n++)
      if(matriz[i][n]==MINA) /*Si la casilla contiene mina*/
	{
	  if(i>0) /*si la casilla no está pegada al borde superior*/
	    {	 
	      if(matriz[i-1][n]!=MINA) /*si la casilla adyacente superior no es bomba*/
		matriz[i-1][n]++;
	      if(n>0) /*si la casilla tampoco está pegada a borde izdo*/
		if(matriz[i-1][n-1]!=MINA) /*si la casilla adyacente superior-izda no es bomba*/
		  matriz[i-1][n-1]++; /*incremento su valor, que indica nº bombas próximas*/
	      if(n<N-1) /*si la casilla tampoco está pegada a borde dcho*/
		if(matriz[i-1][n+1]!=MINA) /*si la casilla adyacente superior-dcha no es bomba*/
		  matriz[i-1][n+1]++; /*incremento*/
	    }
	  if(i<N-1) /*si la casilla no está pegada al borde inferior...mismo procedimiento*/
	    {
	      if(matriz[i+1][n]!=MINA)
		matriz[i+1][n]++;
	      if(n>0) /*para diagonal-inferior-izda*/
		if(matriz[i+1][n-1]!=MINA)
		  matriz[i+1][n-1]++;
	      if(n<N-1) /*para diagonal-inferior-dcha*/
		if(matriz[i+1][n+1]!=MINA)
		  matriz[i+1][n+1]++;
	    }
	  if(n>0) /*para izquierda. Si no está pegada al borde izquierdo*/
	    if(matriz[i][n-1]!=MINA)
	      matriz[i][n-1]++;
	  if(n<N-1) /*para derecha.Si no está pegada al borde derecho*/
	    if(matriz[i][n+1]!=MINA)
	      matriz[i][n+1]++;
	}
 
}

void mostrar_tablero (int matriz[N][N],int perdiste,int ganaste)
{

  /*esta función muetra el tablero por pantalla.*/

  int i,n;

  for(i=0;i<N;i++)
    {
       for(n=0;n<N;n++)
	 {
	  rect=(SDL_Rect) {i*50,n*50,0,0};
	  if((((matriz[i][n]-matriz[i][n]%10)%100))/10==DESTAPAR) /*si la casilla está destapada...*/
	    if(matriz[i][n]parte_minas==NADA) /*si su contenido es '0',mostrar casilla vacía*/
	      SDL_BlitSurface(buscaminas_bmp,&numero[0],pantalla,&rect);
	    else if(matriz[i][n]parte_minas!=MINA) /*si no contiene bomba ni es '0',mostrar valor*/
	      SDL_BlitSurface(buscaminas_bmp,& numero[matriz[i][n]parte_minas],pantalla,&rect);
	    else /*si es bomba, mostrar '*' */
	      SDL_BlitSurface(buscaminas_bmp,&mina[2],pantalla,&rect);
	  
	  if(perdiste||ganaste) /*si he perdido o ganado...*/
	    {
	      if(matriz[i][n]parte_minas==MINA) /*si la casilla contiene mina...*/
		{
		  if((((matriz[i][n]-matriz[i][n]%10)%100))/10==NADA || (((matriz[i][n]-matriz[i][n]%10)%100))/10==DUDA) 
		    SDL_BlitSurface(buscaminas_bmp,&mina[0],pantalla,&rect); 
		  else if((((matriz[i][n]-matriz[i][n]%10)%100))/10==MARCAR) /*si tenía bandera, poner bandera*/
		    SDL_BlitSurface(buscaminas_bmp,&casilla[1],pantalla,&rect);            
		}	    
	      /*si la casilla no contenía mina...*/
	      else if((((matriz[i][n]-matriz[i][n]%10)%100))/10==MARCAR) /*si tenía bandera, aparece una mina tachada*/
		SDL_BlitSurface(buscaminas_bmp,&mina[1],pantalla,&rect); 
	    else if((((matriz[i][n]-matriz[i][n]%10)%100))/10==DUDA) /*si tenía duda, aparece '!' */
	      SDL_BlitSurface(buscaminas_bmp,&casilla[2],pantalla,&rect);
	      
	    }
	  
	  else if((((matriz[i][n]-matriz[i][n]%10)%100))/10!=DESTAPAR)/*si la casilla no está destapada y no he
							    perdido o ganado...*/
	    {
	      if((((matriz[i][n]-matriz[i][n]%10)%100))/10==MARCAR)/*si en la casilla hay bandera(decenas=2)*/
		SDL_BlitSurface(buscaminas_bmp,&casilla[1],pantalla,&rect);
	      else if ((((matriz[i][n]-matriz[i][n]%10)%100))/10==DUDA)/*si en la casilla hay duda(decenas=3)*/
		  SDL_BlitSurface(buscaminas_bmp,&casilla[2],pantalla,&rect);
	      else if ((((matriz[i][n]-matriz[i][n]%10)%100))/10==NADA)/*si en la casilla hay nada*/
		SDL_BlitSurface(buscaminas_bmp,&casilla[0],pantalla,&rect);
	    }
	 }
    }
  SDL_Flip(pantalla);
}

int expandir_casillas_vacias(int matriz[N][N],int sel_x,int sel_y)
{

  if((((matriz[sel_x][sel_y]-matriz[sel_x][sel_y]%10)%100))/10!=DESTAPAR)
    if(matriz[sel_x][sel_y]%10==NADA)
      {
	if(sel_x>0)
	  {
	    expandir_casillas_vacias(matriz,sel_x-1,sel_y);
	    if(sel_y>0)//para esquina sup.izda.
	      expandir_casillas_vacias(matriz,sel_x-1,sel_y-1);
	  }	
	matriz[sel_x][sel_y]=matriz[sel_x][sel_y]parte_minas+10;

	if(sel_x<N-1)
	  {
	    expandir_casillas_vacias(matriz,sel_x+1,sel_y);
	    if(sel_y>0)//para esquina sup.dcha.
	      expandir_casillas_vacias(matriz,sel_x+1,sel_y-1);
	    if(sel_y<N-1)//para esquina inf.dcha.
	      expandir_casillas_vacias(matriz,sel_x+1,sel_y+1);
	  }
	matriz[sel_x][sel_y]=matriz[sel_x][sel_y]parte_minas+10;
	
	if(sel_y>0)
	  expandir_casillas_vacias(matriz,sel_x,sel_y-1);
	matriz[sel_x][sel_y]=matriz[sel_x][sel_y]parte_minas+10;

	if(sel_y<N-1)
	  {
	    expandir_casillas_vacias(matriz,sel_x,sel_y+1);
	    if(sel_x>0)//para esquina inf.izda.
	      expandir_casillas_vacias(matriz,sel_x-1,sel_y+1);
	  }
	matriz[sel_x][sel_y]=matriz[sel_x][sel_y]parte_minas+10;
      }
  matriz[sel_x][sel_y]=matriz[sel_x][sel_y]parte_minas+10;
}

int comprobar_si_he_ganado_ya(int matriz[N][N])
{
  /*función para comprobar si he ganado ya, como consecuencia de mi última tirada*/
  int i,n,num_total_marcadas=0,num_marcadas_bien=0;

  /*¿he ganado marcando sólo las casillas con mina?*/
  for(i=0;i<N;i++)
    for(n=0;n<N;n++) /*si sólo están marcadas todas las casillas con minas*/
      { 
	if(matriz[i][n]parte_minas==MINA && (((matriz[i][n]-matriz[i][n]%10)%100))/10==MARCAR)
	  num_marcadas_bien++;
	if((((matriz[i][n]-matriz[i][n]%10)%100))/10==MARCAR)
	  num_total_marcadas++;
      } 
 if(num_marcadas_bien==M && num_total_marcadas==M)
    return 1; /*devuelve 1*/
  
  num_total_marcadas=0;

  /*¿he ganando destapando todas las casillas menos las bombas?*/
  for(i=0;i<N;i++)
    for(n=0;n<N;n++) /*si he destapado todas menos las bombas*/
      if(matriz[i][n]parte_minas!=MINA && (((matriz[i][n]-matriz[i][n]%10)%100))/10==DESTAPAR)
	num_total_marcadas++;
  if(num_total_marcadas==N*N-M)
    return 1; /*devuelve 1*/

  return 0;
}

void contador_segundos(segundos)
{
  int u,d,c; /*unidades,decenas y centenas de los segundos*/

  u=segundos%10;
  d=((segundos-u)%100)/10;
  c=(segundos-u-d*10)/100;

  rect=(SDL_Rect) {470,2,0,0};
  SDL_BlitSurface(buscaminas_bmp,&display[c],pantalla,&rect);

  rect=(SDL_Rect) {520,2,0,0};
  SDL_BlitSurface(buscaminas_bmp,&display[d],pantalla,&rect);

  rect=(SDL_Rect) {570,2,0,0};
  SDL_BlitSurface(buscaminas_bmp,&display[u],pantalla,&rect);
}

void mostrar_cara(int modo)
{
  color_transparente = SDL_MapRGB(buscaminas_bmp->format, 0,255,0); /*color transparente (verde)*/
  rect=(SDL_Rect) {500,110,0,0};
  SDL_SetColorKey(buscaminas_bmp, SDL_SRCCOLORKEY | SDL_RLEACCEL, color_transparente);
  SDL_BlitSurface(buscaminas_bmp,&cara[modo],pantalla,&rect);

  if(modo==SUSTO)
    {
      SDL_Flip(pantalla);
      SDL_Delay(100);
    }

}

int main(int argc, char *argv[])
{
  int matriz[N][N],i,n;
  int perdiste=0,ganaste=0,entrada;
  int leido_por_teclado,fin=0,segundos=0,decimas=0;
  int x_aux,y_aux,raton=0;
  SDL_Event event;


  if(cargar_modo_grafico() || cargar_imagen()) /*carga modo gráfico e imagen por funciones*/
    exit(-1);

  color=SDL_MapRGB(pantalla->format,100,200,255); /*color fondo de la ventana*/
  rect=(SDL_Rect) {0,0,640,480};
  SDL_FillRect(pantalla,&rect,color);

  sel_x=0;
  sel_y=0;

  SDL_WM_SetCaption("Buscaminas 3.0         BY FRACAMBE",NULL);

  for(i=0;i<10;i++) /*genero los rectángulos donde están las imagenes*/
    {
      display[i] = (SDL_Rect) {i*50,150,50,100};
      if(i<9)
	{
	  numero[i] = (SDL_Rect) {i*50,0,50,50};
	  if(i<3)
	    {
	      casilla[i]= (SDL_Rect) {i*50,50,50,50};
	      mina[i]= (SDL_Rect) {i*50,100,50,50};
	    }
	}
    }

  contador_segundos(0);/*muestro contador de segundos con '000'*/

  srandom(time(NULL));

  inicializo_matriz_a_ceros(matriz);
  poner_minas(matriz); /*pongo M minas en el tablero*/
  generar_tablero_inicial(matriz); /*genera tablero inicial (esta función SÓLO se*/
				                                /* llama una vez)*/
  
  for(i=0;i<N;i++) /*muestro tablero todo tapado*/
    for(n=0;n<N;n++)
      {
	rect=(SDL_Rect) {i*50,n*50,0,0};
	SDL_BlitSurface(buscaminas_bmp,&casilla[0],pantalla,&rect);
      }

  printf("BUSCAMINAS V 3.0\n BY FRACAMBE.\nVersión con SDL y ratón\nNúmero total de bombas:%d\n\n",M);

  for(i=0;i<4;i++)  /*posicion de las caras dentro del .bmp*/
    cara[i]= (SDL_Rect) {i*100,250,100,100};

  while(perdiste==0 && ganaste==0 && fin==0)
    {
      mostrar_cara(CONTENTA);

      while( SDL_PollEvent( &event ) ){
	switch( event.type ){
	case SDL_MOUSEBUTTONDOWN: /*si pulsas boton raton*/
	  x_aux=event.button.x/50;
	  y_aux=event.button.y/50;

	  if(x_aux<N && y_aux<N)
	    {	    
	      sel_x=x_aux;
	      sel_y=y_aux;
	    
	      if (event.button.button==SDL_BUTTON_LEFT)
		raton=DESTAPAR;
	      else if (event.button.button==SDL_BUTTON_RIGHT)
		raton=MARCAR;
	    }
	  break;
	  
	case SDL_KEYDOWN: /*si pulsan ESC...*/
	  switch (event.key.keysym.sym) {
	  case SDLK_ESCAPE: /*ESC*/
	    fin=1;
	    break;
	  }
	  break;
	  
	case SDL_QUIT: /*caso en que cierran la ventana*/
	  fin = 1;
	  break;
	  
	}
      }

      if(raton) /*si pulsé botones del ratón*/
	{
	  if(raton==DESTAPAR) /*si usuario quiere destapar*/
	    {
	      raton=NADA;
	      mostrar_cara(SUSTO);
	      if(!segundos) /*si es 1ª casilla en destapar...*/
		{
		  while(matriz[sel_x][sel_y]parte_minas==MINA)
		    { /*...me aseguro de q no sea mina*/
		      inicializo_matriz_a_ceros(matriz);
		      poner_minas(matriz);
		      generar_tablero_inicial(matriz);
		    }
		  segundos=1; /*arranca el contador de segundos*/
		}
	      if(matriz[sel_x][sel_y]parte_minas==NADA) /*si la casilla seleccionada no está*/
		/* próxima a ninguna mina (si su valor es '0'),llamada a función*/
		expandir_casillas_vacias(matriz,sel_x,sel_y);

	      else if(matriz[sel_x][sel_y]parte_minas==MINA) /*si la casilla contiene mina*/
		{
		  perdiste=1;
		  matriz[sel_x][sel_y]=matriz[sel_x][sel_y]parte_minas+10;
		}
	      else /*si casilla está junto a una bomba (su valor no es ni '0' ni '-1')*/
		{
		matriz[sel_x][sel_y]=matriz[sel_x][sel_y]parte_minas+10*DESTAPAR;/*marco casilla como destapada*/
		}
	    }
	  else if(raton==MARCAR) /*si usuario quiere marcar una casilla con bandera o '?' o desmarcar*/
	    {
	      if((((matriz[sel_x][sel_y]-matriz[sel_x][sel_y]%10)%100))/10==NADA) /*si casilla está sin marcar...*/
		matriz[sel_x][sel_y]=matriz[sel_x][sel_y]parte_minas+20; /*...bandera en casilla*/	
	      else if((((matriz[sel_x][sel_y]-matriz[sel_x][sel_y]%10)%100))/10==MARCAR) /*si casilla está con bandera..*/
		matriz[sel_x][sel_y]=matriz[sel_x][sel_y]parte_minas+30; /*...marcar casilla como duda*/
	      else if((((matriz[sel_x][sel_y]-matriz[sel_x][sel_y]%10)%100))/10==DUDA) /*si esa casilla está con '?'...*/
		matriz[sel_x][sel_y]=matriz[sel_x][sel_y]parte_minas; /*...quitar marca*/
	    }

	  ganaste=comprobar_si_he_ganado_ya(matriz);
	  mostrar_tablero(matriz,perdiste,ganaste);

	  raton=NADA;
	}

      if(segundos)
	{
	  if(decimas<9)
	    decimas++;
	  else if(segundos<999)
	    {
	      decimas=0;
	      contador_segundos(segundos);
	      segundos++;
	    }
	  else
	    {
	      printf("se te acabó el tiempo\n");
	      fin=1;
	    }
	}

      SDL_Flip(pantalla);
      SDL_Delay(100);
    }

  if (perdiste) /*si has perdido*/
    {
      mostrar_cara(TRISTE);
      SDL_Flip(pantalla);
      printf("Has perdido.");
    }  
  else if (ganaste)/*si has ganado*/
    {
      mostrar_cara(WIN);
      SDL_Flip(pantalla);
      printf("Has ganado.");
    }
  SDL_Delay(2000);
  SDL_Quit();

  return 0;
}
