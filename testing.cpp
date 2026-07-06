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
            PDBFileParser parsedfile(molecularFile);

            //Unit test can only be tested in long long, this centers the coordinates and multiplies by
            //1000 to avoid truncation.
            float expected_x = ((16.293 - parsedfile.sum.x));
            float expected_y = ((15.907 - parsedfile.sum.y));
            float expected_z = ((52.123 - parsedfile.sum.z));


            // Epsilon Testing
            float epsilon = 0.0011f;

            CHECKT((parsedfile.atomData[0].position.x - expected_x) < epsilon);
            
            CHECKT((parsedfile.atomData[0].position.y - expected_y) < epsilon);
            
            CHECKT((parsedfile.atomData[0].position.z - expected_z) < epsilon);
            status();
        }
};

int main() {
    ParsingTests testSuite;
    testSuite.single_test();
    if (testSuite.fails > 0) {
        std::cout << " " << testSuite.fails << " failed test(s)" << "\n";
        return 0;
    }
    std::cout << "All tests passed successfully!" << "\n";
    return 0;

}