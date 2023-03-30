/*---------------------------------------------------------------------------------------
* Materia       : PROYECTO DE INVESTIGACIÓN
* Proyecto      : IDENTIFICACIÓN DE POTENCIALES HOTSPOTS USANDO MÉTODOS DE CLASIFICACIÓN
* Algoritmo     : Bayes Ingenuo
* Funcionalidad : Recibe las BD de entrenameinto.  Con base en ella hace la clasificación
*                 en la BD de pruebas de los hotspots.
*
* Entradas      :
*                  - BD de entrenamiento. Se debe de especificar el nombre del archivo. 
*                    El formato del archivo es CSV y contiene las 16 métricas del estudio
*                    más la métrica NBC (17) con la que se hace la validación de la 
*                    clasificación. Constante: cAppE.
*                  - BD de prueba. Se debe de especificar el nombre del archivo.
*                    El formato del archivo es CSV y contiene las 16 métricas del estudio
*                    más la métrica NBC (17) con la que se hace la validación de la 
*                    clasificación. Constante: cAppP.
*                  - Nombre del archivo de salida. Constante: cRes.
*                  - Método de normalización de los datos de entrenamiento o pruebas.
*                    Los métodos disponibles son: nMax(cMN=1), nCLOC(cMN=2) o ninguno(cMN=0).
* Salidas       :
*                  - Archivo con la matriz de confusión obtenida de cada una de las 
*                    combinaciones de métricas evaluada.
*----------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <inttypes.h>
//---------------------------------------------------------------------------------
//CONSTANTES
//---------------------------------------------------------------------------------

#define PI 3.14159265358979323846		//Cosntante de Pi.
#define cNG 17							//Número de métricas en los archivos de entrenamiento y pruebas.
#define cAppE "App9_16_ent_Sflume.csv" 	//Nombre de archivo de entrenamiento.
#define cAppP "flume_16_pru.csv"		//Nombre de archivo de prueba.
#define cRes "Salida_BI_"				//Prefijo del nombre de salida con resultados.
#define cMN 0							//nMax=1, nCLOC=2 o ninguno=0

//---------------------------------------------------------------------------------
// VARIABLES GLOBALES
//---------------------------------------------------------------------------------

//unsigned int SemillaE;		//Variable Semilla para la Generación de Números Aleatorios.
FILE *fp;					//Archivo de resultados.

//---------------------------------------------------------------------------------
// FUNCIONES DE APOYO
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
void AbreArchivoResultados(void) {

	time_t current_time;
	char* c_time_string;
	char fName[50];

	strcpy(fName,cRes);
	strcat(fName,cAppE);
	strcat(fName,"_");

	/* Obtain current time. */
	current_time = time(NULL);

	if (current_time == ((time_t)-1)) {
		(void) fprintf(stderr, "Failure to obtain the current time.\n");
		exit(EXIT_FAILURE);
	}

	/* Convert to local time format. */
	c_time_string = ctime(&current_time);

	if (c_time_string == NULL) {
		(void) fprintf(stderr, "Failure to convert the current time.\n");
		exit(EXIT_FAILURE);
	}

	/* Print to stdout. ctime() has already added a terminating newline character. */
	(void) printf("Current time is %c", c_time_string[23]);
//    exit(EXIT_SUCCESS);

	int f=strlen(fName);

