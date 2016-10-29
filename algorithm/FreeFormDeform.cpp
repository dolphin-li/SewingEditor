#include "FreeFormDeform.h"
#include "SvgPolyPath.h"
#include "kdtree\PointTree.h"

const static int NUM_BINS = 50;

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
	m_X0 = bbox[0] - (bbox[1] - bbox[0]) * ext;
	const float X1 = bbox[1] + (bbox[1] - bbox[0]) * ext;
	m_Y0 = bbox[2] - (bbox[3] - bbox[2]) * ext;
	const float Y1 = bbox[3] + (bbox[3] - bbox[2]) * ext;

	m_step = sqrt((X1 - m_X0)*(Y1 - m_Y0) / numQuadsProxy);
	m_nX = std::ceil((X1 - m_X0) / m_step) + 1;
	m_nY = std::ceil((Y1 - m_Y0) / m_step) + 1;
	m_points_init.reserve(m_nX * m_nY);
	m_quads.reserve((m_nX - 1) * (m_nY - 1));
	for (int iY = 0; iY < m_nY; iY++)
	{
		const float y = m_Y0 + iY * m_step;
		for (int iX = 0; iX < m_nX; iX++)
		{
			const float x = m_X0 + iX * m_step;
			m_points_init.push_back(ldp::Float2(x, y));

			if (iX < m_nX - 1 && iY < m_nY - 1)
			{
				int idx = iY * m_nX + iX;
				m_quads.push_back(ldp::Int4(idx, idx + 1, idx + 1 + m_nX, idx + m_nX));
			}
		} // iX
	} // iY
	m_points = m_points_init;

	buildPolyQuads();
	setupSimilarityMatrix();
	buildLineSegments();
	setupLineMatrix();
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
	m_thetas.clear();
	m_lineSegsInQuads.clear();
	m_lineBinIdx.clear();
	m_AtA_similarity.resize(0, 0);
	m_Atb_similarity.resize(0);
	m_AtA_control.resize(0, 0);
	m_Atb_control.resize(0);
	m_AtA_line.resize(0, 0);
}

std::shared_ptr<FreeFormDeform> FreeFormDeform::clone()const
{
	std::shared_ptr<FreeFormDeform> rhs(new FreeFormDeform);
	rhs->m_polys = m_polys;
	rhs->m_polys_coords = m_polys_coords;
	rhs->m_controlPoints = m_controlPoints;
	rhs->m_points = m_points;
	rhs->m_points_init = m_points_init;
	rhs->m_quads = m_quads;
	rhs->m_nX = m_nX;
	rhs->m_nY = m_nY;
	rhs->m_AtA_similarity = m_AtA_similarity;
	rhs->m_Atb_similarity = m_Atb_similarity;
	rhs->m_AtA_control = m_AtA_control;
	rhs->m_Atb_control = m_Atb_control;
	rhs->m_solver.analyzePattern(m_AtA_similarity);
	if (m_AtA_control.size() == m_AtA_similarity.size())
		rhs->m_solver.factorize(m_AtA_similarity + m_AtA_control);
	return rhs;
}

int FreeFormDeform::findQuadIdx(ldp::Float2 p)const
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
	solve();
}

