#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "main.h"
#include "get_nasdaq.h"

void preparsen(char *filein,char *fileout)
{
	FILE* plik1;
	FILE* plik2;
	char c=0,lastc,lastf=0;
	int in=0;
	
	plik1=fopen(filein,"r");
	if(!plik1)
	{
		printf("cannot open file in");
		return;
	}

	plik2=fopen(fileout,"w");
	if(!plik2)
	{
		fclose(plik1);
		printf("cannot open file out");
		return;
	}


	while(!feof(plik1))
	{
		lastc=c;
		c=fgetc(plik1);
		if(c=='<')
		{
			++in;
		}
		if(c==' ' && lastc==' ')
			continue;
		if(c==' ' && lastc=='\n')
			continue;

		if(c==';')
		{	
			if(lastf != '\n')
				fputc('\n',plik2);
			lastf='\n';
			continue;
		}

		if(in==0)
		{
			lastf=c;
			fputc(c,plik2);
		}

		if(c=='>')
		{
			--in;
			if( in < 0 )
				in = 0;
		
			if(lastf != '\n')
				fputc('\n',plik2);
			lastf='\n';
			
		}
	}
	fclose(plik1);
	fclose(plik2);
}


double parsen(char* filename, char* index)
{
	FILE *plik;
	char buf[BUFSIZE];
	double nasdaq = 0.0;
	double nasdaq_change = 0.0;
	char* c;

	plik = fopen(filename,"r");
	if(!plik)
		return 0.0;

	while(!feof(plik))
	{
		fgets(buf,BUFSIZE,plik);
		if(strncmp(index, buf,strlen(index))==0)
		{
			fgets(buf,BUFSIZE,plik);
			c = strchr(buf, ' ');
			if(c)
				strcpy(c, c+1);
			c = strchr(buf, ',');
			if(c)
				*c='.';
			nasdaq = atof(buf);
			fgets(buf,BUFSIZE,plik);
			c = strchr(buf, ' ');
			if(c)
				strcpy(c, c+1);
			c = strchr(buf, ',');
			if(c)
				*c='.';
			nasdaq_change = atof(buf);
			sprintf( buf, "%s: %f, change: %f",
					 index, nasdaq, nasdaq_change );
			//m_NasdaqStatic = _T(buf);

			break;
		}
	}
	fclose(plik);

	//if(nasdaq_change>=1.0)
	//	return true;

	return nasdaq_change;
}

double get_indexes( SIndexes* indexes)
{
	double nasdaq = 0.0;
	double djia = 0.0;
	double s_p_500 = 0.0;
	double nikkei = 0.0;
	double hangseng = 0.0;
	double cac_40 = 0.0;
	double ftse = 0.0;

    printf( "Getting stocks fromm millenniumdm.pl:\n" );
	system( "wget http://www.millenniumdm.pl/index2.html -O files/millenniumdm.html > /dev/null" );
	printf("[ OK ]\n" );


	preparsen((char*)"./files/millenniumdm.html",(char*)"./files/millenniumdm.txt");

	nasdaq   = parsen((char*)"./files/millenniumdm.txt", (char*)"NASDAQ");
	djia     = parsen((char*)"./files/millenniumdm.txt", (char*)"DJIA");
	s_p_500  = parsen((char*)"./files/millenniumdm.txt", (char*)"S&P 500");
	nikkei   = parsen((char*)"./files/millenniumdm.txt", (char*)"NIKKEI 225");
	hangseng = parsen((char*)"./files/millenniumdm.txt", (char*)"HANG SENG");
	cac_40   = parsen((char*)"./files/millenniumdm.txt", (char*)"CAC 40");
	ftse     = parsen((char*)"./files/millenniumdm.txt", (char*)"FTSE");

	indexes->nasdaq   = nasdaq;
	indexes->djia     = djia;
	indexes->s_p_500  = s_p_500;
	indexes->nikkei   = nikkei;
	indexes->hangseng = hangseng;
	indexes->cac_40   = cac_40;
	indexes->ftse     = ftse;

	return nasdaq;
}

void print_indexes(SIndexes indexes)
{
	printf( "nasdaq:    %15.2f\n", indexes.nasdaq );
	printf( "djia:      %15.2f\n", indexes.djia );
	printf( "S&P 500:   %15.2f\n", indexes.s_p_500 );
	printf( "NIKKEI:    %15.2f\n", indexes.nikkei );
	printf( "HANG SENG: %15.2f\n", indexes.hangseng );
	printf( "CAC 40:    %15.2f\n", indexes.cac_40 );
	printf( "FTSE:      %15.2f\n", indexes.ftse );

	log( "nasdaq:    %15.2f\n", indexes.nasdaq );
	log( "djia:      %15.2f\n", indexes.djia );
	log( "S&P 500:   %15.2f\n", indexes.s_p_500 );
	log( "NIKKEI:    %15.2f\n", indexes.nikkei );
	log( "HANG SENG: %15.2f\n", indexes.hangseng );
	log( "CAC 40:    %15.2f\n", indexes.cac_40 );
	log( "FTSE:      %15.2f\n", indexes.ftse );
}

