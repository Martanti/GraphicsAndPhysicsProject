#include "objReader.h"

using namespace std;

objReader::objReader()
{
	numPts = 0;
	for (int i = 0; i < 5000; i++) {
		vertex[i].x = 0;
		vertex[i].y = 0;
		vertex[i].z = 0;

		normal[i].x = 0;
		normal[i].y = 0;
		normal[i].z = 0;

		texCoord[i].x = 0;
		texCoord[i].y = 0;
	}

	for (int i = 0; i < 5000; i++) {
		faces[i].id1 = 0;
		faces[i].id2 = 0;
		faces[i].id3 = 0;

		texfaces[i].id1 = 0;
		texfaces[i].id2 = 0;
		texfaces[i].id3 = 0;
	}
}


objReader::~objReader()
{
	//delete[] vertices;
	delete[] v;
}

void objReader::loadVertex(char* filename)
{
	ifstream file;
	file.open(filename);

	while (!file.eof()) //while we are still in the file
	{
		getline(file, str); //move one line down
		if (str[0] == 'v' && str[1] != 't') break; //if we have a vertex line, stop
	}

	int v = 0;

	while (str[0] == 'v' && str[1] != 't') {

		//convert data into X,Y,Z
		str[0] = ' ';
		sscanf_s(str.c_str(), "%f %f %f ",							// Read floats from the line: v X Y Z
			&vertex[v].x,
			&vertex[v].y,
			&vertex[v].z);

		//std::cout << vertex[v].x << " " << vertex[v].y << " " << vertex[v].z << endl;

		v++;
		getline(file, str);
	}


	numPts = v;

	//cout << endl;
}

void objReader::LoadFace(char* filename)
{
	int count, tt;
	ifstream file;
	file.open(filename);

	while (true)
	{
		getline(file, str);
		if (str[0] == 'f') {
			break;
		}
	}

	count = 0;
	while (str[0] == 'f')
	{
		str[0] = ' ';
		sscanf_s(str.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d",
			&faces[count].id1, &texfaces[count].id1, &tt,
			&faces[count].id2, &texfaces[count].id2, &tt,
			&faces[count].id3, &texfaces[count].id3, &tt);


		faces[count].id1--;
		faces[count].id2--;
		faces[count].id3--;

		texfaces[count].id1--;
		texfaces[count].id2--;
		texfaces[count].id3--;

		count++;

		getline(file, str);
	}

	numFaces = count;
	numVertices = numFaces * 3;
	num = numFaces * 6;

	v = new fVertex[numVertices];
	int j = 0;

	//Adding vertex coords
	for (int i = 0; i < numVertices; i += 3)
	{
		v[i].coords[0] = vertex[faces[j].id1].x;
		v[i].coords[1] = vertex[faces[j].id1].y;
		v[i].coords[2] = vertex[faces[j].id1].z;
		v[i].texCoords[0] = texCoord[texfaces[j].id1].x;
		v[i].texCoords[1] = texCoord[texfaces[j].id1].y;

		v[i + 1].coords[0] = vertex[faces[j].id2].x;
		v[i + 1].coords[1] = vertex[faces[j].id2].y;
		v[i + 1].coords[2] = vertex[faces[j].id2].z;
		v[i + 1].texCoords[0] = texCoord[texfaces[j].id2].x;
		v[i + 1].texCoords[1] = texCoord[texfaces[j].id2].y;

		v[i + 2].coords[0] = vertex[faces[j].id3].x;
		v[i + 2].coords[1] = vertex[faces[j].id3].y;
		v[i + 2].coords[2] = vertex[faces[j].id3].z;
		v[i + 2].texCoords[0] = texCoord[texfaces[j].id3].x;
		v[i + 2].texCoords[1] = texCoord[texfaces[j].id3].y;

		j++;
	}

	/*cout << "face: " << endl;
	for (int i = 0; i < numVertices; i++)
	{
		cout << endl;
		cout << v[i].texCoords[0] << " " << v[i].texCoords[1];
	}
	*/
	//out << numFaces;
}

void objReader::LoadModel(char* filename)
{
	loadVertex(filename);
	
	LoadTexCoord(filename);

	LoadFace(filename);



	CalNormal();

	//for (int i = 0; i < numVertices; i++)
	//	std::cout << v->coords[0] << " " << v->coords[1] << " " << v->coords[2] << std::endl;
}

