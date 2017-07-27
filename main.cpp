#include <iostream>
#include "csv.h"
#include <vector>
#include <fstream>
#include <list>
#include <iterator>
#include <algorithm>
#include <windows.h>
#include <Commdlg.h>
#include <shlobj.h>
#include <shlwapi.h>
using namespace std;

class Incident
{

public:
    Incident(std::string, std::string);
    std::string type,date;
};

Incident::Incident (std::string incidentType, std::string incidentDate)
{
    type = incidentType;
    date = incidentDate;
}

std::string getInputFile();
std::string getSaveFile();
std::string saveToDesktop();
void logic();


std::list<Incident*> incidents;
std::vector<std::string> uniqueTypes;
std::vector<std::string> uniqueDates;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    logic();
    return 0;

}



std::string getInputFile()
{
    OPENFILENAME ofn;
    char szFile[260];
    HWND hwnd = NULL;
    HANDLE hf;


    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;



    if (GetOpenFileName(&ofn)==TRUE)
        hf = CreateFile(ofn.lpstrFile, GENERIC_READ,
                        0, (LPSECURITY_ATTRIBUTES) NULL,
                        NULL, FILE_ATTRIBUTE_NORMAL,
                        (HANDLE) NULL);

    return ofn.lpstrFile;
}



std::string getSaveFile()
{
    OPENFILENAME ofn;
    char szFile[260];
    HWND hwnd = NULL;
    HANDLE hf;


    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrDefExt = "csv";
    ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;



    if (GetSaveFileName(&ofn)==TRUE)
        hf = CreateFile(ofn.lpstrFile, GENERIC_READ,
                        0, (LPSECURITY_ATTRIBUTES) NULL,
                        NULL, FILE_ATTRIBUTE_NORMAL,
                        (HANDLE) NULL);
    return ofn.lpstrFile;
}

std::string saveToDesktop()
{
    TCHAR   achDevice[MAX_PATH];
    HRESULT  hr;
// include file ShlObj.h contains list of CSIDL defines however only a subset
// are supported with Windows 7 and later.
// for the 3rd argument, hToken, can be a specified Access Token or SSID for
// a user other than the current user. Using NULL gives us the current user.
    if (SUCCEEDED(hr = SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, achDevice)))
    {
        // append a folder name to the user's Documents directory.
        // the Path Handling functions are pretty handy.
        PathAppend(achDevice, "ReportCreator.csv");
    }

    return achDevice;
}


void logic()
{

    MessageBox(NULL, "Select the CSV file to convert to report format.", "ReportCreator", MB_OK);
    ofstream myfile;

    io::CSVReader<2> in(getInputFile());
    in.read_header(io::ignore_extra_column, "Subtype 2", "Submitted On");
    std::string type;
    std::string date;


     MessageBox(NULL, "Select where to save the output file", "ReportCreator", MB_OK);
     myfile.open(getSaveFile());
    while(in.read_row(type,date))
    {
        std::string nullCheck = "Not Assigned";
        if (type == "")
        {
            type = nullCheck;
        }

        Incident *incident = new Incident(type,date);
        incidents.push_back(incident);

    }


    list<Incident*>::iterator it;
    for(it=incidents.begin(); it!=incidents.end(); ++it)
    {

        if (std::find(uniqueTypes.begin(), uniqueTypes.end(), (*it)->type) == uniqueTypes.end())
        {

            uniqueTypes.push_back((*it)->type);
        }

        if (std::find(uniqueDates.begin(), uniqueDates.end(), (*it)->date) == uniqueDates.end())
        {

            uniqueDates.push_back((*it)->date);
        }

    }

    myfile << "Date" << ",";

    for (int i = 0; i < uniqueTypes.size(); i++)
    {
        if (i != uniqueTypes.size())
        {
            myfile << uniqueTypes[i];
        }

        if (i != uniqueTypes.size() - 1)
        {
            myfile << ",";
        }

        if (i == uniqueTypes.size() - 1)
        {
            myfile << endl;
        }

    }

    int defaultValue = 0;
    int counter = 0;
    int dateCheck = 0;
    std::string defaultDate = "";

    while (defaultValue < uniqueDates.size())
    {
        myfile << uniqueDates[dateCheck] << ",";

        for (int i = 0; i < uniqueTypes.size(); i++)
        {

            for(it=incidents.begin(); it!=incidents.end(); ++it)
            {



                if ((*it)->type == uniqueTypes[i] && (*it)->date == uniqueDates[dateCheck])
                {
                    counter++;
                }

            }
            myfile << counter;

            if (i!= uniqueTypes.size() - 1)
            {
                myfile << ",";
            }

            counter = 0;


        }


        dateCheck++;
        myfile << endl;
        if (defaultValue < uniqueTypes.size())
        {
            defaultValue++;
        }




    }


    MessageBox(NULL, "Converted and saved file ", "ReportCreator", MB_OK);
    myfile.close();

}
