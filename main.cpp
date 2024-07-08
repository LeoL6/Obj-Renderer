//#define WIN32_LEAN_AND_MEAN
//#define UNICODE
#define ID_FILE_OPEN 420
#define ID_FILE_CLOSE 421
#define ID_EXIT 999
#define ID_DRAW_MODE 69

#include <strstream>
#include <algorithm>
#include <windows.h>
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
using namespace std;

void* memory;
int client_width;
int client_height;

bool wireframe = false;

float fYaw;
float fTheta;

string model_path;

struct vi2d
{
    int32_t x = 0;
    int32_t y = 0;
};

struct color
{
    int rgb[3] = { 0, 0, 0 };
};

struct vec3d
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;
};

struct triangle
{
	vec3d pos[3];

	color color;
};

struct mesh
{
	vector<triangle> tris;

	bool LoadFromObjectFile(string sFilename)
	{
		if (sFilename == "")
			return false;

		ifstream f(sFilename);
	
		if (sFilename.substr(sFilename.find_last_of(".") + 1) != "obj")
			return false;
	
		if (!f.is_open())
			return false;
	
		// Local cache of verts
		vector<vec3d> verts;
	
		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);
	
			strstream s;
			s << line;
	
			char junk;
	
			// OLDER VERSIONS AND PROLLY NEWER VERSIONS OF OBJ HAVE VT AND 
			// -OTHER TYPES SO IMMA NEED TO RESEARCH AND MAKE THIS BETTER
			// DIS http://paulbourke.net/dataformats/obj/
	
			// Quick fix, remove the last 3 conditions later
			if (line[0] == 'v' && (line[1] != 't' && line[1] != 'n' && line[1] != 'pos'))
			{
				vec3d v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}
	
