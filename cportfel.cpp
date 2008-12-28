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

	m_wolumens[0].SetName( (char*) "WIG20_ALWAYS" );
	m_wolumens[1].SetName( (char*) "WIG20_NASDAQ" );
	
	m_name = name;
	m_count = 0;
	m_str = "" ;
	next = NULL;
	prev = NULL;

	system( "mkdir -p ./portfels");
	config_file = "./portfels/portfel_";
	config_file += name;
	config_file += ".txt";

	log_p( config_file.c_str() );

	for( int i = 0; i < WOLUMENS; i++ )
	{
		m_wolumens[ i ].InitSpolki( (char*)config_file.c_str() );
	}
	m_companies = m_wolumens[ 0 ].NumCompanies();
}

CPortfel::~CPortfel()
{
	Delete();
}

std::string CPortfel::GetName()
{
	return m_name;
}

double CPortfel::GetCash()
{
	//	assert(0);
	printf("REMOVE THIS? %s\n", __FUNCTION__);
	return 0;// -1;m_cash;
}

void CPortfel::SetCash(double cash)
{
	printf("REMOVE THIS? %s\n", __FUNCTION__);
	//	m_cash = cash;
}

void CPortfel::List()
{
	for( int i = 0; i < WOLUMENS; i++ )
	{
		CSpolka *sp = m_wolumens[ i ].Spolki();
	
		while(sp!=NULL)
		{
			log_p(sp->GetName().c_str());
			sp=sp->next;
		}
	}
}

void CPortfel::Print()
{
	for( int i = 0; i < WOLUMENS; i++ )
	{
		CSpolka *sp = m_wolumens[ i ].Spolki();
	
		printf( "\n%20s\t%12s\t%12s\t%12s\n",
				"Spolka","kurs", "zmiana %","otwarcia");
		
		while(sp!=NULL)
		{
			// 		log(sp->GetName().c_str());
			sp->Print();
			sp=sp->next;
		}
	}
}

void CPortfel::SetStr(char* str)
{
	m_str = str;
}
void CPortfel::Save()
{
	for( int i = 0; i < WOLUMENS; i++ )
	{
		CSpolka *sp = m_wolumens[ i ].Spolki();
	
		while(sp!=NULL)
		{
			//log_p(sp->GetName().GetBuffer(0));
			sp->Save(m_name);
			sp=sp->next;
		}
	}
}


bool CPortfel::Update_Shares(std::string str, std::string data)
{
	bool ret = false;

	for( int i = 0; i < WOLUMENS; i++ )
	{
		CSpolka *sp = m_wolumens[ i ].Spolki();
		bool local_ret = false;
	
		while( sp!=NULL )
		{
			if( sp->GetName() == str )
			{
				local_ret = sp->Update( data );
				sp->Save( m_name );
				if( local_ret )
				{
					ret = true;
				}
			}
			sp=sp->next;
		}
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
}

//set changed flag
//to prevent writing delta report at start of app execution
//used in EndOfDay func
void CPortfel::Changed()
{
	printf("REMOVE THIS? %s\n", __FUNCTION__);
	//m_changed = true;
}

void CPortfel::EndOfDay()
{
	printf("END of day\n");

	for( int i = 0; i < WOLUMENS; i++ )
	{
		m_wolumens[ i ].Sell();
	}
}

void CPortfel::StartOfDay(double nasdaq_change )
{
	for( int i = 0; i < WOLUMENS; i++ )
	{
		m_wolumens[ i ].StartOfDay( nasdaq_change );
	}
}
