/**************************************************
* File: pObject.cc.
* Desc: Implementation of the PovObject class.
* Module: AkraLog : PovRayExt.
* Rev: 22 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "maths.h"
#include "pObject.h"


extern "C" {
#undef __cplusplus
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
#define __cplusplus
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


PovObject *PovObject::createRegularPolygonSet(TrMatrix *aTransfo, void **params, unsigned int nbrPolys, int polySize)
{
    // ATTN: TODO.
    return NULL;
}


PovObject *PovObject::createPolygonSet(TrMatrix *aTransfo, void **params, unsigned int nbrPolys)
{
    // ATTN: TODO.
    return NULL;
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