			if (line[0] == 'f')
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}
		printf("LOADED FILE");
		return true;
	}

	bool lobberj(string Path/*, vector<vec3d>& Verticies, vector<vec3d>& Normals, vector<vec3d>& UVs*/ )
	{
		vector<unsigned short> VIndex, NIndex, UVIndex;
		vector<vec3d> TempNorms, TempVerts;
		//vector<vec2> TempUVs;

		tris.clear();
		//Normals.clear();
		//UVs.clear();

		string Data;
		ifstream File(Path.c_str());
		while (!File.eof())
		{
			getline(File, Data);
			string Temp = "";
			if (Data[0] == 'v' && Data[1] != 't' && Data[1] != 'n')
			{
				vec3d Current;

				bool Negative[3] = { false, false, false };
				Negative[0] = (Data[2] == '-') ? true : false;
				for (int x = 0; x < 8 + Negative[0]; x++)
					Temp += Data[x + 2];
				Current.x = stof(Temp);

				for (int vertex = 1; vertex < 3; vertex++)
				{
					Temp = "";
					int Push = 0;
					for (int n = 0; n < vertex; n++)
						Push += Negative[n];
					Negative[vertex] = (Data[2 + 9 * vertex + Push] == '-') ? true : false;
					for (int x = 0; x < 8 + Negative[vertex]; x++)
						Temp += Data[x + 2 + 9 * vertex + Push];
					if (vertex == 1)
					{
						Current.x = stof(Temp);
					}
					else if (vertex == 2)
					{
						Current.y = stof(Temp);
					}
					else
					{
						Current.z = stof(Temp);
					}
				}
				TempVerts.push_back(Current);
				/*
				cout<<"Stored: "<<TempVerts.back()<<endl;
				cout<<"Actual: "<<Data<<endl;
				*/
			}
			//else if (Data[0] == 'v' and Data[1] == 't')
			//{
			//	vec2 Current;
			//	for (int v = 0; v < 2; v++)
			//	{
			//		Temp = "";
			//		for (int i = 0; i < 8; i++)
			//			Temp += Data[3 + i + v * 9];
			//		Current[v] = StringToFloat(Temp);
			//		TempUVs.push_back(Current);
			//	}
			//	/*
			//	cout<<"Stored: "<<TempUVs.back()<<endl;
			//	cout<<"Actual: "<<Data<<endl;
			//	*/
			//}
			//else if (Data[0] == 'v' and Data[1] == 'n')
			//{
			//	vec3d Current;
			//	bool Negative[3] = { false,GL_FALSE,0 };
			//	Negative[0] = (Data[3] == '-') ? true : false;
			//	for (int i = 0; i < 8 + Negative[0]; i++)
			//		Temp += Data[3 + i];
			//	Current[0] = StringToFloat(Temp);
			//
			//	for (int v = 1; v < 3; v++)
			//	{
			//		Temp = "";
			//
			//		int Push = 0;
			//		for (int n = 0; n < v; n++)
			//			Push += Negative[n];
			//		Negative[v] = (Data[3 + 9 * v + Push] == '-') ? true : false;
			//		for (int i = 0; i < 8 + Negative[v]; i++)
			//			Temp += Data[3 + 9 * v + Push + i];
			//		Current[v] = StringToFloat(Temp);
			//		TempNorms.push_back(Current);
			//	}
			//	/*
			//	cout<<"Stored: "<<TempNorms.back()<<endl;
			//	cout<<"Actual: "<<Data<<endl;
			//	*/
			//}
			else if (Data[0] == 'f')
			{
				int position = 2;
				for (int i = 0; i < 3; i++)
				{
					while (isdigit(Data[position]))
					{
						Temp += Data[position];
						position++;
					}
					stof(Temp); ///For some reason it only works if I call it once beforehand
					VIndex.push_back(stof(Temp));

					Temp = "";
					position++;
					while (isdigit(Data[position]))
					{
						Temp += Data[position];
						position++;
					}
					stof(Temp);
					UVIndex.push_back(stof(Temp));

					Temp = "";
					position++;
					while (isdigit(Data[position]))
					{
						Temp += Data[position];
						position++;
					}
					stof(Temp);
					NIndex.push_back(stof(Temp));

					Temp = "";
					position++;
				}
				/*
				cout<<"Actual: "<<Data<<endl;
				cout<<"Stored: ";
				for ( int x = 3; x > 0; x-- )
				{
					cout<<VIndex[VIndex.size() - x]<<'/'<<UVIndex[UVIndex.size() - x]<<'/'<<NIndex[NIndex.size() - x];
					cout<<' ';
				}
				cout<<endl;
				*/
			}
		}
		for (int i = 0; i < VIndex.size(); i++)
		{
			tris.push_back({ TempVerts[VIndex[i] - 1] });
			//Normals.push_back(TempNorms[NIndex[i] - 1]);
			//UVs.push_back(TempUVs[UVIndex[i] - 1]);
		}
		return true;
	}

	// OBJ
	bool loadObj(string sFilename)
	{
		ifstream stream(sFilename);
		assert(stream);

		vector<vec3d> normal;
		vector<float> uv;
		vector<vec3d> vtx;

		while (!stream.eof()) {
			string s;
			getline(stream, s);
			if (s == "# normals") {
				cout << "normals" << endl;
				while (!stream.eof()) {
					string s;
					getline(stream, s);

					// TIPS:
					stringstream ss;
					ss << s;

					string key;
					ss >> key;
					if (key != "vn") break;

					float x, y, z;
					ss >> x >> y >> z;
					normal.push_back({ x, y, z });
				}
				cout << normal.size() / 3 << endl;
			}
			else if (s == "# texcoords") {
				cout << "texcoords" << endl;
				while (!stream.eof()) {
					string s;
					getline(stream, s);

					stringstream ss;
					ss << s;

					string key;
					ss >> key;
					if (key != "vt") break;

					float x, y;
					ss >> x >> y;
					uv.push_back(x);
					uv.push_back(y);
				}
				cout << uv.size() / 2 << endl;
			}
			else if (s == "# verts") {
				cout << "verts" << endl;
				while (!stream.eof()) {
					string s;
					getline(stream, s);

					stringstream ss;
					ss << s;

					string key;
					ss >> key;
					if (key != "v") break;

					float x, y, z;
					ss >> x >> y >> z;
					vtx.push_back({ x, y, z });
				}
				cout << vtx.size() / 3 << endl;
			}
			else if (s == "# faces") {
				cout << "faces" << endl;
				while (!stream.eof()) {
					string s;
					getline(stream, s);

					stringstream ss;
					ss << s;

					string key;
					ss >> key;
					if (key != "f") break;

					// FIXME:
					int vi[3];
					int ti[3];
					int ni[3];
					char separate;

					ss >> vi[0] >> separate >> ti[0] >> separate >> ni[0];
					ss >> vi[1] >> separate >> ti[1] >> separate >> ni[1];
					ss >> vi[2] >> separate >> ti[2] >> separate >> ni[2];

					tris.push_back({ vtx[vi[0] - 1], vtx[vi[1] - 1], vtx[vi[2] - 1] });

					//for (int i = 0; i < 3; ++i) {
					//
					//	obj.vtx.push_back(vtx[(vi[i] - 1) * 3 + 0]);
					//	obj.vtx.push_back(vtx[(vi[i] - 1) * 3 + 1]);
					//	obj.vtx.push_back(vtx[(vi[i] - 1) * 3 + 2]);
					//	
					//	obj.uv.push_back(uv[(ti[i] - 1) * 2 + 0]);
					//	obj.uv.push_back(uv[(ti[i] - 1) * 2 + 1]);
					//	
					//	obj.normal.push_back(normal[(ni[i] - 1) * 3 + 0]);
					//	obj.normal.push_back(normal[(ni[i] - 1) * 3 + 1]);
					//	obj.normal.push_back(normal[(ni[i] - 1) * 3 + 2]);
					//}
				}
			}
		}

		return true;
		//return obj;
	}

	bool load_obj(string sFilename)
	{
		if (sFilename == "")
			return false;

		//Vertex portions
		vector<vec3d> vertex_positions;
		//vector<glm::fvec2> vertex_texcoords;
		vector<vec3d> vertex_normals;

		//Face vectors
		vector<int> vertex_position_indicies;
		//vector<GLint> vertex_texcoord_indicies;
		vector<int> vertex_normal_indicies;

		//Vertex array
		//vector<Vertex> vertices;

		stringstream ss;
		ifstream in_file(sFilename);
		string line = "";
		string prefix = "";
		vec3d temp_vec3;
		//glm::vec2 temp_vec2;
		int temp_glint = 0;

		//File open error check
		if (!in_file.is_open())
		{
			throw "ERROR::OBJLOADER::Could not open file.";
			MessageBoxA(NULL, "HELP", "", MB_OK);
		}

		//Read one line at a time
		while (getline(in_file, line))
		{
			//Get the prefix of the line
			ss.clear();
			ss.str(line);
			ss >> prefix;

			if (prefix == "#")
			{

			}
			else if (prefix == "o")
			{

			}
			else if (prefix == "s")
			{

			}
			else if (prefix == "use_mtl")
			{

			}
			else if (prefix == "v") //Vertex position
			{
				ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
				vertex_positions.push_back(temp_vec3);
			}
			else if (prefix == "vt")
			{
				//ss >> temp_vec2.x >> temp_vec2.y;
				//vertex_texcoords.push_back(temp_vec2);
			}
			else if (prefix == "vn")
			{
				ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
				vertex_normals.push_back(temp_vec3);
			}
			else if (prefix == "f")
			{
				int counter = 0;
				while (ss >> temp_glint)
				{
					//Pushing indices into correct arrays
					if (counter == 0)
						vertex_position_indicies.push_back(temp_glint);
					//else if (counter == 1)
					//	vertex_texcoord_indicies.push_back(temp_glint);
					else if (counter == 2)
						vertex_normal_indicies.push_back(temp_glint);

					//Handling characters
					if (ss.peek() == '/')
					{
						++counter;
						ss.ignore(1, '/');
					}
					else if (ss.peek() == ' ')
					{
						++counter;
						ss.ignore(1, ' ');
					}

					//Reset the counter
					if (counter > 2)
						counter = 0;
				}
			}
			else
			{
				
			}
		}

		//Build final vertex array (mesh)
		//tris.resize(vertex_position_indicies.size(), triangle());

		//Load in all indices
		//for (size_t i = 0; i < tris.size(); ++i)
		//{
		//	vec3d loot = vertex_positions[vertex_position_indicies[i] - 1];
		//	tris[i].pos->x = loot.x;
		//	tris[i].pos->y = loot.y;
		//	tris[i].pos->z = loot.z;
		//	//tris[i].pos = vertex_positions[vertex_position_indicies[i] - 1];
		//	//tris[i].texcoord = vertex_texcoords[vertex_texcoord_indicies[i] - 1];
		//	//tris[i].normal = vertex_normals[vertex_normal_indicies[i] - 1];
		//	tris[i].color = { 255, 255, 255 };
		//}

		//Load in all indices
		for (size_t i = 0; i < tris.size(); ++i)
		{
			vec3d loot = vertex_positions[vertex_position_indicies[i] - 1];
			tris.push_back({ loot.x, loot.y, loot.z });
		}

		//DEBUG
		cout << "Nr of vertices: " << tris.size() << "\n";

		//Loaded success
		cout << "OBJ file loaded!" << "\n";
		MessageBoxA(NULL, "LOADED OBJ", "", MB_OK);
		return true;
		//return vertices;
	}

	bool loabber(string sFilename)
	{
		if (sFilename == "")
			return false;

		if (sFilename.substr(sFilename.find_last_of(".") + 1) != "obj")
			return false;

		vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		vector<vec3d> temp_vertices;
		//vector<vec2d> temp_uvs;
		vector<vec3d> temp_normals;

		FILE* file = fopen(sFilename.c_str(), "r");
		if (file == NULL) {
			printf("Impossible to open the file !\n");
			return false;
		}

		while (1) {

			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

			// else : parse lineHeader
			if (strcmp(lineHeader, "v") == 0) {
				vec3d vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			//else if (strcmp(lineHeader, "vt") == 0) {
			//	glm::vec2 uv;
			//	fscanf(file, "%f %f\n", &uv.x, &uv.y);
			//	temp_uvs.push_back(uv);
			//}
			else if (strcmp(lineHeader, "vn") == 0) {
				vec3d normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					return false;
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);

				//tris.push_back({ temp_vertices[vertexIndex[0]], temp_vertices[vertexIndex[1]], temp_vertices[vertexIndex[2]] });
				//tris.push_back({ vertexIndex[0], vertexIndex[1], vertexIndex[2] });
				//uvIndices.push_back(uvIndex[0]);
				//uvIndices.push_back(uvIndex[1]);
				//uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}

			// For each vertex of each triangle

			vector<vec3d> tree;
			int k = 1;

			for (unsigned int i = 0; i < vertexIndices.size(); i++) {
				unsigned int vertexIndex = vertexIndices[i];
				vec3d vertex = temp_vertices[vertexIndex - 1];
				tree.push_back(vertex);
				k++;
				if (k >= 3)
				{
					tris.push_back({ tree[0], tree[1], tree[2] });
					tree.clear();
					k = 1;
				}
			}
		}
		return true;
	}

	bool clear_tris()
	{
		tris.clear();
		return true;
	}
};

