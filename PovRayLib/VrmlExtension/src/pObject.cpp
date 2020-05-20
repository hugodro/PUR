/**************************************************
* File: pObject.cc.
* Desc: Implementation of the PovObject class.
* Module: AkraLog : PovRayExt.
* Rev: 22 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "maths.h"
#include "pObject.h"


extern "C" {
#if defined(_WIN32)
#undef __cplusplus
#endif
#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "parse.h"
#include "parstxtr.h"
#include "atmosph.h"
#include "bezier.h"   
#include "blob.h"     
#include "boxes.h"
#include "colour.h"
#include "cones.h"    
#include "csg.h"      
#include "discs.h"
#include "express.h"  
#include "fractal.h"
#include "gif.h"      
#include "halos.h"
#include "hfield.h"
#include "iff.h"      
#include "image.h"    
#include "lathe.h"    
#include "polysolv.h"
#include "matrices.h"
#include "mesh.h"
#include "normal.h"
#include "objects.h"
#include "octree.h"
#include "pigment.h"
#include "planes.h"
#include "poly.h"
#include "polygon.h"
#include "povray.h"   
#include "pgm.h"      
#include "ppm.h"      
#include "prism.h"    
#include "quadrics.h" 
#include "radiosit.h"      
#include "render.h"   
#include "sor.h"      
#include "spheres.h"  
#include "super.h"
#include "targa.h"    
#include "texture.h"  
#include "tokenize.h" 
#include "torus.h"
#include "triangle.h" 
#include "truetype.h" 
void Transform_Cone PARAMS((OBJECT *Object, TRANSFORM *Trans));
void Transform_Mesh  PARAMS((OBJECT *Object, TRANSFORM *Trans));
extern METHODS Ellipsoid_Methods;

#if defined(_WIN32)
#define __cplusplus
#endif
}


static TRANSFORM *createTransform(TrMatrix *vrmlTransfo)
{
    TRANSFORM *result= Create_Transform();

    for (unsigned int i= 0; i < 3; i++) {
	(result->matrix)[0][i]= vrmlTransfo->M[i][0];
	(result->matrix)[1][i]= vrmlTransfo->M[i][1];
	(result->matrix)[2][i]= vrmlTransfo->M[i][2];
	(result->matrix)[3][i]= vrmlTransfo->M[i][3];
    }

    MInvers(result->inverse, result->matrix);
    return result;
}


/**************************************************
* Implementation: PovContainer.
**************************************************/

PovContainer::PovContainer(void *anObject)
{
    lowObject= anObject;
}


void *PovContainer::getLowObject(void)
{
    return lowObject;
}


/**************************************************
* Implementation: PovObject.
**************************************************/

PovObject *PovObject::createBox(TrMatrix *aTransfo, void **params)
{
    BOX *aBox= Create_Box();
    if (params[0] != NULL) {
	aBox->bounds[1][0]= ((Vector3 *)params[0])->Vc[0]/2;
	aBox->bounds[1][1]= ((Vector3 *)params[0])->Vc[1]/2;
	aBox->bounds[1][2]= ((Vector3 *)params[0])->Vc[2]/2;
	aBox->bounds[0][0]= -aBox->bounds[1][0];
	aBox->bounds[0][1]= -aBox->bounds[1][1];
	aBox->bounds[0][2]= -aBox->bounds[1][2];
    }
    Compute_Box_BBox(aBox);
    aBox->Trans= createTransform(aTransfo);
    Compute_Box_BBox(aBox);
    return new PovObject(aBox, box);
}


PovObject *PovObject::createCone(TrMatrix *aTransfo, void **params)
{
    TRANSFORM *tmpTransfo;
    CONE *aCone= Create_Cone();

    aCone->base_radius= *((double *)params[0]);     aCone->apex_radius= 0;
    aCone->base[0]= aCone->base[2]= 0;
    aCone->apex[1]= *((double *)params[1])/2;
    aCone->apex[0]= aCone->apex[2]= 0;
    aCone->base[1]= -aCone->apex[1];
    Compute_Cone_Data((OBJECT *)aCone);

    Compute_Cone_BBox(aCone);
    tmpTransfo= createTransform(aTransfo);
    Transform_Cone((OBJECT *)aCone, tmpTransfo);
    Compute_Cone_BBox(aCone);

    POV_FREE(tmpTransfo);

    return new PovObject(aCone, cone);
}


PovObject *PovObject::createCylinder(TrMatrix *aTransfo, void **params)
{
    TRANSFORM *tmpTransfo;
    CONE *aCylinder= Create_Cylinder(); 

    aCylinder->base_radius= *((double *)params[0]);
    aCylinder->apex_radius= aCylinder->base_radius;
    aCylinder->base[0]= aCylinder->base[2]= 0;
    aCylinder->apex[0]= aCylinder->apex[2]= 0;
    aCylinder->apex[1]= *((double *)params[1]) / 2.0;
    aCylinder->base[1]= -aCylinder->apex[1];
    Compute_Cylinder_Data((OBJECT *)aCylinder);
    Compute_Cone_BBox(aCylinder);

    tmpTransfo= createTransform(aTransfo);
    Transform_Cone((OBJECT *)aCylinder, tmpTransfo);
    Compute_Cone_BBox(aCylinder);
    POV_FREE(tmpTransfo);

    return new PovObject(aCylinder, cylinder);
}


