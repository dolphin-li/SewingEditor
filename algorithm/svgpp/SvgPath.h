#pragma once
#include "GL\glew.h"
#include "SvgAbstractObject.h"
#include "PathStyle.hpp"
namespace svg
{
	class SvgPath : public SvgAbstractObject
	{
	public:
		// NOTE: this are paths specific for burdastyle generated garments.
		enum PathUnitShapes
		{
			Unknown = 0x0,
			Solid = 0x01, // solid lines
			Dash = 0x02, // '-----'
			LongShort = 0x04, // '_-_-_-'
			Cross = 0x08, // '-x-x-x'
			Quad = 0x10, // '-q-q-q', where 'q' means a quad
			Circle = 0x20, // 'o'
		};
	public:
		SvgPath();
		~SvgPath();
		ObjectType objectType()const { return ObjectType::Path; }

		virtual void render();
		virtual void renderId();
		virtual std::shared_ptr<SvgAbstractObject> clone()const;
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
		std::shared_ptr<SvgAbstractObject> subPath(int cmdsBegin, int cmdsEnd)const;
		// cmdsPoses: the last element is excluded
		std::shared_ptr<SvgAbstractObject> subPath(const std::vector<int>& cmdsPoses)const;
	protected:
		void cacheNvPaths();
		void renderSelection();
		void configNvParams();
	public:
		std::vector<GLubyte> m_cmds;
		std::vector<int> m_segmentPos;
		std::vector<GLfloat> m_coords;
		GLenum m_gl_fill_rull;
		PathStyle m_pathStyle;
		PathUnitShapes m_pathShape;

	public:
		struct GLPathResource{
			GLuint id;
			GLPathResource();
			~GLPathResource();
		};
		std::shared_ptr<GLPathResource> m_gl_path_res;
	};
}