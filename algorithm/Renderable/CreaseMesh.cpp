#include "CreaseMesh.h"
#include "bmesh.h"
#include "util.h"
#include <set>
#include "OpenMeshWrapper.h"
#include <queue>

using namespace ldp;

#undef min
#undef max

#define WHITESPACE " \t\n\r"
#define OBJ_LINE_SIZE 500

const static float CREASE_EDGE_THRE = ldp::PI_S / 6.f;
const static float KEY_PTS_ANGLE_THRE = ldp::PI_S / 6.f;

CreaseMesh::CreaseMesh()
{
	m_name = "default";
}

CreaseMesh::~CreaseMesh()
{
}

void CreaseMesh::clear()
{
	m_vertices.clear();
	m_faceNormals.clear();
	m_triangles.clear();
	m_creaseEdges.clear();
	m_keyPointsId.clear();
	m_isVertexKeyPoint.clear();
	m_isVertSelected.clear();
	m_local2globalR.eye();
	m_global2localR.eye();
	m_camProj.eye();
	m_rotationOnLocal.eye();
	m_projPtsIds.clear();
	m_projPtsPos.clear();
}

void CreaseMesh::updateFaceNormals()
{
	m_faceNormals.resize(m_triangles.size());
	for (size_t iFace = 0; iFace < m_triangles.size(); iFace++)
	{
		const ldp::Int3& f = m_triangles[iFace];
		ldp::Float3 fn = ldp::Float3(m_vertices[f[1]] - m_vertices[f[0]])
			.cross(m_vertices[f[2]] - m_vertices[f[0]]);
		if (fn.length())
			fn.normalizeLocal();
		m_faceNormals[iFace] = fn;
	}
}

void CreaseMesh::updateCreaseEdges()
{
	// update crease edges and key points
	m_creaseEdges.clear();
	m_keyPointsId.clear();
	m_isVertexKeyPoint.clear();
	m_isVertexKeyPoint.resize(m_vertices.size(), 0);

	std::set<ldp::Int2> creaseEdgeSet;
	ldp::BMesh bmesh;
	bmesh.init_triangles(numVertices(), (float*)verticesPtr(), numTriangles(), (int*)trianglesPtr());
	BMESH_ALL_EDGES(e, eiter, bmesh)
	{
		int cnt = 0;
		ldp::Float3 normals[2];
		BMESH_F_OF_E(f, e, fiter, bmesh)
		{
			normals[cnt++] = ldp::Float3(f->no[0], f->no[1], f->no[2]);
			if (cnt >= 2)
				break;
		}

		bool isCrease = false;
		if (cnt == 1)
			isCrease = true;
		else if (cnt == 2)
		{
			float theta = acos(std::min(1.f, std::max(0.f, normals[0].dot(normals[1]))));
			isCrease = (theta > CREASE_EDGE_THRE);
		}
		if (isCrease)
		{
			ldp::Int2 eg(bmesh.vofe_first(e)->getIndex(), bmesh.vofe_last(e)->getIndex());
			if (eg[0] > eg[1]) std::swap(eg[0], eg[1]);
			m_creaseEdges.push_back(eg);
			creaseEdgeSet.insert(eg);
		}
	}

	// key points: connected to more than 2 crease edges
	BMESH_ALL_VERTS(v, viter, bmesh)
	{
		int cnt = 0;
		BMESH_E_OF_V(e, v, eiter, bmesh)
		{
			ldp::Int2 eg(bmesh.vofe_first(e)->getIndex(), bmesh.vofe_last(e)->getIndex());
			if (eg[0] > eg[1]) std::swap(eg[0], eg[1]);
			if (creaseEdgeSet.find(eg) != creaseEdgeSet.end())
				cnt++;
		}

		if (cnt >= 3)
		{
			m_keyPointsId.push_back(v->getIndex());
			m_isVertexKeyPoint[v->getIndex()] = 1;
		}
	}
}