PovObject *PovObject::createHeightField(TrMatrix *aTransfo, void **params)
{
    // ATTN: TODO.
    return NULL;
}


PovObject *PovObject::createLathe(TrMatrix *aTransfo, void **params)
{
    // ATTN: TODO.
    return NULL;
}


PovObject *PovObject::createMeshSet(TrMatrix *aTransfo, void **params, unsigned int nbrTriangles)
{
    TRANSFORM *tmpTransfo;
    MESH *aMesh;
    MESH_DATA *meshData;
    VECTOR N, P1, P2, P3;
    unsigned int i, *indexes;

    aMesh= Create_Mesh();
    meshData= (MESH_DATA *)POV_MALLOC(sizeof(MESH_DATA), "triangle mesh data");
    aMesh->Data= meshData;

    meshData->References= 1;
    meshData->Tree= NULL;
    meshData->Textures= NULL;
    meshData->Number_Of_Textures= 0;

    meshData->Number_Of_Vertices= (unsigned int)params[2];
    meshData->Vertices= (SNGL_VECT *)POV_MALLOC(meshData->Number_Of_Vertices*sizeof(SNGL_VECT), "triangle normal data");
    for (i= 0; i < meshData->Number_Of_Vertices; i++) {
	meshData->Vertices[i][0]= ((SNGL_VECT *)params[3])[i][0];
	meshData->Vertices[i][1]= ((SNGL_VECT *)params[3])[i][1];
	meshData->Vertices[i][2]= ((SNGL_VECT *)params[3])[i][2];
    }

    meshData->Number_Of_Normals= nbrTriangles;
    meshData->Normals= (SNGL_VECT *)POV_MALLOC(meshData->Number_Of_Normals*sizeof(SNGL_VECT), "triangle normal data");

    meshData->Number_Of_Triangles= nbrTriangles;
    meshData->Triangles= (MESH_TRIANGLE *)POV_MALLOC(meshData->Number_Of_Triangles*sizeof(MESH_TRIANGLE), "triangle mesh data");

    for (i= 0, indexes= (unsigned int *)params[14]; i < meshData->Number_Of_Triangles; i++) {
	Init_Mesh_Triangle(&meshData->Triangles[i]);
	meshData->Triangles[i].P1= indexes[i*4];
	meshData->Triangles[i].P2= indexes[i*4 +1];
	meshData->Triangles[i].P3= indexes[i*4+ 2] ;
	P1[0]= (double)meshData->Vertices[meshData->Triangles[i].P1][0];
	P1[1]= (double)meshData->Vertices[meshData->Triangles[i].P1][1];
	P1[2]= (double)meshData->Vertices[meshData->Triangles[i].P1][2];
	P2[0]= (double)meshData->Vertices[meshData->Triangles[i].P2][0];
	P2[1]= (double)meshData->Vertices[meshData->Triangles[i].P2][1];
	P2[2]= (double)meshData->Vertices[meshData->Triangles[i].P2][2];
	P3[0]= (double)meshData->Vertices[meshData->Triangles[i].P3][0];
	P3[1]= (double)meshData->Vertices[meshData->Triangles[i].P3][1];
	P3[2]= (double)meshData->Vertices[meshData->Triangles[i].P3][2];
	Compute_Mesh_Triangle(&meshData->Triangles[i], FALSE, P1, P2, P3, N);  // TRUE -> smooth triangle.
	meshData->Triangles[i].Normal_Ind= i;
	meshData->Normals[i][0]= (float)N[0];
	meshData->Normals[i][1]= (float)N[1];
	meshData->Normals[i][2]= (float)N[2];
	meshData->Triangles[i].Texture= 0;
    }

    Compute_Mesh_BBox(aMesh);
    tmpTransfo= createTransform(aTransfo);
    Transform_Mesh((OBJECT *)aMesh, tmpTransfo);

    Build_Mesh_BBox_Tree(aMesh);
    POV_FREE(tmpTransfo);

    return new PovObject(aMesh, mesh);
}


