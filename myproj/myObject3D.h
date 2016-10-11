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
	{

		glGenBuffers(4, buffers);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*12, &vertices.front(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 12, &indices.front(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * 12, &normals.front(), GL_STATIC_DRAW);

	}

	//draw the mesh
	void displayObject(GLuint shaderprogram, glm::mat4 viewmatrix)
	{

		glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "mymodel_matrix"), 1, GL_FALSE, &model_matrix[0][0]);

		glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(viewmatrix*model_matrix)));
		glUniformMatrix3fv(glGetUniformLocation(shaderprogram, "mynormal_matrix"), 1, GL_FALSE, &normal_matrix[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[VERTEX_BUFFER]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[NORMAL_BUFFER]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDEX_BUFFER]);
		glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0);
		 
	}

	//draw each vertices' normals
	void displayNormals()
	{
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[NORMAL_BUFFER]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawElements(GL_LINES, normals.size(), GL_UNSIGNED_INT, 0);
	}

	void translate(double x, double y, double z)
	{
		glm::mat4 tmp = glm::mat4(1.0f);
		tmp = glm::translate(tmp, glm::vec3(x, y, z));
		model_matrix = tmp * model_matrix;
	}
	void rotate(double axis_x, double axis_y, double axis_z, double angle)
	{
		glm::mat4 tmp = glm::mat4(1.0f);
		tmp = glm::rotate(tmp, (float)angle, glm::vec3(axis_x, axis_y, axis_z));
		model_matrix = tmp * model_matrix;
	}
};