void CreaseMesh::calcSmoothedCreaseDirections(std::vector<float>& thetas, const ldp::Camera& cam)const
{
	thetas.resize(m_creaseEdges.size());
	std::vector<ldp::Mat2f> vertsTensorSum(m_creaseEdges.size(), ldp::Mat2f().zeros());

	ldp::Mat4f M = getTransformAppliedOnLocal(getRotationOnLocal());
	for (int iE = 0; iE < numCreaseEdges(); iE++)
	{
		ldp::Int2 eg = creaseEdgePtr()[iE];
		ldp::Float3 v0 = verticesPtr()[eg[0]];
		ldp::Float3 v1 = verticesPtr()[eg[1]];
		ldp::Float3 s0 = cam.getScreenCoords(M.getRotationPart()*v0 + M.getTranslationPart());
		ldp::Float3 s1 = cam.getScreenCoords(M.getRotationPart()*v1 + M.getTranslationPart());
		s1 -= s0;
		ldp::Float2 dir(s1[0], s1[1]);
		ldp::Mat2f m = ldp::vec2Tensor_noNormalize(dir);
		vertsTensorSum[eg[0]] += m;
		vertsTensorSum[eg[1]] += m;
	}

	for (int iE = 0; iE < numCreaseEdges(); iE++)
	{
		ldp::Int2 eg = creaseEdgePtr()[iE];
		ldp::Float3 v0 = verticesPtr()[eg[0]];
		ldp::Float3 v1 = verticesPtr()[eg[1]];
		ldp::Float3 s0 = cam.getScreenCoords(M.getRotationPart()*v0 + M.getTranslationPart());
		ldp::Float3 s1 = cam.getScreenCoords(M.getRotationPart()*v1 + M.getTranslationPart());
		s1 -= s0;

		float theta = 0;

		bool isKey0 = m_isVertexKeyPoint[eg[0]];
		bool isKey1 = m_isVertexKeyPoint[eg[1]];
		if (isKey0 || isKey1)
		{
			theta = atan2(s1[1], s1[0]);
			if (theta < 0) theta += ldp::PI_S;
		}
		else
		{
			ldp::Mat2f m = vertsTensorSum[eg[0]] + vertsTensorSum[eg[1]];
			ldp::Float2 d = ldp::tensor2Vec(m);
			theta = atan2(d[1], d[0]);
			if (theta < 0) theta += ldp::PI_S;
		}

		thetas[iE] = theta;
	}
}

ldp::Float3 CreaseMesh::calcCenter()const
{
	ldp::Float3 c = 0.f;
	for (int i = 0; i < numVertices(); i++)
		c += m_vertices[i];
	if (numVertices())
		c /= (float)numVertices();
	return c;
}

ldp::Float3 CreaseMesh::convert2Local(ldp::Float3 p)const
{
	return m_global2localR * (p - m_center);
}

ldp::Float3 CreaseMesh::convert2Global(ldp::Float3 p)const
{
	return m_local2globalR * p + m_center;
}

ldp::Mat4f CreaseMesh::getTransformAppliedOnLocal(ldp::Mat3f R)const
{
	R = m_local2globalR * R * m_global2localR;
	ldp::Float3 t = m_center - R * m_center;
	ldp::Mat4f M;
	M.setRotationPart(R);
	M.setTranslationPart(t);
	M(3, 3) = 1.f;
	return M;
}

void CreaseMesh::setLocal2GlobalR(ldp::Mat3f R)
{
	for (int i = 0; i < m_vertices.size(); i++)
		m_vertices[i] = convert2Local(m_vertices[i]);

	m_local2globalR = R;
	m_global2localR = R.inv();

	for (int i = 0; i < m_vertices.size(); i++)
		m_vertices[i] = convert2Global(m_vertices[i]);
}

bool CreaseMesh::load(const char* objName)
{
	clear();

	// obj 
	if (!loadObj(objName))
		return false;

	std::string path, name, ext;
	ldp::fileparts(objName, path, name, ext);
	setName(name);

	// frame
	std::string frameName = ldp::fullfile(path, name + ".frame");
	if (!loadFrame(frameName.c_str()))
		printf("warning: load frame failed: %s\n", frameName.c_str());

	// projVertices
	std::string projName = ldp::fullfile(path, name + ".proj");
	loadProjVertices(projName.c_str());

	// finally update
	m_center = calcCenter();
	m_isVertSelected.resize(m_vertices.size());
	std::fill(m_isVertSelected.begin(), m_isVertSelected.end(), 0);
	updateFaceNormals();
	updateCreaseEdges();

	return true;
}