PovObject *PovObject::createPolygonSet(TrMatrix *aTransfo, void **params, unsigned int nbrPolys)
{
    TRANSFORM tmpTransfo;
    POLYGON **polygons;
    MESH *aMesh;
    MESH_DATA *meshData;
    PovObject *result= NULL;
    unsigned int scanPos;

#if defined(SUPPORT_POLYGONS)

    nbrPolygons-= nbrTriangles + nbrSquares;
    xferTransform(&tmpTransfo, transfo);

    if (nbrPolygons > 0) {
	POLYGON *aPolygon;
	VECTOR *points;
	unsigned int polyIndex, indexCount;
	unsigned int triangleCount, squareCount, polyFound;
	unsigned int *trianglePositions, *squarePositions;
	unsigned char *pointMap;

	polygons= new POLYGON*[nbrPolygons];
	polyFound= triangleCount= squareCount= scanPos= 0;

	if ((nbrTriangles + nbrSquares) > 0) {
	    if (nbrTriangles > 0) trianglePositions= new unsigned int[nbrTriangles];
	    if (nbrSquares > 0) squarePositions= new unsigned int[nbrSquares];
	    pointMap= new unsigned char[(nbrPoints >> 3) + 1];
	    memset(pointMap, 0, ((nbrPoints >> 3) +1) * sizeof(unsigned char));
	}

	for (polyIndex= 0; (polyIndex < nbrPolygons) && (scanPos < nbrIndexes); polyIndex++) {
	    indexCount= 0;
	    while (scanPos < nbrIndexes) {
	    	if (indexes[scanPos] == 0x0FFFFFFFF) {
		    if (indexCount == 3) {
			pointMap[(indexes[scanPos-3] >> 3)]|= 1 << (indexes[scanPos-3] % 8);
			pointMap[(indexes[scanPos-2] >> 3)]|= 1 << (indexes[scanPos-2] % 8);
			pointMap[(indexes[scanPos-1] >> 3)]|= 1 << (indexes[scanPos-1] % 8);
			trianglePositions[triangleCount++]= scanPos - 3;
		    }
		    else if (indexCount == 4) {
			pointMap[(indexes[scanPos-4] >> 3)]|= 1 << (indexes[scanPos-4] % 8);
			pointMap[(indexes[scanPos-3] >> 3)]|= 1 << (indexes[scanPos-3] % 8);
			pointMap[(indexes[scanPos-2] >> 3)]|= 1 << (indexes[scanPos-2] % 8);
			pointMap[(indexes[scanPos-1] >> 3)]|= 1 << (indexes[scanPos-1] % 8);
			squarePositions[squareCount++]= scanPos - 4;
		    }
		    else {
			VECTOR N;
			AkCoord3D o, u, v, w;
			unsigned int polyOffset, i;
    
			polyOffset= scanPos - indexCount;
			aPolygon= Create_Polygon();
			aPolygon->Data= (POLYGON_DATA *)POV_MALLOC(sizeof(POLYGON_DATA), "polygon data");
			aPolygon->Data->References= 1;
			aPolygon->Data->Number= indexCount + 1;
			aPolygon->Data->Points= (UV_VECT *)POV_MALLOC(aPolygon->Data->Number * sizeof(UV_VECT), "polygon data points");
				// Compute a standard polygon coordinate space.
			o= vertices[indexes[polyOffset]];
			for (i= 1; i < indexCount; i++) {
			    u.sub(o, vertices[indexes[polyOffset + i]]);
			    if ((double)u.sumSquares() > (EPSILON * EPSILON)) break;
			}
			for (i++; i < indexCount; i++) {
			    v.sub(o, vertices[indexes[polyOffset + i]]);
			    w.cross(u, v);		// Cross product.
			    if (((double)v.sumSquares() > (EPSILON * EPSILON)) && ((double)w.sumSquares() > (EPSILON * EPSILON))) break;
			}
			if (i < indexCount) {
			    MATRIX a, b;
			    double x, y, z, d;

			    u.cross(v, w);
			    v.cross(w, u);
			    u.normalize();
			    v.normalize();
			    w.normalize();

			    MIdentity(a);
			    MIdentity(b);
			    a[3][0]= -(double)o.points[0];
			    a[3][1]= -(double)o.points[1];
			    a[3][2]= (double)-o.points[2];

			    b[0][0]= (double)u.points[0];
			    b[1][0]= (double)u.points[1];
			    b[2][0]= (double)u.points[2];

			    b[0][1]= (double)v.points[0];
			    b[1][1]= (double)v.points[1];
			    b[2][1]= (double)v.points[2];

			    b[0][2]= (double)w.points[0];
			    b[1][2]= (double)w.points[1];
			    b[2][2]= (double)w.points[2];

			    MTimes(aPolygon->Trans->inverse, a, b);
			    MInvers(aPolygon->Trans->matrix, aPolygon->Trans->inverse);

			    for (i= 0; i < indexCount; i++) {
				x= (double)(vertices[indexes[polyOffset + i]].points[0] - o.points[0]);
				y= (double)(vertices[indexes[polyOffset + i]].points[1] - o.points[1]);
				z= (double)(vertices[indexes[polyOffset + i]].points[2] - o.points[2]);
				d= x * (double)w.points[0] + y * (double)w.points[1] + z * (double)w.points[2];
/*
				if (fabs(d) > ZERO_TOLERANCE) {
				    Set_Flag(aPolygon, DEGENERATE_FLAG);
				}
				else {
*/
				    aPolygon->Data->Points[i][X]= x * (double)u.points[0] + y * (double)u.points[1] + z * (double)u.points[2];
				    aPolygon->Data->Points[i][Y]= x * (double)v.points[0] + y * (double)v.points[1] + (double)z * v.points[2];
//				}
			    }
			    aPolygon->Data->Points[i][X]= aPolygon->Data->Points[0][X];
			    aPolygon->Data->Points[i][Y]= aPolygon->Data->Points[0][Y];

			}
			else {
			    Set_Flag(aPolygon, DEGENERATE_FLAG);
			}
			Make_Vector(N, 0.0, 0.0, 1.0);
			MTransNormal(aPolygon->S_Normal, N, aPolygon->Trans);
			VNormalizeEq(aPolygon->S_Normal);
			Transform_Polygon((OBJECT *)aPolygon, &tmpTransfo);
			Compute_Polygon_BBox(aPolygon);
			polygons[polyFound++]= aPolygon;
		    }
		    indexCount= 0;
		}
		else {
		    indexCount++;
		}
		scanPos++;
	    }
	}
		// Handle the triangle and squares if there is any.
	if ((nbrTriangles + nbrSquares) > 0) {
	    VECTOR N, P1, P2, P3;
	    unsigned int subPointCount, mapLength, triangleIndex;
	    unsigned int i, j, k;
	    unsigned char map;

	    subPointCount= 0;
	    mapLength= (nbrPoints >> 3) + 1;
	    for (i= 0; i < mapLength; i++) {
	    	map= 1;
		for (j= 0; j < 8; j++) {
		    if (pointMap[i] & map) subPointCount++;
		    map<<= 1;
		}
	    }
	    aMesh= Create_Mesh();
	    meshData= (MESH_DATA *)POV_MALLOC(sizeof(MESH_DATA), "triangle mesh data");
	    aMesh->Data= meshData;
	    meshData->References= 1;
	    meshData->Tree= NULL;
	    meshData->Textures= NULL;
	    meshData->Number_Of_Textures= 0;
	    meshData->Number_Of_Vertices= subPointCount;
	    meshData->Vertices= (SNGL_VECT *)POV_MALLOC(meshData->Number_Of_Vertices * sizeof(SNGL_VECT), "triangle vertices data");
	    meshData->Number_Of_Normals= nbrTriangles + 2 * nbrSquares;
	    meshData->Normals= (SNGL_VECT *)POV_MALLOC(meshData->Number_Of_Normals * sizeof(SNGL_VECT), "triangle normal data");
	    meshData->Number_Of_Triangles= nbrTriangles + 2 * nbrSquares;
	    meshData->Triangles= (MESH_TRIANGLE *)POV_MALLOC(meshData->Number_Of_Triangles * sizeof(MESH_TRIANGLE), "triangle mesh data");

	    for (k= i= 0; i < mapLength; i++) {
	    	map= 1;
		for (j= 0; j < 8; j++) {
		    if (pointMap[i] & map) {
			meshData->Vertices[k][0]= vertices[(i << 3) + j].points[0];
			meshData->Vertices[k][1]= vertices[(i << 3) + j].points[1];
			meshData->Vertices[k][2]= vertices[(i << 3) + j].points[2];
			k++;
		    }
		    map<<= 1;
		}
	    }

	    triangleIndex= 0;
	    for (i= 0; i < nbrTriangles; i++) {
		Init_Mesh_Triangle(&meshData->Triangles[triangleIndex]);
		meshData->Triangles[triangleIndex].P1= indexes[trianglePositions[i]];
		meshData->Triangles[triangleIndex].P2= indexes[trianglePositions[i] + 1];
		meshData->Triangles[triangleIndex].P3= indexes[trianglePositions[i] + 2];
		P1[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][0];
		P1[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][1];
		P1[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][2];
		P2[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][0];
		P2[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][1];
		P2[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][2];
		P3[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][0];
		P3[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][1];
		P3[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][2];
		Compute_Mesh_Triangle(&meshData->Triangles[triangleIndex], FALSE, P1, P2, P3, N);  // TRUE -> smooth triangle.
		meshData->Triangles[triangleIndex].Normal_Ind= triangleIndex;
		meshData->Normals[triangleIndex][0]= (float)N[0];
		meshData->Normals[triangleIndex][1]= (float)N[1];
		meshData->Normals[triangleIndex][2]= (float)N[2];
		meshData->Triangles[triangleIndex].Texture= 0;

		triangleIndex++;
	    }

	    for (i= 0; i < nbrSquares; i++) {
		Init_Mesh_Triangle(&meshData->Triangles[triangleIndex]);
		meshData->Triangles[triangleIndex].P1= indexes[squarePositions[i]];
		meshData->Triangles[triangleIndex].P2= indexes[squarePositions[i]+1];
		meshData->Triangles[triangleIndex].P3= indexes[squarePositions[i]+2];
		P1[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][0];
		P1[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][1];
		P1[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][2];
		P2[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][0];
		P2[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][1];
		P2[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][2];
		P3[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][0];
		P3[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][1];
		P3[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][2];
		Compute_Mesh_Triangle(&meshData->Triangles[triangleIndex], FALSE, P1, P2, P3, N);  // TRUE -> smooth triangle.
		meshData->Triangles[triangleIndex].Normal_Ind= triangleIndex;
		meshData->Normals[triangleIndex][0]= (float)N[0];
		meshData->Normals[triangleIndex][1]= (float)N[1];
		meshData->Normals[triangleIndex][2]= (float)N[2];
		meshData->Triangles[triangleIndex].Texture= 0;

		triangleIndex++;

		Init_Mesh_Triangle(&meshData->Triangles[triangleIndex]);
		meshData->Triangles[triangleIndex].P1= indexes[squarePositions[i] + 2];
		meshData->Triangles[triangleIndex].P2= indexes[squarePositions[i] +3];
		meshData->Triangles[triangleIndex].P3= indexes[squarePositions[i]];
		P1[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][0];
		P1[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][1];
		P1[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][2];
		P2[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][0];
		P2[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][1];
		P2[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][2];
		P3[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][0];
		P3[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][1];
		P3[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][2];
		Compute_Mesh_Triangle(&meshData->Triangles[triangleIndex], FALSE, P1, P2, P3, N);  // TRUE -> smooth triangle.
		meshData->Triangles[triangleIndex].Normal_Ind= triangleIndex;
		meshData->Normals[triangleIndex][0]= (float)N[0];
		meshData->Normals[triangleIndex][1]= (float)N[1];
		meshData->Normals[triangleIndex][2]= (float)N[2];
		meshData->Triangles[triangleIndex].Texture= 0;
		triangleIndex++;
	    }
	    Compute_Mesh_BBox(aMesh);
	    Transform_Mesh((OBJECT *)aMesh, &tmpTransfo);
	    Build_Mesh_BBox_Tree(aMesh);

	    if (nbrTriangles > 0) delete[] trianglePositions;
	    if (nbrSquares > 0) delete[] squarePositions;
	    delete[] pointMap;
	    result= new PovArrayObject(polygons, aMesh, nbrPolygons);
	    delete[] polygons;
	}
	else {
	    result= new PovArrayObject(polygons, nbrPolygons);
	    delete[] polygons;
	}
    }
    else {		// Only made of triangles and squares.
	VECTOR N, P1, P2, P3;
	unsigned int i, j, k, triangleIndex;
    
	nbrPolygons= nbrTriangles + nbrSquares;
	aMesh= Create_Mesh();
	meshData= (MESH_DATA *)POV_MALLOC(sizeof(MESH_DATA), "triangle mesh data");
	aMesh->Data= meshData;
	meshData->References= 1;
	meshData->Tree= NULL;
	meshData->Textures= NULL;
	meshData->Number_Of_Textures= 0;
	meshData->Number_Of_Vertices= nbrPoints;
	meshData->Vertices= (SNGL_VECT *)POV_MALLOC(meshData->Number_Of_Vertices * sizeof(SNGL_VECT), "triangle vertices data");
	meshData->Number_Of_Normals= nbrTriangles + 2 * nbrSquares;
	meshData->Normals= (SNGL_VECT *)POV_MALLOC(meshData->Number_Of_Normals * sizeof(SNGL_VECT), "triangle normal data");
	meshData->Number_Of_Triangles= nbrTriangles + 2 * nbrSquares;
	meshData->Triangles= (MESH_TRIANGLE *)POV_MALLOC(meshData->Number_Of_Triangles * sizeof(MESH_TRIANGLE), "triangle mesh data");
    
	for (i= 0; i < nbrPoints; i++) {
	    meshData->Vertices[i][0]= vertices[i].points[0];
	    meshData->Vertices[i][1]= vertices[i].points[1];
	    meshData->Vertices[i][2]= vertices[i].points[2];
	}
    
	triangleIndex= 0;
	scanPos= 0;
	for (i= 0; (i < nbrPolygons) && (scanPos < nbrIndexes); i++) {
	    Init_Mesh_Triangle(&meshData->Triangles[triangleIndex]);
	    meshData->Triangles[triangleIndex].P1= indexes[scanPos];
	    meshData->Triangles[triangleIndex].P2= indexes[scanPos + 1];
	    meshData->Triangles[triangleIndex].P3= indexes[scanPos+ 2];
	    P1[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][0];
	    P1[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][1];
	    P1[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][2];
	    P2[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][0];
	    P2[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][1];
	    P2[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][2];
	    P3[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][0];
	    P3[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][1];
	    P3[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][2];
	    Compute_Mesh_Triangle(&meshData->Triangles[triangleIndex], FALSE, P1, P2, P3, N);  // TRUE -> smooth triangle.
	    meshData->Triangles[triangleIndex].Normal_Ind= triangleIndex;
	    meshData->Normals[triangleIndex][0]= (float)N[0];
	    meshData->Normals[triangleIndex][1]= (float)N[1];
	    meshData->Normals[triangleIndex][2]= (float)N[2];
	    meshData->Triangles[triangleIndex].Texture= 0;
    
	    triangleIndex++;
    
	    if ((scanPos + 3) != 0x0FFFFFFFF) {
		Init_Mesh_Triangle(&meshData->Triangles[triangleIndex]);
		meshData->Triangles[triangleIndex].P1= indexes[scanPos + 2];
		meshData->Triangles[triangleIndex].P2= indexes[scanPos + 3];
		meshData->Triangles[triangleIndex].P3= indexes[scanPos];
		P1[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][0];
		P1[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][1];
		P1[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P1][2];
		P2[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][0];
		P2[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][1];
		P2[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P2][2];
		P3[0]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][0];
		P3[1]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][1];
		P3[2]= (double)meshData->Vertices[meshData->Triangles[triangleIndex].P3][2];
		Compute_Mesh_Triangle(&meshData->Triangles[triangleIndex], FALSE, P1, P2, P3, N);  // TRUE -> smooth triangle.
		meshData->Triangles[triangleIndex].Normal_Ind= triangleIndex;
		meshData->Normals[triangleIndex][0]= (float)N[0];
		meshData->Normals[triangleIndex][1]= (float)N[1];
		meshData->Normals[triangleIndex][2]= (float)N[2];
		meshData->Triangles[triangleIndex].Texture= 0;
		triangleIndex++;
		scanPos+= 5;
	    }
	    else {
		scanPos+= 4;
	    }
	}
    
	Compute_Mesh_BBox(aMesh);
	Transform_Mesh((OBJECT *)aMesh, &tmpTransfo);
	Build_Mesh_BBox_Tree(aMesh);
	result= new PovObject(aMesh, PovObject::mesh);
    }

