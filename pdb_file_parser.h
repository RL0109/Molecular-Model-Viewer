#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "raylib.h"


using std::ifstream, std::cout, std::cerr, std::cin, std::string, std::vector, std::stof;


class PDBFileParser {
    
    public:
    struct Atom {
        Vector3 position;
        char elementId;
        float size;
        Color color;

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

        while (std::getline(moleculeFile , line)) {
            if (line.rfind("_chem_comp_atom.comp_id",0 ) == 0) {
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
            //Pull vector data from file
            Vector3 coordinates = {stof(moleculedata[i][12]), stof(moleculedata[i][13]), stof(moleculedata[i][14])};
            //Pull element character from file
            char element = moleculedata[i][1][0];
            //Applying coordinates and element to vector
            atomData.push_back(Atom(coordinates, element));
        }
    }

    void centerMolecule() {
        if (atomData.empty()) return;

        Vector3 sum = {0.0f, 0.0f, 0.0f};

        for (auto atom: atomData) {
            sum.x = atom.position.x;
            sum.y = atom.position.y;
            sum.z = atom.position.z;
        }

        Vector3 centroid = {
            sum.x / 1.0f,
            sum.y / 1.0f,
            sum.z / 1.0f
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
