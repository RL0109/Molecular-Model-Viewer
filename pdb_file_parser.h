#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "raylib.h"


using std::ifstream, std::cout, std::cerr, std::cin, std::string, std::vector, std::stof;


class PDBFileParser {
    friend class ParsingTests;
    Vector3 sum = {0.0f, 0.0f, 0.0f};

    vector<int> moleculeCoordinates = {12,13,14};
    vector<int> pdbCoordinates = {10,11,12};

    bool usePDBCoordinates = false;

    public:
    struct Atom {
        Vector3 position;
        char elementId;

        Atom(Vector3 pos, char eId) {
            position = pos;
            elementId = eId;
        } 
    };

    struct Bond {
        Vector3 startPos;
        Vector3 endPos;
        char connectId;

        Bond(Vector3 sP, Vector3 eP, char cId) {
            startPos = sP;
            endPos = eP;
            connectId = cId;
        }
    };


    vector<vector<string>> moleculedata;
    vector<string> columnHeader;
    vector<Vector3> coordinates;
    vector<Atom> atomData;
    vector<Bond> bondData;
    float size;

    PDBFileParser(ifstream &moleculeFile) {
        parseFile(moleculeFile);
        getCoordinates();
        centerMolecule();
        getBonds();
    }

    vector<string> lineParser(const string line) {

        vector<string> row;

        string str;

        std::stringstream ss(line);

        while (ss >> str) {
            row.push_back(str);
        }

        return row;
    } 

    void parseFile (ifstream &moleculeFile) {
        string line; 
        bool insideTargetLoop = false;
        bool finishedHeader = false;
        std::string activeKeyWord = "_chem_comp_atom.comp_id";

        while (std::getline(moleculeFile, line)) {
            if (line.rfind("_atom_site.group_PDB",0) == 0) {
                activeKeyWord = "_atom_site.group_PDB";
                usePDBCoordinates = true;
            }
        }

        moleculeFile.clear();
        moleculeFile.seekg(0, std::ios::beg);

        while (std::getline(moleculeFile , line)) {
            if (line.rfind(activeKeyWord,0 ) == 0) {
                insideTargetLoop = true;
            }
            
            if (insideTargetLoop && !line.empty()) {
                if (line[0] == '_') {
                    columnHeader.push_back(line);
                }
                if (line[0] != '_') {
                    if (!finishedHeader) {
                        moleculedata.push_back(columnHeader);
                        finishedHeader = true;
                    }
                    if (line[0] == '#') {
                        break;
                    }
                    moleculedata.push_back(lineParser(line));
                }
            }
        }
    }




    // void readData (const vector<vector<string>> &dataVector) {

    //     for (auto dv : dataVector) {
    //         for (auto d : dv) {
    //             std::cout << d << " ";
    //         }
    //         cout << "\n";
    //     }

    // }

    void getCoordinates() {
    
        for (int i = 1; i < moleculedata.size(); i++ )
        {

            Vector3 coordinates = {0.0f, 0.0f, 0.0f};
            //Pull vector data from file
            if (usePDBCoordinates == true) {
                coordinates = {stof(moleculedata[i][pdbCoordinates[0]]), stof(moleculedata[i][pdbCoordinates[1]]), stof(moleculedata[i][pdbCoordinates[2]])};
            } else {
                coordinates = {stof(moleculedata[i][moleculeCoordinates[0]]), stof(moleculedata[i][moleculeCoordinates[1]]), stof(moleculedata[i][moleculeCoordinates[2]])};
            }
            
            char element;

            //Pull element character from file
            if (usePDBCoordinates == true) {
                element = moleculedata[i][2][0];
            } else {
                element = moleculedata[i][1][0];
            }
            
            //Applying coordinates and element to vector
            atomData.push_back(Atom(coordinates, element));
        }
    }

    void centerMolecule() {
        sum = {0.0f, 0.0f, 0.0f};

        if (atomData.empty()) return;

        for (auto atom: atomData) {
            sum.x += atom.position.x;
            sum.y += atom.position.y;
            sum.z += atom.position.z;
        }
        int numAtoms = (atomData.size());
        Vector3 centroid = {
            sum.x / (float)numAtoms,
            sum.y / (float)numAtoms,
            sum.z / (float)numAtoms
        };

        for (auto& atom : atomData) {
            atom.position = Vector3Subtract(atom.position, centroid);
        }

    }

    void getBonds() {

        for (int i = 0; i < atomData.size(); i++) {
            if (atomData[i].elementId != 'H') {
            for (int j = i; j < atomData.size(); j++) {
                if (Vector3Distance(atomData[i].position, atomData[j].position) < 1.90001f && Vector3Distance(atomData[i].position, atomData[j].position) > 0.4f) {
                        if (atomData[j].elementId != 'C')
                        bondData.push_back(Bond(atomData[i].position, atomData[j].position, atomData[j].elementId));

                        else {
                            bondData.push_back(Bond(atomData[i].position, atomData[j].position, atomData[i].elementId));
                        }
                }
            }
            }
            

        }
    }


};
