#include <string.h>
#include <stdlib.h>

#include "cwolumen.h"
#include "cspolka.h"
#include "main.h"

void log2(const char* str)
{
	//void
};

CSpolka::CSpolka(std::string dane)
{
	Clear();

	m_name = dane;
	next = NULL;
	prev = NULL;

	m_godzina = "";
	m_ostatnia_godzina = "";

	log2(m_name.c_str());
}


CSpolka::~CSpolka()
{
}

void CSpolka::Clear()
{
	m_amount = 0;
	m_fbiezacy = 0.0;
	m_fbiezacy_ostatnio = 0.0;
	m_fwolumen = 0.0;
	m_fwartosc = 0.0;
	m_fMAXzmiana = -999.0;
	m_fMAXzmiana_od_rana = -999.0;
}

//	IN camount of cash to buy stock
//	OUT cash spent on bought stock
//  OUT remainder will be updated 
double  CSpolka::Buy(double cash, double &remain)
{
	int amount = 0;
	double value = 0.0;
	
	if(m_fbiezacy)
		amount = (int)(cash / m_fbiezacy);

	remain = cash - (amount * m_fbiezacy);
	
	m_amount += amount;
	value = m_amount * m_fbiezacy;

	log( "bought %d shares of %s at %.2f PLN, total amount: %d shares, o wartosci: %.2f PLN\n",
			 amount,
			 m_name.c_str(),
			 m_fbiezacy,
			 m_amount,
			 value );

	return amount * m_fbiezacy;
}

//amount = 0 - sell all
double CSpolka::Sell(int amount)
{
	if (amount == 0)
		amount = m_amount;

	m_amount -= amount;

	log( "sold %d shares of %s at %.2f PLN, total amount: %d shares, wartosc sprzedazy: %.2f PLN\n",
			 amount,
			 m_name.c_str(),
			 m_fbiezacy,
			 m_amount,
			 m_fbiezacy * amount );

	return m_fbiezacy * amount;
}

void CSpolka::Print( char * filename )
{
	printf( "%20s\t%12.2f\t%12.2f\t%12.2f\n", 
			(char*)m_name.c_str(),
			m_fbiezacy,
			m_fzmiana,
			m_fotwarcia );
 	logf( filename,
 		  (char*)"%20s\t%12.2f\t%12.2f\t%12.2f\n", 
 		  (char*)m_name.c_str(),
 		  m_fbiezacy,
 		  m_fzmiana,
 		  m_fotwarcia );
}

// data format:
//  0       1        2        3       4     5   6     7      8       12    13
// nazwa biezacy poprzedni zmiana otwarcia min max wolumen wartosc data godzina
//
bool CSpolka::Update(std::string data)
{
	char buf[BUFSIZE*2];
	char Data[2*BUFSIZE];
	bool ret = false;

	strncpy( Data, data.c_str(), 2*BUFSIZE);
// 	char* p = (char*)data.c_str();	
	char* p = Data;

	replace(p,',','.');
	replace(p,'%',' ');
	replace(p,'+',' ');
	remove(p,-62);
	remove(p,-96);
	cut(p,'(',')');

	// set current 
	m_fbiezacy = 0.0;

	DPF( "%s", p);
	for( int i=0; i < 15; i++ )
	{
		memset(buf,0,BUFSIZE*2);
		sscanf(p,"%20s",buf);

		DPF( "%d - %s\n", i,(char*)(buf));
		
		switch( i )
		{
		case 1: // kurs
			m_biezacy = buf;
			m_fbiezacy = atof(buf);
			if( m_fbiezacy != m_fbiezacy_ostatnio )
			{
				ret = true;
			}
			m_fbiezacy_ostatnio = m_fbiezacy;
			break;
		case 3: // zmiana
			m_zmiana = buf;
			if(buf[0]=='+')
				buf[0]='0';
			//buf[strlen(buf)-1]=0;	//delete ending % char
			m_fzmiana = atof(buf);
			if(m_fzmiana > m_fMAXzmiana)
				m_fMAXzmiana=m_fzmiana;
			break;
		case 2: //odn
			m_poprzedni = buf;
			m_fpoprzedni = atof(buf);
			break;
		case 5: //min
			m_kurs_min = buf;
			m_fkurs_min = atof(buf);
			break;
		case 6: //max
			m_kurs_max = buf;
			m_fkurs_max = atof(buf);
			break;
		case 4: //otw
			m_otwarcia = buf;
			m_fotwarcia = atof(buf);
			break;
		case 7: //wol
			m_wolumen = buf;
			m_fwolumen = atof(buf);
			break;
		case 8: //wart
			m_wartosc = buf;
			m_fwartosc = atof(buf);
			break;
		case 11: //data 
			m_data = buf;
			break;
		case 13: //godzina
			m_godzina = buf;
			if( m_ostatnia_godzina != m_godzina &&  // if something has changed
				m_ostatnia_godzina != "" )          // and it is not start of aplication
			{
				ret = true;                         // return true - wake up!
			}
			m_ostatnia_godzina = m_godzina;
			break;
		}
		p += strlen(buf)+1;
	}

	//procentowa zmiana
	m_fzmiana_od_rana = ((m_fbiezacy - m_fotwarcia) / m_fotwarcia)*100;
	if(m_fzmiana_od_rana > m_fMAXzmiana_od_rana)
	{
		m_fMAXzmiana_od_rana = m_fzmiana_od_rana ;
	}

	return ret;
}

