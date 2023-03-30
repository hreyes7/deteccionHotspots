/*---------------------------------------------------------------------------------------
* Materia       : PROYECTO DE INVESTIGACIÓN
* Proyecto      : IDENTIFICACIÓN DE POTENCIALES HOTSPOTS USANDO MÉTODOS DE CLASIFICACIÓN
* Algoritmo     : De búsqueda exhaustiva.
* Funcionalidad : Recibe las BD de entrenameinto.  Con base en ella hace la clasificación
*                 en la BD de pruebas de los hotspots.
*
* Entradas      :
*                  - BD a evaluar exhaustivamente. Se debe de especificar el nombre 
*                    del archivo. El formato del archivo es CSV y contiene las 16 métricas
*                    del estudio más la etiqueta de que indica si es hotspot (1) o 
*                    no es hotspot (0). Constante: cAppE.
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
#define cAppE "flume_16_pru.csv"		//Nombre de archivo de entrada para evaluar.		
										//"App9_16_ent.csv" //"Entrena.txt"	//"brokerj_7_pru.csv" //
#define cRes "Salida_Exh_"				//Prefijo del nombre de salida con resultados.
#define cMN 0							//nMax=1, nCLOC=2 o ninguno=0

//---------------------------------------------------------------------------------
// VARIABLES GLOBALES
//---------------------------------------------------------------------------------

FILE *fp;								//Archivo de resultados.

//---------------------------------------------------------------------------------
// FUNCIONES DE APOYO
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
		Metricas[i]= (float*)malloc((cNG)*sizeof(float));
	}
	
		
//--Carga la BD a la matriz de datos.
	archivo = fopen(fName,"r");
	if (archivo == NULL) {
		printf("\nError de apertura del archivo. \n\n");
		exit(0);
	} else {
	
		
		int i=0;
		
		while(fgets(Linea, 200, (FILE*) archivo)) {
		
//				printf("\nLinea: %d ------l(%d)---------> %s",j,strlen(Linea),Linea);
			if (strlen(Linea)>1) {
				t=0;
				c=0;
				for (i=0; i<strlen(Linea); i++) {
					if (Linea[i] == ',') {
						for (int x=t; t<10; t++) {
							Str[x]='\0';
						}
						f1= atof(Str);
//                           printf("%4.0f>",f1);
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
//              	printf("%4.0f>\n",f1);
				Metricas[j][c]=f1;
//				if(j == 0) {
//					*numGenes = c+1;
//				}
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
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
void GuardaResultado(float **BDEvaluada, int NumFiles)
{

    int TP=0, TN=0, FP=0, FN=0;
		 
    for(int f=0; f<NumFiles; f++){
    	
    	if(BDEvaluada[f][2] == BDEvaluada[f][3]){	//HPP == HPR
    		if (BDEvaluada[f][2] == 1){
    			TP++;
			}else{
				TN++;
			}
		}else{
    		if (BDEvaluada[f][2] == 1){
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
int FromDecToBin(int n, int *R){

    *R = n%2;               //almaceno el resto en R
    return (n - *R)/2;
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
void NormailizaBD_CLOC(float **BD, int NumFiles)
{

	float MaxValAtr;

//		printf("--------------------------------------------------------------------\n");
//	for(int a=0;a<cNG;a++){
//		for(int f=0;f<NumFiles;f++){
//			printf("BD[%d][%d]:%1.5f, ",f,a,BD[f][a]);
//		}
//		printf("\n");
//	}
//
//		printf("--------------------------------------------------------------------\n");
	
	
	//---Busca el archivo con el mayor número de líneas.  SCC_CLOC esta en la segunda columna de la BD.
	MaxValAtr = -1;
	
	for(int f=0; f<NumFiles; f++){
		if (BD[f][1] > MaxValAtr){
			MaxValAtr = BD[f][1];
		}
	}		
		
	for(int a=0; a<(cNG - 1); a++){

//		printf("MaxValAtr(%d): %f\n",a,MaxValAtr);
		
		//--Normaliza los Valores del Atributo con respecto al Máximo Valor en el Atributo.
		for(int f=0; f<NumFiles; f++){
		
			if(MaxValAtr != 0){
				BD[f][a] = round(BD[f][a] * (MaxValAtr / BD[f][1]));
			}
		
		}
	}

//	for(int a=0;a<cNG;a++){
//		for(int f=0;f<NumFiles;f++){
//			printf("BD[%d][%d]:%1.5f, ",f,a,BD[f][a]);
//		}
//		printf("\n");
//	}

}
//---------------------------------------------------------------------------------
void NormailizaBD_MAX(float **BD, int NumFiles)
{

	float MaxValAtr;

//		printf("--------------------------------------------------------------------\n");
//	for(int a=0;a<cNG;a++){
//		for(int f=0;f<NumFiles;f++){
//			printf("BD[%d][%d]:%1.5f, ",f,a,BD[f][a]);
//		}
//		printf("\n");
//	}
//
//		printf("--------------------------------------------------------------------\n");
		
		
	for(int a=0; a<(cNG - 1); a++){

        MaxValAtr = -1;

		//--Obtiene Máximo Valor en el Atributo.
		for(int f=0; f<NumFiles; f++){
				
			if (BD[f][a] > MaxValAtr){
				MaxValAtr = BD[f][a];
			}		
		
		}

//		printf("MaxValAtr(%d): %f\n",a,MaxValAtr);
		
		//--Normaliza los Valores del Atributo con respecto al Máximo Valor en el Atributo.
		for(int f=0; f<NumFiles; f++){
		
			if(MaxValAtr != 0){
				BD[f][a] = BD[f][a] / MaxValAtr;
			}
		
		}
	}

//	for(int a=0;a<cNG;a++){
//		for(int f=0;f<NumFiles;f++){
//			printf("BD[%d][%d]:%1.5f, ",f,a,BD[f][a]);
//		}
//		printf("\n");
//	}

}
//---------------------------------------------------------------------------------
float **EvaluaBD(float **BDE, int NumFiles, int *VComb)
{

    int iTopN;

	//----Obtiene los HPR
	for(int f=0;f<NumFiles;f++){
		if(BDE[f][cNG-1] == 1){
			iTopN += 1;
		}	
	}
	
	
	//----Crea vector de resultados de clases TopN
	float **TopN = (float**)malloc(NumFiles*sizeof(float*));
	for (int f=0;f<NumFiles;f++){
		TopN[f] = (float*)malloc(4*sizeof(float));	
		
		//---
		TopN[f][0] = -1;	//--- ValFO
		TopN[f][1] = -1;	//--- #File
		TopN[f][2] = -1;	//--- HotsPot Real (HPR)     : 1=EsHotspot, 0=NoEsHotspot
		TopN[f][3] = -1;	//--- Hotspot Propuesto (HPP): 1=EsHotspot, 0=NoEsHotspot
		
	}	


//------------
	int n = -1;

	for (int f=0;f<NumFiles;f++){

		float vFO = 0;
		
		//-- Calcula el Valor de la FO para el archivo f con base a sus atributos a's.
		for(int a=0; a<(cNG - 1);a++){
			
			vFO += (BDE[f][a] * VComb[a]);
			
		}

		//-------------Inicio de Ordenamiento del resultado para tener los resultados de los f archivos y obtener el Q3 (cuartil 3).			
	    if (n<0){	//Es el primer valor para ingresar en el TopN
	    	n=0;
	    	TopN[n][0] = vFO;
	    	TopN[n][1] = (f);
			TopN[n][3] = BDE[f][cNG-1];
		}else{
			int t=0;
			int m=0;
			while (m == 0 && t<NumFiles){
				if (vFO >= TopN[t][0]){
					m=1;
				}else{
					t++;
				}
			}
			if (t < NumFiles){
				if(t>n){
			    	TopN[t][0] = vFO;
			    	TopN[t][1] = f;
			    	TopN[t][3] = BDE[f][cNG-1];
			    	n++;
				}else{
					if(t<=n){

			    		for(int d=(NumFiles-1); d>t ;d--){
					    	TopN[d][0]=TopN[d-1][0];
					    	TopN[d][1]=TopN[d-1][1];					    			
					    	TopN[d][3]=TopN[d-1][3];
						}

				    	TopN[t][0]=vFO;
				    	TopN[t][1]=f;
				    	TopN[t][3]=BDE[f][cNG-1];;

				    	n++;

				    	if (n >= NumFiles){
							n--;
						}
					}
				}
			}
		}//-------------Fin de Ordenamiento.
	}
	
	//----- Determina si es Hotspot o no.
	for (int f=0;f<NumFiles;f++){
		if (f < iTopN){			//Es Hotspot
			TopN[f][2] = 1;
		}else{					//NO es Hotspot
			TopN[f][2] = 0 ;
		}
	}

//	printf("%f\n",Q3);
//	for (int f=0;f<NumFiles;f++){
//		printf("TopN[%d]: %f,%f,%f,%f\n",f,TopN[f][0],TopN[f][1],TopN[f][2],TopN[f][3]);
//	}	

	return(TopN);
	
}
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
    
    while( n >= 1 ){            //el bucle finaliza cuando ya no es posible seguir dividiendo por 2
        n = FromDecToBin( n, &R );
        B[i] = R;     //armo el vector con los restos de las divisiones
        i++;
    }

	fprintf(fp,"%d,",NumComb);
//    printf("%d,",NumComb);
    for(int j=0; j<cNG-1; j++ ){      //imprime todos los elementos almacenados en el vector
        fprintf(fp,"%d", B[j]);
//        printf("%i", B[j]);
    }

	return(B);
}


//********************************************************************************************
//***********************       I N I C I A   P R O G R A M A      ***************************
//********************************************************************************************
int main() {

	float **BDEntrena;
	int   NumClsE=0;
	float **BDEvaluada;
	int   *VComb;

//----	
	float **BDPrueba;
	float **BDEstEntrena;
	float ***BDEntrenada;

	int   *VClsDif = (int*)malloc(cNG*sizeof(int));
	float *VSolCls;

	int   NumClsP=0;

	//-------- Inicializaciòn de Valores.
	BDEntrena    = CargaBaseDatos(&NumClsE,cAppE);

	//-------- Aplica método de normalización.
	switch (cMN)
	{
		case 1: //nMax
			NormailizaBD_MAX(BDEntrena,NumClsE);
	  	case 2: //nCLOC
	    	NormailizaBD_CLOC(BDEntrena,NumClsE);
			break;
	}
	
	AbreArchivoResultados();


	for(int nComb=1; nComb < pow (2, cNG-1); nComb++){

		VComb = ObtenCombAtributos(nComb);
		BDEvaluada = EvaluaBD(BDEntrena, NumClsE, VComb);
		GuardaResultado(BDEvaluada, NumClsE);

	}

	CierraArchivoResultados();
	
	return(0);

}

//-------------------------------------------------------------------------------------------------------------

