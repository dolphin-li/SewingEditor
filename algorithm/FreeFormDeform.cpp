#include "FreeFormDeform.h"
#include "SvgPolyPath.h"
#include "kdtree\PointTree.h"
FreeFormDeform::FreeFormDeform()
{
	m_nX = 0;
	m_nY = 0;
}

FreeFormDeform::~FreeFormDeform()
{

}

void FreeFormDeform::init(const std::vector<svg::SvgPolyPath*>& polys, int numQuadsProxy)
{
	clear();

	if (polys.size() == 0)
		return;

	m_polys = polys;
	ldp::Float4 bbox = polys[0]->getBound();
	for (size_t i = 1; i < polys.size(); i++)
		bbox = polys[i]->unionBound(bbox);
	const float ext = 0.1f;
	const float X0 = bbox[0] - (bbox[1] - bbox[0]) * ext;
	const float X1 = bbox[1] + (bbox[1] - bbox[0]) * ext;
	const float Y0 = bbox[2] - (bbox[3] - bbox[2]) * ext;
	const float Y1 = bbox[3] + (bbox[3] - bbox[2]) * ext;

	const float step = sqrt((X1 - X0)*(Y1 - Y0) / numQuadsProxy);
	m_nX = std::ceil((X1 - X0) / step) + 1;
	m_nY = std::ceil((Y1 - Y0) / step) + 1;
	m_points_init.reserve(m_nX * m_nY);
	m_quads.reserve((m_nX - 1) * (m_nY - 1));
	for (int iX = 0; iX < m_nX; iX++)
	{
		float x = X0 + iX * step;
		for (int iY = 0; iY < m_nY; iY++)
		{
			float y = Y0 + iY * step;
			m_points_init.push_back(ldp::Float2(x, y));

			if (iX < m_nX - 1 && iY < m_nY - 1)
			{
				int idx = iY * m_nX + iX;
				m_quads.push_back(ldp::Int4(idx, idx + 1, idx + 1 + m_nX, idx + m_nX));
			}
		} // iY
	} // iX
	m_points = m_points_init;

	buildPolyQuads();
	setupSimilarityMatrix();
}

void FreeFormDeform::clear()
{
	m_nX = 0;
	m_nY = 0;
	m_polys.clear();
	m_polys_coords.clear();
	m_points.clear();
	m_points_init.clear();
	m_quads.clear();
	m_controlPoints.clear();
}

int FreeFormDeform::findQuadIdx(ldp::Float2 p)
{
	for (size_t iq = 0; iq < m_quads.size(); iq++)
	{
		const auto& q = m_quads[iq];
		ldp::Float2 pts[4];
		for (int k = 0; k < 4; k++)
			pts[k] = m_points[q[k]];
		if (ldp::PointInPolygon(4, pts, p))
			return iq;
	} // end for iq
	return -1;
}

int FreeFormDeform::addControlPoint(ldp::Float2 pos)
{
	int idx = findQuadIdx(pos);
	if (idx < 0)
		return -1;

	const auto& q = m_quads[idx];
	ldp::Float2 pts[4];
	for (int k = 0; k < 4; k++)
		pts[k] = m_points[q[k]];

	ControlPoint cp;
	cp.quad_id = idx;
	ldp::CalcBilinearCoef(cp.quad_w, pos, pts);
	cp.tarPos = ldp::BilinearInterpolate(cp.quad_w, pts);
	m_controlPoints.push_back(cp);
	setupControlMatrix();
	factor();
	solve();
	return m_controlPoints.size() - 1;
}

void FreeFormDeform::clearControlPoints()
{
	resetControlPoints();
	m_controlPoints.clear();
	setupControlMatrix();
	updatePoly();
}

void FreeFormDeform::resetControlPoints()
{
	m_points = m_points_init;
	for (auto& cp : m_controlPoints)
		cp.tarPos = calcPosition(cp);
	setupControlMatrix();
	updatePoly();
}

ldp::Float2 FreeFormDeform::calcPosition(const ControlPoint&  cp)
{
	const auto& q = m_quads[cp.quad_id];
	ldp::Float2 pts[4];
	for (int k = 0; k < 4; k++)
		pts[k] = m_points[q[k]];
	return ldp::BilinearInterpolate(cp.quad_w, pts);
}

void FreeFormDeform::removeControlPoint(int i)
{
	m_controlPoints.erase(m_controlPoints.begin() + i);
	setupControlMatrix();
	factor();
	solve();
}

void FreeFormDeform::setControlPointTargetPos(int i, ldp::Float2 p)
{
	m_controlPoints[i].tarPos = p;
	setupControlMatrix();
	solve();
}