#endif
    return result;
}


PovObject *PovObject::createSquareSet(TrMatrix *aTransfo, void **params, unsigned int nbrPolys)
{
    TRANSFORM *tmpTransfo;
    MESH *aMesh;
    MESH_DATA *meshData;
    VECTOR N, P1, P2, P3;
    unsigned int i, *indexes;

    aMesh= Create_Mesh();
    meshData= (MESH_DATA *)POV_MALLOC(sizeof(MESH_DATA), "triangle mesh data");
    aMesh->Data= meshData;

    meshData->References= 1;
    meshData->Tree= NULL;
    meshData->Textures= NULL;
    meshData->Number_Of_Textures= 0;

    meshData->Number_Of_Vertices= (unsigned int)params[2];
    meshData->Vertices= (SNGL_VECT *)POV_MALLOC(meshData->Number_Of_Vertices*sizeof(SNGL_VECT), "triangle normal data");
    for (i= 0; i < meshData->Number_Of_Vertices; i++) {
	meshData->Vertices[i][0]= ((SNGL_VECT *)params[3])[i][0];
	meshData->Vertices[i][1]= ((SNGL_VECT *)params[3])[i][1];
	meshData->Vertices[i][2]= ((SNGL_VECT *)params[3])[i][2];
    }

    meshData->Number_Of_Normals= nbrPolys *2;
    meshData->Normals= (SNGL_VECT *)POV_MALLOC(meshData->Number_Of_Normals*sizeof(SNGL_VECT), "triangle normal data");

    meshData->Number_Of_Triangles= nbrPolys *2;
    meshData->Triangles= (MESH_TRIANGLE *)POV_MALLOC(meshData->Number_Of_Triangles*sizeof(MESH_TRIANGLE), "triangle mesh data");

    for (i= 0, indexes= (unsigned int *)params[14]; i < nbrPolys; i++) {
	Init_Mesh_Triangle(&meshData->Triangles[i*2]);
	meshData->Triangles[i*2].P1= indexes[i*5];
	meshData->Triangles[i*2].P2= indexes[i*5 +1];
	meshData->Triangles[i*2].P3= indexes[i*5+ 2] ;
	P1[0]= (double)meshData->Vertices[meshData->Triangles[i*2].P1][0];
	P1[1]= (double)meshData->Vertices[meshData->Triangles[i*2].P1][1];
	P1[2]= (double)meshData->Vertices[meshData->Triangles[i*2].P1][2];
	P2[0]= (double)meshData->Vertices[meshData->Triangles[i*2].P2][0];
	P2[1]= (double)meshData->Vertices[meshData->Triangles[i*2].P2][1];
	P2[2]= (double)meshData->Vertices[meshData->Triangles[i*2].P2][2];
	P3[0]= (double)meshData->Vertices[meshData->Triangles[i*2].P3][0];
	P3[1]= (double)meshData->Vertices[meshData->Triangles[i*2].P3][1];
	P3[2]= (double)meshData->Vertices[meshData->Triangles[i*2].P3][2];
	Compute_Mesh_Triangle(&meshData->Triangles[i*2], FALSE, P1, P2, P3, N);  // TRUE -> smooth triangle.
	meshData->Triangles[i*2].Normal_Ind= i*2;
	meshData->Normals[i*2][0]= (float)N[0];
	meshData->Normals[i*2][1]= (float)N[1];
	meshData->Normals[i*2][2]= (float)N[2];
	meshData->Triangles[i*2].Texture= 0;

	Init_Mesh_Triangle(&meshData->Triangles[i*2+1]);
	meshData->Triangles[i*2+1].P1= indexes[i*5+2];
	meshData->Triangles[i*2+1].P2= indexes[i*5 +3];
	meshData->Triangles[i*2+1].P3= indexes[i*5] ;
	P1[0]= (double)meshData->Vertices[meshData->Triangles[i*2+1].P1][0];
	P1[1]= (double)meshData->Vertices[meshData->Triangles[i*2+1].P1][1];
	P1[2]= (double)meshData->Vertices[meshData->Triangles[i*2+1].P1][2];
	P2[0]= (double)meshData->Vertices[meshData->Triangles[i*2+1].P2][0];
	P2[1]= (double)meshData->Vertices[meshData->Triangles[i*2+1].P2][1];
	P2[2]= (double)meshData->Vertices[meshData->Triangles[i*2+1].P2][2];
	P3[0]= (double)meshData->Vertices[meshData->Triangles[i*2+1].P3][0];
	P3[1]= (double)meshData->Vertices[meshData->Triangles[i*2+1].P3][1];
	P3[2]= (double)meshData->Vertices[meshData->Triangles[i*2+1].P3][2];
	Compute_Mesh_Triangle(&meshData->Triangles[i*2+1], FALSE, P1, P2, P3, N);  // TRUE -> smooth triangle.
	meshData->Triangles[i*2+1].Normal_Ind= i*2+1;
	meshData->Normals[i*2+1][0]= (float)N[0];
	meshData->Normals[i*2+1][1]= (float)N[1];
	meshData->Normals[i*2+1][2]= (float)N[2];
	meshData->Triangles[i*2+1].Texture= 0;
    }

    Compute_Mesh_BBox(aMesh);

    tmpTransfo= createTransform(aTransfo);
    Transform_Mesh((OBJECT *)aMesh, tmpTransfo);

    Build_Mesh_BBox_Tree(aMesh);
    POV_FREE(tmpTransfo);

    return new PovObject(aMesh, mesh);
}


