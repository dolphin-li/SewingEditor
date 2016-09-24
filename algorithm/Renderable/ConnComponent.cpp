#include "ConnComponent.h"
#include <hash_set>
#undef min
#undef max

void ConnComponents::clear()
{
	m_data.clear();
	m_center = 0;
	m_wh = 0;
}

bool ConnComponents::load(const char* filename)
{
	FILE* pFile = fopen(filename, "rb");

	if (pFile == nullptr)
		return false;

	bool ret = load(pFile);

	fclose(pFile);
	return ret;
}

bool ConnComponents::load(FILE* pFile)
{
	clear();

	int num = 0;
	fread(&num, sizeof(num), 1, pFile);
	m_data.resize(num);
	Point2 bMin = FLT_MAX, bMax = -FLT_MAX;
	for (int i = 0; i < num; i++)
	{
		m_data[i].reset(new ConnComponent);
		if (!m_data[i]->load(pFile))
			return false;
		m_data[i]->extendBoundingBox(bMin, bMax);
	}

	m_center = (bMin + bMax) / 2;
	m_wh = std::max(bMax[0]-bMin[0], bMax[1]-bMin[1]) + 20;

	return true;
}

bool ConnComponents::save(const char* filename)const
{
	FILE* pFile = fopen(filename, "wb");

	if (pFile == nullptr)
		return false;

	bool ret = save(pFile);

	fclose(pFile);
	return ret;
}

bool ConnComponents::save(FILE* pFile)const
{
	int num = m_data.size();
	fwrite(&num, sizeof(num), 1, pFile);
	for (int i = 0; i < num; i++)
	{
		if (!m_data[i]->save(pFile))
			return false;
	}
	return true;
}

void ConnComponents::resetComponentDirtyFlag()const
{
	for (int i = 0; i < size(); i++)
		getComponent(i)->resetDirtyFlag();
}

ConnComponents::Point2 ConnComponents::convertToGlViewport(Point2 p, const Camera& cam)const
{
	double cl = cam.getViewPortLeft();
	double ct = cam.getViewPortTop();
	double cr = cam.getViewPortRight();
	double cb = cam.getViewPortBottom();
	double cw = cr - cl;
	double ch = cb - ct;
	double ml = m_center[0] - m_wh[0] / 2;
	double mt = m_center[1] - m_wh[1] / 2;
	p[0] = (p[0] - ml) / m_wh[0] * cw + cl;
	p[1] = (1 - (p[1] - mt) / m_wh[1]) * ch + ct;
	return p;
}

ConnComponents::Point2 ConnComponents::convertFromGlViewport(Point2 p, const Camera& cam)const
{
	double cl = cam.getViewPortLeft();
	double ct = cam.getViewPortTop();
	double cr = cam.getViewPortRight();
	double cb = cam.getViewPortBottom();
	double cw = cr - cl;
	double ch = cb - ct;
	double ml = m_center[0] - m_wh[0] / 2;
	double mt = m_center[1] - m_wh[1] / 2;
	p[0] = (p[0] - cl) / cw * m_wh[0] + ml;
	p[1] = (1 - (p[1] - ct) / ch) * m_wh[1] + mt;
	return p;
}

float ConnComponents::scale_fromGL(const Camera& cam)const
{
	double cl = cam.getViewPortLeft();
	double ct = cam.getViewPortTop();
	double cr = cam.getViewPortRight();
	double cb = cam.getViewPortBottom();
	double cw = cr - cl;
	double ch = cb - ct;
	return std::max(m_wh[0], m_wh[1]) / std::max(cw, ch);
}

void ConnComponents::mergeSelectedConn()
{
	std::vector<std::shared_ptr<ConnComponent>> to_merge;
	std::vector<std::shared_ptr<ConnComponent>> to_left;
	for (size_t i = 0; i < m_data.size(); i++)
	if (m_data[i]->isSelected())
		to_merge.push_back(m_data[i]);
	else
		to_left.push_back(m_data[i]);

	if (to_merge.size() <= 1)
		return;

	std::shared_ptr<ConnComponent> merged = to_merge[0];
	for (size_t i = 1; i < to_merge.size(); i++)
		merged->mergeWith(*to_merge[i]);
	merged->setSelected(true);

	m_data.clear();
	m_data.push_back(merged);
	m_data.insert(m_data.end(), to_left.begin(), to_left.end());
}