//  0123456789+123456789+123
//	Tue May 26 21:51:03 2015
//	Tue May 26 21:51:03 2015
//	Fri Oct 23 15:08:24 2020

	fName[f]    =  c_time_string[20];
	fName[f+1]  =  c_time_string[21];
	fName[f+2]  =  c_time_string[22];
	fName[f+3] =  c_time_string[23];

	fName[f+4] =  c_time_string[4];
	fName[f+5] =  c_time_string[5];
	fName[f+6] =  c_time_string[6];

	fName[f+7] =  c_time_string[8];
	fName[f+8] =  c_time_string[9];

	fName[f+9] =  c_time_string[11];
	fName[f+10] =  c_time_string[12];
	fName[f+11] =  c_time_string[14];
	fName[f+12] =  c_time_string[15];
	fName[f+13] =  c_time_string[17];
	fName[f+14] =  c_time_string[18];
	fName[f+15] =  '\0';

	strcat(fName,".txt");

	printf("Name: %s\n",fName);


	if((fp=fopen(fName,"a+t")) == NULL) {
		printf("xxxx no se pudo abrir el archivo.\n");
		exit(1);
	}

	//----Escribe la cabecera del detalle a generar (evaluaciones del algoritmo con la BD de prueba).
	fprintf(fp,"NumCom,Combinacion,TP,TN,FP,FN\n");

}
//---------------------------------------------------------------------------------
void CierraArchivoResultados(void) {

	fclose(fp);
}
//-----------------------------------------------------------
// Lee el archivo especificado y lo carga en el arreglo *App
// para ser procesado. Regularmente será la BD de
// entrenamiento o la BD de prueba. También se obtiene el 
// número de archivo tiene la BD de entremiento o pruebas,
// reflejandose en numCls.
//-----------------------------------------------------------
float **CargaBaseDatos(int *numCls, char *App) {
	FILE *archivo;

	char Linea[200];
	float f1;
	char Str[10];
	int i1, i, j=0 ,t ,c,vNG,vNumFiles;

	*numCls   = 0;
	vNumFiles = 0;
	
	int *numGenes = 0;
	char *fName = (char*)malloc(50*sizeof(char));


	vNG = cNG;



//----Lee archivo de métricas y carga a la matriz de Métricas.

	strcpy(fName,App);

//---------------------------------
//--Lee #Files
	archivo = fopen(fName,"r");
	if (archivo == NULL) {
		printf("\nError de apertura del archivo. \n\n");
	} else {
		//printf("\nEl contenido del archivo de prueba es \n\n");
		
		
		//Lee total de Archivos en la BD de Entrenamiento.
		while(fgets(Linea, 200, (FILE*) archivo)) {
			vNumFiles = vNumFiles + 1;
		}
	}
	fclose(archivo);
	printf("NumFiles: %d\n",vNumFiles);
//---------------------------------



//--Definición de la Matriz de las Métricas cargadas.
	float **Metricas = (float**)malloc(vNumFiles*sizeof(float*));
	for(int i=0; i<vNumFiles; i++) {
		Metricas[i]= (float*)malloc(cNG*sizeof(float));
	}
	
		
//--Carga la BD a la matriz de datos.
	archivo = fopen(fName,"r");
	if (archivo == NULL) {
		printf("\nError de apertura del archivo. \n\n");
		exit(0);
	} else {
	
		
		int i=0;
		
		while(fgets(Linea, 200, (FILE*) archivo)) {
			if (strlen(Linea)>1) {
				t=0;
				c=0;
				for (i=0; i<strlen(Linea); i++) {
					if (Linea[i] == ',') {
						for (int x=t; t<10; t++) {
							Str[x]='\0';
						}
						f1= atof(Str);
						Metricas[j][c]=f1;
						t=0;
						c++;
					} else {
						if (Linea[i] != '\n') {
							Str[t]=Linea[i];
							t++;
						}
					}
				}
				Str[t]='\0';
				f1= atof(Str);
				Metricas[j][c]=f1;
			}
			j++;

		}

	}	
	fclose(archivo);		

	free(fName);
	*numCls = vNumFiles;
	return Metricas;
}

//---------------------------------------------------------------------------------
// Rutina que calcula la distribución normal apartir de un valor x, la media 
// y la varianza.
//---------------------------------------------------------------------------------
float CalcDistriNormal(float Valor, float Med, float Var){
	
	return( (1/(sqrt(2*PI*Var))) * (exp(-(pow(Valor - Med,2))/(2*Var))) );

}

//---------------------------------------------------------------------------------
// Rutina con distribución normal.
// Rutina que evalua la BD de prueba (BDP) con las estadísticas de la BD de 
// entrenamiento (BDEE).  Se aplica para los archivos de prueba que se especifican
// (NumFiles).  Regresa la evaluación de cada archivo si es hotspot o no, en el
// vector solución: VSol.
//---------------------------------------------------------------------------------
float *ValidaBDdn(float **BDP,int NumFiles,float **BDEE, int *VComb){

	float *VSol = (float*)malloc(NumFiles*sizeof(float));
	
	float vProb0;
	float vProb1;
	
	
		vProb0 = BDEE[cNG-1][0];
		vProb1 = BDEE[cNG-1][2];
	
	
	//printf("Prob(0):%1.4f, Prob(1):%1.4f\n",vProb0,vProb1);
	
	for(int i=0;i<NumFiles;i++){
    	VSol[i]=0;
	}
	
    for(int i=0;i<NumFiles;i++){			//Files

		float vProbSol = 0;

    	for(int c=0; c<2; c++){				//Clases a agrupar
    	
		    int vCls = 0;
		    float vProb;

			//Probabilidad a PRIORI (probabilidad de que sea 1=Hotspot o 0=No Hotspot)
			if (c == 0){
				vProb = vProb0;
			}else{
				vProb = vProb1;
			}

			//Calcula la porbabilidad a POSTERIORI con distribución normal en cada Atributo.
    		for(int a=0; a<(cNG-1);a++){	//Atributos a Evaluar
    		
    			if (VComb[a] == 1){
					if (c == 0){
						vProb = vProb *  CalcDistriNormal(BDP[i][a], BDEE[a][0], BDEE[a][1]);
					}else{
						vProb = vProb *  CalcDistriNormal(BDP[i][a], BDEE[a][2], BDEE[a][3]);
					}
				}
			}
			
			if (vProb > vProbSol){
				VSol[i]=c;
				vProbSol = vProb;
			}
    		
		}
	}
	
	return(VSol);

}
//---------------------------------------------------------------------------------
// Rutina que despliega en pantalla los resultados de cada uno de los archivos 
// evaluados. Los resultados están representados por los valores de la matriz de 
// confusión: verdadesrps positivos (TP), verdaderos negativos (TN),
// falsos positivos (FP) y falsos negativos (FN).
//---------------------------------------------------------------------------------
void DespliegaResultados(int NumClsP, float **BDPrueba, float *VSolCls){

    int TP=0, TN=0, FP=0, FN=0;
		 
    for(int i=0; i<NumClsP;i++){
    	
    	if(VSolCls[i] == BDPrueba[i][cNG-1]){
    		if (VSolCls[i] == 1){
    			TP++;
			}else{
				TN++;
			}
		}else{
    		if (VSolCls[i] == 1){
    			FP++;
			}else{
				FN++;
			}
			
		}
	}
	fprintf(fp,",%d,%d,%d,%d\n",TP,TN,FP,FN);
	//printf(".");

}

