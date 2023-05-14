#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <wchar.h>
#include <boost/filesystem.hpp>
#include <vector>

namespace fs = boost::filesystem;

typedef struct{
	std::string m_str;
	unsigned int m_wert;
}lstPraefix;

int MAX_LST_GROESSE = 50;
int MAX_BENUTZT = 0;

void PraefixFinden(std::string, lstPraefix*);
unsigned int LaufendeNummerErmitteln(std::string, unsigned int&);
void PraefixListeHinzufuegen(std::string, unsigned int, lstPraefix*);
int ObsoletenListeEinlesen(std::vector<std::string>& strListeObsolet);
void EntferneObsoletes(std::string& strFileName, std::vector<std::string>& vObsoletenContainer);

int main(int argc, char** argv)
{
	if(argc <2)
	{
		std::cout<<"\nVerwendung: InkDatNmb.exe Datei_1 Datei_2 ...\n\n";
		system("PAUSE");
		return 1;
	}

	std::vector<std::string> vObsoletenContainer;
	ObsoletenListeEinlesen(vObsoletenContainer);

	lstPraefix lstPrFx[MAX_LST_GROESSE];
	unsigned int uiMaxLstEintrag = 0;
	for(int datNr = 1; datNr < argc; datNr++)
	{
		fs::path pfad(argv[datNr]);
		if(fs::exists(pfad))
		{
			if(fs::is_regular_file(pfad))
			{
				//std::cout<<"Bearbeite Datei: "<<pfad<<"\n";

				std::vector<fs::path> v;
				for(int i = 0; i < MAX_LST_GROESSE; i++)
					lstPrFx[i].m_str = "";

				fs::path subPfad = pfad.parent_path();

				for(auto&& x : fs::directory_iterator(subPfad))
					v.push_back(x.path());
				for(auto&& y : v)
				{
					if(is_regular_file(y))
					{
						PraefixFinden(y.filename().generic_string(), lstPrFx);
					}else
					{
						//std::cout<<y<<" ist keine Datei\n";
					}
				}
			}
		}
		else
		{
			std::cout<<"Datei existiert nicht";
		}
	}

	std::string stEingabe;
	unsigned int uiGewPrFx = -1;

	if(MAX_BENUTZT)
	{
		std::cout<<"Welches Praefix soll verwendet werden:\n\n";
		std::cout<<"[0]: "<<"Neues Praefix\n\n";
		for(int i = 0; i<MAX_LST_GROESSE; i++)
		{
			if(!lstPrFx[i].m_str.empty())
			{
				std::cout<<"["<<i+1<<"]: "<<lstPrFx[i].m_str<<"\n\n";
				uiMaxLstEintrag = i;
			}else
				break;
		}
		std::cout<<"\n[Nummer]: ";
		do
		{
			std::getline(std::cin, stEingabe);
			if(stEingabe[0] == '0')
			{

				std::cout<<"\n\nNeues Praefix eingeben: ";
				std::getline(std::cin, stEingabe);
				lstPrFx[0].m_str = stEingabe;
				lstPrFx[0].m_wert = 0;
				uiGewPrFx = 0;
				break;
			}
			if((stEingabe[0] <'0')||(stEingabe[0]>'9'))
			{
				std::cout<<"Vorgang abgebrochen\n";
				system("pause");
				return 0;;
			}
			uiGewPrFx = stoi(stEingabe)-1;
			if(uiMaxLstEintrag < uiGewPrFx)
				std::cout<<"[Fehler] Bitte Zahl kleiner "<<uiMaxLstEintrag+1<<" eingeben: ";
		}while(uiMaxLstEintrag < uiGewPrFx);
	}else
	{
		std::cout<<"\nKein Praefix vorhanden\nBitte Praefix eingeben: ";
		std::getline(std::cin, stEingabe);
		lstPrFx[0].m_str = stEingabe;
		lstPrFx[0].m_wert = 0;
		uiGewPrFx = 0;
	}

	//std::cout<<"\nSie haben "<<lstPrFx[uiGewPrFx].m_str<<" als Praefix gewaehlt\n";

	for(int datNr = 1; datNr < argc; datNr++)
	{
		fs::path pfad(argv[datNr]);
		if(fs::exists(pfad))
		{
			if(fs::is_regular_file(pfad))
			{
				std::string strExt(lstPrFx[uiGewPrFx].m_str);

				std::string strNr("");
				if((lstPrFx[uiGewPrFx].m_wert + 1)<10)
				{
					strNr = "0";
				}
				strNr.append(std::to_string(lstPrFx[uiGewPrFx].m_wert + 1));

				std::string strDir = pfad.parent_path().generic_string();
				std::string strFileName = pfad.filename().generic_string();

				EntferneObsoletes(strFileName, vObsoletenContainer);

				std::string neuerPfad = strDir + "/" + strExt + strNr + " - " + strFileName;

				fs::rename(pfad, neuerPfad);
			}
		}
	}

	//system("pause");
	return 0;
}