bool CreaseMesh::save(const char* objName)const
{
	// obj 
	if (!saveObj(objName))
		return false;

	std::string path, name, ext;
	ldp::fileparts(objName, path, name, ext);

	// frame
	std::string frameName = ldp::fullfile(path, name + ".frame");
	if (!saveFrame(frameName.c_str()))
		printf("warning: load frame failed: %s\n", frameName.c_str());

	// proj
	std::string projName = ldp::fullfile(path, name + ".proj");
	saveProjVertices(projName.c_str());

	return true;
}

static void obj_parse_vertex_index(std::vector<int> &vertex_index)
{
	char *token = nullptr;
	vertex_index.clear();
	while ((token = strtok(NULL, WHITESPACE)) != NULL)
	{
		vertex_index.push_back(atoi(token) - 1);
	}
}

bool CreaseMesh::loadObj(FILE* pFile)
{
	m_vertices.clear();
	m_triangles.clear();

	//parser loop
	char current_line[OBJ_LINE_SIZE];
	char* current_token = nullptr;
	int line_number = 0;
	int group_id = -1;
	std::vector<std::vector<int>> polyFaces;
	while (fgets(current_line, OBJ_LINE_SIZE, pFile))
	{
		current_token = strtok(current_line, " \t\n\r");
		line_number++;

		//skip comments
		if (current_token == NULL || current_token[0] == '#')
			continue;

		//parse objects
		if (strcmp(current_token, "v") == 0) //process vertex
		{
			Float3 v;
			v[0] = (float)atof(strtok(NULL, WHITESPACE));
			v[1] = (float)atof(strtok(NULL, WHITESPACE));
			v[2] = (float)atof(strtok(NULL, WHITESPACE));
			m_vertices.push_back(v);
		}
		else if (strcmp(current_token, "f") == 0) //process face
		{
			if (group_id < 0)
				group_id++;
			polyFaces.push_back(std::vector<int>());
			obj_parse_vertex_index(polyFaces.back());
		}
		else if (strcmp(current_token, "g") == 0)
		{
			group_id++;
		}
	}// end parser

	// converting to triangles
	std::vector<ldp::Float3> tmpV;
	std::vector<ldp::Int3> tmpF;
	for (size_t iPoly = 0; iPoly < polyFaces.size(); iPoly++)
	{
		const std::vector<int>& poly = polyFaces[iPoly];
		if (poly.size() == 3)
		{
			m_triangles.push_back(ldp::Int3(poly[0], poly[1], poly[2]));
		}
		else if (poly.size() == 4)
		{
			m_triangles.push_back(ldp::Int3(poly[0], poly[1], poly[2]));
			m_triangles.push_back(ldp::Int3(poly[0], poly[2], poly[3]));
		}
		else
		{
			tmpV.resize(poly.size());
			for (size_t k = 0; k < poly.size(); k++)
				tmpV[k] = m_vertices[poly[k]];
			OpenMeshWrapper::triangulate_poly(tmpV, tmpF);
			for (size_t i = 0; i < tmpF.size(); i++)
			{
				ldp::Int3 t = tmpF[i];
				m_triangles.push_back(ldp::Int3(poly[t[0]], poly[t[1]], poly[t[2]]));
			}
		}
	}// iPoly

	return true;
}

bool CreaseMesh::saveObj(FILE* pFile)const
{
	const int nVerts = m_vertices.size();
	const int nTris = m_triangles.size();

	fprintf(pFile, "#auto-save: %d vertices, %d triangles\n", nVerts, nTris);
	fprintf(pFile, "g default\n");

	for (int i = 0; i < nVerts; i++)
	{
		ldp::Float3 v = m_vertices[i];
		fprintf(pFile, "v %f %f %f\n", v[0], v[1], v[2]);
	}

	for (int i = 0; i < nTris; i++)
	{
		ldp::Int3 f = m_triangles[i] + 1;
		fprintf(pFile, "f %d %d %d\n", f[0], f[1], f[2]);
	}

	return true;
}

