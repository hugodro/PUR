/**************************************************
* File: pLink.cc.
* Desc: Definition of the LinkPovRay class.
* Module: AkraLog : VrmlSupport.
* Rev: 24 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#if defined(__linux__)
#include <string.h>
#endif

extern "C" {
#if defined(_WIN32)
#undef __cplusplus
#endif

#include <povray/frame.h>
#include <povray/vector.h>
#include <povray/povproto.h>
#include <povray/parse.h>
#include <povray/parstxtr.h>
#include <povray/atmosph.h>
#include <povray/bezier.h>   
#include <povray/blob.h>     
#include <povray/boxes.h>
#include <povray/colour.h>
#include <povray/cones.h>    
#include <povray/csg.h>      
#include <povray/discs.h>
#include <povray/express.h>  
#include <povray/fractal.h>
#include <povray/gif.h>      
#include <povray/halos.h>
#include <povray/hfield.h>
#include <povray/iff.h>      
#include <povray/image.h>    
#include <povray/lathe.h>    
#include <povray/polysolv.h>
#include <povray/matrices.h>
#include <povray/mesh.h>
#include <povray/normal.h>
#include <povray/objects.h>
#include <povray/octree.h>
#include <povray/pigment.h>
#include <povray/planes.h>
#include <povray/poly.h>
#include <povray/polygon.h>
#include <povray/povray.h>   
#include <povray/pgm.h>      
#include <povray/ppm.h>      
#include <povray/prism.h>    
#include <povray/quadrics.h> 
#include <povray/radiosit.h>      
#include <povray/render.h>   
#include <povray/sor.h>      
#include <povray/spheres.h>  
#include <povray/super.h>
#include <povray/targa.h>    
#include <povray/texture.h>  
#include <povray/tokenize.h> 
#include <povray/torus.h>
#include <povray/triangle.h> 
#include <povray/truetype.h> 
#include <povray/polygon.h>

extern METHODS Ellipsoid_Methods;
extern CAMERA *Default_Camera;
extern TEXTURE *Default_Texture;
extern FRAME Frame;

extern void Transform_Cone PARAMS((OBJECT *Object, TRANSFORM *Trans));
extern void Transform_Mesh  PARAMS((OBJECT *Object, TRANSFORM *Trans));
extern void EntryPointPovRayLib PARAMS((void));
extern void CreateDefaultsPovRayLib PARAMS((void));
extern void FrameRenderPovRayLib PARAMS((char *aName));
extern void ExitPointPovRayLib PARAMS((void));
extern void setOptionsPovRayLib PARAMS((int optID, void *optValue));
extern void Link_To_Frame PARAMS((OBJECT *Object));
extern void Post_Process PARAMS((OBJECT *Object, OBJECT *Parent));
extern void Transform_Polygon PARAMS((OBJECT *Object, TRANSFORM *Trans));
extern void Compute_Polygon_BBox PARAMS((POLYGON *Polyg));

#define ZERO_TOLERANCE 1.0e-10

#if defined(_WIN32)
#define __cplusplus
#define NULL	(0L)
#endif
}

#include <vrcore/geometry3D.h>
#include "pObject.h"
#include "pLink.h"


static void xferTransform(TRANSFORM *result, TrMatrix *vrmlTransfo)
{
    for (unsigned int i= 0; i < 4; i++) {
	(result->matrix)[0][i]= vrmlTransfo->M[i][0];
	(result->matrix)[1][i]= vrmlTransfo->M[i][1];
	(result->matrix)[2][i]= vrmlTransfo->M[i][2];
	(result->matrix)[3][i]= vrmlTransfo->M[i][3];
    }

    MInvers(result->inverse, result->matrix);
}


/**************************************************
* Implementation: LinkPovRay.
**************************************************/

LinkPovRay *LinkPovRay::contexts= NULL;
void *LinkPovRay::referenceTexture= NULL;
void *LinkPovRay::referenceCamera= NULL;
bool LinkPovRay::coldInitFlag= true;


