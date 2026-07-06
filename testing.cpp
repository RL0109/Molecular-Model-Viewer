#include "cppunit.h"
#include "raylib.h"
#include "raymath.h"
#include <fstream>
#include <sstream>
#include "pdb_file_parser.h"
#include <iostream>
#include <string>
#include <vector>


class ParsingTests : public Cppunit {
    public: 
        void single_test() override {
            int failedAmount = 0;
            ifstream molecularFile("LEU.cif");
            PDBFileParser parsedFile(molecularFile);

            Vector3 testingSum = {0.0f, 0.0f, 0.0f};


            float expected_x = ((16.293 - parsedFile.sum.x/parsedFile.atomData.size()));
            float expected_y = ((15.907 - parsedFile.sum.y/parsedFile.atomData.size()));
            float expected_z = ((52.123 - parsedFile.sum.z/parsedFile.atomData.size()));


            // Epsilon Testing
            float epsilon = 0.0011f;

            CHECKT(std::abs(parsedFile.atomData[0].position.x - expected_x) < epsilon);
            if (fails > failedAmount) {
                failedAmount++;
                std::cout << " Failed value: " << (parsedFile.atomData[0].position.x - expected_x) << "\n";
            }
            CHECKT(std::abs(parsedFile.atomData[0].position.y - expected_y) < epsilon);
            if (fails > failedAmount) {
                failedAmount++;
                std::cout << " Failed value: " << (parsedFile.atomData[0].position.y - expected_y) << "\n";
            }
            CHECKT(std::abs(parsedFile.atomData[0].position.z - expected_z) < epsilon);
            if (fails > failedAmount) {
                failedAmount++;
                std::cout << " Failed value: " << (parsedFile.atomData[0].position.z - expected_z) << "\n";
            }

            size_t expected_atom_count = 22;
            CHECKT(parsedFile.atomData.size() == expected_atom_count);
            
            CHECKT(parsedFile.atomData[0].elementId == 'N');

            for (auto atom : parsedFile.atomData) {
                testingSum.x += atom.position.x;
                testingSum.y += atom.position.y;
                testingSum.z += atom.position.z;
            }

            CHECKT(testingSum.x - parsedFile.sum.x < epsilon);
            CHECKT(testingSum.y - parsedFile.sum.y < epsilon);
            CHECKT(testingSum.z - parsedFile.sum.z < epsilon);

            status();
        }
};

int main() {
    ParsingTests testSuite;
    testSuite.single_test();
    testSuite.status();
    return 0;

}