void ConnComponents::samplePointsOnUintsAndConvertToGlViewport(double stepOnGlView, const Camera& cam)
{
	double step = stepOnGlView * scale_fromGL(cam);

	for (int iConn = 0; iConn < size(); iConn++)
	{
		ConnComponent* cnn = getComponent(iConn);
		cnn->samplePointsOnUints(step);
		for (int iUnit = 0; iUnit < cnn->numUnits(); iUnit++)
		{
			BaseShape* bs = cnn->getUnit(iUnit);
			for (int i = 0; i < bs->numSamplePts(); i++)
			{
				bs->setSamplePt(i, convertToGlViewport(bs->getSamplePt(i), cam));
			}
		} // iUnit
	}// iConn
}

void ConnComponents::selectUnitsViaGLBox(Point2 bMinGL, Point2 bMaxGL, const Camera& cam, SelectType stype)const
{
	Point2 bMin = convertFromGlViewport(bMinGL, cam);
	Point2 bMax = convertFromGlViewport(bMaxGL, cam);
	for (int k = 0; k<2; k++)
	if (bMin[k] > bMax[k]) std::swap(bMin[k], bMax[k]);
	for (int iConn = 0; iConn < size(); iConn++)
	{
		const ConnComponent* conn = getComponent(iConn);
		for (int iUnit = 0; iUnit < conn->numUnits(); iUnit++)
		{
			bool sel = conn->getUnit(iUnit)->isIntersectedWithBoundingBox(bMin, bMax);
			switch (stype)
			{
			case ConnComponents::SelectType_Add:
				if (sel)
					conn->getUnit(iUnit)->setSelected(sel);
				break;
			case ConnComponents::SelectType_Erase:
				if (sel)
					conn->getUnit(iUnit)->setSelected(!sel);
				break;
			case ConnComponents::SelectType_Only:
				conn->getUnit(iUnit)->setSelected(sel);
				break;
			default:
				break;
			}
		}
	}
}

void ConnComponents::setAllUnitsDisabled(bool ds)
{
	for (int iConn = 0; iConn < size(); iConn++)
		getComponent(iConn)->setAllUnitsDisabled(ds);
}

void ConnComponents::setSelectedUnitsDisabled(bool ds)
{
	for (int iConn = 0; iConn < size(); iConn++)
		getComponent(iConn)->setSelectedUnitsDisabled(ds);
}

void ConnComponents::removeSelectedUnits()
{
	for (int iConn = 0; iConn < size(); iConn++)
		getComponent(iConn)->removeSelectedUnits();
}

void ConnComponents::removeMatchedUnits()
{
	for (int iConn = 0; iConn < size(); iConn++)
		getComponent(iConn)->removeMatchedUnits();
}

void ConnComponents::setSelectedUnitsMatched(bool ds)
{
	for (int iConn = 0; iConn < size(); iConn++)
		getComponent(iConn)->setSelectedUnitsMatched(ds);
}

void ConnComponents::setAllUnitsMatched(bool ds)
{
	for (int iConn = 0; iConn < size(); iConn++)
		getComponent(iConn)->setAllUnitsMatched(ds);
}

void ConnComponents::setSelectedUnitsIgnored(bool ds)
{
	for (int iConn = 0; iConn < size(); iConn++)
		getComponent(iConn)->setSelectedUnitsIgnored(ds);
}

void ConnComponents::setAllUnitsIgnored(bool ds)
{
	for (int iConn = 0; iConn < size(); iConn++)
		getComponent(iConn)->setAllUnitsIgnored(ds);
}

void ConnComponents::setSelectedUnitsNew(bool ds)
{
	for (int iConn = 0; iConn < size(); iConn++)
		getComponent(iConn)->setSelectedUnitsNew(ds);
}

void ConnComponents::setAllUnitsNew(bool ds)
{
	for (int iConn = 0; iConn < size(); iConn++)
		getComponent(iConn)->setAllUnitsNew(ds);
}

