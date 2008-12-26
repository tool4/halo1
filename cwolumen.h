#pragma once

#include <string>
#include "main.h"

class CWolumen
{
	std::string m_name;

	double m_buy_price;
    long   m_amount;

  public:
	CWolumen();
	~CWolumen();

	CWolumen* prev;
	CWolumen* next;

	double Buy(double cash, double &remain);
	double Sell(int amount = 0);
	void   Print();
	void   SetName( char * name );
	char*  GetName( void );

/* 	void Save(std::string portfel, bool end=0); */
/* 	void Report(std::string portfel, double cash=0.0); */
};