//---------------------------------------------------------------------------------
// Rutina que convierte un número decimal a un número binario.
//---------------------------------------------------------------------------------
int FromDecToBin(int n, int *R){

    *R = n%2;               //almaceno el resto en R
    return (n - *R)/2;
}
//---------------------------------------------------------------------------------
// Rutina que calcula la media y varianza por cada archivo  para realiza la 
// evaluación de la probabilidad condicional por clase: p(Cj|M1,...,Mn) y se 
// clasifique como hotspot un archivo cuando p(C1|M1,...,Mn) > p(C0|M1,...,Mn), 
// en otro caso se clasificará como no hotspot.
//---------------------------------------------------------------------------------
float **CalculaEstadBD(int NumFiles, float **BDE)
{

	float **EstBD = (float**)malloc(cNG*sizeof(float*));
	for (int a=0;a<cNG;a++){
		EstBD[a] = (float*)malloc(4*sizeof(float));	
		
		//Clase 0
		EstBD[a][0]=0;	//Media (promedio=sum(X)/n
		EstBD[a][1]=0;	//Varianza (sum(X-Media)/n-1)

		//Clase 1
		EstBD[a][2]=0;	//Media (promedio=sum(X)/n
		EstBD[a][3]=0;	//Varianza (sum(X-Media)/n-1)

	}
	
	//--- Obtiene Frecuencia de las clases 0 y 1.
	float FreqCls0=0, FreqCls1=0;
	
	for(int f=0; f<NumFiles;f++){
		
		if(BDE[f][cNG-1] == 0){
			FreqCls0++;
		}else{
			FreqCls1++;
		}		
	}

	EstBD[cNG-1][0]= FreqCls0 / NumFiles;
	EstBD[cNG-1][1]= FreqCls0;
	EstBD[cNG-1][2]= FreqCls1 / NumFiles;
	EstBD[cNG-1][3]= FreqCls1;

	
	//--- Calcula Estadísticas de la BD (f,a)
	float MedX0=0, MedX1=0;
	float VarX0=0, VarX1=0;

	for(int a=0; a<(cNG-1);a++){
		
		//--Calcula MEDIA
		MedX0=0;
		MedX1=0;
		
		for(int f=0; f<NumFiles; f++){

			if(BDE[f][cNG-1] == 0){
				MedX0 += BDE[f][a];
			}else{
				MedX1 += BDE[f][a];
			}

		}
		MedX0 = MedX0 / NumFiles;
		MedX1 = MedX1 / NumFiles;
		
		//--Calcula VARIANZA
		VarX0=0;
		VarX1=0;
		
		for(int f=0; f<NumFiles; f++){

			if(BDE[f][cNG-1] == 0){
				VarX0 += pow(BDE[f][a] - MedX0,2);
			}else{
				VarX1 += pow(BDE[f][a] - MedX1,2);
			}

		}
		VarX0 = VarX0 / (FreqCls0 - 1); 
		VarX1 = VarX1 / (FreqCls1 - 1);
		
		//---Graba resultados
//		if (a<(cNG-2)){
			EstBD[a][0] = MedX0;
			EstBD[a][1] = VarX0;
			EstBD[a][2] = MedX1;
			EstBD[a][3] = VarX1;
//		}
		
	}
	
	return(EstBD);
}