void ConnComponents::clearSelection()const
{
	for (int iConn = 0; iConn < size(); iConn++)
	{
		getComponent(iConn)->setSelected(false);
		getComponent(iConn)->setAllUnitsSelected(false);
	}
}

void ConnComponents::splitComponetSelectedUnits(int iConn)
{
	std::shared_ptr<ConnComponent> conn(new ConnComponent);
	getComponent(iConn)->splitSelectedTo(*conn);
	if (conn->numUnits())
		m_data.push_back(conn);
}

ConnComponent::ConnComponent()
{
	m_dirtyFlag = 0;
	m_numSamplePoints = 0;
	m_boundMax = -FLT_MAX;
	m_boundMin = FLT_MAX;
}

ConnComponent::~ConnComponent()
{
}

bool ConnComponent::load(const char* filename)
{
	FILE* pFile = fopen(filename, "rb");

	if (pFile == nullptr)
		return false;

	bool ret = load(pFile);

	fclose(pFile);
	return ret;
}

bool ConnComponent::load(FILE* pFile)
{
	m_shapes.clear();

	// read line
	int nLines = 0;
	fread(&nLines, sizeof(int), 1, pFile);
	for (int i = 0; i < nLines; i++)
	{
		std::shared_ptr<BaseShape> shape;
		shape.reset(BaseShape::create(BaseShape::Line));
		if (!shape->load(pFile))
			return false;
		m_shapes.push_back(shape);
	}

	// read poly line
	int nPolyLines = 0;
	fread(&nPolyLines, sizeof(int), 1, pFile);
	for (int i = 0; i < nPolyLines; i++)
	{
		std::shared_ptr<BaseShape> shape;
		shape.reset(BaseShape::create(BaseShape::PolyLine));
		if (!shape->load(pFile))
			return false;
		m_shapes.push_back(shape);
	}

	// read polygon
	int nPoly = 0;
	fread(&nPoly, sizeof(int), 1, pFile);
	for (int i = 0; i < nPoly; i++)
	{
		std::shared_ptr<BaseShape> shape;
		shape.reset(BaseShape::create(BaseShape::Polygon));
		if (!shape->load(pFile))
			return false;
		m_shapes.push_back(shape);
	}

	// read path
	int nPath = 0;
	fread(&nPath, sizeof(int), 1, pFile);
	for (int i = 0; i < nPath; i++)
	{
		std::shared_ptr<BaseShape> shape;
		shape.reset(BaseShape::create(BaseShape::Path));
		if (!shape->load(pFile))
			return false;
		m_shapes.push_back(shape);
	}

	// remove duplicates
	struct DuplicateOp
	{
		bool operator () (const std::shared_ptr<BaseShape>& a, const std::shared_ptr<BaseShape>& b)const
		{
			return a->isDuplicatedWith(b.get());
		}
	};
	std::hash_set<std::shared_ptr<BaseShape>> spset;
	for (size_t i = 0; i < m_shapes.size(); i++)
		spset.insert(m_shapes[i]);
	int nbefore = (int)m_shapes.size();
	m_shapes.assign(spset.begin(), spset.end());
	int nafter = (int)m_shapes.size();
	if (nbefore != nafter)
		printf("duplicate removed: %d -> %d\n", nbefore, nafter);

	// update bounding box
	m_boundMin = FLT_MAX;
	m_boundMax = -FLT_MAX;
	for (size_t i = 0; i < m_shapes.size(); i++)
		m_shapes[i]->extendBoundingBox(m_boundMin, m_boundMax);

	return true;
}

bool ConnComponent::save(const char* filename)const
{
	FILE* pFile = fopen(filename, "wb");

	if (pFile == nullptr)
		return false;

	bool ret = save(pFile);

	fclose(pFile);
	return ret;
}