struct mat4x4
{
	float m[4][4] = { 0 };
};

mesh meshCube;
mat4x4 matProj;

vec3d vCamera;
vec3d vLookDir;

vec3d Vector_Add(vec3d& v1, vec3d& v2)
{
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

vec3d Vector_Sub(vec3d& v1, vec3d& v2)
{
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

vec3d Vector_Mul(vec3d& v1, float k)
{
	return { v1.x * k, v1.y * k, v1.z * k };
}

vec3d Vector_Div(vec3d& v1, float k)
{
	return { v1.x / k, v1.y / k, v1.z / k };
}

float Vector_DotProduct(vec3d& v1, vec3d& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float Vector_Length(vec3d& v)
{
	return sqrtf(Vector_DotProduct(v, v));
}

vec3d Vector_Normalise(vec3d& v)
{
	float l = Vector_Length(v);
	return { v.x / l, v.y / l, v.z / l };
}

vec3d Vector_CrossProduct(vec3d& v1, vec3d& v2)
{
	vec3d v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}

vec3d Vector_IntersectPlane(vec3d& plane_p, vec3d& plane_n, vec3d& lineStart, vec3d& lineEnd)
{
	plane_n = Vector_Normalise(plane_n);
	float plane_d = -Vector_DotProduct(plane_n, plane_p);
	float ad = Vector_DotProduct(lineStart, plane_n);
	float bd = Vector_DotProduct(lineEnd, plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	vec3d lineStartToEnd = Vector_Sub(lineEnd, lineStart);
	vec3d lineToIntersect = Vector_Mul(lineStartToEnd, t);
	return Vector_Add(lineStart, lineToIntersect);
}

vec3d Matrix_MultiplyVector(mat4x4& m, vec3d& i)
{
	vec3d v;
	v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
	v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
	v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
	v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
	return v;
}

mat4x4 Matrix_MakeIdentity()
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 Matrix_MakeRotationX(float fAngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[1][2] = sinf(fAngleRad);
	matrix.m[2][1] = -sinf(fAngleRad);
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 Matrix_MakeRotationY(float fAngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][2] = sinf(fAngleRad);
	matrix.m[2][0] = -sinf(fAngleRad);
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 Matrix_MakeRotationZ(float fAngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][1] = sinf(fAngleRad);
	matrix.m[1][0] = -sinf(fAngleRad);
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 Matrix_MakeTranslation(float x, float y, float z)
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	matrix.m[3][0] = x;
	matrix.m[3][1] = y;
	matrix.m[3][2] = z;
	return matrix;
}

mat4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
	float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
	mat4x4 matrix;
	matrix.m[0][0] = fAspectRatio * fFovRad;
	matrix.m[1][1] = fFovRad;
	matrix.m[2][2] = fFar / (fFar - fNear);
	matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][3] = 0.0f;
	return matrix;
}

mat4x4 Matrix_MultiplyMatrix(mat4x4& m1, mat4x4& m2)
{
	mat4x4 matrix;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
	return matrix;
}

mat4x4 Matrix_PointAt(vec3d& pos, vec3d& target, vec3d& up)
{
	// Calculate new forward direction
	vec3d newForward = Vector_Sub(target, pos);
	newForward = Vector_Normalise(newForward);

	// Calculate new Up direction
	vec3d a = Vector_Mul(newForward, Vector_DotProduct(up, newForward));
	vec3d newUp = Vector_Sub(up, a);
	newUp = Vector_Normalise(newUp);

	// New Right direction is easy, its just cross product
	vec3d newRight = Vector_CrossProduct(newUp, newForward);

	// Construct Dimensioning and Translation Matrix
	mat4x4 matrix;
	matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
	return matrix;

}

mat4x4 Matrix_QuickInverse(mat4x4& m) // Only for Rotation/Translation Matrices
{
	mat4x4 matrix;
	matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

int Triangle_ClipAgainstPlane(vec3d plane_p, vec3d plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2)
{
	// Make sure plane normal is indeed normal
	plane_n = Vector_Normalise(plane_n);

	// Return signed shortest distance from point to plane, plane normal must be normalised
	auto dist = [&](vec3d& pos)
	{
		vec3d n = Vector_Normalise(pos);
		return (plane_n.x * pos.x + plane_n.y * pos.y + plane_n.z * pos.z - Vector_DotProduct(plane_n, plane_p));
	};

	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	vec3d* inside_points[3];  int nInsidePointCount = 0;
	vec3d* outside_points[3]; int nOutsidePointCount = 0;

	// Get signed distance of each point in triangle to plane
	float d0 = dist(in_tri.pos[0]);
	float d1 = dist(in_tri.pos[1]);
	float d2 = dist(in_tri.pos[2]);

	if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.pos[0]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.pos[0]; }
	if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.pos[1]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.pos[1]; }
	if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.pos[2]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.pos[2]; }

	// Now classify triangle points, and break the input triangle into
	// smaller output triangles if required. There are four possible
	// outcomes...

	if (nInsidePointCount == 0)
	{
		// All points lie on the outside of plane, so clip whole triangle
		// It ceases to exist

		return 0; // No returned triangles are valid
	}

	if (nInsidePointCount == 3)
	{
		// All points lie on the inside of plane, so do nothing
		// and allow the triangle to simply pass through
		out_tri1 = in_tri;

		return 1; // Just the one returned original triangle is valid
	}

	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		// Triangle should be clipped. As two points lie outside
		// the plane, the triangle simply becomes a smaller triangle

		// Copy appearance info to new triangle

		//out_tri1.color = in_tri.color;
		out_tri1.color = { 255, 0, 0 };

		// The inside point is valid, so keep that...
		out_tri1.pos[0] = *inside_points[0];

		// but the two new points are at the locations where the
		// original sides of the triangle (lines) intersect with the plane
		out_tri1.pos[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
		out_tri1.pos[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

		return 1; // Return the newly formed single triangle
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		// Triangle should be clipped. As two points lie inside the plane,
		// the clipped triangle becomes a "quad". Fortunately, we can
		// represent a quad with two new triangles

		// Copy appearance info to new triangles

		//out_tri1.color = in_tri.color;

		//out_tri2.color = in_tri.color;

		out_tri1.color = { 0, 0, 255 };

		out_tri2.color = { 0, 255, 0 };

		// The first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1.pos[0] = *inside_points[0];
		out_tri1.pos[1] = *inside_points[1];
		out_tri1.pos[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

		// The second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the
		// triangle and the plane, and the newly created point above
		out_tri2.pos[0] = *inside_points[1];
		out_tri2.pos[1] = out_tri1.pos[2];
		out_tri2.pos[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

		return 2; // Return two newly formed triangles which form a quad
	}
}

void open_file(HWND hWnd)
{
	OPENFILENAME ofn;

	char char_path[256];

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = (LPWSTR)char_path;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 100;
	ofn.lpstrFilter = L"All Files\0*.*\0Source Files\0*.CPP\0Text Files\0*.TXT\0";
	ofn.nFilterIndex = 1;

	GetOpenFileName(&ofn);

	// SHITTY WORKING CODE
	wstring ws(ofn.lpstrFile);
	string sVar = string(ws.begin(), ws.end());
	string file_name = sVar.substr(sVar.find_last_of("/\\") + 1);
	wstring stemp = wstring(file_name.begin(), file_name.end());
	LPCWSTR sw = stemp.c_str();

	if (meshCube.clear_tris() && meshCube.LoadFromObjectFile(sVar)/*meshCube.LoadFromObjectFile(sVar)*/)
	{
		//MessageBox(NULL, sw, L"", MB_OK);
		return;
	}

	//MessageBox(NULL, L"Failed to load file", L"", MB_OK);
	//MessageBox(hWnd, L"Failed to load file", L"", MB_OK);
}

//template <class T> void swap(T& a, T& b);

color get_color(float lum);
void clear_screen(uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color, uint32_t pattern);
void draw_triangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);
void draw_triangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, color color);
void FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, color color);
void FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t c);

