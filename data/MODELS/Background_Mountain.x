xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 22;
 45.97349;8.83644;-69.90133;,
 0.00000;8.83644;-83.40030;,
 0.00000;-8.83644;-83.40030;,
 45.97349;-8.83644;-69.90133;,
 77.35073;8.83644;-33.69002;,
 77.35073;-8.83644;-33.69002;,
 84.16968;8.83644;13.73664;,
 84.16968;-8.83644;13.73664;,
 64.26531;8.83644;57.32108;,
 64.26531;-8.83644;57.32108;,
 23.95716;8.83644;83.22551;,
 23.95716;-8.83644;83.22551;,
 -23.95721;8.83644;83.22550;,
 -23.95721;-8.83644;83.22550;,
 -64.26533;8.83644;57.32105;,
 -64.26533;-8.83644;57.32105;,
 -84.16968;8.83644;13.73661;,
 -84.16968;-8.83644;13.73661;,
 -77.35070;8.83644;-33.69002;,
 -77.35070;-8.83644;-33.69002;,
 -45.97349;8.83644;-69.90133;,
 -45.97349;-8.83644;-69.90133;;
 
 11;
 4;0,1,2,3;,
 4;4,0,3,5;,
 4;6,4,5,7;,
 4;8,6,7,9;,
 4;10,8,9,11;,
 4;12,10,11,13;,
 4;14,12,13,15;,
 4;16,14,15,17;,
 4;18,16,17,19;,
 4;20,18,19,21;,
 4;1,20,21,2;;
 
 MeshMaterialList {
  1;
  11;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "data\\TEXTURE\\Mountain1.png";
   }
  }
 }
 MeshNormals {
  11;
  0.000000;0.000000;1.000000;,
  -0.540641;0.000000;0.841254;,
  -0.909632;0.000000;0.415415;,
  -0.989821;0.000000;-0.142315;,
  -0.755749;0.000000;-0.654861;,
  -0.281732;0.000000;-0.959493;,
  0.281733;0.000000;-0.959493;,
  0.755749;0.000000;-0.654861;,
  0.989822;0.000000;-0.142314;,
  0.909632;0.000000;0.415415;,
  0.540641;0.000000;0.841254;;
  11;
  4;1,0,0,1;,
  4;2,1,1,2;,
  4;3,2,2,3;,
  4;4,3,3,4;,
  4;5,4,4,5;,
  4;6,5,5,6;,
  4;7,6,6,7;,
  4;8,7,7,8;,
  4;9,8,8,9;,
  4;10,9,9,10;,
  4;0,10,10,0;;
 }
 MeshTextureCoords {
  22;
  -0.027384;-0.000303;,
  0.500141;-0.000303;,
  0.500141;0.999698;,
  -0.027384;0.999698;,
  -0.387423;-0.000303;,
  -0.387423;0.999698;,
  -0.465667;-0.000303;,
  -0.465667;0.999698;,
  -0.237274;-0.000303;,
  -0.237274;0.999698;,
  0.225244;-0.000303;,
  0.225244;0.999698;,
  0.775039;-0.000303;,
  0.775039;0.999698;,
  1.237556;-0.000303;,
  1.237556;0.999698;,
  1.465949;-0.000303;,
  1.465949;0.999698;,
  1.387704;-0.000303;,
  1.387704;0.999698;,
  1.027665;-0.000303;,
  1.027665;0.999698;;
 }
}
