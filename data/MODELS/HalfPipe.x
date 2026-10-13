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
 20;
 0.00000;-10.11708;52.69796;,
 10.71896;-7.53782;52.69796;,
 10.71896;-7.53782;-56.74204;,
 0.00000;-10.11707;-56.74204;,
 19.80606;-3.25400;52.69796;,
 19.80606;-3.25399;-56.74204;,
 25.87786;8.69692;52.69796;,
 25.87786;8.69692;-56.74204;,
 38.74522;9.13267;52.69796;,
 38.74522;9.13267;-56.74204;,
 -39.91022;8.76493;52.69796;,
 -25.87787;8.69692;52.69796;,
 -25.87787;8.69693;-56.74204;,
 -39.91022;8.76494;-56.74204;,
 -19.80605;-3.25399;52.69796;,
 -19.80605;-3.25399;-56.74204;,
 -10.71895;-7.53781;52.69796;,
 -10.71895;-7.53781;-56.74204;,
 0.00000;-10.11708;52.69796;,
 0.00000;-10.11707;-56.74204;;
 
 8;
 4;0,1,2,3;,
 4;1,4,5,2;,
 4;4,6,7,5;,
 4;6,8,9,7;,
 4;10,11,12,13;,
 4;11,14,15,12;,
 4;14,16,17,15;,
 4;16,18,19,17;;
 
 MeshMaterialList {
  1;
  8;
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
  }
 }
 MeshNormals {
  9;
  0.000000;1.000000;0.000000;,
  -0.331911;0.943311;0.000000;,
  -0.696581;0.717478;0.000000;,
  -0.891534;0.452954;0.000000;,
  -0.033845;0.999427;0.000000;,
  0.004846;0.999988;0.000000;,
  0.696581;0.717479;0.000000;,
  0.331912;0.943310;0.000000;,
  0.891533;0.452955;0.000000;;
  8;
  4;0,1,1,0;,
  4;1,2,2,1;,
  4;2,3,3,2;,
  4;4,4,4,4;,
  4;5,5,5,5;,
  4;8,6,6,8;,
  4;6,7,7,6;,
  4;7,0,0,7;;
 }
 MeshTextureCoords {
  20;
  0.000000;0.000000;,
  0.062500;0.000000;,
  0.062500;1.000000;,
  0.000000;1.000000;,
  0.125000;0.000000;,
  0.125000;1.000000;,
  0.187500;0.000000;,
  0.187500;1.000000;,
  0.250000;0.000000;,
  0.250000;1.000000;,
  0.750000;0.000000;,
  0.812500;0.000000;,
  0.812500;1.000000;,
  0.750000;1.000000;,
  0.875000;0.000000;,
  0.875000;1.000000;,
  0.937500;0.000000;,
  0.937500;1.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;;
 }
}