bool ConnComponent::save(FILE* pFile)const
{
	std::vector<BaseShape*> lines, polyLines, polys, paths;
	for (int i = 0; i < m_shapes.size(); i++)
	{
		switch (m_shapes[i]->getType())
		{
		default:
			break;
		case BaseShape::Line:
			lines.push_back(m_shapes[i].get());
			break;
		case BaseShape::PolyLine:
			polyLines.push_back(m_shapes[i].get());
			break;
		case BaseShape::Polygon:
			polys.push_back(m_shapes[i].get());
			break;
		case BaseShape::Path:
			paths.push_back(m_shapes[i].get());
			break;
		}
	}

	// read line
	int nLines = lines.size();
	fwrite(&nLines, sizeof(int), 1, pFile);
	for (int i = 0; i < nLines; i++)
	{
		if (!lines[i]->save(pFile))
			return false;
	}

	// read poly line
	int nPolyLines = polyLines.size();
	fwrite(&nPolyLines, sizeof(int), 1, pFile);
	for (int i = 0; i < nPolyLines; i++)
	{
		if (!polyLines[i]->save(pFile))
			return false;
	}

	// read polygon
	int nPoly = polys.size();
	fwrite(&nPoly, sizeof(int), 1, pFile);
	for (int i = 0; i < nPoly; i++)
	{
		if (!polys[i]->save(pFile))
			return false;
	}

	// read path
	int nPath = paths.size();
	fwrite(&nPath, sizeof(int), 1, pFile);
	for (int i = 0; i < nPath; i++)
	{
		if (!paths[i]->save(pFile))
			return false;
	}

	return true;
}

void ConnComponent::mergeWith(const ConnComponent& rhs)
{
	m_shapes.insert(m_shapes.end(), rhs.m_shapes.begin(), rhs.m_shapes.end());

	rhs.extendBoundingBox(m_boundMin, m_boundMax);
	m_numSamplePoints += rhs.m_numSamplePoints;
}

void ConnComponent::splitSelectedTo(ConnComponent& rhs)
{
	std::vector<std::shared_ptr<BaseShape>> to_left, to_split;
	m_boundMax = -FLT_MAX;
	m_boundMin = FLT_MAX;
	for (size_t i = 0; i < m_shapes.size(); i++)
	{
		if (m_shapes[i]->isSelected())
		{
			to_split.push_back(m_shapes[i]);
			m_numSamplePoints -= m_shapes[i]->numSamplePts();
			rhs.m_numSamplePoints += m_shapes[i]->numSamplePts();
			m_shapes[i]->extendBoundingBox(rhs.m_boundMin, rhs.m_boundMax);
		}
		else
		{
			to_left.push_back(m_shapes[i]);
			m_shapes[i]->extendBoundingBox(m_boundMin, m_boundMax);
		}
	}

	m_shapes.swap(to_left);
	rhs.m_shapes.insert(rhs.m_shapes.end(), to_split.begin(), to_split.end());
}

void ConnComponent::extendBoundingBox(Point2& bMin, Point2& bMax)const
{
	bMin[0] = std::min(bMin[0], m_boundMin[0]);
	bMin[1] = std::min(bMin[1], m_boundMin[1]);
	bMax[0] = std::max(bMax[0], m_boundMax[0]);
	bMax[1] = std::max(bMax[1], m_boundMax[1]);
}

void ConnComponent::samplePointsOnUints(double step)
{
	m_numSamplePoints = 0;
	for (int i = 0; i < numUnits(); i++)
	{
		getUnit(i)->samplePts(step);
		m_numSamplePoints += getUnit(i)->numSamplePts();
	}
}

bool ConnComponent::isIntersectedWithBoundingBox(
	const Point2& bMin, const Point2& bMax)const
{
	for (int i = 0; i < numUnits(); i++)
	{
		if (getUnit(i)->isIntersectedWithBoundingBox(bMin, bMax))
			return true;
	}
	return false;
}

void ConnComponent::setAllUnitsDisabled(bool ds)
{
	for (int i = 0; i < numUnits(); i++)
		getUnit(i)->setDisabled(ds);
}

void ConnComponent::setSelectedUnitsDisabled(bool ds)
{
	for (int i = 0; i < numUnits(); i++)
	if (getUnit(i)->isSelected())
		getUnit(i)->setDisabled(ds);
}

void ConnComponent::removeSelectedUnits()
{
	m_numSamplePoints = 0;
	std::vector<std::shared_ptr<BaseShape>> tmp;
	for (int i = 0; i < numUnits(); i++)
	if (!m_shapes[i]->isSelected())
	{
		tmp.push_back(m_shapes[i]);
		m_numSamplePoints += tmp.back()->numSamplePts();
	}
	m_shapes.swap(tmp);
}

