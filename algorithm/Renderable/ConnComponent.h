#pragma once
#include <stdio.h>
#include "ldp_basic_mat.h"
#include "camera\Camera.h"
class ConnComponent
{
public:
	typedef ldp::Double2 Point2;
	class BaseShape
	{
	public:
		enum Type
		{
			Line,
			PolyLine,
			Polygon,
			Path
		};
		enum LabelType
		{
			LabelTypeNew = 0x01,
			LabelTypeDisabled = 0x02,
			LabelTypeMatched = 0x04,
			LabelTypeIgnored = 0x08,
		};
	protected:
		int m_label;
		mutable bool m_selectFlag;
		std::vector<Point2> m_ptsSamples;
	public:
		BaseShape(){ m_label = 0; m_selectFlag = false; }
		int getLabel()const { return m_label; }
		void setLabel(int label){ m_label = label; }
		bool isNewUnit()const{ return (m_label & LabelTypeNew) != 0; }
		bool isDisabled()const{ return (m_label & LabelTypeDisabled) != 0; }
		bool isMatched()const{ return (m_label & LabelTypeMatched) != 0; }
		bool isIgnored()const{ return (m_label & LabelTypeIgnored) != 0; }
		bool isSelected()const{ return m_selectFlag; }
		bool isDuplicatedWith(const BaseShape* other)const;
		void setSelected(bool s)const{ m_selectFlag = s; }
		void setDisabled(bool ds){
			if (ds)
				m_label |= LabelTypeDisabled;
			else
				m_label = (m_label | LabelTypeDisabled) - LabelTypeDisabled;
		}
		void setMatched(bool ds){
			if (ds)
				m_label |= LabelTypeMatched;
			else
				m_label = (m_label | LabelTypeMatched) - LabelTypeMatched;
		}
		void setIgnored(bool ds){
			if (ds)
			{
				m_label |= LabelTypeIgnored;
				m_ptsSamples.clear();
			}
			else
				m_label = (m_label | LabelTypeIgnored) - LabelTypeIgnored;
		}
		void setNewUnit(bool ds){
			if (ds)
				m_label |= LabelTypeNew;
			else
				m_label = (m_label | LabelTypeNew) - LabelTypeNew;
		}
		double length()const;
		bool isIntersectedWithBoundingBox(const Point2& bMin, const Point2& bMax)const;
		virtual bool isClosed()const = 0;
		virtual int getType()const = 0;
		virtual int numEndPoints()const = 0;
		virtual Point2 getEndPoint(int i)const = 0;
		virtual int numPoints()const = 0;
		virtual Point2 getPoint(int i)const = 0;
		virtual void setPoint(int i, Point2 p) = 0;
		virtual bool load(FILE* pFile) = 0;
		virtual bool save(FILE* pFile)const = 0;
		virtual void build(int n, const Point2* pts) = 0;
		virtual void extendBoundingBox(Point2& bMin, Point2& bMax)const;
		void samplePts(double step);
		inline int numSamplePts()const{ return (int)m_ptsSamples.size(); }
		inline Point2 getSamplePt(int i)const{ return m_ptsSamples[i]; }
		inline Point2 getSamplePtDir(int i)const{ 
			int ip = (i > 0 ? i-1 : i);
			int in = (i < numSamplePts() - 1 ? i + 1 : i);
			return (getSamplePt(in) - getSamplePt(ip)).normalize();
		}
		inline void setSamplePt(int i, Point2 p){ m_ptsSamples[i] = p; }
		static BaseShape* create(Type type);
	};
	class Line : public BaseShape
	{
	public:
		Line() :BaseShape(){}
		virtual int getType()const { return Type::Line; }
		virtual bool isClosed()const { return false; }
		virtual int numEndPoints()const { return 2; }
		virtual Point2 getEndPoint(int i)const { return m_pts[i]; }
		virtual int numPoints()const { return 2; }
		virtual Point2 getPoint(int i)const { return m_pts[i]; }
		virtual void setPoint(int i, Point2 p) { m_pts[i] = p; }
		virtual bool load(FILE* pFile);
		virtual bool save(FILE* pFile)const;
		virtual void build(int n, const Point2* pts);
	protected:
		Point2 m_pts[2];
	};
	class PolyLine : public BaseShape
	{
	public:
		PolyLine() :BaseShape(){}
		virtual int getType()const { return Type::PolyLine; }
		virtual bool isClosed()const { return false; }
		virtual int numEndPoints()const { return 2; }
		virtual Point2 getEndPoint(int i)const { return i == 0 ? m_pts[0] : m_pts.back(); }
		virtual int numPoints()const { return (int)m_pts.size(); }
		virtual Point2 getPoint(int i)const { return m_pts[i]; }
		virtual void setPoint(int i, Point2 p) { m_pts[i] = p; }
		virtual bool load(FILE* pFile);
		virtual bool save(FILE* pFile)const;
		virtual void build(int n, const Point2* pts);
	protected:
		std::vector<Point2> m_pts;
	};
	class Path : public BaseShape
	{
	public:
		Path() :BaseShape(){ m_bClosed = false; }
		virtual int getType()const { return Type::Path; }
		virtual bool isClosed()const { return m_bClosed; }
		virtual int numEndPoints()const { return m_bClosed ? 0 : 2; }
		virtual Point2 getEndPoint(int i)const { return i == 0 ? m_pts[0] : m_pts.back(); }
		virtual int numPoints()const { return (int)m_pts.size(); }
		virtual Point2 getPoint(int i)const { return m_pts[i]; }
		virtual void setPoint(int i, Point2 p) { m_pts[i] = p; }
		virtual bool load(FILE* pFile);
		virtual bool save(FILE* pFile)const;
		virtual void build(int n, const Point2* pts);
	protected:
		std::vector<Point2> m_pts;
		bool m_bClosed;
	};
	class Polygon : public BaseShape
	{
	public:
		Polygon() :BaseShape(){}
		virtual int getType()const { return Type::Polygon; }
		virtual bool isClosed()const { return true; }
		virtual int numEndPoints()const { return (int)m_pts.size(); }
		virtual Point2 getEndPoint(int i)const { return m_pts[i]; }
		virtual int numPoints()const { return (int)m_pts.size(); }
		virtual Point2 getPoint(int i)const { return m_pts[i]; }
		virtual void setPoint(int i, Point2 p) { m_pts[i] = p; }
		virtual bool load(FILE* pFile);
		virtual bool save(FILE* pFile)const;
		virtual void build(int n, const Point2* pts);
	protected:
		std::vector<Point2> m_pts;
	};
public:
	ConnComponent();
	~ConnComponent();

