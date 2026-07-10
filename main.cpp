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

void renderBonds(const PDBFileParser& moleculedata, const vector<Matrix>& oxygen, const vector<Matrix>& nitrogen,
 const vector<Matrix>& sulfur, const vector<Matrix>& hydrogen, const vector<Matrix>& carbon, Material& bondMaterial, const Mesh& bondMesh, const float& modelScale);

void renderAtoms(const PDBFileParser& moleculedata, const vector<Matrix>& oxygen, const vector<Matrix>& nitrogen,
 const vector<Matrix>& sulfur, const vector<Matrix>& hydrogen, const vector<Matrix>& carbon, Material& atomMaterial, const Mesh& atomMesh, const float& modelScale);


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

    bool showAtoms = true;
    bool showBonds = true;

    std::cout << "\n" << parsedFile.atomData[0].position.x << " , " << parsedFile.atomData[0].position.y << " , " << parsedFile.atomData[0].position.z << "\n";

    // Generate basic sphere mesh material
    Mesh atomMesh = GenMeshSphere(0.5f, 16,16);
    Material atomMaterial = LoadMaterialDefault(); 

    //Generate basic cylinder mesh material
    Mesh bondMesh = GenMeshCylinder(1.0f, 1.0f, 6);
    Material bondMaterial = LoadMaterialDefault();

    Shader instancingShader = LoadShaderFromMemory(instancingVertexShader, instancingFragmentShader);

    atomMaterial.shader = instancingShader;

    instancingShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(instancingShader, "instanceTransform");

    Shader bondInstancingShader = LoadShaderFromMemory(instancingVertexShader, instancingFragmentShader);

    bondMaterial.shader = bondInstancingShader;

    bondInstancingShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(bondInstancingShader, "instanceTransform");

    //Creating unique vectors of matrices for each element type to avoid shader computing for now.
    vector<Matrix> oxygenMatrices;
    vector<Matrix> hydrogenMatrices;
    vector<Matrix> carbonMatrices;
    vector<Matrix> sulfurMatrices;
    vector<Matrix> nitrogenMatrices;

    //Creating unique vectors of matrices for each connect type to avoid shader computing.
    vector<Matrix> oConnectMatrices;
    vector<Matrix> hConnectMatrices;
    vector<Matrix> cConnectMatrices;
    vector<Matrix> sConnectMatrices;
    vector<Matrix> nConnectMatrices;


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

    for (int i = 0; i < parsedFile.bondData.size(); i++) {
        
        Vector3 delta = Vector3Subtract(parsedFile.bondData[i].endPos, parsedFile.bondData[i].startPos);
        float distance = Vector3Length(delta);
        Vector3 direction = Vector3Normalize(delta);

        Quaternion q = QuaternionFromVector3ToVector3({0.0f, 1.0f, 0.0f}, direction);

        Matrix rotMat = QuaternionToMatrix(q);

        //Offset correct from GenCylinder
        Matrix centerShift = MatrixTranslate(0.0f, -0.5f, 0.0f);

        Matrix scaleMat = MatrixScale(0.1f, distance, 0.1f);
        Vector3 midPoint = Vector3Scale(Vector3Add(parsedFile.bondData[i].endPos, parsedFile.bondData[i].startPos), 0.5f);
        Matrix transMat = MatrixTranslate(midPoint.x, midPoint.y, midPoint.z);

        Matrix transform = MatrixMultiply(MatrixMultiply(MatrixMultiply(centerShift, scaleMat), rotMat), transMat);

        if (parsedFile.bondData[i].connectId == 'O') {
            oConnectMatrices.push_back(transform);
        }
        else if (parsedFile.bondData[i].connectId == 'N') {
            nConnectMatrices.push_back(transform);
        }
        else if (parsedFile.bondData[i].connectId == 'S') {
            sConnectMatrices.push_back(transform);
        }
        else if (parsedFile.bondData[i].connectId == 'C') {
            cConnectMatrices.push_back(transform);
        }
        else if (parsedFile.bondData[i].connectId == 'H') {
            hConnectMatrices.push_back(transform);
        }



    }

    Matrix globalRotation = MatrixIdentity();

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

        if (key == 65) {
            if (showAtoms)
            showAtoms = false;
            else 
            showAtoms = true;
            
        }

        if (key == 66) {
            if (showBonds)
            showBonds = false;
            else
            showBonds = true;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (rotate) {
                Vector2 mouseDelta = GetMouseDelta();
                if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) {
                    Matrix rotX = MatrixRotate({1.0f, 0.0f, 0.0f}, mouseDelta.y * 0.01f);
                    Matrix rotY = MatrixRotate({0.0f, 1.0f, 0.0f}, mouseDelta.x * 0.01f);

                    Matrix increamentalRot = MatrixMultiply(rotX, rotY);

                    globalRotation = MatrixMultiply(globalRotation, increamentalRot);
                }
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

        Matrix transformMatrix = MatrixMultiply(scaleMatrix, globalRotation);
        transformMatrix = MatrixMultiply(transformMatrix, translateMatrix);



        BeginDrawing();
            //Indentation for clarity
            ClearBackground(BLACK);
            BeginMode3D(camera);

            rlPushMatrix();
            rlMultMatrixf(MatrixToFloat(transformMatrix));
            
            if (showBonds) {
                renderBonds(parsedFile, oConnectMatrices, nConnectMatrices, sConnectMatrices
                 ,hConnectMatrices,cConnectMatrices,bondMaterial, bondMesh, modelScale);
            }
            if (showAtoms){
                renderAtoms(parsedFile, oxygenMatrices,
             nitrogenMatrices, sulfurMatrices, hydrogenMatrices, 
             carbonMatrices, atomMaterial, atomMesh, modelScale);
            }

            rlPopMatrix();

            EndMode3D();
        EndDrawing();

    }







    return 0;
}

