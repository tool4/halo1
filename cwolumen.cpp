#include <stdio.h>
#include "cwolumen.h"


CWolumen::CWolumen()
{
	m_buy_price = 0.0;
	m_amount = 0;
}

CWolumen::~CWolumen()
{
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
	return 0;
}

void CWolumen::Print()
{
	printf( "current amount: %ld, value: %.2f PLN\n",
			m_amount, 
			m_buy_price * m_amount );
}

