#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include "pdb_file_parser.h"
#include "molecule_file_parser.h"
#include <fstream>
#include <unordered_map>
#include "rlgl.h"

std::unordered_map<char, Color> atomsToColors = {
    {'O', RED}, {'H', WHITE}, {'N', BLUE}, {'C' , GRAY}
};



int main () {
    
    //Open text file
    ifstream molecularFile("LEU.cif");
    
    PDBFileParser parsedFile(molecularFile);
    
    InitWindow(800, 450, "Molecular Model Viewer");
    SetTargetFPS(60);

    std::cout << "Total Atoms in cif file: " << parsedFile.atomData.size();

    Vector3 startPos = {0,0,10};
    Vector3 upPos = {0, 1, 0};
    Camera3D camera = {startPos, {0,0,0}, upPos, 90, CAMERA_PERSPECTIVE};

    float atomicSizeToSize = 0.15f;
    float modelScale = 1.0f;

    float rotationX = 0.0f;
    float rotationY = 0.0f;

    float translateX = 0.0f;
    float translateY = 0.0f;

    bool rotate = true;
    bool translate = false;

    while (!WindowShouldClose()) {
        int key = GetKeyPressed();

        if (key == 84) {
            rotate = false;
            translate = true;
        }

        if (key == 82) {
            rotate = true;
            translate = false;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (rotate) {
                Vector2 mouseDelta = GetMouseDelta();
                rotationX += mouseDelta.y * 0.015f;
                rotationY += mouseDelta.x * 0.015f;
            }
            if (translate) {
                Vector2 mouseDelta = GetMouseDelta();
                translateX += mouseDelta.x * 0.015f;
                translateY += -mouseDelta.y * 0.015f;

            }
        }


        float scroll = GetMouseWheelMove();
        if(scroll != 0.0f) {
            modelScale += scroll * 0.05f;

            if (modelScale < 0.1f) 
            modelScale = 0.1f;
        }

        Vector3 rotationVector = {rotationX, rotationY, 0.0f};
        Matrix rotationMat = MatrixRotateXYZ(rotationVector);
        Matrix scaleMatrix = MatrixScale(modelScale, modelScale, modelScale);
        Matrix translateMatrix = MatrixTranslate(translateX, translateY, 0.0f);

        Matrix transformMatrix = MatrixMultiply(scaleMatrix, rotationMat);
        transformMatrix = MatrixMultiply(transformMatrix, translateMatrix);



        BeginDrawing();
            //Indentation for clarity
            ClearBackground(BLACK);
            BeginMode3D(camera);

            rlPushMatrix();
            rlMultMatrixf(MatrixToFloat(transformMatrix));


            
            for (int i = 0; i < parsedFile.atomData.size(); i++) {
                DrawSphere(parsedFile.atomData[i].position, 0.50f, atomsToColors[(parsedFile.atomData[i].elementId)]);
                          
            }
            for (int i = 0; i < parsedFile.bondData.size(); i++) {
                DrawCylinderEx(parsedFile.bondData[i].startPos, 
                    parsedFile.bondData[i].endPos,
                    .05f * modelScale, .05f * modelScale, 6, atomsToColors[parsedFile.bondData[i].connectId]);
            }

            rlPopMatrix();

            EndMode3D();
            DrawText("A molecular model of leucine!", 20, 20, 20, LIGHTGRAY);
        EndDrawing();

    }







    return 0;
}