LinkPovRay *LinkPovRay::createContext(void)
{
    if (coldInitFlag) {
	EntryPointPovRayLib();
	CreateDefaultsPovRayLib();
	referenceTexture= Default_Texture;
	referenceCamera= Default_Camera;
	    // TMP: Beef up the ambient light.
	Frame.Ambient_Light[0]= 2.0;
	Frame.Ambient_Light[1]= 2.0;
	Frame.Ambient_Light[2]= 2.0;
	coldInitFlag= false;
    }
    contexts= new LinkPovRay(contexts);
    return contexts;
}


void LinkPovRay::terminateRenderer(void)
{
    ExitPointPovRayLib();
}


LinkPovRay::LinkPovRay(LinkPovRay *link)
{
    next= link;
}


void LinkPovRay::setImageFile(char *aFormat, char *aPath)
{
    setOptionsPovRayLib(1, aPath);
    if (strcmp(aFormat, "ppm") == 0) {
	setOptionsPovRayLib(2, (void *)(int)'p');
    }
    else if (strcmp(aFormat, "tga") == 0) {
	setOptionsPovRayLib(2, (void *)(int)'t');
    }
    else if (strcmp(aFormat, "png") == 0) {
	setOptionsPovRayLib(2, (void *)(int)'n');
    }
}


void LinkPovRay::produceImage(char *aName)
{
    FrameRenderPovRayLib(aName);
}


PovObject *LinkPovRay::createBox(TrMatrix *transfo, double sizeX, double sizeY, double sizeZ)
{
    BOX *aBox= Create_Box();

    aBox->bounds[1][0]= sizeX / 2.0;
    aBox->bounds[1][1]= sizeY / 2.0;
    aBox->bounds[1][2]= sizeZ / 2.0;
    aBox->bounds[0][0]= -aBox->bounds[1][0];
    aBox->bounds[0][1]= -aBox->bounds[1][1];
    aBox->bounds[0][2]= -aBox->bounds[1][2];
    Compute_Box_BBox(aBox);
    aBox->Trans= Create_Transform();
    xferTransform(aBox->Trans, transfo);
    Compute_Box_BBox(aBox);

    return new PovObject(aBox, PovObject::box);
}


PovObject *LinkPovRay::createCone(TrMatrix *transfo, double bottomRadius, double height)
{
    TRANSFORM tmpTransfo;
    CONE *aCone= Create_Cone();

    aCone->base_radius= bottomRadius;
    aCone->apex_radius= 0;
    aCone->base[0]= aCone->base[2]= 0;
    aCone->apex[1]= height / 2.0;
    aCone->apex[0]= aCone->apex[2]= 0;
    aCone->base[1]= -aCone->apex[1];
    Compute_Cone_Data((OBJECT *)aCone);

    Compute_Cone_BBox(aCone);
    xferTransform(&tmpTransfo, transfo);
    Transform_Cone((OBJECT *)aCone, &tmpTransfo);
    Compute_Cone_BBox(aCone);

    return new PovObject(aCone, PovObject::cone);
}


PovObject *LinkPovRay::createCylinder(TrMatrix *transfo, double radius, double height)
{
    TRANSFORM tmpTransfo;
    CONE *aCylinder= Create_Cylinder(); 

    aCylinder->base_radius= radius;
    aCylinder->apex_radius= aCylinder->base_radius;
    aCylinder->base[0]= aCylinder->base[2]= 0;
    aCylinder->apex[0]= aCylinder->apex[2]= 0;
    aCylinder->apex[1]= height / 2.0;
    aCylinder->base[1]= -aCylinder->apex[1];
    Compute_Cylinder_Data((OBJECT *)aCylinder);
    Compute_Cone_BBox(aCylinder);

    xferTransform(&tmpTransfo, transfo);
    Transform_Cone((OBJECT *)aCylinder, &tmpTransfo);
    Compute_Cone_BBox(aCylinder);

    return new PovObject(aCylinder, PovObject::cylinder);
}


