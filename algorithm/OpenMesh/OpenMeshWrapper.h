#pragma once

#include <Core\IO\MeshIO.hh>
#include <Core\Mesh\TriMeshT.hh>
#include <Core\Mesh\TriMesh_ArrayKernelT.hh>
#include <Tools\Decimater\DecimaterT.hh>
#include <Tools\Decimater\ModQuadricT.hh>
#include <Tools\Decimater\ModProgMeshT.hh>
#include <Tools\Decimater\ModEdgeLengthT.hh>
#include <Tools\Decimater\ModHausdorffT.hh>
#include <Tools\Decimater\ModIndependentSetsT.hh>
#include <Tools\Decimater\ModNormalDeviationT.hh>
#include <Tools\Decimater\ModAspectRatioT.hh>
#include <Tools\Dualizer\meshDualT.hh>
#include <Tools\Smoother\SmootherT.hh>
#include <Tools\Subdivider\Adaptive\Composite\CompositeT.hh>
#include <Tools\Subdivider\Uniform\SubdividerT.hh>
#include <Tools\VDPM\VHierarchyWindow.hh>

#include <ObjMesh.h>

namespace OpenMeshWrapper
{
	typedef OpenMesh::TriMesh_ArrayKernelT<>  TriMesh;
	typedef OpenMesh::PolyMesh_ArrayKernelT<>  PolyMesh;

	void objMeshToOpenMesh(const ObjMesh& objmesh, TriMesh& openMesh);
	void openMeshToObjMesh(const TriMesh& openMesh, ObjMesh& objmesh);
	
	void triangulate_poly(const std::vector<ldp::Float3>& verts,
		std::vector<ldp::Int3>& tris);
}

