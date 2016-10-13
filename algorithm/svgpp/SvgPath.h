#pragma once
#include "GL\glew.h"
#include "SvgAbstractObject.h"
#include "PathStyle.hpp"
namespace svg
{
	class SvgPath : public SvgAbstractObject
	{
	public:
		SvgPath();
		~SvgPath();
		ObjectType objectType()const { return ObjectType::Path; }

		virtual void render(PathUnitShapes shapeToRender = ShapeAll);
		virtual void renderId(PathUnitShapes shapeToRender = ShapeAll);
		virtual std::shared_ptr<SvgAbstractObject> clone(bool selectedOnly = false)const;
		virtual TiXmlElement* toXML(TiXmlNode* parent)const;
		virtual void copyTo(SvgAbstractObject* obj)const;
		virtual ldp::Float2 getStartPoint()const;
		virtual ldp::Float2 getEndPoint()const;

		// the path is closed or open
		bool isClosed()const;

		virtual void updateBoundFromGeometry();

		// split all segments to individual paths
		// if there is only one segment, we do not split and return an nullptr
		// else we return grouped splittings
		std::shared_ptr<SvgAbstractObject> splitToSegments()const;

		std::shared_ptr<SvgAbstractObject> splitToDifferentShapes()const;

		// extract path from cmdsBegin(include) to cmdsEnd(exclude)
		// to_single_segment will convert 'M-L-L-L' to 'M-L M-L M-L'
		std::shared_ptr<SvgAbstractObject> subPath(int cmdsBegin, int cmdsEnd,
			bool to_single_segment = false)const;
		std::shared_ptr<SvgAbstractObject> subPath(const std::vector<int>& cmdsBegins,
			const std::vector<int>& cmdsEnds, bool to_single_segment = false)const;
	protected:
		void cacheNvPaths();
		void renderSelection();
		void configNvParams();

		// assume m_cmds and m_coords are all 'M-L-M-L...' types and ordered
		// then we decide whether the segments, from a given cmd postion, is the given pattern
		bool isOrderedSegmentsCrossPattern(int cmdPos)const;
		bool isOrderedSegmentsQuadPattern(int cmdPos)const;
		bool isOrderedSolidPattern(int cmdPos)const;
		bool isOrderedTinyDashPattern(int cmdPos, int& nCmdLength)const;
		bool isOrderedMidDashPattern(int cmdPos, int& nCmdLength)const;
		bool isOrderedLongShortPattern(int cmdPos, int& nCmdLength)const;
	public:
		std::vector<GLubyte> m_cmds;
		std::vector<GLfloat> m_coords;
		GLenum m_gl_fill_rull;
		PathStyle m_pathStyle;

		// return number of coordinates associated with a cmd
		static int numCoords(GLubyte cmd);
		static char svgCmd(GLubyte cmd);
		static const char* strokeFillMap(int fill);
		static const char* strokeLineCapMap(int cap);
		static const char* strokeLineJoinMap(int join);
	public:
		struct GLPathResource{
			GLuint id;
			GLPathResource();
			~GLPathResource();
		};
		std::shared_ptr<GLPathResource> m_gl_path_res;
	};
}