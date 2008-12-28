#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cwolumen.h"

CWolumen::CWolumen()
{
	m_buy_price = 0.0;
	m_amount = 0;
	m_spolki = NULL;
	m_cash = 1250;
	m_changed=false;
}

CWolumen::~CWolumen()
{
	Delete();
}

void CWolumen::Delete()
{
	CSpolka *sp = m_spolki;
	CSpolka *prev;
	while( sp!=NULL )
	{
		prev=sp;
		sp=sp->next;
		delete prev;
	}
	m_spolki = NULL;
}

CSpolka* CWolumen::Spolki()
{
	return m_spolki;
}

void CWolumen::InitSpolki( char* filename )
{
	char str[BUFSIZE];
	CSpolka *prev,*cur;
	m_companies = 0;

	GetCashFromFile();

	FILE* plik = fopen( filename,"r" );
	if(!plik)
	{
		printf("cannot open file %s\n",
			   filename );
		return;
	}
	while(!feof(plik))			//read config for list of companies (spolka)
	{
		memset(str,0,BUFSIZE);
		fgets(str,BUFSIZE,plik);
		str[strlen(str)-1]=0;	//delete ending\n
		if(str[0]==0)			//break on empty line
			break;

		cur = new CSpolka(str);
		if(m_spolki == NULL)
		{
			m_spolki = prev = cur;
		}
		else{
			prev->next = cur;
			cur->prev = prev;
			prev = cur;
		}
		m_companies++;
		if(feof(plik))
			break;
	}
	fclose(plik);
}

void CWolumen::SetName( char * name )
{
	m_name = name;
}

char* CWolumen::GetName( void )
{
	return (char *)m_name.c_str();
}


double CWolumen::Buy(double cash, double &remainder)
{
	return 0.0;
}

double CWolumen::Sell(int amount)
{
	CSpolka *sp = Spolki();
	
	while(sp!=NULL)
	{
		sp->Save(m_name,1);
		m_cash += sp->Sell();
		if(m_changed)
		{
			if(sp->next)		
				sp->Report(m_name);
			else						//add \n char to report file
				sp->Report(m_name,m_cash);
		}
		//	sp->Report(m_name);
		sp=sp->next;
	}
	log( "%s ALL shares sold, available cash: %.2f\n",
		 gettime(),
		 m_cash );
		
	SetCashToFile( m_cash );

	return  m_cash;
}

void CWolumen::Print()
{
	printf( "current amount: %ld, value: %.2f PLN\n",
			m_amount, 
			m_buy_price * m_amount );
}

int CWolumen::NumCompanies()
{
	return m_companies;
}

void CWolumen::SetCashToFile( double cash )
{
	char filename[BUFSIZE];
	FILE *plik;

	strncpy( filename, "./config/", BUFSIZE );
	strncpy( filename+strlen("./config/"),
			 m_name.c_str(),
			 BUFSIZE - strlen("_cash.txt") -strlen("./config/") );

	strcat( filename,"_cash.txt");
	plik = fopen(filename,"w");
	if(plik)
	{
		fprintf(plik,"%.2f\n",cash);
		fclose(plik);
	}
	m_cash = cash;
}

double CWolumen::GetCashFromFile()
{
	char filename[BUFSIZE];
	FILE *plik;
	double cash = 100000;	//dafaultcash 100k

	char buf[BUFSIZE];

	strncpy( filename, "./config/", BUFSIZE );
	strncpy( filename+strlen("./config/"),
			 m_name.c_str(),
			 BUFSIZE - strlen("_cash.txt") -strlen("./config/") );
	strcat(filename,"_cash.txt");

	plik = fopen(filename,"r");
	if( plik )
	{
		fscanf( plik,"%32s ", buf );
		cash = atof( buf );
		fclose( plik );
	}
	else
		SetCashToFile( cash );

	logp( "%s cash: %5.2f\n", m_name.c_str(), cash );

	m_cash = cash;
	return cash;
}

void CWolumen::StartOfDay(double nasdaq_change )
{
	double cash_per_co = 0.0;
	double remaining_cash = m_cash;
	double remain = 0.0;

	logp( "%s\n", m_name.c_str() );
	printf("start of day\n");

	if( m_name == "WIG20_NASDAQ" )
	{
		if( nasdaq_change < 1.0 )
		{
			logp( "%s nasdaq change: %.2f, disregard buying - too risky, avail cash: %.2f\n",
				 gettime(),
				 nasdaq_change,
				 m_cash );
			return;
		}
		logp( "%s nasdaq change: %.2f,  buying wig20, available cash: %.2f\n",
			 gettime(),
			 nasdaq_change,
			 m_cash );
	}

	if( m_cash > 0)
	{
		cash_per_co = m_cash / m_companies;
	}
	logp( "cash per co: %.2f\n", cash_per_co );

	char buf[BUFSIZE];
	strcpy( buf, "./notowania/");
	strncat( buf, GetName(),BUFSIZE/2);
	strcat( buf, ".txt");
	logf( buf, gettime() );
	logf( buf, (char*) "\n" );

	CSpolka *sp = Spolki();
	while(sp!=NULL)
	{
		remaining_cash -= sp->Buy(cash_per_co + remain,remain);
		sp->Print( buf );
		sp=sp->next;
	}
	m_cash = remaining_cash;
	logp( "cash left: %.2f\n", m_cash );
	
}
