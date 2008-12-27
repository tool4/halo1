#include "cspolka.h"
#include "cportfel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

void log_p(char* str)
{
	printf("%s\n", str );
}
void log_p(const char* str)
{
	printf("%s\n", str );
}

CPortfel::CPortfel(std::string name)
{
	FILE* plik;
	char str[BUFSIZE];

	CSpolka *prev,*cur;

	m_wolumens[0].SetName( (char*) "ALWAYS" );
	m_wolumens[1].SetName( (char*) "NASDAQ" );
	
	m_name = name;
	m_count = 0;
	m_cash = 1250;
	m_str = "" ;
	m_companies = 0;
	m_changed=false;
	m_spolki = NULL;
	next = NULL;
	prev = NULL;

	system( "mkdir -p ./portfels");
	config_file = "./portfels/portfel_";
	config_file += name;
	config_file += ".txt";

	log_p(config_file.c_str());
	plik = fopen(config_file.c_str(),"r");
	if(!plik)
	{
		printf("cannot open file %s\n",
			   config_file.c_str() );
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

CPortfel::~CPortfel()
{
	Delete();
}

std::string CPortfel::GetName()
{
	return m_name;
}

CSpolka *CPortfel::GetSpolka(int index)
{
	return m_spolki;
}
CSpolka *CPortfel::GetSpolki()
{
	return m_spolki;
}

CSpolka *CPortfel::GetSpolka(std::string name)
{
	return m_spolki;
}

double CPortfel::GetCash()
{
	return m_cash;
}

void CPortfel::SetCash(double cash)
{
	m_cash = cash;
}

void CPortfel::List()
{
	CSpolka *sp = m_spolki;
	
	while(sp!=NULL)
	{
		log_p(sp->GetName().c_str());
		sp=sp->next;
	}
}

void CPortfel::Print()
{
	CSpolka *sp = m_spolki;
	
	printf( "\n%20s\t%12s\t%12s\t%12s\n",
			"Spolka","kurs", "zmiana %","otwarcia");

	while(sp!=NULL)
	{
// 		log(sp->GetName().c_str());
		sp->Print();
		sp=sp->next;
	}
}

void CPortfel::SetStr(char* str)
{
	m_str = str;
}
void CPortfel::Save()
{
	CSpolka *sp = m_spolki;
	
	while(sp!=NULL)
	{
		//log_p(sp->GetName().GetBuffer(0));
		sp->Save(m_name);
		sp=sp->next;
	}
}


bool CPortfel::Update_Shares(std::string str, std::string data)
{
	CSpolka *sp = m_spolki;
	bool ret = false;
	
	while(sp!=NULL)
	{
		if( sp->GetName() == str )
		{
			ret = sp->Update(data);
			sp->Save(m_name);
			return ret;
		}
		sp=sp->next;
	}
	return ret;
}

bool CPortfel::Update(std::string data)
{
 	char buf[BUFSIZE];
 	sscanf(data.c_str(),"%s",buf);
	
//  	log(buf);
//  	log("\n");

	return Update_Shares(buf, data);
}

void CPortfel::Delete()
{
	CSpolka *sp = m_spolki;
	CSpolka *prev;
	while(sp!=NULL)
	{
		prev=sp;
		sp=sp->next;
		delete prev;
	}
	m_spolki = NULL;
}

//set changed flag
//to prevent writing delta report at start of app execution
//used in EndOfDay func
void CPortfel::Changed()
{
	m_changed = true;
}

void CPortfel::SetCashToFile(double cash, const char* str)
{
	char filename[BUFSIZE];
	FILE *plik;

	strncpy( filename, "./config/", BUFSIZE );
	strncpy( filename+strlen("./config/"),
			 m_name.c_str(),
			 BUFSIZE - strlen("_cash.txt") -strlen("./config/") );

	strcat( filename,str);
	strcat( filename,"_cash.txt");
	plik = fopen(filename,"w");
	if(plik)
	{
		fprintf(plik,"%.2f\n",cash);
		fclose(plik);
	}
	m_cash = cash;
}

double CPortfel::GetCashFromFile(char* str)
{
	char filename[BUFSIZE];
	FILE *plik;
	double cash = 100000;	//dafaultcash 100k

	char buf[BUFSIZE];

	strncpy( filename, "./config/", BUFSIZE );
	strncpy( filename+strlen("./config/"),
			 m_name.c_str(),
			 BUFSIZE - strlen("_cash.txt") -strlen("./config/") );
	
	strcat(filename,str);
	strcat(filename,"_cash.txt");
	plik = fopen(filename,"r");
	if(plik)
	{
		fscanf(plik,"%32s ",buf);
		cash = atof(buf);
		fclose(plik);
	}
	else
		SetCashToFile(cash, str);

	printf( "%s cash: %5.2f\n",m_name.c_str() , cash);

	m_cash = cash;
	return cash;
}


void CPortfel::EndOfDay()
{
	CSpolka *sp = m_spolki;

	printf("END of day\n");

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

	SetCashToFile( m_cash, m_str.c_str() );
}

void CPortfel::StartOfDay(double nasdaq_change )
{
	double cash_per_co = 0.0;
	double remaining_cash = m_cash;
	double remain = 0.0;

	printf("start of day\n");

	if( nasdaq_change < 1.0 )
	{
		log( "%s nasdaq change: %.2f, disregard buying - too risky\n",
			 gettime(),
			 nasdaq_change );
		return;
	}

	log( "%s nasdaq change: %.2f,  buying wig20, available cash: %.2f\n",
		 gettime(),
		 nasdaq_change,
		 m_cash );
	
	if( m_cash > 0)
	{
		cash_per_co = m_cash / m_companies;
	}
	log( "cash per co: %.2f\n", cash_per_co );

	char buf[BUFSIZE];
	strcpy( buf, "./notowania/");
	strncat( buf, m_wolumens[ 0 ].GetName(),BUFSIZE/2);
	strcat( buf, ".txt");
	logf( buf, gettime() );
	logf( buf, (char*) "\n" );

	CSpolka *sp = m_spolki;
	while(sp!=NULL)
	{
		remaining_cash -= sp->Buy(cash_per_co + remain,remain);
		sp->Print( buf );
		sp=sp->next;
	}
	m_cash = remaining_cash;
	log( "cash left: %.2f\n", m_cash );
}