void ConnComponent::removeMatchedUnits()
{
	m_numSamplePoints = 0;
	std::vector<std::shared_ptr<BaseShape>> tmp;
	for (int i = 0; i < numUnits(); i++)
	if (!m_shapes[i]->isMatched())
	{
		tmp.push_back(m_shapes[i]);
		m_numSamplePoints += tmp.back()->numSamplePts();
	}
	m_shapes.swap(tmp);
}

void ConnComponent::setSelectedUnitsMatched(bool ds)
{
	for (int i = 0; i < numUnits(); i++)
	if (getUnit(i)->isSelected())
		getUnit(i)->setMatched(ds);
}

void ConnComponent::setAllUnitsMatched(bool ds)
{
	for (int i = 0; i < numUnits(); i++)
		getUnit(i)->setMatched(ds);
}

void ConnComponent::setSelectedUnitsIgnored(bool ds)
{
	for (int i = 0; i < numUnits(); i++)
	if (getUnit(i)->isSelected())
		getUnit(i)->setIgnored(ds);

	m_numSamplePoints = 0;
	for (int i = 0; i < numUnits(); i++)
		m_numSamplePoints += getUnit(i)->numSamplePts();
}

void ConnComponent::setAllUnitsIgnored(bool ds)
{
	for (int i = 0; i < numUnits(); i++)
		getUnit(i)->setIgnored(ds);

	m_numSamplePoints = 0;
	for (int i = 0; i < numUnits(); i++)
		m_numSamplePoints += getUnit(i)->numSamplePts();
}

void ConnComponent::setSelectedUnitsNew(bool ds)
{
	for (int i = 0; i < numUnits(); i++)
	if (getUnit(i)->isSelected())
		getUnit(i)->setNewUnit(ds);
}

void ConnComponent::setAllUnitsNew(bool ds)
{
	for (int i = 0; i < numUnits(); i++)
		getUnit(i)->setNewUnit(ds);
}

void ConnComponent::setAllUnitsSelected(bool s)const
{
	for (int i = 0; i < numUnits(); i++)
		getUnit(i)->setSelected(s);
}

void ConnComponent::BaseShape::samplePts(double step)
{
	m_ptsSamples.clear();
	if (numPoints() == 0)
		return;
	if (isIgnored())
		return;
	Point2 lp = getPoint(0);
	for (int i = 0; i <= numPoints(); i++)
	{
		if (i == numPoints() && !isClosed())
			break;
		int pos = i % numPoints();
		Point2 p = getPoint(pos);
		Point2 dif = p - lp;
		double len = dif.length();
		if (len < step) continue;
		dif /= len;
		for (double s = 0; s < len; s += step)
			m_ptsSamples.push_back(lp + s * dif);
		lp = p;
	}
}

ConnComponent::BaseShape* ConnComponent::BaseShape::create(ConnComponent::BaseShape::Type type)
{
	switch (type)
	{
	case ConnComponent::BaseShape::Line:
		return new ConnComponent::Line();
		break;
	case ConnComponent::BaseShape::PolyLine:
		return new ConnComponent::PolyLine();
		break;
	case ConnComponent::BaseShape::Polygon:
		return new ConnComponent::Polygon();
		break;
	case ConnComponent::BaseShape::Path:
		return new ConnComponent::Path();
		break;
	default:
		assert(0);
		return nullptr;
		break;
	}
}

double ConnComponent::BaseShape::length()const
{
	const int n = numPoints();
	double len = 0;
	if (n)
	{
		Point2 lp = getPoint(0);
		for (int i = 1; i < numPoints(); i++)
		{
			Point2 p = getPoint(i);
			len += (p - lp).length();
			lp = p;
		}
	}
	return len;
}

bool ConnComponent::BaseShape::isIntersectedWithBoundingBox(
	const Point2& bMin, const Point2& bMax)const
{
	const int n = numPoints();
	for (int i = 0; i < n; i++)
	{
		Point2 p = getPoint(i);
		if (p[0] >= bMin[0] && p[0] < bMax[0]
			&& p[1] >= bMin[1] && p[1] < bMax[1])
			return true;
	}
	return false;
}