void objReader::CalNormal(void)
{
	int i, id1, id2, id3;
	VertexCoords edge1, edge2;
	VertexCoords testNor, unitNor, upVec;
	float len, dot;

	//upVec.x = 0; upVec.y = 0; upVec.z = 1;
	for (i = 0; i < numFaces; i++)
	{
		id1 = faces[i].id1;
		id2 = faces[i].id2;
		id3 = faces[i].id3;

		edge1.x = vertex[id2].x - vertex[id1].x;
		edge1.y = vertex[id2].y - vertex[id1].y;
		edge1.z = vertex[id2].z - vertex[id1].z;

		edge2.x = vertex[id3].x - vertex[id2].x;
		edge2.y = vertex[id3].y - vertex[id2].y;
		edge2.z = vertex[id3].z - vertex[id2].z;

		testNor.x = edge1.y * edge2.z - edge1.z * edge2.y;
		testNor.y = edge1.z * edge2.x - edge1.x * edge2.z;
		testNor.z = edge1.x * edge2.y - edge1.y * edge2.x;

		len = sqrt(testNor.x * testNor.x + testNor.y * testNor.y + testNor.z * testNor.z);
		unitNor.x = testNor.x / len;
		unitNor.y = testNor.y / len;
		unitNor.z = testNor.z / len;



		/*unitNor.x = 0;
		unitNor.y = 0;
		unitNor.z = 1;*/


		/*dot = unitNor.x*upVec.x + unitNor.y*upVec.y + unitNor.z*upVec.z;
		if (dot < 0.0)
		{
		unitNor.x = -unitNor.x;
		unitNor.y = -unitNor.y;
		unitNor.z = -unitNor.z;
		}*/


		normal[id1].x = unitNor.x;
		normal[id1].y = unitNor.y;
		normal[id1].z = unitNor.z;

		normal[id2].x = unitNor.x;
		normal[id2].y = unitNor.y;
		normal[id2].z = unitNor.z;

		normal[id3].x = unitNor.x;
		normal[id3].y = unitNor.y;
		normal[id3].z = unitNor.z;
	}
	int j = 0;
	for (int i = 0; i < numVertices; i += 3)
	{
		v[i].normals[0] = normal[faces[j].id1].x;
		v[i].normals[1] = vertex[faces[j].id1].y;
		v[i].normals[2] = vertex[faces[j].id1].z;

		v[i + 1].normals[0] = normal[faces[j].id2].x;
		v[i + 1].normals[1] = vertex[faces[j].id2].y;
		v[i + 1].normals[2] = vertex[faces[j].id2].z;

		v[i + 2].normals[0] = normal[faces[j].id3].x;
		v[i + 2].normals[1] = vertex[faces[j].id3].y;
		v[i + 2].normals[2] = vertex[faces[j].id3].z;

		j++;
	}
}

void objReader::drawObj()
{
}

void objReader::LoadTexCoord(char* filename)
{
	ifstream file;
	file.open(filename);

	while (!file.eof()) //while we are still in the file
	{
		getline(file, str); //move one line down
		if (str[0] == 'v' && str[1] == 't') break; //if we have a vertex line, stop
	}

	int v = 0;

	while (str[0] == 'v' && str[1] == 't') {

		//convert data into X,Y,Z
		str[0] = ' '; str[1] = ' ';

		sscanf_s(str.c_str(), "%f %f",							// Read floats from the line: vt X Y Z
			&texCoord[v].x,
			&texCoord[v].y);

		//std::cout << texCoord[v].x << " " << texCoord[v].y << endl;

		v++;
		getline(file, str);
	}

	numPts = v;

	//cout << endl;
}

void objReader::LoadNormals(char* filename)
{
	ifstream file;
	file.open(filename);

	while (!file.eof()) //while we are still in the file
	{
		getline(file, str); //move one line down
		if (str[0] == 'v' && str[1] == 'n') break; //if we have a vertex line, stop
	}

	int v = 0;

	while (str[0] == 'v' && str[1] == 'n') {

		//convert data into X,Y,Z
		str[0] = ' ';
		str[1] = ' ';
		sscanf_s(str.c_str(), "%f %f %f",							// Read floats from the line: v X Y Z
			&normal[v].x,
			&normal[v].y,
			&normal[v].z);

		//std::cout << vertex[v].x << " " << vertex[v].y << " " << vertex[v].z << endl;

		v++;
		getline(file, str);
	}


	numNormals = v;

	//cout << endl;
}