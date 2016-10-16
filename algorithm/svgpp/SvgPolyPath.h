#pragma once
#include "GL\glew.h"
#include "SvgPath.h"
#include "PathStyle.hpp"
#include <set>
namespace svg
{
	class SvgPolyPath;
	struct SvgEdgeGroup
	{
		std::set<std::pair<SvgPolyPath*, int>> group;
		ldp::Float3 color;
		bool intersect(const SvgEdgeGroup& rhs);
		void mergeWith(const SvgEdgeGroup& rhs);
		TiXmlElement* toXML(TiXmlNode* parent)const;
	};

	class SvgPolyPath : public SvgPath
	{
	public:
		SvgPolyPath();
		SvgPolyPath(int id);
		virtual ~SvgPolyPath();
		ObjectType objectType()const { return ObjectType::PolyPath; }

		virtual void render();
		virtual void renderId();
		virtual std::shared_ptr<SvgAbstractObject> clone(bool selectedOnly = false)const;
		// similar with clone, bug GL context also cloned.
		virtual std::shared_ptr<SvgAbstractObject> deepclone(bool selectedOnly = false)const;
		virtual TiXmlElement* toXML(TiXmlNode* parent)const;
		virtual void copyTo(SvgAbstractObject* obj)const;
		virtual int numId()const;
		virtual void setSelected(bool s, int idx = -1); // idx: m_id + corner/edge_id
		virtual void setHighlighted(bool s, int idx = -1); // idx: m_id + corner/edge_id

		// given this polygon (closed or non-closed), we find all the corners
		void findCorners();

		bool isClosed()const;
		void makeClosed();
		int numCorners()const{ return (int)m_cornerPos.size(); }
		const int numCornerEdges()const{ return numCorners() - !isClosed(); }
		ldp::Float2 getCorner(int i)const{ 
			return ldp::Float2(m_coords[m_cornerPos[i] * 2], 
			m_coords[m_cornerPos[i] * 2 + 1]); 
		}
		std::set<int> selectedEdgeIds()const{
			std::set<int> r;
			for (auto c : m_selectedCorner_arrayIds){
				if (c < 0 || c >= numCornerEdges()) continue;
				r.insert(c);
			}
			return r;
		}
		std::set<int> selectedCornerIds()const{
			std::set<int> r;
			for (auto c : m_selectedCorner_arrayIds){
				if (c < numCornerEdges() || c >= numId()) continue;
				r.insert(c - numCornerEdges());
			}
			return r;
		}
		std::set<int> highlighedEdgeIds()const{
			std::set<int> r;
			for (auto c : m_highlightedCorner_arrayIds){
				if (c < 0 || c >= numCornerEdges()) continue;
				r.insert(c);
			}
			return r;
		}		
		std::set<int> highlighedCornerIds()const{
			std::set<int> r;
			for (auto c : m_highlightedCorner_arrayIds){
				if (c < numCornerEdges() || c >= numId()) continue;
				r.insert(c - numCornerEdges());
			}
			return r;
		}
		int globalIdFromEdgeId(int id)const{
			if (id < 0 || id >= numCornerEdges()) return -1;
			return m_id + id;
		}
		int globalIdFromCornerId(int id)const{
			if (id < 0 || id >= numCorners()) return -1;
			return m_id + numCornerEdges() + id;
		}
		int edgeIdFromGlobalId(int id)const{
			id -= m_id;
			if (id < 0 || id >= numCornerEdges()) return -1;
			return id;
		}
		int cornerIdFromGlobalId(int id)const{
			id -= m_id + numCornerEdges();
			if (id < 0 || id >= numCorners()) return -1;
			return id;
		}
		const std::set<const SvgEdgeGroup*>& edgeGroups()const{ return m_edgeGroups; }
		std::set<const SvgEdgeGroup*>& edgeGroups(){ return m_edgeGroups; }
	protected:
		void cacheNvPaths();
		void renderSelection(bool idxMode = false);
		void configNvParams();
		void updateEdgeRenderData();
	protected:
		std::vector<int> m_cornerPos;
		std::set<int> m_selectedCorner_arrayIds; // LDP TO DO: enable multi-selection
		std::set<int> m_highlightedCorner_arrayIds;

		// corner-splitted path data, for rendering
		std::vector<std::vector<GLubyte>> m_edgeCmds;
		std::vector<std::vector<float>> m_edgeCoords;
		std::vector<std::shared_ptr<GLPathResource>> m_edgeGLIds;

		// is this edge related with others?
		std::set<const SvgEdgeGroup*> m_edgeGroups;
	};
}