std::string CSpolka::GetName()
{
	return m_name;
}

void CSpolka::Save(std::string portfel,bool end)
{
	FILE *plik;
	char filename[80] = "";
	char buf[BUFSIZE] = "";

	//	printf( "\tsave %s - %s\n", portfel.c_str(), m_name.c_str() );

	strcpy( buf,"mkdir -p ./portfels/");
	system( buf );
	strncat( buf, portfel.c_str(), BUFSIZE-10 );
	system( buf );

	strcpy( filename,"./portfels/");
	strncat( filename, portfel.c_str(), 22 );
	strncat( filename, "/", 3 );

	strncat(filename,this->m_name.c_str(),20 );
	strncat(filename,".txt",4 );

	plik = fopen( filename,"a" );
	if( plik )
	{
		if( !end )
		{
			fprintf( plik,"%s %s\t%s\t%s\t%s\n",
					 m_data.c_str(),
					 m_godzina.c_str(),
					 m_biezacy.c_str(),
					 m_otwarcia.c_str(),
					 m_zmiana.c_str() );
		}
		else
		{
			fprintf( plik, "End Of Day %s, max zmiana: %f\n", m_data.c_str(), m_fMAXzmiana );
			fprintf( plik, "%s, max zmiana od rana: %f\n\n",  m_data.c_str(), m_fMAXzmiana_od_rana );
		}

		fclose(plik);
	}
	else
	{
		printf( "Cannot open file: %s\n", filename);
	}
	return;
}

static double	m_fsrednia = 0.0;
static double	m_fMAXsrednia = 0.0;
static int		m_licznik_sredniej = 0;


void CSpolka::Report(std::string portfel, double cash)
{
	char *buf;
	FILE *plik;
	char filename[80];
	buf=(char*)portfel.c_str();
	strcpy(filename,"./portfels/");
	strncat(filename,"delta_",13);
	strncat(filename,buf,20);
	strncat(filename,".txt",4);
	plik = fopen(filename,"a");
	
	m_fsrednia += m_fzmiana_od_rana;
	m_fMAXsrednia += m_fMAXzmiana_od_rana;
	m_licznik_sredniej++;
	if(plik)
	{
		fprintf(plik,"%16s - %s, delta %10f\n",
				this->m_name.c_str(),
				m_data.c_str(),
				m_fMAXzmiana_od_rana);
		if(cash!=0.0)
		{
			fprintf(plik,"%16s: %10f\n","SREDNIO (MAX)", m_fMAXsrednia/m_licznik_sredniej );
			fprintf(plik,"%16s: %10f\n","SREDNIO", m_fsrednia/m_licznik_sredniej );
			fprintf(plik,"cash: %10f\n\n",cash );
			m_licznik_sredniej = 0;
			m_fMAXsrednia = 0.0;
			m_fsrednia = 0.0;
		}
		fclose(plik);
	}
	return;
}