PovObject *PovObject::createRegularPolygonSet(TrMatrix *aTransfo, void **params, unsigned int nbrPolys, int polySize)
{
    POLYGON **polygons, *aPolygon;
    VECTOR *points, N;
    TRANSFORM tmpTransfo;
    PovArrayObject *result;
    unsigned int i, j, polyOffset;

#if defined(SUPPORT_POLYGONS)
    AkCoord3D o, u, v, w;

    xferTransform(&tmpTransfo, transfo);
    polygons= new POLYGON*[nbrPolygons];

    for (i= 0; i < nbrPolygons; i++) {
	aPolygon= Create_Polygon();
	aPolygon->Data= (POLYGON_DATA *)POV_MALLOC(sizeof(POLYGON_DATA), "polygon data");
	aPolygon->Data->References= 1;
	aPolygon->Data->Number= polySize + 1;
	aPolygon->Data->Points= (UV_VECT *)POV_MALLOC(aPolygon->Data->Number * sizeof(UV_VECT), "polygon data points");
		// Compute a standard polygon coordinate space.
	polyOffset= (polySize +1) * i;
	o= vertices[indexes[polyOffset]];
	for (j= 1; j < polySize; j++) {
	    u.sub(o, vertices[indexes[polyOffset + j]]);
	    if (u.sumSquares() > EPSILON) break;
	}
	for (j++; j < polySize; j++) {
	    v.sub(o, vertices[indexes[polyOffset + j]]);
	    w.cross(u, v);		// Cross product.
	    if ((v.sumSquares() > EPSILON) && (w.sumSquares() > EPSILON)) break;
	}
	if (j < polySize) {
	    MATRIX a, b;
	    double x, y, z, d;

	    u.cross(v, w);
	    v.cross(w, u);
	    u.normalize();
	    v.normalize();
	    w.normalize();
	    a[3][0]= -(double)o.points[0];
	    a[3][1]= -(double)o.points[1];
	    a[3][2]= (double)-o.points[2];

	    b[0][0]= (double)u.points[0];
	    b[1][0]= (double)u.points[1];
	    b[2][0]= (double)u.points[2];

	    b[0][1]= (double)v.points[0];
	    b[1][1]= (double)v.points[1];
	    b[2][1]= (double)v.points[2];

	    b[0][2]= (double)w.points[0];
	    b[1][2]= (double)w.points[1];
	    b[2][2]= (double)w.points[2];

	    MTimes(aPolygon->Trans->inverse, a, b);
	    MInvers(aPolygon->Trans->matrix, aPolygon->Trans->inverse);

	    for (j= 0; j < polySize; j++) {
		x= (double)(vertices[indexes[polyOffset + j]].points[0] - o.points[0]);
		y= (double)(vertices[indexes[polyOffset + j]].points[1] - o.points[1]);
		z= (double)(vertices[indexes[polyOffset + j]].points[2] - o.points[2]);
		d= x * (double)w.points[0] + y * (double)w.points[1] + z * (double)w.points[2];
/*
		if (fabs(d) > ZERO_TOLERANCE) {
		    Set_Flag(aPolygon, DEGENERATE_FLAG);
		}
		else {
*/
		    aPolygon->Data->Points[j][X]= x * (double)u.points[0] + y * (double)u.points[1] + z * (double)u.points[2];
		    aPolygon->Data->Points[j][Y]= x * (double)v.points[0] + y * (double)v.points[1] + (double)z * v.points[2];
//		    }
	    }
	    aPolygon->Data->Points[j][X]= aPolygon->Data->Points[0][X];
	    aPolygon->Data->Points[j][Y]= aPolygon->Data->Points[0][Y];
	}
	else {
	    Set_Flag(aPolygon, DEGENERATE_FLAG);
	}
	Make_Vector(N, 0.0, 0.0, 1.0);
	MTransNormal(aPolygon->S_Normal, N, aPolygon->Trans);
	VNormalizeEq(aPolygon->S_Normal);
	Transform_Polygon((OBJECT *)aPolygon, &tmpTransfo);
	Compute_Polygon_BBox(aPolygon);
	polygons[i]= aPolygon;
    }

    result= new PovArrayObject(polygons, nbrPolygons);
    delete[] polygons;
#endif

    return result;
}