//---------------------------------------------------------------------------------
// Rutina que aplica la normalización por el máximo valor en la métrica de número 
// de líneas (es la columna 2 de la BD).  Aplica la nomralización a las 16 métricas
// que tiene la BD. Regresa la misma BD nomralziada.
//---------------------------------------------------------------------------------
void NormailizaDBconCLOC(float **BD, int NumFiles)
{
	//----Obten el máximo número de líneas
	float MaxCLOC=0;
	for(int f=0;f<NumFiles; f++){
		if(BD[f][1] > MaxCLOC){
			MaxCLOC = BD[f][1];
		}
	}
	
	//----Normaliza BD con base CLOC Máximo
	for(int f=0; f<NumFiles;f++){
		for(int g=0;g<cNG;g++){
			if (BD[f][1] != 0){
				BD[f][g] = BD[f][g] * (MaxCLOC / BD[f][1]);
			}else{
				BD[f][g] = 0;
			}			
		}
	}
}
//---------------------------------------------------------------------------------
// Rutina que aplica la normalización por máximo valor en cada una de las métricas.
// Se le especifica la BD y el número de archivos que contiene la BD. Regresa la 
// misma BD nomralziada.
//---------------------------------------------------------------------------------

void NormailizaDBconMax(float **BD, int NumFiles)
{

	
	//----Normaliza BD con base Máximo por cada métrica F
	for(int g=0;g<cNG -1;g++){

		//----Obten el máximo de la métrica g
		float MaxG=0;
		for(int f=0;f<NumFiles; f++){
			if(BD[f][g] > MaxG){
				MaxG = BD[f][g];
			}
		}
		
		//----Normaliza con el MaxF
		for(int f=0; f<NumFiles;f++){
			if (BD[f][g] != 0){
				BD[f][g] = BD[f][g] / MaxG;
			}else{
				BD[f][g] = 0;
			}			
		}
	}
}

//---------------------------------------------------------------------------------
// Rutina que con el numComb se obtiene una cada binaria de longitud cNG - 1 
// (métricas que se ocupan) donde un 1 es que la métrica participa en esta
// evaluación y un 0 que la métrica no participa en esta evaluación.
//---------------------------------------------------------------------------------
int *ObtenCombAtributos(int NumComb)
{

    int *B = (int*)malloc((cNG-1)*sizeof(int));
    int n;
    int R;

	for(int i=0;i<cNG-1;i++){
		B[i]=0;
	}
	n=NumComb;

    int i = 0;
    
    while( n >= 1 ){            		//el bucle finaliza cuando ya no es posible seguir dividiendo por 2
        n = FromDecToBin( n, &R );
        B[i] = R;     					//armo el vector con los restos de las divisiones
        i++;
    }

	fprintf(fp,"%d,",NumComb);
    for(int j=0; j<cNG-1; j++ ){      	//imprime todos los elementos almacenados en el vector
        fprintf(fp,"%d", B[j]);
    }

	return(B);
}


//********************************************************************************************
//***********************       I N I C I A   P R O G R A M A      ***************************
//********************************************************************************************
int main() {

	float **BDEntrena;
	float **BDPrueba;
	float ***BDEntrenada;
	float **BDEstEntrena;

	int   *VComb;
	float *VSolCls;

	int   NumClsE=0;
	int   NumClsP=0;

	//-------- Inicializaciòn de Valores.
	BDEntrena    = CargaBaseDatos(&NumClsE,cAppE);
	BDPrueba     = CargaBaseDatos(&NumClsP,cAppP);
	
	//-------- Aplica método de normalización.
	switch (cMN)
	{
		case 1:	//nMax
			NormailizaDBconMax(BDEntrena, NumClsE);
			NormailizaDBconMax(BDPrueba, NumClsP);
	  	case 2:	//nCLOC
	    	NormailizaDBconCLOC(BDEntrena, NumClsE);
			NormailizaDBconCLOC(BDPrueba, NumClsP);
			break;
	}
	
	//--------- Calcula las estadísticas de la BD de entrenamiento para aplicar el algoritmo.
	BDEstEntrena = CalculaEstadBD(NumClsE, BDEntrena);

	//--------- Abre el archivo de reultados para comenzar a escribir en él.
	AbreArchivoResultados();

	//--------- Aplica el algoritmo de BI para todas las combinaciones de métricas.
	for(int nComb=1; nComb < pow (2, cNG-1); nComb++){


		VComb = ObtenCombAtributos(nComb);

		//--------- Evalua la combianción de métricas seleccionada.
		VSolCls = ValidaBDdn(BDPrueba,NumClsP,BDEstEntrena,VComb);

		DespliegaResultados(NumClsP, BDPrueba, VSolCls);
	
	}
	//-------- Cierra el archivo de resultados.
	CierraArchivoResultados();
	
	return(0);

}

//-------------------------------------------------------------------------------------------------------------