bool CreaseMesh::saveObj(const char* filename)const
{
	FILE* pFile = fopen(filename, "w");
	if (!pFile)
		return false;
	bool ret = saveObj(pFile);
	fclose(pFile);
	return ret;
}

bool CreaseMesh::loadFrame(FILE* pFile)
{
	// camera projection
	ldp::Mat4d tmp;
	fread(tmp.ptr(), sizeof(double), 16, pFile);
	m_camProj = tmp;

	// center
	ldp::Double3 C, X, Y, Z;
	fread(&C, sizeof(ldp::Double3), 1, pFile);

	// three axes
	fread(&X, sizeof(ldp::Double3), 1, pFile);
	fread(&Y, sizeof(ldp::Double3), 1, pFile);
	fread(&Z, sizeof(ldp::Double3), 1, pFile);

	X.normalizeLocal();
	Y.normalizeLocal();
	Z.normalizeLocal();

	for (int k = 0; k < 3; k++)
	{
		m_local2globalR(k, 0) = X[k];
		m_local2globalR(k, 1) = Y[k];
		m_local2globalR(k, 2) = Z[k];
	}
	m_global2localR = m_local2globalR.inv();

	return true;
}

bool CreaseMesh::saveFrame(FILE* pFile)const
{
	// camera projection
	ldp::Mat4d tmp = m_camProj;
	fwrite(tmp.ptr(), sizeof(double), 16, pFile);

	// center
	ldp::Double3 C, X, Y, Z;
	C = m_center;
	for (int k = 0; k < 3; k++)
	{
		X[k] = m_local2globalR(k, 0);
		Y[k] = m_local2globalR(k, 1);
		Z[k] = m_local2globalR(k, 2);
	}
	fwrite(&C, sizeof(ldp::Double3), 1, pFile);
	fwrite(&X, sizeof(ldp::Double3), 1, pFile);
	fwrite(&Y, sizeof(ldp::Double3), 1, pFile);
	fwrite(&Z, sizeof(ldp::Double3), 1, pFile);
	return true;
}

bool CreaseMesh::saveProjVertices(FILE* pFile)const
{
	int n = numProjVertices_Idx();
	if (n != numProjVertices_Pos())
	{
		printf("error: number of idx and pos not matched: %d != %d\n",
			numProjVertices_Idx(), numProjVertices_Pos());
		return false;
	}
	fwrite(&n, sizeof(int), 1, pFile);

	fwrite(m_projPtsIds.data(), sizeof(int), n, pFile);
	fwrite(m_projPtsPos.data(), sizeof(ldp::Float2), n, pFile);

	return true;
}

bool CreaseMesh::loadProjVertices(FILE* pFile)
{
	clearProjVertices();
	int n = 0;
	fread(&n, sizeof(int), 1, pFile);
	m_projPtsIds.resize(n);
	m_projPtsPos.resize(n);
	fread(m_projPtsIds.data(), sizeof(int), n, pFile);
	fread(m_projPtsPos.data(), sizeof(ldp::Float2), n, pFile);
	return true;
}

bool CreaseMesh::loadObj(const char* filename)
{
	FILE* pFile = fopen(filename, "r");
	if (!pFile)
		return false;
	bool ret = loadObj(pFile);
	fclose(pFile);
	return ret;
}

bool CreaseMesh::loadFrame(const char* filename)
{
	FILE* pFile = fopen(filename, "rb");
	if (!pFile)
		return false;
	bool ret = loadFrame(pFile);
	fclose(pFile);
	return ret;
}

bool CreaseMesh::saveFrame(const char* filename)const
{
	FILE* pFile = fopen(filename, "wb");
	if (!pFile)
		return false;
	bool ret = saveFrame(pFile);
	fclose(pFile);
	return ret;
}

bool CreaseMesh::loadProjVertices(const char* filename)
{
	FILE* pFile = fopen(filename, "rb");
	if (!pFile)
		return false;
	bool ret = loadProjVertices(pFile);
	fclose(pFile);
	return ret;
}

bool CreaseMesh::saveProjVertices(const char* filename)const
{
	if (numProjVertices_Idx() != numProjVertices_Pos())
	{
		printf("error: number of idx and pos not matched: %d != %d\n",
			numProjVertices_Idx(), numProjVertices_Pos());
		return false;
	}
	FILE* pFile = fopen(filename, "wb");
	if (!pFile)
		return false;
	bool ret = saveProjVertices(pFile);
	fclose(pFile);
	return ret;
}