void FreeFormDeform::buildPolyQuads()
{
	// build a kdtree for all quad points
	typedef ldp::kdtree::PointTree<float, 2> Tree;
	typedef Tree::Point Point;
	std::vector<Point> points;
	points.resize(m_points.size());
	for (size_t i = 0; i < points.size(); i++)
	{
		points[i].idx = i;
		points[i].p = m_points[i];
	}
	Tree tree;
	tree.build(points);

	// build bilinear coordinates of all poly points
	m_polys_coords.resize(m_polys.size());
	for (size_t ipoly = 0; ipoly < m_polys.size(); ipoly++)
	{
		auto poly = m_polys[ipoly];
		auto& poly_coords = m_polys_coords[ipoly];
		poly_coords.resize(poly->m_cmds.size());
		for (size_t i = 0; i < poly->m_cmds.size(); i++)
		{
			// find a nearest point using kdtree
			ldp::Float2 p(poly->m_coords[i * 2], poly->m_coords[i * 2 + 1]);
			float dist = 0;
			Point np = tree.nearestPoint(p, dist);

			// check the four quads surrounding the nearest point
			int iX = std::max(0, std::min(m_nX-2, np.idx % m_nX));
			int iY = std::max(0, std::min(m_nY-2, np.idx / m_nX));
			int iX_prev = std::max(0, iX - 1), iY_prev = std::max(0, iY - 1);
			std::vector<int> nbQuadIds;
			nbQuadIds.push_back(iY*(m_nX - 1) + iX);
			nbQuadIds.push_back(iY*(m_nX - 1) + iX_prev);
			nbQuadIds.push_back(iY_prev*(m_nX - 1) + iX_prev);
			nbQuadIds.push_back(iY_prev*(m_nX - 1) + iX);
			int qid = -1;
			ldp::Float2 pts[4];
			for (auto iq : nbQuadIds)
			{
				const auto& q = m_quads[iq];
				for (int k = 0; k < 4; k++)
					pts[k] = m_points[q[k]];
				if (ldp::PointInPolygon(4, pts, p))
				{
					qid = iq;
					break;
				}
			} // end for iq
			assert(qid >= 0);
			if (qid < 0)
				throw std::exception("FreeFormDeform: not valid bounding box!");
			ControlPoint cp;
			cp.quad_id = qid;
			ldp::CalcBilinearCoef(cp.quad_w, p, pts);
			cp.tarPos = ldp::BilinearInterpolate(cp.quad_w, pts);
			poly_coords[i] = cp;
		} // end for i
	} // end for ipoly
}

void FreeFormDeform::setupSimilarityMatrix()
{
	std::vector<Eigen::Triplet<ldp::real>> cooSys;
	int nRows = 0;

	// adding quads to the linear system: as-similar-as-possible style.

	// setup similarity matrix
	ldp::SpMat A_similarity;
	A_similarity.resize(nRows, m_points.size() * 2);
	if (cooSys.size())
		A_similarity.setFromTriplets(cooSys.begin(), cooSys.end());
	m_AtA_similarity = A_similarity.transpose() * A_similarity;
	m_solver.analyzePattern(m_AtA_similarity);
}

void FreeFormDeform::setupControlMatrix()
{
	std::vector<Eigen::Triplet<ldp::real>> cooSys;
	std::vector<ldp::real> cooRhs;

	// adding control points to the linear system
	for (const auto& cp : m_controlPoints)
	{
		const auto& q = m_quads[cp.quad_id];
		for (int r = 0; r < 2; r++)
		{
			float dif = cp.tarPos[r];
			for (int k = 0; k < 4; k++)
			{
				cooSys.push_back(Eigen::Triplet<ldp::real>(cooRhs.size(), q[k] * 2 + r, cp.quad_w[k]));
				dif -= cp.quad_w[k] * m_points_init[q[k]][r];
			} // end for k
			cooRhs.push_back(dif);
		} // end for r
	} // end for cp

	// setup control point matrix and rhs
	ldp::SpMat A;
	A.resize(cooRhs.size(), m_points.size() * 2);
	if (cooSys.size())
		A.setFromTriplets(cooSys.begin(), cooSys.end());
	m_Atb_control = A.transpose() * A;
	ldp::Vec b;
	b.resize(cooRhs.size());
	for (size_t i = 0; i < b.size(); i++)
		b[i] = cooRhs[i];
	m_Atb_control = A.transpose() * b;
}

void FreeFormDeform::updatePoly()
{
	for (size_t ipoly = 0; ipoly < m_polys.size(); ipoly++)
	{
		auto poly = m_polys[ipoly];
		auto& poly_coords = m_polys_coords[ipoly];
		for (size_t i = 0; i < poly->m_cmds.size(); i++)
		{
			const ControlPoint& cp = poly_coords[i];

			ldp::Float2 pts[4];
			const auto& q = m_quads[cp.quad_id];
			for (int k = 0; k < 4; k++)
				pts[k] = m_points[q[k]];
			ldp::Float2 p = ldp::BilinearInterpolate(cp.quad_w, pts);
			poly->m_coords[i * 2] = p[0];
			poly->m_coords[i * 2 + 1] = p[1];
		} // end for i
		poly->invalid();
	} // end for ipoly
}

void FreeFormDeform::factor()
{
	m_solver.factorize(m_AtA_similarity + m_AtA_control);
}

void FreeFormDeform::solve()
{
	auto X = m_solver.solve(m_Atb_control);
	for (size_t i = 0; i < m_points.size(); i++)
		m_points[i] = ldp::Float2(X[i * 2], X[i * 2 + 1]);
	updatePoly();
}