	bool load(const char* filename);
	bool load(FILE* pFile);
	bool save(const char* filename)const;
	bool save(FILE* pFile)const;

	void mergeWith(const ConnComponent& rhs);

	void splitSelectedTo(ConnComponent& rhs);

	int numUnits()const{ return (int)m_shapes.size(); }
	const BaseShape* getUnit(int i)const{ return m_shapes[i].get(); }
	BaseShape* getUnit(int i){ return m_shapes[i].get(); }
	void extendBoundingBox(Point2& bMin, Point2& bMax)const;
	void samplePointsOnUints(double step);
	int numSamplePoints()const{ return m_numSamplePoints; }
	bool isDirty()const { return m_dirtyFlag != 0; }
	void resetDirtyFlag()const{ m_dirtyFlag = 0; }
	void setDirty()const{ m_dirtyFlag = 1; }
	bool isIntersectedWithBoundingBox(const Point2& bMin, const Point2& bMax)const;
	void setAllUnitsDisabled(bool ds);
	void setSelectedUnitsDisabled(bool ds);
	void setAllUnitsMatched(bool ds);
	void setSelectedUnitsMatched(bool ds);
	void setAllUnitsIgnored(bool ds);
	void setSelectedUnitsIgnored(bool ds);
	void setAllUnitsNew(bool ds);
	void setSelectedUnitsNew(bool ds);
	void setAllUnitsSelected(bool s)const; 
	void removeSelectedUnits();
	void removeMatchedUnits();
	bool isSelected()const{ return m_isSelected; }
	void setSelected(bool s)const{ m_isSelected = s; }
private:
	std::vector<std::shared_ptr<BaseShape>> m_shapes;
	Point2 m_boundMin;
	Point2 m_boundMax;
	int m_numSamplePoints;
	mutable int m_dirtyFlag;
	mutable int m_isSelected;
};

class ConnComponents
{
public:
	enum SelectType
	{
		SelectType_Add,
		SelectType_Erase,
		SelectType_Only,
	};
public:
	typedef ConnComponent::Point2 Point2;

	bool load(const char* filename);
	bool load(FILE* pFile);
	bool save(const char* filename)const;
	bool save(FILE* pFile)const;
	void clear();

	Point2 convertToGlViewport(Point2 p, const ldp::Camera& cam)const;
	Point2 convertFromGlViewport(Point2 p, const ldp::Camera& cam)const;

	int size()const{ return m_data.size(); }
	ConnComponent* getComponent(int i){ return m_data[i].get(); }
	const ConnComponent* getComponent(int i)const{ return m_data[i].get(); }
	void samplePointsOnUintsAndConvertToGlViewport(double stepOnGlView, const ldp::Camera& cam);
	void resetComponentDirtyFlag()const;
	ldp::Float2 getCenter()const{ return m_center; }
	ldp::Float2 getWH()const{ return m_wh; }
	float scale_fromGL(const ldp::Camera& cam)const;
	void mergeSelectedConn();
	void splitComponetSelectedUnits(int iConn);

	void selectUnitsViaGLBox(Point2 bMinGL, Point2 bMaxGL, const ldp::Camera& cam, SelectType stype)const;
	void clearSelection()const;
	void setSelectedUnitsDisabled(bool ds);
	void setAllUnitsDisabled(bool ds);
	void setSelectedUnitsMatched(bool ds);
	void setAllUnitsMatched(bool ds);
	void setSelectedUnitsIgnored(bool ds);
	void setAllUnitsIgnored(bool ds);
	void setSelectedUnitsNew(bool ds);
	void setAllUnitsNew(bool ds);
	void removeSelectedUnits();
	void removeMatchedUnits();
private:
	std::vector<std::shared_ptr<ConnComponent>> m_data;
	Point2 m_center;
	Point2 m_wh;
};

typedef ConnComponent::BaseShape BaseShape;
