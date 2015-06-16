#pragma once
#include <string>

using namespace std;

class LastFM
{
private:
	string adres_;
	string adres_zrodla_;
	string zrodlo_strony_cale;
public:
	LastFM(string adres, string adres_zrodla);
	~LastFM();
	void odczytaj_zrodlo_strony();
	void zapisz_zrodlo_do_pliku();
};

