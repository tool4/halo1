#if !defined(AFX_CSPOLKA_H__0FD29485_E54D_4351_BB56_C7C567A8DC74__INCLUDED_)
#define AFX_CSPOLKA_H__0FD29485_E54D_4351_BB56_C7C567A8DC74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class CSpolka
{
	std::string m_name;
	std::string m_biezacy;
	std::string m_poprzedni;
	std::string m_zmiana;
	std::string m_otwarcia;
	std::string m_kurs_min;
	std::string m_kurs_max;
	std::string m_wolumen;
	std::string m_wartosc;
	std::string m_data;
	std::string m_godzina;

	std::string m_ostatnia_godzina; // last change
	
  public:
	CSpolka(std::string dane);
	~CSpolka();

	std::string GetName();

	double Buy(double cash, double &remain);
	double Sell(int amount = 0);
	bool Update(std::string dane);
	void Print(char* filename = ((char*)"files/makler_log.txt") );
	void Clear();
	void Add(std::string dane);
	void Save(std::string portfel, bool end=0);
	void Report(std::string portfel, double cash=0.0);
	void Delete(std::string dane);
	CSpolka* next;
	CSpolka* prev;

	double m_fbiezacy;
	double m_fbiezacy_ostatnio;
	double m_fpoprzedni;
	double m_fzmiana;
	double m_fMAXzmiana;
	double m_fzmiana_od_rana;
	double m_fMAXzmiana_od_rana;
	double m_fotwarcia;
	double m_fkurs_min;
	double m_fkurs_max;
	double m_fwolumen;
	double m_fwartosc;

	int m_amount;
};

#endif