PovObject *PovObject::createSphere(TrMatrix *aTransfo, void **params)
{
    SPHERE *aSphere= Create_Sphere();
    if (params[0] != NULL) {
	aSphere->Radius= *((double *)params[0]);
    }
    Compute_Sphere_BBox(aSphere);
    aSphere->Methods = &Ellipsoid_Methods;
    aSphere->Trans= createTransform(aTransfo);
    Compute_Sphere_BBox(aSphere);
    return new PovObject(aSphere, sphere);
}


PovObject *PovObject::createDirLight(TrMatrix *aTransfo, void **params)
{
    DBL Len;
    LIGHT_SOURCE *aLight= Create_Light_Source();
    Vector3 tmpCenter(aLight->Center[0], aLight->Center[1], aLight->Center[2]);

    aTransfo->transformPoint(&tmpCenter);
    aLight->Center[0]= tmpCenter.Vc[0];
    aLight->Center[1]= tmpCenter.Vc[1];
    aLight->Center[2]= tmpCenter.Vc[2];

    if (params[1] != NULL) {
	aLight->Colour[0]= ((Vector3 *)params[1])->Vc[0];
	aLight->Colour[1]= ((Vector3 *)params[1])->Vc[1];
	aLight->Colour[2]= ((Vector3 *)params[1])->Vc[2];
    }

//    VSub(aLight->Direction, aLight->Points_At, aLight->Center);
    if (params[2] != NULL) {
	aLight->Direction[0]= ((Vector3 *)params[2])->Vc[0];
	aLight->Direction[1]= ((Vector3 *)params[2])->Vc[1];
	aLight->Direction[2]= ((Vector3 *)params[2])->Vc[2];
    }
    VLength(Len, aLight->Direction);
    if (Len > EPSILON) {
	VInverseScaleEq(aLight->Direction, Len);
    }
    return new PovObject(aLight, light);
}


