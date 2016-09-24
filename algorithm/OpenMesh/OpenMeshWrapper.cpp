#include "OpenMeshWrapper.h"
extern "C"{
#include "triangle.h"
};
#ifdef NDEBUG
#pragma comment(lib, "OpenMeshCore.lib")
#pragma comment(lib, "OpenMeshTools.lib")
#else
#pragma comment(lib, "OpenMeshCored.lib")
#pragma comment(lib, "OpenMeshToolsd.lib")
#endif

namespace OpenMeshWrapper
{
	typedef ldp::Double3 VEC3D;
	typedef ldp::Double2 VEC2D;
#pragma region --triangle
	VEC2D getUV(const VEC3D &plane_pt, const VEC3D &U_dir, const VEC3D &V_dir, const VEC3D &pt)
	{
		VEC2D uv_para;
		VEC3D dir_3d = pt - plane_pt;
		uv_para[0] = dir_3d.dot(U_dir);
		uv_para[1] = dir_3d.dot(V_dir);
		return uv_para;
	}
	void build_plane_UV(const VEC3D &plane_normal, VEC3D &plane_U, VEC3D &plane_V)
	{
		if (fabs(plane_normal[2])>1e-3f)
		{
			plane_U[0] = -plane_normal[2]; plane_U[1] = 0; plane_U[2] = plane_normal[0];
		}
		else if (fabs(plane_normal[1])>1e-3f)
		{
			plane_U[0] = -plane_normal[1]; plane_U[1] = plane_normal[0]; plane_U[2] = 0;
		}
		else
		{
			plane_U[0] = 0;	plane_U[1] = -plane_normal[2]; plane_U[2] = plane_normal[1];
		}
		plane_U.normalizeLocal();
		plane_V = plane_normal.cross(plane_U);
		plane_V.normalizeLocal();
	}
	
	void destroyMem_trianglulateio(struct triangulateio *io)
	{
		if (io->pointlist)  free(io->pointlist);                                               /* In / out */
		if (io->pointattributelist) free(io->pointattributelist);                                      /* In / out */
		if (io->pointmarkerlist) free(io->pointmarkerlist);                                          /* In / out */

		if (io->trianglelist) free(io->trianglelist);                                             /* In / out */
		if (io->triangleattributelist) free(io->triangleattributelist);                                   /* In / out */
		if (io->trianglearealist) free(io->trianglearealist);                                    /* In only */
		if (io->neighborlist) free(io->neighborlist);                                           /* Out only */

		if (io->segmentlist) free(io->segmentlist);                                              /* In / out */
		if (io->segmentmarkerlist) free(io->segmentmarkerlist);                             /* In / out */

		if (io->holelist) free(io->holelist);                        /* In / pointer to array copied out */

		if (io->regionlist) free(io->regionlist);                      /* In / pointer to array copied out */

		if (io->edgelist) free(io->edgelist);                                                 /* Out only */
		if (io->edgemarkerlist) free(io->edgemarkerlist);           /* Not used with Voronoi diagram; out only */
		if (io->normlist) free(io->normlist);              /* Used only with Voronoi diagram; out only */

		//all set to 0
		init_trianglulateio(io);
	}

	void init_triangulateIO_from_poly(triangulateio *pIO, const std::vector<VEC2D> &poly, int close_flag)
	{
		init_trianglulateio(pIO);
		pIO->numberofpoints = (int)poly.size();
		pIO->pointlist = (REAL *)malloc(pIO->numberofpoints * 2 * sizeof(REAL));
		for (int i = 0; i<pIO->numberofpoints; i++)
		{
			pIO->pointlist[i * 2] = poly[i][0];
			pIO->pointlist[i * 2 + 1] = poly[i][1];
		}
		/*********input segments************/
		pIO->numberofsegments = (int)poly.size() - 1;
		if (close_flag) pIO->numberofsegments++;
		pIO->segmentlist = (int *)malloc(pIO->numberofsegments * 2 * sizeof(int));
		for (int i = 0; i<(int)poly.size() - 1; i++)
		{
			pIO->segmentlist[i * 2] = i;
			pIO->segmentlist[i * 2 + 1] = i + 1;
		}
		if (close_flag)
		{
			pIO->segmentlist[pIO->numberofsegments * 2 - 2] = (int)poly.size() - 1;
			pIO->segmentlist[pIO->numberofsegments * 2 - 1] = 0;
		}
	}

	int tess_poly_2D(const std::vector<VEC2D> &poly, std::vector<int> &vTriangle, int close_flag)
	{
		vTriangle.resize(0);

		triangulateio input, triout, vorout;

		/* Define input points. */
		init_triangulateIO_from_poly(&input, poly, close_flag);

		/* Make necessary initializations so that Triangle can return a */
		/*   triangulation in `mid' and a voronoi diagram in `vorout'.  */
		init_trianglulateio(&triout);
		init_trianglulateio(&vorout);

		/*****triangulate**************/
		triangulate("pz", &input, &triout, &vorout);

		//if vertex change, no need!!!!
		if (triout.numberofpoints != input.numberofpoints || triout.numberoftriangles<1)
		{
			return 0;
		}

		vTriangle.resize(triout.numberoftriangles * 3);
		memcpy(&vTriangle[0], triout.trianglelist, sizeof(int)*triout.numberoftriangles * 3);

		destroyMem_trianglulateio(&input);
		destroyMem_trianglulateio(&triout);
		destroyMem_trianglulateio(&vorout);


		return vTriangle.size();
	}
	
