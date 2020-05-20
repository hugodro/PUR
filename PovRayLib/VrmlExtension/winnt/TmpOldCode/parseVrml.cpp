/**************************************************
* File: parseVrml.cc.
* Desc: Implementation of the ParseVrml fonction.
* Module: AkraLog : PovRayExt.
* Rev: 10 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <VrmlParser/vrmlParser.h>
#include <VrmlParser/metaObjects.h>
#include "renderContext.h"
#include "povScene.h"
#include "pObject.h"
#include "maths.h"
#include "glue.h"

extern "C" {
#undef __cpluscplus
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
extern void Frame_Init PARAMS((void));
extern void Link_To_Frame PARAMS((OBJECT *Object));
extern Post_Process PARAMS((OBJECT *Object, OBJECT *Parent));
extern CAMERA *Default_Camera;
extern TEXTURE *Default_Texture;
extern FRAME Frame;
#define __cplusplus
}

extern "C" {
void ParseVrml(void);
#if defined(USE_OLD_CODE)
static BOX *createBox(double *transfo, void **params);
static CONE *createCone(double *transfo, void **params);
static CONE *createCylinder(double *transfo, void **params);
static MESH *createMeshSet(double *transfo, void **params, unsigned int nbrTriangles);
#if defined(USE_POV_POLYGON)
static POLYGON *createRegularPolygonSet(double *transfo, void **params, unsigned int nbrPolys, int polySize);
#else
static MESH *createSquareSet(double *transfo, void **params, unsigned int nbrPolys);
#endif
static SPHERE *createSphere(double *transfo, void **params);
static LIGHT_SOURCE *createDirLight(double *transfo, void **params);
static LIGHT_SOURCE *createPointLight(double *transfo, void **params);
static LIGHT_SOURCE *createSpotLight(double *transfo, void **params);
#endif
}


void ParseVrml(void)
{
    static VRParser *parser= new VRParser();
    PovScene *theScene= new PovScene();
    RenderContext *singleContext;

    if (parser->parseFile(opts.Input_File_Name, theScene) == 0) {
	Default_Camera= Create_Camera();
	Default_Texture= Create_Texture();
	PovTexture::setReference(Default_Texture);
	Default_Texture->Pigment= Create_Pigment();
	Default_Texture->Tnormal= NULL;
	Default_Texture->Finish= Create_Finish();
	Default_Texture->Halo= NULL;

	Frame_Init();
	
	Stage= STAGE_PARSING;

	theScene->useVrml2Map();
	singleContext= theScene->createContext();
	singleContext->setFrame(&Frame);
	theScene->buildContext(singleContext);
	
	if (Frame.Objects == NULL) {
	    Error("No objects in scene.");
	}
	    
	Stage= STAGE_CLEANUP_PARSE;
	
	Destroy_Textures(Default_Texture); 
	Destroy_Camera(Default_Camera); 
	
	Default_Texture= NULL;
	Default_Camera= NULL;
    }
    else {
	Error("Can't parse VRML scene.");
    }
}


void PovRayGlue::defineCamera(void *framePtr, void **params)
{
   DBL Direction_Length = 1.0, Up_Length, Right_Length, Direction_Lenght, Handedness;
   CAMERA *camera;

    camera= ((FRAME *)framePtr)->Camera= Copy_Camera(Default_Camera);

//    camera->Right[0]= -1.33;
    camera->Direction[2]= -1;

    if (params[0] != NULL) {
	camera->Angle= (*((double *)params[0])) *180.0 / M_PI;
         VNormalize(camera->Direction,camera->Direction);
         VLength (Right_Length,camera->Right);
         Direction_Length= Right_Length / tan((*((double *)params[0])))/2.0;
         VScaleEq(camera->Direction, Direction_Length);
    }

    if (params[2] != NULL) {
	camera->Location[0]= ((double *)params[2])[0];
	camera->Location[1]= ((double *)params[2])[1];
	camera->Location[2]= ((double *)params[2])[2];
    }


    if (params[1] != NULL) {
	TrMatrix transfo;
	Vector4 rotator(((double *)params[1])[0], ((double *)params[1])[1],
				((double *)params[1])[2], ((double *)params[1])[3]);
	Vector3 direction(camera->Direction[0], camera->Direction[1], camera->Direction[2]),
			up(camera->Up[0], camera->Up[1], camera->Up[2]),
			right(camera->Right[0], camera->Right[1], camera->Right[2]);
	transfo.rotateBy(&rotator);
	transfo.transformPoint(&direction);
//	transfo.transformPoint(&up);
//	transfo.transformPoint(&right);
	camera->Direction[0]= direction.Vc[0]; camera->Direction[1]= direction.Vc[1]; camera->Direction[2]= direction.Vc[2];
	camera->Up[0]= up.Vc[0]; camera->Up[1]= up.Vc[1]; camera->Up[2]= up.Vc[2];
	camera->Right[0]= right.Vc[0]; camera->Right[1]= right.Vc[1]; camera->Right[2]= right.Vc[2];
    }
}


void PovRayGlue::frameObject(PovObject *anObject)
{
    OBJECT *pObject= (OBJECT *)anObject->getLowObject();

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
		Transform_Textures (pObject->Texture, ((SPHERE *)pObject)->Trans);
		Transform_Halo_Container (pObject->Texture, ((SPHERE *)pObject)->Trans);
	    }
	    break;
    }

    Post_Process(pObject, NULL);
    Link_To_Frame(pObject);
}


#if defined(USE_OLD_CODE)
void *PovRayGlue::createObject(V2OptiDef::Protos aProto, double *transfo, void **params)
{
    OBJECT *result= NULL;

    switch(aProto) {
	case V2OptiDef::v2Box:
	    result= (OBJECT *)createBox(transfo, params);
	    break;
	case V2OptiDef::v2Cone:
	    result= (OBJECT *)createCone(transfo, params);
	    break;
	case V2OptiDef::v2Cylinder:
	    result= (OBJECT *)createCylinder(transfo, params);
	    break;
	case V2OptiDef::v2IndexedFaceSet:
	    // First, scan the indices to see the size of the polynomials.
	    // If all are size 3, then create a mesh.
	    if (((unsigned int)params[13]) != 0) {
		unsigned int minSize, maxSize, nbrIndexes, *indexes, currentSize, nbrPolys;
		nbrIndexes= (unsigned int)params[13];
		indexes= (unsigned int *)params[14];
		nbrPolys= maxSize= currentSize= 0;
		minSize= 0x0FFFFFFFF;
		
		for (; nbrIndexes != 0; indexes++) {
		    if (*indexes == 0x0FFFFFFFF) {
			if (minSize > currentSize) minSize= currentSize;
			if (maxSize < currentSize) maxSize= currentSize;
			nbrPolys++;
			currentSize= 0;
		    }
		    else {
			currentSize++;
		    }
		    nbrIndexes--;
		}
		if (minSize == maxSize) {
		    if (minSize == 3) {
			result= (OBJECT *)createMeshSet(transfo, params, nbrPolys);
		    }
		    else {
			// Create Regular Polygons.
#if defined(USE_POV_POLYGON)
			result= (OBJECT *)createRegularPolygonSet(transfo, params, nbrPolys, minSize);
#else
			if (minSize == 4) {
			    result= (OBJECT *)createSquareSet(transfo, params, nbrPolys);
			}
			else {
				// Must do serious tessalation.
			    result= NULL;
			}
#endif
		    }
		}
		else {
			// Must create unregular polygons.
		    result= NULL;
		}
	    }
	    if (params[3] != NULL) delete[] (float *)params[3];
	    if (params[14] != NULL) delete[] (int *)params[14];
	    break;
	case V2OptiDef::v2Sphere:
	    result= (OBJECT *)createSphere(transfo, params);
	    break;
	case V2OptiDef::v2DirectionalLight:
	    result= (OBJECT *)createDirLight(transfo, params);;
	    break;
	case V2OptiDef::v2PointLight:
	    result= (OBJECT *)createPointLight(transfo, params);;
	    break;
	case V2OptiDef::v2SpotLight:
	    result= (OBJECT *)createSpotLight(transfo, params);;
	    break;
	case V2OptiDef::v2ElevationGrid:		// TODO.
	case V2OptiDef::v2Extrusion:			// TODO.
	case V2OptiDef::v2IndexedLineSet:		// TODO.
	case V2OptiDef::v2PointSet:				// TODO.
	    break;
	default:
	    break;
    }

    return result;
}


void PovRayGlue::modifyTexture(void *aTexture, float **params)
{
    if (aTexture != NULL) {
	TEXTURE *texture= (TEXTURE *)aTexture;
	texture->Pigment->Type= PLAIN_PATTERN;
	if (params[1] != NULL) {
	    texture->Pigment->Colour[0]= (params[1])[0];
	    texture->Pigment->Colour[1]= (params[1])[1];
	    texture->Pigment->Colour[2]= (params[1])[2];
	}
    }
}


void PovRayGlue::addTextureToObject(PovObject *anObject, void *aTexture)
{
    OBJECT *pObject= anObject->getLowObject();

    if (aTexture != NULL) {
	((OBJECT *)pObject)->Type |= TEXTURED_OBJECT;
	Link_Textures(&(((OBJECT *)pObject)->Texture), (TEXTURE *)aTexture);
    }
}


BOX *createBox(double *transfo, void **params)
{
    BOX *aBox= Create_Box();
    if (params[0] != NULL) {
	aBox->bounds[1][0]= ((double *)params[0])[0]/2;
	aBox->bounds[1][1]= ((double *)params[0])[1]/2;
	aBox->bounds[1][2]= ((double *)params[0])[2]/2;
	aBox->bounds[0][0]= -aBox->bounds[1][0];
	aBox->bounds[0][1]= -aBox->bounds[1][1];
	aBox->bounds[0][2]= -aBox->bounds[1][2];
    }
    Compute_Box_BBox(aBox);
    aBox->Trans= createTransform(transfo);
    Compute_Box_BBox(aBox);
    return aBox;
}


CONE *createCone(double *transfo, void **params)
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
    tmpTransfo= createTransform(transfo);
    Transform_Cone((OBJECT *)aCone, tmpTransfo);
    Compute_Cone_BBox(aCone);

    POV_FREE(tmpTransfo);
    return aCone;
}


CONE *createCylinder(double *transfo, void **params)
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

    tmpTransfo= createTransform(transfo);
    Transform_Cone((OBJECT *)aCylinder, tmpTransfo);
    Compute_Cone_BBox(aCylinder);

    POV_FREE(tmpTransfo);
    return aCylinder;
}


MESH *createMeshSet(double *transfo, void **params, unsigned int nbrTriangles)
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

    tmpTransfo= createTransform(transfo);
    Transform_Mesh((OBJECT *)aMesh, tmpTransfo);

    Build_Mesh_BBox_Tree(aMesh);
    POV_FREE(tmpTransfo);
    return aMesh;
}


#if defined(USE_POV_POLYGON)
POLYGON *createRegularPolygonSet(double *transfo, void **params, unsigned int nbrPolys, int polySize)
{
    TRANSFORM *tmpTransfo;
    POLYGON *aPolygon;
    VECTOR *points, *tmpPoint;
    unsigned int *indexes;
    unsigned int i, j;

    aPolygon= Create_Polygon();
    tmpPoint= points= (VECTOR *)POV_MALLOC((nbrPolys * (polySize + 1))*sizeof(VECTOR), "temporary polygon points");
    indexes= (unsigned int *)params[14];
    for (i= 0; i < nbrPolys; i++) {
	for (j= 0; j < polySize; j++, indexes++, tmpPoint++) {
	    (*tmpPoint)[0]= (double)((SNGL_VECT *)params[3])[*indexes][0];
	    (*tmpPoint)[1]= (double)((SNGL_VECT *)params[3])[*indexes][1];
	    (*tmpPoint)[2]= (double)((SNGL_VECT *)params[3])[*indexes][2];
	}
	(*tmpPoint)[0]= (double)((SNGL_VECT *)params[3])[*(indexes - polySize)][0];
	(*tmpPoint)[1]= (double)((SNGL_VECT *)params[3])[*(indexes - polySize)][1];
	(*tmpPoint)[2]= (double)((SNGL_VECT *)params[3])[*(indexes - polySize)][2];
	indexes++; tmpPoint++;
    }

    Compute_Polygon(aPolygon, (nbrPolys * (polySize + 1)), points);
    POV_FREE (points);

    return aPolygon;
}
#else
MESH *createSquareSet(double *transfo, void **params, unsigned int nbrPolys)
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

    tmpTransfo= createTransform(transfo);
    Transform_Mesh((OBJECT *)aMesh, tmpTransfo);

    Build_Mesh_BBox_Tree(aMesh);
    POV_FREE(tmpTransfo);
    return aMesh;

}
#endif


SPHERE *createSphere(double *transfo, void **params)
{
    SPHERE *aSphere= Create_Sphere();
    if (params[0] != NULL) {
	aSphere->Radius= *((double *)params[0]);
    }
    Compute_Sphere_BBox(aSphere);
    aSphere->Methods = &Ellipsoid_Methods;
    aSphere->Trans= createTransform(transfo);
    Compute_Sphere_BBox(aSphere);
    return aSphere;
}


LIGHT_SOURCE *createDirLight(double *transfo, void **params)
{
    DBL Len;
    LIGHT_SOURCE *aLight= Create_Light_Source();
	// TMP: Must multiply center by transfo matrix.
    aLight->Center[0]+= (transfo)[3];
    aLight->Center[1]+= (transfo+4)[3];
    aLight->Center[2]+= (transfo+8)[3];

    VSub(aLight->Direction, aLight->Points_At, aLight->Center);
    VLength(Len, aLight->Direction);
    if (Len > EPSILON) {
	VInverseScaleEq(aLight->Direction, Len);
    }
    return aLight;
}


LIGHT_SOURCE *createPointLight(double *transfo, void **params)
{
    DBL Len;
    LIGHT_SOURCE *aLight= Create_Light_Source();
	// TMP: Must multiply center by transfo matrix.
    aLight->Center[0]+= (transfo)[3];
    aLight->Center[1]+= (transfo+4)[3];
    aLight->Center[2]+= (transfo+8)[3];

    VSub(aLight->Direction, aLight->Points_At, aLight->Center);
    VLength(Len, aLight->Direction);
    if (Len > EPSILON) {
	VInverseScaleEq(aLight->Direction, Len);
    }
    return aLight;
}


LIGHT_SOURCE *createSpotLight(double *transfo, void **params)
{
    DBL Len;
    LIGHT_SOURCE *aLight= Create_Light_Source();
	// TMP: Must multiply center by transfo matrix.
    aLight->Center[0]+= (transfo)[3];
    aLight->Center[1]+= (transfo+4)[3];
    aLight->Center[2]+= (transfo+8)[3];

    VSub(aLight->Direction, aLight->Points_At, aLight->Center);
    VLength(Len, aLight->Direction);
    if (Len > EPSILON) {
	VInverseScaleEq(aLight->Direction, Len);
    }
    return aLight;
}
#endif