PovObject *PovObject::createPointLight(TrMatrix *aTransfo, void **params)
{
    // ATTN: TODO.
    return createDirLight(aTransfo, params);
}


PovObject *PovObject::createSpotLight(TrMatrix *aTransfo, void **params)
{
    // ATTN: TODO.
    return createDirLight(aTransfo, params);
}



PovObject::PovObject(void *anObject, Type aType)
    : PovContainer(anObject)
{
    type= aType;
}


PovObject::Type PovObject::getType(void)
{
    return type;
}


void PovObject::addTexture(PovTexture *aTexture)
{
    TEXTURE *pTexture= (TEXTURE *)aTexture->getLowObject();

    if (pTexture != NULL) {
	((OBJECT *)lowObject)->Type |= TEXTURED_OBJECT;
	Link_Textures(&(((OBJECT *)lowObject)->Texture), (TEXTURE *)pTexture);
    }
}


/**************************************************
* Implementation: PovArrayObject.
**************************************************/

PovArrayObject::PovArrayObject(void *content, unsigned int aCount)
    : PovObject(NULL, PovObject::polygonArray)
{
    void **tmpValue;

    count= aCount;
    tmpValue= new void*[count];
    memcpy(tmpValue, (void **)content, count * sizeof(void *));
    lowObject= tmpValue;
    contentType= pure;
}


