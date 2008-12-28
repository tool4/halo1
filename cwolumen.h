#pragma once

#include <string>
#include "main.h"
#include "cspolka.h"

class CWolumen
{
	std::string m_name;

	double m_buy_price;
    long   m_amount;
	CSpolka *m_spolki;
	int m_companies;
	bool m_changed;
	double m_cash;

  public:
	CWolumen();
	~CWolumen();

	CWolumen* prev;
	CWolumen* next;

	CSpolka *Spolki();
	double  Buy( double cash, double &remain );
	double  Sell( int amount = 0 );
	void    Print();
	void    SetName( char * name );
	char*   GetName( void );
	int     NumCompanies();
	void    InitSpolki( char* filename );
	void    Delete();
	void    SetCashToFile( double cash );
	double  GetCashFromFile();
	void    StartOfDay( double nasdaq_change );
	void    EndOfDay();
};