LRESULT CALLBACK
window_proc(HWND window,
    UINT message,
    WPARAM w_param,
    LPARAM l_param)
{
    switch (message)
    {
		case WM_CREATE:
		{
			CreateWindow(L"BUTTON", L"Draw Mode",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				530, 390, 80, 20,
				window , (HMENU)ID_DRAW_MODE, NULL, NULL
			);

			HMENU hMenuBar = CreateMenu();
			HMENU hFile = CreateMenu();

			// Menubar
			AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR) hFile, L"File");

			// File Options
			AppendMenu(hFile, MF_STRING, ID_FILE_OPEN, L"Open File");
			AppendMenu(hFile, MF_STRING, ID_FILE_CLOSE, L"Close File");
			AppendMenu(hFile, MF_STRING, ID_EXIT, L"Exit");

			SetMenu(window, hMenuBar);

		} break;

		case WM_COMMAND:
		{
			if (w_param == ID_FILE_OPEN)
			{
				open_file(window);
				string s = to_string(meshCube.tris.size());
				MessageBoxA(NULL, s.c_str(), "", MB_OK);
			}

			if (w_param == ID_FILE_CLOSE)
			{
				meshCube.clear_tris();
			}

			if (w_param == ID_DRAW_MODE)
			{
				wireframe = !wireframe;
			}

			if (w_param == ID_EXIT)
			{
				DestroyWindow(window);
			}

		}break;

		case WM_KEYDOWN:
		{
			switch (w_param)
			{
				// "q" quits the program
				case 'Q':
				{
					DestroyWindow(window);
				};
			}
		} break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
		} break;

		default:
		{
			return DefWindowProc(window,
				message,
				w_param,
				l_param);
		}
    }

    return 0;
}

