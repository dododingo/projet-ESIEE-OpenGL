#include <math.h>
#include <GL/glew.h>
#include <vector>
#include <string>
#include <fstream>
#define PI 3.14159265

using namespace std;

class myObject3D
{
public:
	GLuint buffers[6];
	GLuint CubeBuffers[2];
	enum { VERTEX_BUFFER = 0, INDEX_BUFFER, NORMAL_BUFFER };

	//vertices of the object
	std::vector<glm::vec3> vertices;
	//indicies of the triplets of vertices forming faces
	std::vector<glm::ivec3> indices;
	//vertices' normals
	std::vector<glm::vec3> normals;

	//Model matrix (position of the mesh in the scene)
	glm::mat4 model_matrix;

	myObject3D() {
		model_matrix = glm::mat4(1.0f);
		clear();
	}

	~myObject3D()
	{
		clear();
	}

	//empty the object
	void clear() {		
		vertices.clear();
		indices.clear();
		normals.clear();
		glDeleteBuffers(6, buffers);
	}

	//normalize each vertices
	void normalize()
	{
		unsigned int tmpxmin = 0, tmpymin = 0, tmpzmin = 0, tmpxmax = 0, tmpymax = 0, tmpzmax = 0;

		for (unsigned i = 0; i<vertices.size(); i++) {
			if (vertices[i].x < vertices[tmpxmin].x) tmpxmin = i;
			if (vertices[i].x > vertices[tmpxmax].x) tmpxmax = i;

			if (vertices[i].y < vertices[tmpymin].y) tmpymin = i;
			if (vertices[i].y > vertices[tmpymax].y) tmpymax = i;

			if (vertices[i].z < vertices[tmpzmin].z) tmpzmin = i;
			if (vertices[i].z > vertices[tmpzmax].z) tmpzmax = i;
		}

		float xmin = vertices[tmpxmin].x, xmax = vertices[tmpxmax].x,
			   ymin = vertices[tmpymin].y, ymax = vertices[tmpymax].y,
			   zmin = vertices[tmpzmin].z, zmax = vertices[tmpzmax].z;

		float scale = (xmax - xmin) <= (ymax - ymin) ? (xmax - xmin) : (ymax - ymin);
		scale = scale >= (zmax - zmin) ? scale : (zmax - zmin);

		for (unsigned int i = 0; i<vertices.size(); i++) {
			vertices[i].x -= (xmax + xmin) / 2;
			vertices[i].y -= (ymax + ymin) / 2;
			vertices[i].z -= (zmax + zmin) / 2;

			vertices[i].x /= scale;
			vertices[i].y /= scale;
			vertices[i].z /= scale;
		}
	}

	//open a mesh from the specified file
	bool readMesh(string filename)
	{
		clear();
		string s, t;
		float x, y, z;
		int index1, index2, index3;

		ifstream fin(filename);
		if (!fin.is_open())
		{
			cout << "Error: unable to open file in readMesh().\n";
			return false;
		}

		while (getline(fin, s))
		{
			stringstream myline(s);
			myline >> t;
			if (t == "v")
			{
				myline >> x;  myline >> y; myline >> z;
				vertices.push_back(glm::vec3(x, y, z));
			}
			else if (t == "f")
			{
				myline >> t; index1 = atoi((t.substr(0, t.find("/"))).c_str());
				myline >> t; index2 = atoi((t.substr(0, t.find("/"))).c_str());
				while (myline >> t)
				{
					index3 = atoi((t.substr(0, t.find("/"))).c_str());
					indices.push_back(glm::ivec3(index1-1, index2-1, index3-1));
					index2 = index3;
				}
			}
		}
		return true;
	}
 
	//calculate normals for each vertices from surfaces' normals
	void computeNormals()
	{
		normals.assign(vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));
		for (unsigned int i = 0; i<indices.size(); i++)
		{
			glm::vec3 face_normal = glm::cross( vertices[indices[i][1]] - vertices[indices[i][0]], vertices[indices[i][2]] - vertices[indices[i][1]] );
			normals[indices[i][0]] += face_normal;
			normals[indices[i][1]] += face_normal;
			normals[indices[i][2]] += face_normal;
		}
		for (unsigned int i = 0; i < vertices.size(); i++)  normals[i] = glm::normalize(normals[i]);
	}

	//create object buffers to store shaders
	void createObjectBuffers()
	{		float vertices[3][2] = { {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f} };		glGenBuffers(4, buffers);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	}

	//draw the mesh
	void displayObject(int render_method = 1)
	{
		if (render_method == 0) {
			GLfloat CubeArray[48] = {
				1.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f,
				1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f,
				1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
				0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 1.0f,
				0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
				0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f,
				1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f
			};

			GLuint IndiceArray[36] = {
				0,1,2,2,1,3,
				4,5,6,6,5,7,
				3,1,5,5,1,7,
				0,2,6,6,2,4,
				6,7,0,0,7,1,
				2,3,4,4,3,5
			};

			// Génération des buffers
			glGenBuffers(2, CubeBuffers);

			// Buffer d'informations de vertex
			glBindBuffer(GL_ARRAY_BUFFER, CubeBuffers[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(CubeArray), CubeArray, GL_STATIC_DRAW);

			// Buffer d'indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeBuffers[1]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(IndiceArray), IndiceArray, GL_STATIC_DRAW);


			// Utilisation des données des buffers
			glBindBuffer(GL_ARRAY_BUFFER, CubeBuffers[0]);
			glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), ((float*)NULL + (3)));
			glColorPointer(3, GL_FLOAT, 6 * sizeof(float), 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeBuffers[1]);

			// Activation d'utilisation des tableaux
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			// Rendu de notre géométrie
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
		}

		if (render_method == 1)
		{
			glBegin(GL_TRIANGLES);
			for (unsigned int i = 0; i < indices.size(); i++)
			{
				glVertex3f(vertices[indices[i][0]].x, vertices[indices[i][0]].y, vertices[indices[i][0]].z);
				glVertex3f(vertices[indices[i][1]].x, vertices[indices[i][1]].y, vertices[indices[i][1]].z);
				glVertex3f(vertices[indices[i][2]].x, vertices[indices[i][2]].y, vertices[indices[i][2]].z);
			}
			glEnd();
		}
		 
	}

	//draw each vertices' normals
	void displayNormals()
	{
		glBegin(GL_LINES);
		for (unsigned int i = 0; i<vertices.size(); i++)
		{
			glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
			glVertex3f(vertices[i].x + normals[i].x / 10.0f, vertices[i].y + normals[i].y / 10.0f, vertices[i].z + normals[i].z / 10.0f);
		}
		glEnd();
	}
};