	int tess_poly_3D(const std::vector<VEC3D> &poly, std::vector<int> &vTriangle, int close_flag)
	{
		if (poly.size()<3)
		{
			return 0;
		}
		const VEC3D &p0 = poly[0];
		const VEC3D &p1 = poly[1];
		const VEC3D &p2 = poly[2];
		VEC3D fn = VEC3D(p1 - p0).cross(p2 - p1);
		fn.normalizeLocal();
		VEC3D fu, fv;
		build_plane_UV(fn, fu, fv);
		std::vector<VEC2D> poly2D(poly.size());
		VEC2D tmp;
		for (int i = 0; i<(int)poly.size(); i++)
		{
			tmp = getUV(p0, fu, fv, poly[i]);
			poly2D[i][0] = tmp[0];
			poly2D[i][1] = tmp[1];
		}

		return tess_poly_2D(poly2D, vTriangle, close_flag);
	}
#pragma endregion

	void objMeshToOpenMesh(const ObjMesh& objmesh, TriMesh& openMesh)
	{
		std::vector<TriMesh::VertexHandle> vhandles(objmesh.vertex_list.size());

		for (int vid = 0; vid < objmesh.vertex_list.size(); vid++)
		{
			const ldp::Float3& v = objmesh.vertex_list[vid];
			vhandles[vid] = openMesh.add_vertex(TriMesh::Point(v.ptr()));
		}// end for vid

		for (int fid = 0; fid < objmesh.face_list.size(); fid++)
		{
			const ObjMesh::obj_face& f = objmesh.face_list[fid];

			TriMesh::VertexHandle fvhandles[3];
			for (int kb = 0; kb < f.vertex_count - 2; kb++)
			{
				fvhandles[0] = vhandles[f.vertex_index[0]];
				fvhandles[1] = vhandles[f.vertex_index[kb + 1]];
				fvhandles[2] = vhandles[f.vertex_index[kb + 2]];
				openMesh.add_face(fvhandles, 3);
			}
		}// end for fid
	}

	void openMeshToObjMesh(const TriMesh& openMesh, ObjMesh& objmesh)
	{
		throw std::exception("bugs here");
		objmesh.clear();

		for (TriMesh::ConstVertexIter it = openMesh.vertices_begin(); it != openMesh.vertices_end(); ++it)
		{
			TriMesh::Point p = openMesh.point(*it);
			objmesh.vertex_list.push_back(ldp::Float3(p[0], p[1], p[2]));
		}

		for (TriMesh::ConstFaceIter it = openMesh.faces_begin(); it != openMesh.faces_end(); ++it)
		{
			ObjMesh::obj_face f;
			f.material_index = -1;
			f.vertex_count = 0;
			for (TriMesh::ConstFaceVertexIter fviter = openMesh.cfv_begin(*it);
				fviter != openMesh.cfv_end(*it); ++fviter)
			{
				openMesh.vertex(*fviter);
				f.vertex_index[f.vertex_count++] = fviter->idx();
			}
			objmesh.face_list.push_back(f);
		}

		objmesh.updateNormals();
		objmesh.updateBoundingBox();
	}

	void triangulate_poly(const std::vector<ldp::Float3>& verts,
		std::vector<ldp::Int3>& tris)
	{
#if 0
		PolyMesh mesh;
		
		std::vector<PolyMesh::VertexHandle> vhandles(verts.size());
		for (size_t i = 0; i < verts.size(); i++)
		{
			ldp::Float3 p = verts[i];
			vhandles[i] = mesh.add_vertex(PolyMesh::Point(p[0], p[1], p[2]));
		}

		mesh.add_face(vhandles);

		mesh.triangulate();

		tris.clear();
		for (auto fiter = mesh.faces_begin(); fiter != mesh.faces_end(); ++fiter)
		{
			ldp::Int3 v;
			int cnt = 0;
			for (auto viter = mesh.cfv_begin(fiter.handle()); viter != mesh.cfv_end(fiter.handle()); ++viter)
			{
				assert(cnt < 3);
				v[cnt++] = viter->idx();
			}
			tris.push_back(v);
		}

		OpenMesh::IO::write_mesh(mesh, "data/tmp.obj");
#else
		std::vector<VEC3D> vd(verts.size());
		for (size_t i = 0; i < vd.size(); i++)
			vd[i] = verts[i];
		std::vector<int> fd;
		tess_poly_3D(vd, fd, true);
		tris.resize(fd.size()/3);
		for (size_t i = 0; i < fd.size() / 3; i++)
			tris[i] = ldp::Int3(fd[3*i], fd[3*i+1], fd[3*i+2]);
#endif
	}
}