void CreaseMesh::selectByBox(ldp::Float2 bMin, ldp::Float2 bMax, const Camera& cam)
{
	//
	for (int i = 0; i<2; i++)
	{
		if (bMin[i] > bMax[i])
			std::swap(bMin[i], bMax[i]);
	}

	//
	ldp::BMesh bmesh;
	bmesh.init_triangles(numVertices(), (float*)verticesPtr(), numTriangles(), (int*)trianglesPtr());
	std::vector<BMVert*> bverts;
	BMESH_ALL_VERTS(v, viter, bmesh)
	{
		bverts.push_back(v);
	}

	//
	ldp::Mat4f M = getTransformAppliedOnLocal(m_rotationOnLocal);
	for (int iVert = 0; iVert < m_vertices.size(); iVert++)
	{
		ldp::Float3 p = M.getRotationPart() * m_vertices[iVert] + M.getTranslationPart();
		ldp::Float3 p2 = cam.getScreenCoords(p);
		if (p2[0] > bMin[0] && p2[0] <= bMax[0] && p2[1] > bMin[1] && p2[1] <= bMax[1])
		{
			bmesh.select_linked_verts(bverts[iVert], true, false);
		}
	}

	for (int iVert = 0; iVert < m_vertices.size(); iVert++)
	{
		m_isVertSelected[iVert] = bverts[iVert]->isSelect();
	}
}

std::shared_ptr<CreaseMesh> CreaseMesh::separateByBoxSelected()
{
	std::vector<int> vertMap(m_vertices.size());
	int nSelected = 1, nNonSelected = -1;
	for (int i = 0; i < m_isVertSelected.size(); i++)
	{
		if (m_isVertSelected[i])
			vertMap[i] = nSelected++;
		else
			vertMap[i] = nNonSelected--;
	}

	if (nSelected == 1)
		return std::shared_ptr<CreaseMesh>(nullptr);

	//
	std::shared_ptr<CreaseMesh> subMesh1(new CreaseMesh), subMesh2(new CreaseMesh);
	for (int i = 0; i < m_vertices.size(); i++)
	{
		if (vertMap[i] > 0)
			subMesh1->m_vertices.push_back(m_vertices[i]);
		else
			subMesh2->m_vertices.push_back(m_vertices[i]);
	}

	//
	for(int i=0; i<m_triangles.size(); i++)
	{
		ldp::Int3 f = m_triangles[i];
		ldp::Int3 tf(vertMap[f[0]], vertMap[f[1]], vertMap[f[2]]);
		if(tf[0] > 0)
			subMesh1->m_triangles.push_back(tf-1);
		else
			subMesh2->m_triangles.push_back(-1-tf);
	}

	subMesh1->m_camProj = m_camProj;
	subMesh1->m_name = m_name;
	subMesh1->m_rotationOnLocal = m_rotationOnLocal;
	subMesh1->m_local2globalR = m_local2globalR;
	subMesh1->m_global2localR = m_global2localR;
	subMesh1->m_center = subMesh1->calcCenter();
	subMesh1->updateFaceNormals();
	subMesh1->updateCreaseEdges();
	subMesh1->m_isVertSelected.resize(subMesh1->m_vertices.size(), 0);

	subMesh2->m_camProj = m_camProj;
	subMesh2->m_name = m_name;
	subMesh2->m_rotationOnLocal = m_rotationOnLocal;
	subMesh2->m_local2globalR = m_local2globalR;
	subMesh2->m_global2localR = m_global2localR;
	subMesh2->m_center = subMesh2->calcCenter();
	subMesh2->updateFaceNormals();
	subMesh2->updateCreaseEdges();
	subMesh2->m_isVertSelected.resize(subMesh2->m_vertices.size(), 0);

	*this = *subMesh2;

	return subMesh1;
}