bool ConnComponent::BaseShape::load(FILE* pFile)
{
	return 1 == fread(&m_label, sizeof(int), 1, pFile);
}

bool ConnComponent::BaseShape::save(FILE* pFile)const
{
	return 1 == fwrite(&m_label, sizeof(int), 1, pFile);
}

void ConnComponent::BaseShape::extendBoundingBox(Point2& bMin, Point2& bMax)const
{
	for (int i = 0; i < numPoints(); i++)
	{
		Point2 p = getPoint(i);
		bMin[0] = std::min(bMin[0], p[0]);
		bMin[1] = std::min(bMin[1], p[1]);
		bMax[0] = std::max(bMax[0], p[0]);
		bMax[1] = std::max(bMax[1], p[1]);
	}
}

bool ConnComponent::BaseShape::isDuplicatedWith(const BaseShape* other)const
{
	if (getType() != other->getType())
		return false;
	if (numPoints() != other->numPoints())
		return false;

	for (int i = 0; i < numPoints(); i++)
	{
		if (getPoint(i) != other->getPoint(i))
			return false;
	}
	return true;
}

bool ConnComponent::Line::load(FILE* pFile)
{
	if (!BaseShape::load(pFile))
		return false;
	return numPoints() == fread(m_pts, sizeof(Point2), numPoints(), pFile);
}

bool ConnComponent::Line::save(FILE* pFile)const
{
	if (!BaseShape::save(pFile))
		return false;
	return numPoints() == fwrite(m_pts, sizeof(Point2), numPoints(), pFile);
}

void ConnComponent::Line::build(int n, const Point2* pts)
{
	assert(n == 2);
	m_pts[0] = pts[0];
	m_pts[1] = pts[1];
}

bool ConnComponent::PolyLine::load(FILE* pFile)
{
	if (!BaseShape::load(pFile))
		return false;
	int n = 0;
	fread(&n, sizeof(int), 1, pFile);
	m_pts.resize(n);
	return numPoints() == fread(m_pts.data(), sizeof(Point2), numPoints(), pFile);
}

bool ConnComponent::PolyLine::save(FILE* pFile)const
{
	if (!BaseShape::save(pFile))
		return false;
	int n = numPoints();
	fwrite(&n, sizeof(int), 1, pFile);
	return numPoints() == fwrite(m_pts.data(), sizeof(Point2), numPoints(), pFile);
}

void ConnComponent::PolyLine::build(int n, const Point2* pts)
{
	m_pts.assign(pts, pts + n);
}

bool ConnComponent::Polygon::load(FILE* pFile)
{
	if (!BaseShape::load(pFile))
		return false;
	int n = 0;
	fread(&n, sizeof(int), 1, pFile);
	m_pts.resize(n);
	return numPoints() == fread(m_pts.data(), sizeof(Point2), numPoints(), pFile);
}

bool ConnComponent::Polygon::save(FILE* pFile)const
{
	if (!BaseShape::save(pFile))
		return false;
	int n = numPoints();
	fwrite(&n, sizeof(int), 1, pFile);
	return numPoints() == fwrite(m_pts.data(), sizeof(Point2), numPoints(), pFile);
}

void ConnComponent::Polygon::build(int n, const Point2* pts)
{
	m_pts.assign(pts, pts + n);
}

bool ConnComponent::Path::load(FILE* pFile)
{
	if (!BaseShape::load(pFile))
		return false;
	int closeFlag = 0;
	fread(&closeFlag, sizeof(int), 1, pFile);
	m_bClosed = (closeFlag!=0);
	int n = 0;
	fread(&n, sizeof(int), 1, pFile);
	m_pts.resize(n);
	return numPoints() == fread(m_pts.data(), sizeof(Point2), numPoints(), pFile);
}

bool ConnComponent::Path::save(FILE* pFile)const
{
	if (!BaseShape::save(pFile))
		return false;
	fwrite(&m_bClosed, sizeof(int), 1, pFile);
	int n = numPoints();
	fwrite(&n, sizeof(int), 1, pFile);
	return numPoints() == fwrite(m_pts.data(), sizeof(Point2), numPoints(), pFile);
}

void ConnComponent::Path::build(int n, const Point2* pts)
{
	m_pts.assign(pts, pts + n);
}