int WINAPI
wWinMain(HINSTANCE instance,
    HINSTANCE prev_instance,
    PWSTR cmd_line,
    int cmd_show)
{
    // window creation

    WNDCLASS window_class = {};

    const wchar_t class_name[] = L"MyWindowClass";

    window_class.lpfnWndProc = window_proc;
    window_class.hInstance = instance;
    window_class.lpszClassName = class_name;
    window_class.hCursor = LoadCursor(0, IDC_CROSS);

    if (!RegisterClass(&window_class))
    {
        MessageBox(0, L"RegisterClass failed", 0, 0);
        return GetLastError();
    }

    HWND window = CreateWindowEx(0,
        class_name,
        L"Pixel Engine",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, // X 
        CW_USEDEFAULT, // Y
        640, // Width
        480, // Height
        0,
        0,
        instance,
        0);

    if (!window)
    {
        MessageBox(0, L"CreateWindowEx failed", 0, 0);
        return GetLastError();
    }

    // allocate memory

    RECT rect;
    GetClientRect(window, &rect);
	// - 10 so that it is a little smaller, we gunna make a whole 3dmodel viewer with UI :)
    client_width = (rect.right - rect.left);
    client_height = (rect.bottom - rect.top) - 40;

    memory = VirtualAlloc(0,
        client_width * client_height * 4,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    // create BITMAPINFO struct for StretchDIBits

    BITMAPINFO bitmap_info;
    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = client_width;
    bitmap_info.bmiHeader.biHeight = client_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;

    HDC hdc = GetDC(window);
	
	// Called once at the start, so create things here

	//meshCube.LoadFromObjectFile("teapot.obj");
	meshCube.LoadFromObjectFile("teapot.obj");

	// PROJECTION MATRIX
	matProj = Matrix_MakeProjection(90.0f, (float)client_height / (float)client_width, 0.1f, 1000.0f);

    // loop

    bool running = true;

    while (running)
    {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) running = false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // draw white pixel at 100, 100 (from bottom left)
        //for (int i = 1; i < 10; i++)
        //{ 
        //    for (int j = 1; j < 10; j++)
        //    {
        //        draw_pixel(100 + i, 100 + j, 0xffffff);
        //    }
        //}

        //color c = { 255, 0, 0 };
        //draw_triangle(70, 50, 30, 90, 20, 20, c);

		//if (GetKey(olc::Key::UP).bHeld)
		//	vCamera.y += 8.0f * fElapsedTime;
		//
		//if (GetKey(olc::Key::DOWN).bHeld)
		//	vCamera.y -= 8.0f * fElapsedTime;
		//
		//if (GetKey(olc::Key::LEFT).bHeld)
		//	vCamera.x += 8.0f * fElapsedTime;
		//
		//if (GetKey(olc::Key::RIGHT).bHeld)
		//	vCamera.x -= 8.0f * fElapsedTime;
		//
		//vec3d vForward = Vector_Mul(vLookDir, 8.0f * fElapsedTime);
		//
		//if (GetKey(olc::Key::W).bHeld)
		//	vCamera = Vector_Add(vCamera, vForward);
		//
		//if (GetKey(olc::Key::S).bHeld)
		//	vCamera = Vector_Sub(vCamera, vForward);
		//
		//if (GetKey(olc::Key::A).bHeld)
		//	fYaw -= 2.0f * fElapsedTime;
		//
		//if (GetKey(olc::Key::D).bHeld)
		//	fYaw += 2.0f * fElapsedTime;

		// Clear screen
		clear_screen(0x0);

		//draw_triangle(70 + ra, 50, 30 + ra, 90, 20 + ra, 20, 0xffffff);
		//cout << "POSTING";
		//cout.flush();

		mat4x4 matRotZ, matRotX;
		//fTheta += 1.0f * fElapsedTime;

		matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);
		matRotX = Matrix_MakeRotationX(fTheta);

		mat4x4 matTrans;
		matTrans = Matrix_MakeTranslation(0.0f, -1.0f, 4.0f);

		mat4x4 matWorld;
		matWorld = Matrix_MakeIdentity();
		matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX);
		matWorld = Matrix_MultiplyMatrix(matWorld, matTrans);

		// Create "Point at" matrix for camera
		vec3d vUp = { 0, 1, 0 };
		vec3d vTarget = { 0, 0, 1 };
		mat4x4 matCameraRot = Matrix_MakeRotationY(fYaw);
		vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);
		vTarget = Vector_Add(vCamera, vLookDir);
		mat4x4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);

		// Make view matrix from camera
		mat4x4 matView = Matrix_QuickInverse(matCamera);

		// Store triangles for rastering later
		vector<triangle> vecTrianglesToRaster;

		// DRAW TRIANGLES
		for (auto tri : meshCube.tris)
		{
			triangle triProjected, triTransformed, triViewed;

			// World Matrix Transform
			triTransformed.pos[0] = Matrix_MultiplyVector(matWorld, tri.pos[0]);
			triTransformed.pos[1] = Matrix_MultiplyVector(matWorld, tri.pos[1]);
			triTransformed.pos[2] = Matrix_MultiplyVector(matWorld, tri.pos[2]);

			// Calculate triangle Normal
			vec3d normal, line1, line2;

			// Get lines either side of triangle
			line1 = Vector_Sub(triTransformed.pos[1], triTransformed.pos[0]);
			line2 = Vector_Sub(triTransformed.pos[2], triTransformed.pos[0]);

			// Take cross product of lines to get normal to triangle surface
			normal = Vector_CrossProduct(line1, line2);

			// You normally need to normalise a normal!
			normal = Vector_Normalise(normal);

			// Get Ray from triangle to camera
			vec3d vCameraRay = Vector_Sub(triTransformed.pos[0], vCamera);

			// If ray is aligned with normal, then triangle is visible
			if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
			{
				// Illumination
				vec3d light_direction = { 0.0f, 1.0f, -1.0f };
				light_direction = Vector_Normalise(light_direction);

				// How "aligned" are light direction and triangle surface normal?
				float dp = max(0.1f, Vector_DotProduct(light_direction, normal));

				// Get and set colors
				color pos = get_color(dp);
				triTransformed.color = pos;

				// Convert world space --> view space
				triViewed.pos[0] = Matrix_MultiplyVector(matView, triTransformed.pos[0]);
				triViewed.pos[1] = Matrix_MultiplyVector(matView, triTransformed.pos[1]);
				triViewed.pos[2] = Matrix_MultiplyVector(matView, triTransformed.pos[2]);
				triViewed.color = triTransformed.color;

				// Clip Viewed Triangle against near plane, this could form two additional
				// additional triangles.
				int nClippedTriangles = 0;
				triangle clipped[2];
				nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

				// We may end up with multiple triangles form the clip, so project as
				// required
				for (int n = 0; n < nClippedTriangles; n++)
				{

					// Project triangles from 3D --> 2D
					triProjected.pos[0] = Matrix_MultiplyVector(matProj, clipped[n].pos[0]);
					triProjected.pos[1] = Matrix_MultiplyVector(matProj, clipped[n].pos[1]);
					triProjected.pos[2] = Matrix_MultiplyVector(matProj, clipped[n].pos[2]);
					triProjected.color = clipped[n].color;

					// Normalise the projected triangles
					triProjected.pos[0] = Vector_Div(triProjected.pos[0], triProjected.pos[0].w);
					triProjected.pos[1] = Vector_Div(triProjected.pos[1], triProjected.pos[1].w);
					triProjected.pos[2] = Vector_Div(triProjected.pos[2], triProjected.pos[2].w);

					// X/Y are inverted so put them back (OLC PIXEL GE THING)
					triProjected.pos[0].x *= -1.0f;
					triProjected.pos[1].x *= -1.0f;
					triProjected.pos[2].x *= -1.0f;
					//triProjected.pos[0].y *= -1.0f;
					//triProjected.pos[1].y *= -1.0f;  CHANGED THESE TO POSITIVE BECAUSE I THOUGHT IT 
					//triProjected.pos[2].y *= -1.0f;  -MADE IT UPSIDE DOWN DUE TO PROJECTION AND COORDINATE DIFFERENCES BETWEEN WIN32 API and OLCPIXELENGINE
					triProjected.pos[0].y *= 1.0f;
					triProjected.pos[1].y *= 1.0f;
					triProjected.pos[2].y *= 1.0f;

					// SCALE INTO VIEW (offset vertices into visible normalised space)
					vec3d vOffsetView = { 1,1,0 };

					triProjected.pos[0] = Vector_Add(triProjected.pos[0], vOffsetView);
					triProjected.pos[1] = Vector_Add(triProjected.pos[1], vOffsetView);
					triProjected.pos[2] = Vector_Add(triProjected.pos[2], vOffsetView);

					triProjected.pos[0].x *= 0.5f * (float)client_width;
					triProjected.pos[0].y *= 0.5f * (float)client_height;
					triProjected.pos[1].x *= 0.5f * (float)client_width;
					triProjected.pos[1].y *= 0.5f * (float)client_height;
					triProjected.pos[2].x *= 0.5f * (float)client_width;
					triProjected.pos[2].y *= 0.5f * (float)client_height;

					// Store triangle for sorting
					vecTrianglesToRaster.push_back(triProjected);
				}
			}
		}

		// Sort triangles from back to front
		sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
			{
				float z1 = (t1.pos[0].z + t1.pos[1].z + t1.pos[2].z) / 3.0f;
				float z2 = (t2.pos[0].z + t2.pos[1].z + t2.pos[2].z) / 3.0f;
				return z1 > z2;
			});

		for (auto& triToRaster : vecTrianglesToRaster)
		{
			// Clip triangles against all four screen edges, this could yield
			// a bunch of triangles, so create a queue that we traverse to
			//  ensure we only test new triangles generated against planes
			triangle clipped[2];
			list<triangle> listTriangles;

			// Add initial triangle
			listTriangles.push_back(triToRaster);
			int nNewTriangles = 1;

			for (int pos = 0; pos < 4; pos++)
			{
				int nTrisToAdd = 0;
				while (nNewTriangles > 0)
				{
					// Take triangle from front of queue
					triangle test = listTriangles.front();
					listTriangles.pop_front();
					nNewTriangles--;

					// Clip it against a plane. We only need to test each
					// subsequent plane, against subsequent new triangles
					// as all triangles after a plane clip are guaranteed
					// to lie on the inside of the plane. I like how this
					// comment is almost completely and utterly justified
					switch (pos)
					{
					case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)client_height - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)client_width - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					}

					// Clipping may yield a variable number of triangles, so
					// add these new ones to the back of the queue for subsequent
					// clipping against next planes
					for (int w = 0; w < nTrisToAdd; w++)
						listTriangles.push_back(clipped[w]);
				}
				nNewTriangles = listTriangles.size();
			}

			// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
			for (auto& t : listTriangles)
			{
				if (wireframe)
				{
					draw_triangle(t.pos[0].x, t.pos[0].y, t.pos[1].x, t.pos[1].y, t.pos[2].x, t.pos[2].y, t.color);
				}
				else {
					FillTriangle(t.pos[0].x, t.pos[0].y, t.pos[1].x, t.pos[1].y, t.pos[2].x, t.pos[2].y, t.color);
				}
				//DrawTriangle(t.pos[0].x, t.pos[0].y, t.pos[1].x, t.pos[1].y, t.pos[2].x, t.pos[2].y, olc::BLACK);
			}
		}

        StretchDIBits(hdc,
            0,
            0,
            client_width,
            client_height,
            0,
            0,
            client_width,
            client_height,
            memory,
            &bitmap_info,
            DIB_RGB_COLORS,
            SRCCOPY
        );
    }

    return 0;
}

