#pragma once

#include "ldpMat\ldp_basic_mat.h"
#include "util.h"
namespace svg
{
	class SvgPolyPath;
}
class FreeFormDeform
{
public:
	enum
	{
		INDEX_BEGIN = 10000000
	};
	struct ControlPoint
	{
		int quad_id;
		ldp::Float4 quad_w;
		ldp::Float2 tarPos;
		ControlPoint()
		{
			quad_id = -1;
		}
	};
public:
	FreeFormDeform();
	~FreeFormDeform();

	// bbox: left, right, top, bottom
	void init(const std::vector<svg::SvgPolyPath*>& polys, int numQuadsProxy = 400);
	void clear();
	std::shared_ptr<FreeFormDeform> clone()const;

	int numQuads()const { return m_quads.size(); }
	int numPoints()const { return m_points.size(); }
	const ldp::Int4& getQuad(int i)const { return m_quads[i]; }
	const ldp::Float2& getPoint(int i)const { return m_points[i]; }
	const ldp::Float2& getPointInit(int i)const { return m_points_init[i]; }

	// return the idx of the added control points, -1 means not added
	int addControlPoint(ldp::Float2 pos);
	void clearControlPoints();
	void resetControlPoints();
	void removeControlPoint(int i);
	void setControlPointTargetPos(int i, ldp::Float2 p);
	int numControlPoints()const { return m_controlPoints.size(); }
	const ControlPoint& getControlPoint(int i)const { return m_controlPoints[i]; }
	ldp::Float2 calcPosition(const ControlPoint&  cp);

	// given a position p, find the quad contains it
	int findQuadIdx(ldp::Float2 p);
protected:
	void factor();
	void solve();

	void setupSimilarityMatrix();
	void setupControlMatrix();

	void buildPolyQuads();
	void updatePoly();
private:
	std::vector<svg::SvgPolyPath*> m_polys;
	std::vector<std::vector<ControlPoint>> m_polys_coords;
	std::vector<ControlPoint> m_controlPoints;
	std::vector<ldp::Float2> m_points;
	std::vector<ldp::Float2> m_points_init;
	std::vector<ldp::Int4> m_quads;
	int m_nX;
	int m_nY;
	ldp::SpMat m_AtA_similarity;
	ldp::Vec m_Atb_similarity;
	ldp::SpMat m_AtA_control;
	ldp::Vec m_Atb_control;
	ldp::SpSolver m_solver;
};
