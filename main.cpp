#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include "pdb_file_parser.h"
#include <fstream>
#include <unordered_map>
#include "rlgl.h"
#include "shader_commands.h"

std::unordered_map<char, Color> atomsToColors = {
    {'O', RED}, {'H', WHITE}, {'N', BLUE}, {'C' , GRAY}, {'S', YELLOW}
};



int main () {
    
    //Open text file
    ifstream molecularFile("9PZB.cif");
    
    PDBFileParser parsedFile(molecularFile);
    
    InitWindow(800, 450, "Molecular Model Viewer");
    SetTargetFPS(60);

    std::cout << "Total Atoms in cif file: " << parsedFile.atomData.size();

    Vector3 startPos = {0,0,150};
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

    std::cout << "\n" << parsedFile.atomData[0].position.x << " , " << parsedFile.atomData[0].position.y << " , " << parsedFile.atomData[0].position.z << "\n";

    // Generate basic sphere mesh a material
    Mesh atomMesh = GenMeshSphere(0.5f, 16,16);
    Material atomMaterial = LoadMaterialDefault(); 

    Shader instancingShader = LoadShaderFromMemory(instancingVertexShader, instancingFragmentShader);

    atomMaterial.shader = instancingShader;

    instancingShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(instancingShader, "instanceTransform");


    //Creating unique vectors of matrices for each element type to avoid shader computing for now.
    vector<Matrix> oxygenMatrices;
    vector<Matrix> hydrogenMatrices;
    vector<Matrix> carbonMatrices;
    vector<Matrix> sulfurMatrices;
    vector<Matrix> nitrogenMatrices;

    for (int i = 0; i < parsedFile.atomData.size(); i++) {
        Matrix mat = MatrixTranslate(parsedFile.atomData[i].position.x, parsedFile.atomData[i].position.y, parsedFile.atomData[i].position.z);

        if (parsedFile.atomData[i].elementId == 'O') {
            oxygenMatrices.push_back(mat);
        }
        else if (parsedFile.atomData[i].elementId == 'N') {
            nitrogenMatrices.push_back(mat);
        }
        else if (parsedFile.atomData[i].elementId == 'S') {
            sulfurMatrices.push_back(mat);
        }
        else if (parsedFile.atomData[i].elementId == 'C') {
            carbonMatrices.push_back(mat);
        }
        else if (parsedFile.atomData[i].elementId == 'H') {
            hydrogenMatrices.push_back(mat);
        }
    
    }



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
            
            atomMaterial.maps[MATERIAL_MAP_DIFFUSE].color= RED;
            DrawMeshInstanced(atomMesh, atomMaterial, oxygenMatrices.data(), oxygenMatrices.size());

            atomMaterial.maps[MATERIAL_MAP_DIFFUSE].color= YELLOW;
            DrawMeshInstanced(atomMesh, atomMaterial, sulfurMatrices.data(), sulfurMatrices.size());

            atomMaterial.maps[MATERIAL_MAP_DIFFUSE].color= BLUE;
            DrawMeshInstanced(atomMesh, atomMaterial, nitrogenMatrices.data(), nitrogenMatrices.size());

            atomMaterial.maps[MATERIAL_MAP_DIFFUSE].color= GRAY;
            DrawMeshInstanced(atomMesh, atomMaterial, carbonMatrices.data(), carbonMatrices.size());

            atomMaterial.maps[MATERIAL_MAP_DIFFUSE].color= WHITE;
            DrawMeshInstanced(atomMesh, atomMaterial, hydrogenMatrices.data(), hydrogenMatrices.size());
            
            for (int i = 0; i < parsedFile.bondData.size(); i++) {
                DrawCylinderEx(parsedFile.bondData[i].startPos, 
                    parsedFile.bondData[i].endPos,
                    .05f * modelScale, .05f * modelScale, 6, atomsToColors[parsedFile.bondData[i].connectId]);
            }

            rlPopMatrix();

            EndMode3D();
        EndDrawing();

    }







    return 0;
}