uint32_t rgb_to_hex(color c)
{
    return (c.rgb[0] << 16) + (c.rgb[1] << 8) + (c.rgb[2]);
}

//template <class T> void swap(T& a, T& b)
//{
//    T c(a); a = b; b = c;
//}

void clear_screen(uint32_t color)
{
    uint32_t* pixel = (uint32_t*)memory;
    for (int i = 0; i < client_width * client_height;  ++i)
    {
        *pixel++ = color;
    }
}

color get_color(float lum)
{
	// Lil bit of leo code
	// So like, its rlly simple, 13 color options, 255 / 13 = 19.6153846154
	color color;
	int pixel_bw = (int)(13.0f * lum);
	float c = pixel_bw * 19.6153846154;

	color = { (int)c, (int)c, (int)c };

	return color;
}

void draw_pixel(int x, int y, uint32_t color)
{
    uint32_t* pixel = (uint32_t*)memory;
    pixel += y * client_width + x;
    *pixel = color;
}

void draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color, uint32_t pattern)
{
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = x2 - x1; dy = y2 - y1;

    auto rol = [&](void) { pattern = (pattern << 1) | (pattern >> 31); return pattern & 1; };

    vi2d p1, p2;
    p1.x = x1;
    p1.y = y1;
    p2.x = x2;
    p2.y = y2;
    //if (!ClipLineToScreen(p1, p2))
    //	return;
    x1 = p1.x; y1 = p1.y;
    x2 = p2.x; y2 = p2.y;

    // straight lines idea by gurkanctn
    if (dx == 0) // Line is vertical
    {
        if (y2 < y1) std::swap(y1, y2);
        for (y = y1; y <= y2; y++) if (rol()) draw_pixel(x1, y, color);
        return;
    }

    if (dy == 0) // Line is horizontal
    {
        if (x2 < x1) std::swap(x1, x2);
        for (x = x1; x <= x2; x++) if (rol()) draw_pixel(x, y1, color);
        return;
    }

    // Line is Funk-aye
    dx1 = abs(dx); dy1 = abs(dy);
    px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
    if (dy1 <= dx1)
    {
        if (dx >= 0)
        {
            x = x1; y = y1; xe = x2;
        }
        else
        {
            x = x2; y = y2; xe = x1;
        }

        if (rol()) draw_pixel(x, y, color);

        for (i = 0; x < xe; i++)
        {
            x = x + 1;
            if (px < 0)
                px = px + 2 * dy1;
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
                px = px + 2 * (dy1 - dx1);
            }
            if (rol()) draw_pixel(x, y, color);
        }
    }
    else
    {
        if (dy >= 0)
        {
            x = x1; y = y1; ye = y2;
        }
        else
        {
            x = x2; y = y2; ye = y1;
        }

        if (rol()) draw_pixel(x, y, color);

        for (i = 0; y < ye; i++)
        {
            y = y + 1;
            if (py <= 0)
                py = py + 2 * dx1;
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
                py = py + 2 * (dx1 - dy1);
            }
            if (rol()) draw_pixel(x, y, color);
        }
    }
}