void CreaseMesh::updateFrameByApplyingCam(const Camera& cam)
{
	m_camProj = cam.getProjectionMatrix();
	ldp::Mat4f M = cam.getModelViewMatrix() * getTransformAppliedOnLocal(m_rotationOnLocal);
	m_local2globalR = M.getRotationPart() * m_local2globalR;
	m_global2localR = m_local2globalR.inv();

	for(int i=0; i<m_vertices.size(); i++)
	{
		m_vertices[i] = M.getRotationPart() * m_vertices[i] + M.getTranslationPart();
	}
	m_center = calcCenter();
	updateFaceNormals();
}

void CreaseMesh::updateFrameByApplyingLocalRT(const ldp::Mat3f& R, ldp::Float3 t)
{
	for (int i = 0; i<m_vertices.size(); i++)
	{
		m_vertices[i] = m_local2globalR * (R * m_global2localR * (m_vertices[i] - m_center) + t) + m_center;
	}

	m_center += m_local2globalR * t;
	m_local2globalR = m_local2globalR * R;
	m_global2localR = m_local2globalR.inv();

	updateFaceNormals();
}

void CreaseMesh::addProjVertices(int vid, ldp::Float2 scrPos)
{
	m_projPtsIds.push_back(vid);
	m_projPtsPos.push_back(vid);
}

void CreaseMesh::addProjVertices_Idx(int vid)
{
	m_projPtsIds.push_back(vid);
}

void CreaseMesh::addProjVertices_Pos(ldp::Float2 scrPos)
{
	m_projPtsPos.push_back(scrPos);
}

void CreaseMesh::clearProjVertices()
{
	m_projPtsIds.clear();
	m_projPtsPos.clear();
}

#if 0
std::vector<std::shared_ptr<CreaseMesh>> CreaseMesh::separateByGroups()const
{
	int nGroups = 0;
	for (int i = 0; i < m_faceGroupIds.size(); i++)
		nGroups = std::max(nGroups, m_faceGroupIds[i] + 1);

	// partition vertex into groups
	std::vector<std::map<int, int>> vertGroupMaps(nGroups);
	for (int iFace = 0; iFace < m_triangles.size(); iFace++)
	{
		int gid = m_faceGroupIds[iFace];
		ldp::Int3 f = m_triangles[iFace];
		std::map<int, int>& vmap = vertGroupMaps[gid];
		for (int k = 0; k < 3; k++)
		if (vmap.find(f[0]) == vmap.end())
			vmap.insert(std::make_pair(f[0], (int)vmap.size()));
	}

	// save each group=================================================
	std::vector<std::shared_ptr<CreaseMesh>> subMeshes(nGroups);

	for (int iGroup = 0; iGroup < nGroups; iGroup++)
	{
		subMeshes[iGroup].reset(new CreaseMesh);
		CreaseMesh* mesh = subMeshes[iGroup].get();
		std::map<int, int>& vmap = vertGroupMaps[iGroup];

		mesh->m_camProj = m_camProj;
		mesh->m_local2globalR = m_local2globalR;
		mesh->m_global2localR = m_global2localR;
		mesh->m_rotationOnLocal = m_rotationOnLocal;
		mesh->m_name = m_name;
		if (iGroup > 0)
			mesh->m_name += std::string("-") + std::to_string(iGroup);

		// verts
		for (auto it = vmap.begin(); it != vmap.end(); ++it)
			mesh->m_vertices.push_back(m_vertices[it->first]);
	}// iGroup

	for (int iFace = 0; iFace < m_triangles.size(); iFace++)
	{
		ldp::Int3 f = m_triangles[iFace];
		int gid = m_faceGroupIds[iFace];
		std::map<int, int>& vmap = vertGroupMaps[gid];
		ldp::Int3 tf = -1;
		for (int k = 0; k < 3; k++)
		{
			auto it = vmap.find(f[k]);
			if (it != vmap.end())
				tf[k] = it->second;
			else
				break;
		}// k
		subMeshes[gid]->m_triangles.push_back(tf);
		subMeshes[gid]->m_faceGroupIds.push_back(0);
	}// iFace

	for (int iGroup = 0; iGroup < nGroups; iGroup++)
	{
		CreaseMesh* mesh = subMeshes[iGroup].get();
		mesh->m_center = calcCenter();
		mesh->updateFaceNormals();
		mesh->updateCreaseEdges();
	}// iGroup

	return subMeshes;
}
#endif