PovObject *LinkPovRay::createTriangleSet(TrMatrix *transfo, AkCoord3D *vertices, int *indexes, unsigned int nbrPoints, unsigned int nbrTriangles)
{
    TRANSFORM tmpTransfo;
    MESH *aMesh;
    MESH_DATA *meshData;
    VECTOR N, P1, P2, P3;
    unsigned int i;

    aMesh= Create_Mesh();
    meshData= (MESH_DATA *)POV_MALLOC(sizeof(MESH_DATA), "triangle mesh data");
    aMesh->Data= meshData;

    meshData->References= 1;
    meshData->Tree= NULL;
    meshData->Textures= NULL;
    meshData->Number_Of_Textures= 0;

    meshData->Number_Of_Vertices= nbrPoints;
    meshData->Vertices= (SNGL_VECT *)POV_MALLOC(meshData->Number_Of_Vertices*sizeof(SNGL_VECT), "triangle normal data");
    for (i= 0; i < meshData->Number_Of_Vertices; i++) {
	meshData->Vertices[i][0]= vertices[i].points[0];
	meshData->Vertices[i][1]= vertices[i].points[1];
	meshData->Vertices[i][2]= vertices[i].points[2];
    }

    meshData->Number_Of_Normals= nbrTriangles;
    meshData->Normals= (SNGL_VECT *)POV_MALLOC(meshData->Number_Of_Normals*sizeof(SNGL_VECT), "triangle normal data");

    meshData->Number_Of_Triangles= nbrTriangles;
    meshData->Triangles= (MESH_TRIANGLE *)POV_MALLOC(meshData->Number_Of_Triangles*sizeof(MESH_TRIANGLE), "triangle mesh data");

    for (i= 0; i < meshData->Number_Of_Triangles; i++) {
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
    xferTransform(&tmpTransfo, transfo);
    Transform_Mesh((OBJECT *)aMesh, &tmpTransfo);
    Build_Mesh_BBox_Tree(aMesh);

    return new PovObject(aMesh, PovObject::mesh);
}


PovObject *LinkPovRay::createRegularPolygonSet(TrMatrix *transfo, AkCoord3D *vertices, int *indexes, unsigned int nbrPoints, unsigned int nbrPolygons, unsigned int polySize)
{
    if (polySize == 4) {
	return createTriaSquares(transfo, vertices, indexes, nbrPoints, nbrPolygons);
    }
    else {
	POLYGON **polygons, *aPolygon;
	VECTOR *points, N;
	TRANSFORM tmpTransfo;
	PovArrayObject *result;
	AkCoord3D o, u, v, w;
	unsigned int i, j, polyOffset;

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
	return result;
    }
}


PovObject *LinkPovRay::createPolygonSet(TrMatrix *transfo, AkCoord3D *vertices, int *indexes, unsigned int nbrPoints, unsigned int nbrIndexes, unsigned int nbrPolygons, unsigned int nbrTriangles, unsigned int nbrSquares)
{
    TRANSFORM tmpTransfo;
    POLYGON **polygons;
    MESH *aMesh;
    MESH_DATA *meshData;
    PovObject *result= NULL;
    unsigned int scanPos;

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

    return result;
}


PovObject *LinkPovRay::createSphere(TrMatrix *transfo, double radius)
{
    SPHERE *aSphere= Create_Sphere();

    aSphere->Radius= radius;
    Compute_Sphere_BBox(aSphere);
    aSphere->Methods = &Ellipsoid_Methods;
    aSphere->Trans= Create_Transform();
    xferTransform(aSphere->Trans, transfo);
    Compute_Sphere_BBox(aSphere);

    return new PovObject(aSphere, PovObject::sphere);
}


PovObject *LinkPovRay::createDirLight(TrMatrix *transfo, double ambient, double intensity, AkColor *color, AkCoord3D *direction)
{
    LIGHT_SOURCE *aLight= Create_Light_Source();

// TMP: Make the right mapping from dir light to ambient light or very far point light.
    Vector3 tmpCenter(aLight->Center[0], aLight->Center[1], aLight->Center[2]);

    transfo->transformPoint(&tmpCenter);
    aLight->Center[0]= tmpCenter.Vc[0];
    aLight->Center[1]= tmpCenter.Vc[1];
    aLight->Center[2]= tmpCenter.Vc[2];

    aLight->Colour[0]= color->indexes[0];
    aLight->Colour[1]= color->indexes[1];
    aLight->Colour[2]= color->indexes[2];

    return new PovObject(aLight, PovObject::light);
}


PovObject *LinkPovRay::createPointLight(TrMatrix *transfo, double ambient, double intensity, AkColor *color, float radius, AkCoord3D *attenuation, AkCoord3D *location)
{
    LIGHT_SOURCE *aLight= Create_Light_Source();
    Vector3 tmpCenter(location->points[0], location->points[1], location->points[2]);
    DBL Len;   

    transfo->transformPoint(&tmpCenter);
    aLight->Center[0]= tmpCenter.Vc[0];
    aLight->Center[1]= tmpCenter.Vc[1];
    aLight->Center[2]= tmpCenter.Vc[2];

    aLight->Colour[0]= color->indexes[0];
    aLight->Colour[1]= color->indexes[1];
    aLight->Colour[2]= color->indexes[2];

    aLight->Fade_Distance= radius * intensity;

   VSub(aLight->Direction, aLight->Points_At, aLight->Center);
   VLength(Len, aLight->Direction);
    if (Len > EPSILON) {
	VInverseScaleEq(aLight->Direction, Len);
    }

    return new PovObject(aLight, PovObject::light);
}


PovObject *LinkPovRay::createSpotLight(TrMatrix *transfo, double ambient, double intensity, AkColor *color, float width, float cutAngle, float radius, AkCoord3D *attenuation, AkCoord3D *location, AkCoord3D *direction)
{
    LIGHT_SOURCE *aLight= Create_Light_Source();
    DBL Len;   
    Vector3 tmpCenter(location->points[0], location->points[1], location->points[2]);

    transfo->transformPoint(&tmpCenter);
    aLight->Center[0]= tmpCenter.Vc[0];
    aLight->Center[1]= tmpCenter.Vc[1];
    aLight->Center[2]= tmpCenter.Vc[2];

    aLight->Colour[0]= color->indexes[0];
    aLight->Colour[1]= color->indexes[1];
    aLight->Colour[2]= color->indexes[2];
    aLight->Fade_Distance= radius * intensity;

    aLight->Light_Type= SPOT_SOURCE;
    aLight->Radius= cos((double)width);
    aLight->Falloff = cos((double)cutAngle);

    Vector3 tmpDir(direction->points[0], direction->points[1], direction->points[2]);
    transfo->transformPoint(&tmpDir);
    aLight->Direction[0]= tmpDir.Vc[0];
    aLight->Direction[1]= tmpDir.Vc[1];
    aLight->Direction[2]= tmpDir.Vc[2];

    VLength(Len, aLight->Direction);
    if (Len > EPSILON) {
	VInverseScaleEq(aLight->Direction, Len);
    }

    return new PovObject(aLight, PovObject::light);
}


PovTexture *LinkPovRay::createTexture(TrMatrix *transfo, AkColor *diffuse, AkColor *emissive, AkColor *specular, float shininess, float transparency)
{
    TEXTURE *aTexture= Copy_Textures((TEXTURE *)referenceTexture);
    aTexture->Pigment->Type= PLAIN_PATTERN;
    aTexture->Pigment->Colour[0]= diffuse->indexes[0];
    aTexture->Pigment->Colour[1]= diffuse->indexes[1];
    aTexture->Pigment->Colour[2]= diffuse->indexes[2];

    aTexture->Finish->Phong= shininess;

    return new PovTexture(aTexture);
}


PovTexture *LinkPovRay::createDefaultTexture(void)
{
    TEXTURE *aTexture= Copy_Textures((TEXTURE *)referenceTexture);
    aTexture->Pigment->Type= PLAIN_PATTERN;
    aTexture->Pigment->Colour[0]= 1.0;
    aTexture->Pigment->Colour[1]= 1.0;
    aTexture->Pigment->Colour[2]= 1.0;
    return new PovTexture(aTexture);
}


PovObject *LinkPovRay::createTriaSquares(TrMatrix *transfo, AkCoord3D *vertices, int *indexes, unsigned int nbrPoints, unsigned int nbrPolygons)
{
    TRANSFORM tmpTransfo;
    MESH *aMesh;
    MESH_DATA *meshData;
    VECTOR N, P1, P2, P3;
    unsigned int i, j;

    aMesh= Create_Mesh();
    meshData= (MESH_DATA *)POV_MALLOC(sizeof(MESH_DATA), "triangle mesh data");
    aMesh->Data= meshData;

    meshData->References= 1;
    meshData->Tree= NULL;
    meshData->Textures= NULL;
    meshData->Number_Of_Textures= 0;

    meshData->Number_Of_Vertices= nbrPoints;
    meshData->Vertices= (SNGL_VECT *)POV_MALLOC(meshData->Number_Of_Vertices * sizeof(SNGL_VECT), "triangle normal data");
    for (i= 0; i < meshData->Number_Of_Vertices; i++) {
	meshData->Vertices[i][0]= vertices[i].points[0];
	meshData->Vertices[i][1]= vertices[i].points[1];
	meshData->Vertices[i][2]= vertices[i].points[2];
    }

    meshData->Number_Of_Normals= nbrPolygons *2;
    meshData->Normals= (SNGL_VECT *)POV_MALLOC(meshData->Number_Of_Normals * sizeof(SNGL_VECT), "triangle normal data");

    meshData->Number_Of_Triangles= nbrPolygons *2;
    meshData->Triangles= (MESH_TRIANGLE *)POV_MALLOC(meshData->Number_Of_Triangles * sizeof(MESH_TRIANGLE), "triangle mesh data");

    for (i= 0; i < nbrPolygons; i++) {
	j= i* 2;
	Init_Mesh_Triangle(&meshData->Triangles[j]);
	meshData->Triangles[j].P1= indexes[i*5];
	meshData->Triangles[j].P2= indexes[i*5 +1];
	meshData->Triangles[j].P3= indexes[i*5+ 2] ;
	P1[0]= (double)meshData->Vertices[meshData->Triangles[j].P1][0];
	P1[1]= (double)meshData->Vertices[meshData->Triangles[j].P1][1];
	P1[2]= (double)meshData->Vertices[meshData->Triangles[j].P1][2];
	P2[0]= (double)meshData->Vertices[meshData->Triangles[j].P2][0];
	P2[1]= (double)meshData->Vertices[meshData->Triangles[j].P2][1];
	P2[2]= (double)meshData->Vertices[meshData->Triangles[j].P2][2];
	P3[0]= (double)meshData->Vertices[meshData->Triangles[j].P3][0];
	P3[1]= (double)meshData->Vertices[meshData->Triangles[j].P3][1];
	P3[2]= (double)meshData->Vertices[meshData->Triangles[j].P3][2];
	Compute_Mesh_Triangle(&meshData->Triangles[j], FALSE, P1, P2, P3, N);  // TRUE -> smooth triangle.
	meshData->Triangles[j].Normal_Ind= j;
	meshData->Normals[j][0]= (float)N[0];
	meshData->Normals[j][1]= (float)N[1];
	meshData->Normals[j][2]= (float)N[2];
	meshData->Triangles[j].Texture= 0;

	Init_Mesh_Triangle(&meshData->Triangles[j+1]);
	meshData->Triangles[j+1].P1= indexes[i*5+2];
	meshData->Triangles[j+1].P2= indexes[i*5 +3];
	meshData->Triangles[j+1].P3= indexes[i*5] ;
	P1[0]= (double)meshData->Vertices[meshData->Triangles[j+1].P1][0];
	P1[1]= (double)meshData->Vertices[meshData->Triangles[j+1].P1][1];
	P1[2]= (double)meshData->Vertices[meshData->Triangles[j+1].P1][2];
	P2[0]= (double)meshData->Vertices[meshData->Triangles[j+1].P2][0];
	P2[1]= (double)meshData->Vertices[meshData->Triangles[j+1].P2][1];
	P2[2]= (double)meshData->Vertices[meshData->Triangles[j+1].P2][2];
	P3[0]= (double)meshData->Vertices[meshData->Triangles[j+1].P3][0];
	P3[1]= (double)meshData->Vertices[meshData->Triangles[j+1].P3][1];
	P3[2]= (double)meshData->Vertices[meshData->Triangles[j+1].P3][2];
	Compute_Mesh_Triangle(&meshData->Triangles[j+1], FALSE, P1, P2, P3, N);  // TRUE -> smooth triangle.
	meshData->Triangles[j+1].Normal_Ind= j+1;
	meshData->Normals[j+1][0]= (float)N[0];
	meshData->Normals[j+1][1]= (float)N[1];
	meshData->Normals[j+1][2]= (float)N[2];
	meshData->Triangles[j+1].Texture= 0;
    }

    Compute_Mesh_BBox(aMesh);
    xferTransform(&tmpTransfo, transfo);
    Transform_Mesh((OBJECT *)aMesh, &tmpTransfo);
    Build_Mesh_BBox_Tree(aMesh);

    return new PovObject(aMesh, PovObject::mesh);
}


void LinkPovRay::linkTextureToObject(PovObject *anObject, PovTexture *aTexture)
{

    if (anObject->getType() == PovObject::polygonArray) {
	PovArrayObject *arrayObject= (PovArrayObject *)anObject;
	OBJECT **pObjects;
	unsigned int  i, count;

	count= arrayObject->getCount();
	if (arrayObject->getContentType() == PovArrayObject::mixed) count++;
	pObjects= (OBJECT **)anObject->getLowObject();

	for (i= 0; i < count; i++) {
	    pObjects[i]->Type|= TEXTURED_OBJECT;
	    Link_Textures(&(pObjects[i]->Texture), (TEXTURE *)aTexture->getLowObject());
	}
	((TEXTURE *)aTexture->getLowObject())->References+= (count-1);
    }
    else {
	OBJECT *pObject= (OBJECT *)anObject->getLowObject();

	pObject->Type|= TEXTURED_OBJECT;
	Link_Textures(&(pObject->Texture), (TEXTURE *)aTexture->getLowObject());
    }
    anObject->addTexture(aTexture);
}


void LinkPovRay::useObject(PovObject *anObject)
{
    OBJECT *pObject= (OBJECT *)anObject->getLowObject();
    MESH *meshPtr;
    POLYGON **polygonPtr;
    unsigned int i, nbrObjects;

    switch(anObject->getType()) {
	case PovObject::box:
	    if ((((BOX *)pObject)->Trans != NULL) && (pObject->Texture != NULL)) {
		Transform_Textures (pObject->Texture, ((BOX *)pObject)->Trans);
		Transform_Halo_Container (pObject->Texture, ((BOX *)pObject)->Trans);
	    }
	    break;
	case PovObject::cone:
	    if ((((CONE *)pObject)->Trans != NULL) && (pObject->Texture != NULL)) {
		Transform_Textures (pObject->Texture, ((CONE *)pObject)->Trans);
		Transform_Halo_Container (pObject->Texture, ((CONE *)pObject)->Trans);
	    }
	    break;
	case PovObject::cylinder:
	    if ((((CONE *)pObject)->Trans != NULL) && (pObject->Texture != NULL)) {
		Transform_Textures (pObject->Texture, ((CONE *)pObject)->Trans);
		Transform_Halo_Container (pObject->Texture, ((CONE *)pObject)->Trans);
	    }
	    break;
	case PovObject::heightField:
	    if ((((HFIELD *)pObject)->Trans != NULL) && (pObject->Texture != NULL)) {
		Transform_Textures (pObject->Texture, ((HFIELD *)pObject)->Trans);
		Transform_Halo_Container (pObject->Texture, ((HFIELD *)pObject)->Trans);
	    }
	    break;
	case PovObject::lathe:
	    if ((((LATHE *)pObject)->Trans != NULL) && (pObject->Texture != NULL)) {
		Transform_Textures (pObject->Texture, ((LATHE *)pObject)->Trans);
		Transform_Halo_Container (pObject->Texture, ((LATHE *)pObject)->Trans);
	    }
	    break;
	case PovObject::mesh:
	    if ((((MESH *)pObject)->Trans != NULL) && (pObject->Texture != NULL)) {
		Transform_Textures (pObject->Texture, ((MESH *)pObject)->Trans);
		Transform_Halo_Container (pObject->Texture, ((MESH *)pObject)->Trans);
	    }
	    break;
	case PovObject::polygon:
	    if ((((POLYGON *)pObject)->Trans != NULL) && (pObject->Texture != NULL)) {
		Transform_Textures (pObject->Texture, ((POLYGON *)pObject)->Trans);
		Transform_Halo_Container (pObject->Texture, ((POLYGON *)pObject)->Trans);
	    }
	    break;
	case PovObject::sphere:
	    if ((((SPHERE *)pObject)->Trans != NULL) && (pObject->Texture != NULL)) {
		Transform_Textures(pObject->Texture, ((SPHERE *)pObject)->Trans);
		Transform_Halo_Container (pObject->Texture, ((SPHERE *)pObject)->Trans);
	    }
	    break;
	case PovObject::polygonArray:
	    nbrObjects= ((PovArrayObject *)anObject)->getCount();
	    if (((PovArrayObject *)anObject)->getContentType() == PovArrayObject::mixed) {
		meshPtr= (MESH *)(((void **)anObject->getLowObject())[0]);
		polygonPtr= (POLYGON **)&(((void **)anObject->getLowObject())[1]);

		if (nbrObjects == 0) {		// Attn: This should not happen.
		    if ((meshPtr->Trans != NULL) && (meshPtr->Texture != NULL)) {
			Transform_Textures(meshPtr->Texture, meshPtr->Trans);
			Transform_Halo_Container(meshPtr->Texture, meshPtr->Trans);
		    }
		}
		Post_Process((OBJECT *)meshPtr, NULL);
		Link_To_Frame((OBJECT *)meshPtr);
	    }
	    else {
		polygonPtr= (POLYGON **)anObject->getLowObject();
	    }

	    if (nbrObjects > 0) {
		    // The texture is shared by all objects.
		if ((polygonPtr[0]->Trans != NULL) && (polygonPtr[0]->Texture != NULL)) {
		    Transform_Textures (polygonPtr[0]->Texture, polygonPtr[0]->Trans);
		    Transform_Halo_Container (polygonPtr[0]->Texture, polygonPtr[0]->Trans);
		}
	    }
	    for (i= 0; i < nbrObjects; i++) {
		Post_Process((OBJECT *)polygonPtr[i], NULL);
		Link_To_Frame((OBJECT *)polygonPtr[i]);
	    }
	    break;
    }

    if (anObject->getType() != PovObject::polygonArray) {
	Post_Process(pObject, NULL);
	Link_To_Frame(pObject);
    }
}


void LinkPovRay::setCameraTo(AkCoord3D *pos, AkCoord4D *orient, float fov)
{
   DBL Direction_Length = 1.0, Up_Length, Right_Length, Handedness;
   CAMERA *camera;
    TrMatrix transfo;

    camera= Frame.Camera= Copy_Camera((CAMERA *)referenceCamera);

//    camera->Right[0]= -1.33;
    camera->Direction[2]= -1;

    camera->Angle= ((double)fov) *180.0 / M_PI;
    VNormalize(camera->Direction,camera->Direction);
    VLength (Right_Length, camera->Right);
    Direction_Length= Right_Length / tan(camera->Angle * M_PI_360)/2.0;
    VScaleEq(camera->Direction, Direction_Length);

    camera->Location[0]= pos->points[0];
    camera->Location[1]= pos->points[1];
    camera->Location[2]= pos->points[2];


    Vector4 rotator(orient->points[0], orient->points[1], orient->points[2], orient->points[3]);
    Vector3 direction(camera->Direction[0], camera->Direction[1], camera->Direction[2]),
		    up(camera->Up[0], camera->Up[1], camera->Up[2]),
		    right(camera->Right[0], camera->Right[1], camera->Right[2]);

    transfo.rotateBy(&rotator);
    transfo.transformPoint(&direction);
    transfo.transformPoint(&up);
    transfo.transformPoint(&right);
    camera->Direction[0]= direction.Vc[0];
    camera->Direction[1]= direction.Vc[1];
    camera->Direction[2]= direction.Vc[2];
    camera->Up[0]= up.Vc[0];
    camera->Up[1]= up.Vc[1];
    camera->Up[2]= up.Vc[2];
    camera->Right[0]= right.Vc[0];
    camera->Right[1]= right.Vc[1];
    camera->Right[2]= right.Vc[2];
}