void draw_triangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color)
{
    draw_line(x1, y1, x2, y2, color, 0xFFFFFFFF);
    draw_line(x2, y2, x3, y3, color, 0xFFFFFFFF);
    draw_line(x3, y3, x1, y1, color, 0xFFFFFFFF);
}

void draw_triangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, color color)
{
    uint32_t c = rgb_to_hex(color);

    draw_line(x1, y1, x2, y2, c, 0xFFFFFFFF);
    draw_line(x2, y2, x3, y3, c, 0xFFFFFFFF);
    draw_line(x3, y3, x1, y1, c, 0xFFFFFFFF);
}

void FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, color color)
{
	uint32_t c = rgb_to_hex(color);

	FillTriangle(x1, y1, x2, y2, x3, y3, c);
}

// https://www.avrfreaks.net/sites/default/files/triangles.c
void FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t c)
{
	auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) draw_pixel(i, ny, c); };

	int t1x, t2x, y, minx, maxx, t1xp, t2xp;
	bool changed1 = false;
	bool changed2 = false;
	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;
	// Sort vertices
	if (y1 > y2) { std::swap(y1, y2); std::swap(x1, x2); }
	if (y1 > y3) { std::swap(y1, y3); std::swap(x1, x3); }
	if (y2 > y3) { std::swap(y2, y3); std::swap(x2, x3); }

	t1x = t2x = x1; y = y1;   // Starting points
	dx1 = (int)(x2 - x1);
	if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y2 - y1);

	dx2 = (int)(x3 - x1);
	if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
	else signx2 = 1;
	dy2 = (int)(y3 - y1);

	if (dy1 > dx1) { std::swap(dx1, dy1); changed1 = true; }
	if (dy2 > dx2) { std::swap(dy2, dx2); changed2 = true; }

	e2 = (int)(dx2 >> 1);
	// Flat top, just process the second half
	if (y1 == y2) goto next;
	e1 = (int)(dx1 >> 1);

	for (int i = 0; i < dx1;) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			i++;
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) t1xp = signx1;//t1x += signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
		// Move line
	next1:
		// process second line until y value is about to change
		while (1) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;//t2x += signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next2:
		if (minx > t1x) minx = t1x;
		if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x;
		if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);    // Draw line from min to max points found on the y
									// Now increase y
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y == y2) break;
	}