void FreeFormDeform::setControlPointTargetPos(int i, ldp::Float2 p)
{
	if (i < 0 || i >= numControlPoints())
		return;
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
	points.resize(m_points_init.size());
	for (size_t i = 0; i < points.size(); i++)
	{
		points[i].idx = i;
		points[i].p = m_points_init[i];
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
					pts[k] = m_points_init[q[k]];
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

void FreeFormDeform::buildLineSegments()
{
	// 1. cut segments into quads
	m_lineSegsInQuads.clear();
	for (const auto& cpoly : m_polys_coords)
	{
		for (size_t ipoint = 1; ipoint < cpoly.size(); ipoint++)
		{
			const auto& cp_e = cpoly[ipoint];
			const auto& cp_s = cpoly[ipoint - 1];
			// if the start and end points are in the same quad, we got the segment inside the quad
			if (cp_s.quad_id == cp_e.quad_id)
			{
				auto cp_seg = cp_e;
				cp_seg.quad_w -= cp_s.quad_w;
				m_lineSegsInQuads.push_back(cp_seg);
			}
			// if not, we should cut the segment.
			else
			{
				cutLineIntoQuadSegs(cp_s, cp_e, m_lineSegsInQuads);
			}
		} // ipoint
	} // cpoly

	// 2. group segments into bins
	m_thetas.clear();
	m_thetas.resize(NUM_BINS, 0);
	m_lineBinIdx.resize(m_lineSegsInQuads.size());
	//[0, pi)
	const float binStep = ldp::PI_S / float(m_thetas.size() - 1);
	for (size_t iseg = 0; iseg < m_lineSegsInQuads.size(); iseg++)
	{
		auto& cp = m_lineSegsInQuads[iseg];
		const auto& dir = calcPosition(cp);
		float theta = atan2f(dir[1], dir[0]);
		if (theta < 0.f)
		{
			cp.quad_w = 0.f - cp.quad_w;
			theta += ldp::PI_S;
		}
		if (std::isinf(theta) || std::isnan(theta))
			continue;
		int idBin = std::min((int)m_thetas.size()-1, (int)std::lroundf(theta / binStep));
		m_lineBinIdx[iseg] = idBin;
	}//end for id
}

static inline bool raySegIntersect(ldp::Float2 s, ldp::Float2 d, ldp::Float2 a, ldp::Float2 b,
	ldp::Float2& sect)
{
	d = d.normalize();
	ldp::Float2 ab = b - a;
	float len_ab = ab.length();
	ab /= len_ab;
	float d_x_ab = d.cross(ab);
	if (abs(d_x_ab) < std::numeric_limits<float>::epsilon())
		return false;
	ldp::Float2 as = s - a;
	float t = -as.cross(ab) / d_x_ab;
	if (t < 0)
		return false;
	sect = s + t * d;
	float t1 = (sect - a).dot(ab);
	if (!(t1 >= 0 && t1 < len_ab))
		return false;
	sect = a + t1 * ab;
	return true;
}

void FreeFormDeform::cutLineIntoQuadSegs(const ControlPoint& cp_s, 
	const ControlPoint& cp_e, std::vector<ControlPoint>& segs)
{
	ldp::Float2 p_s = calcPosition(cp_s);
	const ldp::Float2 p_e = calcPosition(cp_e);
	const ldp::Float2 p_d = (p_e - p_s).normalize();
	const float Xe = (p_e[0] - m_X0) / m_step;
	const float Ye = (p_e[1] - m_Y0) / m_step;
	const int iXe = floor(Xe), iYe = floor(Ye);
	float Xs = (p_s[0] - m_X0) / m_step;
	float Ys = (p_s[1] - m_Y0) / m_step;
	int iXs = floor(Xs), iYs = floor(Ys);
	const static int x_inc[4] = { -1, 0, 1, 0 };
	const static int y_inc[4] = { 0, -1, 0, 1 };
	for (int iter = 0; iter < m_nX + m_nY; iter++)
	{
		Xs = (p_s[0] - m_X0) / m_step;
		Ys = (p_s[1] - m_Y0) / m_step;
		int iq = iXs + iYs * (m_nX - 1);
		const auto& q = m_quads[iq];
		ldp::Float2 pts[4];
		for (int k = 0; k < 4; k++)
			pts[k] = m_points_init[q[k]];
		for (int i = 0, j = 3; i < 4; j = i++)
		{
			ldp::Float2 sect;
			if (raySegIntersect(p_s, p_d, pts[i], pts[j], sect))
			{
				float dist = (sect - p_s).length();
				if (dist < std::numeric_limits<float>::epsilon())
					continue;
				ControlPoint cp;
				cp.quad_id = iq;
				cp.quad_w = ldp::CalcBilinearCoef(sect, pts) - ldp::CalcBilinearCoef(p_s, pts);
				if (cp.quad_w.length() > std::numeric_limits<float>::epsilon())
				{
					segs.push_back(cp);
					p_s = sect;
					iXs += x_inc[i];
					iYs += y_inc[i];
					break;
				}
			} // end if sect
		} // end for i, j
		if (iXs == iXe && iYs == iYe)
			break;
	} // end for iter
}

void FreeFormDeform::setupLineMatrix()
{
	std::vector<Eigen::Triplet<ldp::real>> cooSys;
	const float line_w = sqrt(100.f / m_lineSegsInQuads.size());

	int iRow = 0;
	for (size_t iseg = 0; iseg < m_lineSegsInQuads.size(); iseg++)
	{
		int binId = m_lineBinIdx[iseg];
		float theta = m_thetas[binId];
		const auto& seg = m_lineSegsInQuads[iseg];
		const auto& q = m_quads[seg.quad_id];
		ldp::Float2 pts[4];
		for (int k = 0; k < 4; k++)
			pts[k] = m_points_init[q[k]];
		ldp::Float2 e = ldp::BilinearInterpolate(seg.quad_w, pts);

		ldp::Mat2f R;
		R(0, 0) = cos(theta);	R(0, 1) = -sin(theta);
		R(1, 0) = -R(0, 1);		R(1, 1) = R(0, 0);

		ldp::Mat2f ete;
		ete(0, 0) = e[0] * e[0];	ete(0, 1) = e[0] * e[1];
		ete(1, 0) = e[1] * e[0];	ete(1, 1) = e[1] * e[1];
		if (e.sqrLength())
			ete /= (e.sqrLength());
		else
			ete.eye();

		ldp::Mat2f C = R * ete * R.trans();
		C(0, 0) -= 1;	C(1, 1) -= 1;

		for (int i = 0; i<2; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				cooSys.push_back(Eigen::Triplet<ldp::real>(iRow, q[k] * 2 + 0, line_w * seg.quad_w[k] * C(i, 0)));
				cooSys.push_back(Eigen::Triplet<ldp::real>(iRow, q[k] * 2 + 1, line_w * seg.quad_w[k] * C(i, 1)));
			} // end for k
			iRow++;
		} // end for i
	} // end for seg

	// setup similarity matrix
	ldp::SpMat A_line;
	A_line.resize(iRow, m_points_init.size() * 2);
	if (cooSys.size())
		A_line.setFromTriplets(cooSys.begin(), cooSys.end());
	m_AtA_line = A_line.transpose() * A_line;
}

void FreeFormDeform::setupSimilarityMatrix()
{
	std::vector<Eigen::Triplet<ldp::real>> cooSys;
	std::vector<ldp::real> cooRhs;

	const float sim_w = sqrt(1.f / m_quads.size());

	// adding quads to the linear system: as-similar-as-possible style.
	ldp::Mat A, M;
	A.resize(8, 4);
	M.resize(8, 8);
	ldp::Mat I = M;
	I.setIdentity();
	for (const auto& q : m_quads)
	{
		for (int k = 0; k < 4; k++)
		{
			const auto& p = m_points_init[q[k]];
			A(k * 2 + 0, 0) = p[0];
			A(k * 2 + 0, 1) = -p[1];
			A(k * 2 + 0, 2) = 1;
			A(k * 2 + 0, 3) = 0;
			A(k * 2 + 1, 0) = p[1];
			A(k * 2 + 1, 1) = p[0];
			A(k * 2 + 1, 2) = 0;
			A(k * 2 + 1, 3) = 1;
		} // end for k
		M = A*(A.transpose()*A).inverse()*A.transpose() - I;

		for (int r = 0; r < 8; r++)
		{
			for (int k = 0; k < 4; k++)
			{
				cooSys.push_back(Eigen::Triplet<ldp::real>(cooRhs.size(), q[k] * 2 + 0, sim_w * M(r, k * 2 + 0)));
				cooSys.push_back(Eigen::Triplet<ldp::real>(cooRhs.size(), q[k] * 2 + 1, sim_w * M(r, k * 2 + 1)));
			} // end for k
			cooRhs.push_back(0);
		} // end for r
	} // end for q

	// add line-preserving term

	// add regularity term
	const ldp::real reg_w = sqrt(1e-6 / m_points_init.size());
	for (size_t i = 0; i < m_points_init.size(); i++)
	{
		for (int r = 0; r < 2; r++)
		{
			cooSys.push_back(Eigen::Triplet<ldp::real>(cooRhs.size(), i * 2 + r, reg_w));
			cooRhs.push_back(m_points_init[i][r] * reg_w);
		}
	}

	// setup similarity matrix
	ldp::SpMat A_similarity;
	A_similarity.resize(cooRhs.size(), m_points_init.size() * 2);
	if (cooSys.size())
		A_similarity.setFromTriplets(cooSys.begin(), cooSys.end());
	ldp::Vec b_similarity;
	b_similarity.resize(cooRhs.size());
	for (size_t i = 0; i < cooRhs.size(); i++)
		b_similarity[i] = cooRhs[i];
	m_AtA_similarity = A_similarity.transpose() * A_similarity;
	m_Atb_similarity = A_similarity.transpose() * b_similarity;
	m_solver.analyzePattern(m_AtA_similarity);
}

void FreeFormDeform::setupControlMatrix()
{
	std::vector<Eigen::Triplet<ldp::real>> cooSys;
	std::vector<ldp::real> cooRhs;

	const float cp_w = sqrt(100000.f / m_controlPoints.size());

	// adding control points to the linear system
	for (const auto& cp : m_controlPoints)
	{
		const auto& q = m_quads[cp.quad_id];
		for (int r = 0; r < 2; r++)
		{
			for (int k = 0; k < 4; k++)
				cooSys.push_back(Eigen::Triplet<ldp::real>(cooRhs.size(), q[k] * 2 + r, cp_w * cp.quad_w[k]));
			cooRhs.push_back(cp_w * cp.tarPos[r]);
		} // end for r
	} // end for cp

	// setup control point matrix and rhs
	ldp::SpMat A;
	A.resize(cooRhs.size(), m_points.size() * 2);
	if (cooSys.size())
		A.setFromTriplets(cooSys.begin(), cooSys.end());
	m_AtA_control = A.transpose() * A;
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

void FreeFormDeform::solve()
{
	std::fill(m_thetas.begin(), m_thetas.end(), 0.f);
	for (int iter = 0; iter < 10; iter++)
	{
		// 1. solve for X
		setupLineMatrix();
		m_solver.factorize(m_AtA_similarity + m_AtA_control + m_AtA_line);
		ldp::Vec X = m_solver.solve(m_Atb_similarity + m_Atb_control);
		for (size_t i = 0; i < m_points.size(); i++)
			m_points[i] = ldp::Float2(X[i * 2], X[i * 2 + 1]);
		updatePoly();

		// 2. solve for R
		std::vector<int> thetaCnt(m_thetas.size(), 0);
		std::fill(m_thetas.begin(), m_thetas.end(), 0.f);
		for (size_t iseg = 0; iseg < m_lineSegsInQuads.size(); iseg++)
		{
			int binId = m_lineBinIdx[iseg];
			float theta = m_thetas[binId];
			const auto& seg = m_lineSegsInQuads[iseg];
			const auto& q = m_quads[seg.quad_id];
			ldp::Float2 pts[4];
			for (int k = 0; k < 4; k++)
				pts[k] = m_points_init[q[k]];
			ldp::Float2 e_init = ldp::BilinearInterpolate(seg.quad_w, pts);
			ldp::Float2 e_cur = calcPosition(seg);

			//ignore those point-overlapped segments
			if (e_init.sqrLength() < std::numeric_limits<float>::epsilon()
				|| e_cur.sqrLength() < std::numeric_limits<float>::epsilon())
				continue;
			e_init.normalizeLocal();
			e_cur.normalizeLocal();

			double sint = e_init.cross(e_cur);	//the order is important

			//avoid Nan by asin
			if (sint <= -1.f + 1e-6)
				sint = -1.f + 1e-6;
			if (sint >= 1.f - 1e-6)
				sint = 1.f - 1e-6;

			double t = asin(sint);
			m_thetas[binId] += t;
			thetaCnt[binId]++;
		} // end for i

		for (size_t i = 0; i < m_thetas.size(); i++)
		if (thetaCnt[i])
			m_thetas[i] /= thetaCnt[i];
	} // end for iter
}
