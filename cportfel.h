#if !defined(AFX_CPORTFEL_H__0FD29485_E54D_4351_BB56_C7C567A8DC74__INCLUDED_)
#define AFX_CPORTFEL_H__0FD29485_E54D_4351_BB56_C7C567A8DC74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "cspolka.h"
#include "cwolumen.h"

//class CWolumen;

class CPortfel
{
	std::string config_file;
	std::string m_name;
	CWolumen m_wolumens[ WOLUMENS ];
	int m_count;
	int m_companies;
	std::string m_str;
public:
	CPortfel(std::string name);
	~CPortfel();

	std::string GetName();
	void Changed();
	void Delete();
	void List();
	void Print();
	void Save();
	void SetStr( char* str);
	double GetCash();
	CSpolka* GetSpolki();
	void SetCash(double cash);
	void EndOfDay();
	void StartOfDay(double nasdaq);
	bool Update_Shares(std::string str, std::string data);
	bool Update(std::string data);
	CPortfel* next;
	CPortfel* prev;
};

#endif