next:
	// Second half
	dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y3 - y2);
	t1x = x2;

	if (dy1 > dx1) {   // swap values
		std::swap(dy1, dx1);
		changed1 = true;
	}
	else changed1 = false;

	e1 = (int)(dx1 >> 1);

	for (int i = 0; i <= dx1; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) { t1xp = signx1; break; }//t1x += signx1;
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx1;
			if (i < dx1) i++;
		}
	next3:
		// process second line until y value is about to change
		while (t2x != x3) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next4:

		if (minx > t1x) minx = t1x;
		if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x;
		if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y > y3) return;
	}
}

//#ifndef UNICODE
//#define UNICODE
//#endif 
//
//#include <windows.h>
//
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//
//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
//{
//
//    // Register the window class.
//    const wchar_t CLASS_NAME[]  = L"Sample Window Class";
//    
//    WNDCLASS wc = { };
//
//    wc.lpfnWndProc   = WindowProc;
//    wc.hInstance     = hInstance;
//    wc.lpszClassName = CLASS_NAME;
//    
//    RegisterClass(&wc);
//
//    // Create the window.
//
//    HWND hwnd = CreateWindowEx(
//        0,                              // Optional window styles.
//        CLASS_NAME,                     // Window class
//        L"Learn to Program Windows",    // Window text
//        WS_OVERLAPPEDWINDOW,            // Window style
//
//        // Size and position
//        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
//
//        NULL,       // Parent window    
//        NULL,       // Menu
//        hInstance,  // Instance handle
//        NULL        // Additional application data
//        );
//
//    if (hwnd == NULL)
//    {
//        return 0;
//    }
//
//    ShowWindow(hwnd, nCmdShow);
//
//    // Run the message loop.
//    MSG msg = { };
//    while (GetMessage(&msg, NULL, 0, 0))
//    {
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//
//    return 0;
//}
//
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    switch (uMsg)
//    {
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//
//    case WM_PAINT:
//        {
//            PAINTSTRUCT ps;
//            HDC hdc = BeginPaint(hwnd, &ps);
//
//            // All painting occurs here, between BeginPaint and EndPaint.
//            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
//            // DO STUFF
//            Rectangle(hdc, 100, 100, 150, 150);
//            EndPaint(hwnd, &ps);
//        }
//        return 0;
//    }
//
//    return DefWindowProc(hwnd, uMsg, wParam, lParam);
//}
//struct Obj {
//		std::vector<GLfloat> vtx;
//		std::vector<GLfloat> normal;
//		std::vector<GLfloat> uv;
//	};
//
//	// OBJ
//	Obj loadObj(const std::string& path) {
//		std::ifstream stream(path);
//		assert(stream);
//
//		std::vector<GLfloat> normal;
//		std::vector<GLfloat> uv;
//		std::vector<GLfloat> vtx;
//
//		Obj obj;
//
//		while (!stream.eof()) {
//			std::string s;
//			std::getline(stream, s);
//			if (s == "# normals") {
//				std::cout << "normals" << std::endl;
//				while (!stream.eof()) {
//					std::string s;
//					std::getline(stream, s);
//
//					// TIPS:
//					std::stringstream ss;
//					ss << s;
//
//					std::string key;
//					ss >> key;
//					if (key != "vn") break;
//
//					float x, y, z;
//					ss >> x >> y >> z;
//					normal.push_back(x);
//					normal.push_back(y);
//					normal.push_back(z);
//				}
//				std::cout << normal.size() / 3 << std::endl;
//			}
//			else if (s == "# texcoords") {
//				std::cout << "texcoords" << std::endl;
//				while (!stream.eof()) {
//					std::string s;
//					std::getline(stream, s);
//
//					std::stringstream ss;
//					ss << s;
//
//					std::string key;
//					ss >> key;
//					if (key != "vt") break;
//
//					float x, y;
//					ss >> x >> y;
//					uv.push_back(x);
//					uv.push_back(y);
//				}
//				std::cout << uv.size() / 2 << std::endl;
//			}
//			else if (s == "# verts") {
//				std::cout << "verts" << std::endl;
//				while (!stream.eof()) {
//					std::string s;
//					std::getline(stream, s);
//
//					std::stringstream ss;
//					ss << s;
//
//					std::string key;
//					ss >> key;
//					if (key != "v") break;
//
//					float x, y, z;
//					ss >> x >> y >> z;
//					vtx.push_back(x);
//					vtx.push_back(y);
//					vtx.push_back(z);
//				}
//				std::cout << vtx.size() / 3 << std::endl;
//			}
//			else if (s == "# faces") {
//				std::cout << "faces" << std::endl;
//				while (!stream.eof()) {
//					std::string s;
//					std::getline(stream, s);
//
//					std::stringstream ss;
//					ss << s;
//
//					std::string key;
//					ss >> key;
//					if (key != "f") break;
//
//					// FIXME:
//					int vi[3];
//					int ti[3];
//					int ni[3];
//					char separate;
//
//					ss >> vi[0] >> separate >> ti[0] >> separate >> ni[0];
//					ss >> vi[1] >> separate >> ti[1] >> separate >> ni[1];
//					ss >> vi[2] >> separate >> ti[2] >> separate >> ni[2];
//
//					// UV
//					for (int i = 0; i < 3; ++i) {
//						obj.vtx.push_back(vtx[(vi[i] - 1) * 3 + 0]);
//						obj.vtx.push_back(vtx[(vi[i] - 1) * 3 + 1]);
//						obj.vtx.push_back(vtx[(vi[i] - 1) * 3 + 2]);
//
//						obj.uv.push_back(uv[(ti[i] - 1) * 2 + 0]);
//						obj.uv.push_back(uv[(ti[i] - 1) * 2 + 1]);
//
//						obj.normal.push_back(normal[(ni[i] - 1) * 3 + 0]);
//						obj.normal.push_back(normal[(ni[i] - 1) * 3 + 1]);
//						obj.normal.push_back(normal[(ni[i] - 1) * 3 + 2]);
//					}
//				}
//			}
//		}
//
//		return obj;
//	}

