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
		std::shared_ptr<SvgAbstractObject> subPath(int cmdsBegin, int cmdsEnd,
			bool to_single_segment = false)const;
		std::shared_ptr<SvgAbstractObject> subPath(const std::vector<int>& cmdsBegins,
			const std::vector<int>& cmdsEnds, bool to_single_segment = false)const;
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

	public:
		struct GLPathResource{
			GLuint id;
			GLPathResource();
			~GLPathResource();
		};
		std::shared_ptr<GLPathResource> m_gl_path_res;
	};
}