void renderBonds(const PDBFileParser& moleculedata, const vector<Matrix>& oxygen, const vector<Matrix>& nitrogen,
 const vector<Matrix>& sulfur, const vector<Matrix>& hydrogen, const vector<Matrix>& carbon, Material& bondMaterial, const Mesh& bondMesh, const float& modelScale) {
            bondMaterial.maps[MATERIAL_MAP_DIFFUSE].color= RED;
            DrawMeshInstanced(bondMesh, bondMaterial, oxygen.data(), oxygen.size());

            bondMaterial.maps[MATERIAL_MAP_DIFFUSE].color= YELLOW;
            DrawMeshInstanced(bondMesh, bondMaterial, sulfur.data(), sulfur.size());

            bondMaterial.maps[MATERIAL_MAP_DIFFUSE].color= BLUE;
            DrawMeshInstanced(bondMesh, bondMaterial, nitrogen.data(), nitrogen.size());

            bondMaterial.maps[MATERIAL_MAP_DIFFUSE].color= GRAY;
            DrawMeshInstanced(bondMesh, bondMaterial, carbon.data(), carbon.size());

            bondMaterial.maps[MATERIAL_MAP_DIFFUSE].color= WHITE;
            DrawMeshInstanced(bondMesh, bondMaterial, hydrogen.data(), hydrogen.size());
}

void renderAtoms(const PDBFileParser& moleculedata, const vector<Matrix>& oxygen, const vector<Matrix>& nitrogen,
 const vector<Matrix>& sulfur, const vector<Matrix>& hydrogen, const vector<Matrix>& carbon, Material& atomMaterial, const Mesh& atomMesh, const float& modelScale) 
{
            atomMaterial.maps[MATERIAL_MAP_DIFFUSE].color= RED;
            DrawMeshInstanced(atomMesh, atomMaterial, oxygen.data(), oxygen.size());

            atomMaterial.maps[MATERIAL_MAP_DIFFUSE].color= YELLOW;
            DrawMeshInstanced(atomMesh, atomMaterial, sulfur.data(), sulfur.size());

            atomMaterial.maps[MATERIAL_MAP_DIFFUSE].color= BLUE;
            DrawMeshInstanced(atomMesh, atomMaterial, nitrogen.data(), nitrogen.size());

            atomMaterial.maps[MATERIAL_MAP_DIFFUSE].color= GRAY;
            DrawMeshInstanced(atomMesh, atomMaterial, carbon.data(), carbon.size());

            atomMaterial.maps[MATERIAL_MAP_DIFFUSE].color= WHITE;
            DrawMeshInstanced(atomMesh, atomMaterial, hydrogen.data(), hydrogen.size());
            
}