void PraefixFinden(std::string dateiname, lstPraefix* lstPrFx)
{
	std::string strAn("An-");
	std::string strVon("Von-");
	unsigned int uiNrStart;

	if((dateiname).find(strAn, 0) == 0)
	{
		unsigned int uiWert = LaufendeNummerErmitteln(dateiname, uiNrStart);
		PraefixListeHinzufuegen(std::string(dateiname, 0, uiNrStart), uiWert, lstPrFx);
	}
	if((dateiname).find(strVon, 0) == 0) /*!= std::string::npos fuer das Auffinden an beliebiger Stelle*/
	{
		unsigned int uiWert = LaufendeNummerErmitteln(dateiname, uiNrStart);
		PraefixListeHinzufuegen(std::string(dateiname, 0, uiNrStart), uiWert, lstPrFx);
	}
	return;
}

unsigned int LaufendeNummerErmitteln(std::string dateiname, unsigned int& uiNrStart)
{
	bool bZahlGefunden = false;
	unsigned int uiWert = 0;
	uiNrStart = 0;
	for(size_t i = 0; i < dateiname.length(); i++)
	{
		if((dateiname[i] >= '0')&&(dateiname[i] <= '9'))
		{
			if(!bZahlGefunden)uiNrStart = (i);
			bZahlGefunden = true;
			uiWert *= 10;
			uiWert += (dateiname[i] - '0');
		}else
		{
			if(bZahlGefunden)
				return uiWert;
		}
	}
	return uiWert;
}

void PraefixListeHinzufuegen(std::string str, unsigned int uiLaufNummer, lstPraefix* lstPrFx)
{
	for(int i = 0; i<MAX_LST_GROESSE; i++)
	{
		if(lstPrFx[i].m_str.empty())
		{
			lstPrFx[i].m_str = str;
			lstPrFx[i].m_wert = uiLaufNummer;
			MAX_BENUTZT++;
			return;
		}
		if(lstPrFx[i].m_str == str)
		{
			if(lstPrFx[i].m_wert < uiLaufNummer)
			{
				lstPrFx[i].m_wert = uiLaufNummer;
			}
			return;
		}
	}
	return;
}


int ObsoletenListeEinlesen(std::vector<std::string>& strListeObsolet)
{
	int dGroesseListe = 0;

	std::fstream fObsoletenDatei;
	fObsoletenDatei.open("Obsoletenliste.txt", std::ifstream::in);

	if(fObsoletenDatei.good())
	{
		char c[256];
		std::string str;
		while(!fObsoletenDatei.eof())
		{
			fObsoletenDatei.getline(c, 256);
			str = c;
			strListeObsolet.push_back(str);
			dGroesseListe++;
		}
	}
	else
	{
		std::cout<<"Die Datei 'Obsoletenliste.txt' konnte nicht geöffnet werden!\n\n";
		system("PAUSE");
		return -1;
	}

	return dGroesseListe;
}

void EntferneObsoletes(std::string& strFileName, std::vector<std::string>& vObsoletenContainer)
{
	std::string strNewFileName;
	size_t dStelle;
	for(auto&& strObsoletes : vObsoletenContainer)
	{
		if(strObsoletes.empty())continue;
		do
		{
			dStelle = strFileName.find(strObsoletes, 0);

			if(dStelle != std::string::npos)
			{
				strFileName.erase(dStelle, strObsoletes.size());
			}
		}while (dStelle != std::string::npos);
	}
	do
	{
		dStelle = strFileName.find("  ", 0);

		if(dStelle != std::string::npos)
		{
			strFileName.erase(dStelle, (size_t)1);
		}
	}while (dStelle != std::string::npos);
	return;
}
