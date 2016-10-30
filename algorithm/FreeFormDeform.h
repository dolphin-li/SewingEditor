#pragma once

#include "ldpMat\ldp_basic_mat.h"
#include "util.h"
namespace svg
{
	class SvgPolyPath;
	class SvgManager;
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
	std::shared_ptr<FreeFormDeform> clone(const svg::SvgManager* manager)const;

	// return the idx of the added control points, -1 means not added
	int addControlPoint(ldp::Float2 pos);
	void clearControlPoints();
	void resetControlPoints();
	void removeControlPoint(int i);
	void setControlPointTargetPos(int i, ldp::Float2 p);
	int numControlPoints()const { return m_controlPoints.size(); }
	const ControlPoint& getControlPoint(int i)const { return m_controlPoints[i]; }
	ldp::Float2 calcPosition(const ControlPoint&  cp);
protected:
	// given a position p, find the quad contains it
	int findQuadIdx(ldp::Float2 p)const;

	void solve();

	void setupSimilarityMatrix();
	void setupControlMatrix();
	void setupLineMatrix();

	void buildPolyQuads();
	void updatePoly();

	void buildLineSegments();
	void cutLineIntoQuadSegs(const ControlPoint& cp_s, const ControlPoint& cp_e, std::vector<ControlPoint>& segs);
private:
	std::vector<svg::SvgPolyPath*> m_polys;
	std::vector<std::vector<ControlPoint>> m_polys_coords;
	std::vector<ControlPoint> m_lineSegsInQuads; // for line preserving
	std::vector<int> m_lineBinIdx; // for line preserving
	std::vector<ControlPoint> m_controlPoints;
	std::vector<ldp::Float2> m_points;
	std::vector<ldp::Float2> m_points_init;
	std::vector<ldp::Int4> m_quads;
	int m_nX;
	int m_nY;
	float m_X0;
	float m_Y0;
	float m_step;
	std::vector<float> m_thetas; // for line preserving
	ldp::SpMat m_AtA_similarity;
	ldp::Vec m_Atb_similarity;
	ldp::SpMat m_AtA_control;
	ldp::Vec m_Atb_control;
	ldp::SpMat m_AtA_line;
	ldp::SpSolver m_solver;
};