PovArrayObject::PovArrayObject(void *content, void *prefix, unsigned int aCount)
    : PovObject(NULL, PovObject::polygonArray)
{
    void **tmpValue;

    count= aCount;
    tmpValue= new void*[count+1];
    tmpValue[0]= prefix;
    memcpy(&tmpValue[1], (void **)content, count * sizeof(void *));
    lowObject= tmpValue;
    contentType= mixed;
}


PovArrayObject::~PovArrayObject(void)
{
// TODO.
}


PovArrayObject::ContentType PovArrayObject::getContentType(void)
{
    return contentType;
}


unsigned int PovArrayObject::getCount(void)
{
    return count;
}


/**************************************************
* Implementation: PovTexture.
**************************************************/

void *PovTexture::reference= NULL;

void PovTexture::setReference(void *aValue)
{
    reference= aValue;
}


PovTexture *PovTexture::createTexture(void)
{
    return new PovTexture(Copy_Textures((TEXTURE *)reference));
}


PovTexture::PovTexture(void *aTexture)
    : PovContainer(aTexture)
{
    // Things will get here.
}


void PovTexture::modify(void **params)
{
    ((TEXTURE *)lowObject)->Pigment->Type= PLAIN_PATTERN;
    if (params[1] != NULL) {
	((TEXTURE *)lowObject)->Pigment->Colour[0]= ((Vector3 *)params[1])->Vc[0];
	((TEXTURE *)lowObject)->Pigment->Colour[1]= ((Vector3 *)params[1])->Vc[1];
	((TEXTURE *)lowObject)->Pigment->Colour[2]= ((Vector3 *)params[1])->Vc[2];
    }
}

