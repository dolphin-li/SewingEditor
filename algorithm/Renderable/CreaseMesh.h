#pragma once

#include "ldp_basic_mat.h"
#include "Camera.h"
class CreaseMesh
{
public:
	CreaseMesh();
	~CreaseMesh();

	bool load(const char* objName);
	bool save(const char* objName)const;
	void clear();

	// return global coordinates...
	const ldp::Float3* verticesPtr()const{ return m_vertices.data(); }
	ldp::Float3 getKeyPoint(int i)const{ return m_vertices[m_keyPointsId[i]]; }
	const ldp::Float3* faceNormalPtr()const{ return m_faceNormals.data(); }

	const ldp::Int3* trianglesPtr()const{ return m_triangles.data(); }
	const ldp::Int2* creaseEdgePtr()const{ return m_creaseEdges.data(); }
	const int* getKeyPointsIdPtr()const{ return m_keyPointsId.data(); }
	int numVertices()const{ return (int)m_vertices.size(); }
	int numTriangles()const{ return (int)m_triangles.size(); }
	int numCreaseEdges()const{ return (int)m_creaseEdges.size(); }
	int numKeyPoints()const{ return m_keyPointsId.size(); }

	ldp::Float3 convert2Local(ldp::Float3 p)const;
	ldp::Float3 convert2Global(ldp::Float3 p)const;

	// assume R is applied on local, we return the resulted global transform
	ldp::Mat4f getTransformAppliedOnLocal(ldp::Mat3f R)const;

	ldp::Float3 getCenter()const{ return m_center; }
	const ldp::Mat3f& getLocal2GlobalR()const{ return m_local2globalR; }
	const ldp::Mat3f& getGlobal2LocalR()const{ return m_global2localR; }
	void setLocal2GlobalR(ldp::Mat3f R);

	const ldp::Mat4f& getCameraProjection()const{ return m_camProj; }
	void setCameraProjection(ldp::Mat4f P){ m_camProj = P; }

	const std::string& getName()const{ return m_name; }
	void setName(std::string name){ m_name = name; }

	// the class itself does not use this variable inside
	// it is simiply for storage and external usage.
	// rotations applied on local coordinates
	// v <- local_to_global * rotationOnLocal * global_to_local * v;
	ldp::Mat3f getRotationOnLocal()const{ return m_rotationOnLocal; }
	void setRotationOnLocal(ldp::Mat3f R)const{ m_rotationOnLocal = R; }

	// calculate the smoothed theta direction under given camera
	void calcSmoothedCreaseDirections(std::vector<float>& thetas, const Camera& cam)const;

	bool loadObj(const char* filename);
	bool saveObj(const char* filename)const;
	bool loadFrame(const char* filename);
	bool saveFrame(const char* filename)const;
	bool loadProjVertices(const char* filename);
	bool saveProjVertices(const char* filename)const;

	bool isVertSelected(int i)const{ return !!m_isVertSelected[i]; }

	void selectByBox(ldp::Float2 bMin, ldp::Float2 bMax, const Camera& cam);
	std::shared_ptr<CreaseMesh> separateByBoxSelected();

	void updateFrameByApplyingCam(const Camera& cam);

	// applying R, T on local coordinates
	void updateFrameByApplyingLocalRT(const ldp::Mat3f& R, ldp::Float3 t);

	void addProjVertices(int vid, ldp::Float2 scrPos);
	void addProjVertices_Idx(int vid);
	void addProjVertices_Pos(ldp::Float2 scrPos);
	void clearProjVertices();
	int getProjVertices_Idx(int i){ return m_projPtsIds[i]; }
	ldp::Float2 getProjVertices_Pos(int i){ return m_projPtsPos[i]; }
	int numProjVertices_Idx()const{ return (int)m_projPtsIds.size(); }
	int numProjVertices_Pos()const{ return (int)m_projPtsPos.size(); }
protected:
	bool loadObj(FILE* pFile);
	bool saveObj(FILE* pFile)const;
	bool loadFrame(FILE* pFile);
	bool saveFrame(FILE* pFile)const;
	bool loadProjVertices(FILE* pFile);
	bool saveProjVertices(FILE* pFile)const;
	void updateFaceNormals();
	void updateCreaseEdges();
	ldp::Float3 calcCenter()const;
private:
	std::vector<ldp::Float3> m_vertices; // in global coordinates
	std::vector<ldp::Int3> m_triangles;
	std::vector<ldp::Int2> m_creaseEdges;
	std::vector<ldp::Float3> m_faceNormals;
	std::vector<int> m_keyPointsId;
	std::vector<int> m_isVertexKeyPoint;
	std::vector<int> m_isVertSelected;
	std::string m_name;
	ldp::Float3 m_center;
	ldp::Mat3f m_local2globalR;
	ldp::Mat3f m_global2localR;
	ldp::Mat4f m_camProj;

	// the class itself does not use this variable inside
	// it is simiply for storage and external usage.
	// rotations applied on local coordinates
	// v <- local_to_global * rotationOnLocal * global_to_local * v;
	mutable ldp::Mat3f m_rotationOnLocal;

	// for projection constraints: some mesh vertices must be projected to some screen position
	std::vector<int> m_projPtsIds;
	std::vector<ldp::Float2> m_projPtsPos;
};
