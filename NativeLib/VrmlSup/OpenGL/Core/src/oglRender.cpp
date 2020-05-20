/**************************************************
* File: oglRender.cc.
* Desc: Implementation of the OglRenderer class.
* Module: AkraLog : VrmlSupport.
* Rev: 18 septembre 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <vrcore/utility.h>
#include <vrcore/generalVrml.h>
#include <vrcore/containerStack.h>
#include <vrcore/geometriesVrml.h>
#include <vrcore/materialVrml.h>
#include <vrcore/lightingVrml.h>

#include <GL/gl.h>
#include <GL/glu.h>
#if defined(__linux__)
#include <GL/glut.h>
#else
#include <GL/glut.h>
#endif

#include "guiControler.h"
#include "oglRender.h"

#define RADTODEG	(180.0f / 3.141592654f)
#define DEGTORAD	(3.141592654f / 180.0f)

#define NBR_SLICES	25
#define GLUT_STACK 25

/**************************************************
* Implementation: OglRenderer.
**************************************************/

OglRenderer::OglRenderer(void)
{
    containerStack= new ContainerStack();
    guiControler= new GuiControler(320, 200);
    flags= 0;
}


OglRenderer::~OglRenderer(void)
{
// TODO.
}


void OglRenderer::initOGL(int width, int height)
{
    float ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
    float diffuse[] = {0.5f, 1.0f, 1.0f, 1.0f};
    float position[] = {90.0f, 90.0f, 150.0f, 0.0f};
    float front_mat_shininess[] = {30.0f};
    float front_mat_specular[] = {0.2f, 0.2f, 0.2f, 1.0f};
    float front_mat_diffuse[] = {0.5f, 0.28f, 0.38f, 1.0f};
    float back_mat_shininess[] = {50.0f};
    float back_mat_specular[] = {0.5f, 0.5f, 0.2f, 1.0f};
    float back_mat_diffuse[] = {1.0f, 1.0f, 0.2f, 1.0f};
    float lmodel_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float lmodel_twoside[] = {(float)GL_TRUE};

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

//    glDrawBuffer(GL_FRONT);
    glEnable(GL_DEPTH_TEST);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHTING);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0f);
    glEnable(GL_LIGHT0);

    AkCoord4D firstLook(0.0f, 1.0f, 0.0f, 0.0f);
    AkCoord3D firstPos(0.0f, 15.0f, 200.0f);
    GLfloat aspect= (GLfloat) width / height;

    viewPoint= new ViewPointVrml(firstPos, firstLook, 0.785398f);
    guiControler->locateAt(viewPoint);
    minView= 1.0f;
    maxView= 2000.0f;
    flags= drawSolid;

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(45.0f, aspect, minView, maxView);
    glMatrixMode( GL_MODELVIEW );
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void OglRenderer::useViewPoint(ViewPointVrml *aVP)
{
    if (viewPoint != NULL) delete viewPoint;
    viewPoint= aVP;
}


void OglRenderer::pushContainer(ContainerVrml *aContainer)
{
    containerStack->push(aContainer);
}


void OglRenderer::popContainer(void)
{
    containerStack->pop();
}


void OglRenderer::deleteSceneTree(void)
{
    if (nodeTree != NULL) {
	nodeTree->destroy();
    }
    nodeTree= NULL;
}


void OglRenderer::renderTree(void)
{
    if (nodeTree != NULL) {
	nodeTree->renderOn(this);
    }
}


void OglRenderer::addToScene(NodeVrml *aNode)
{
    if (nodeTree == NULL) nodeTree= aNode;
    else {
	if (containerStack->isUsed()) {
	    containerStack->topNode()->addChild(aNode);
	}
    }
}


void OglRenderer::drawBox(BoxVrml *aBox, AppearanceVrml *appearance)
{
	float edges[8][3];
	GLenum faceMode;

	edges[0][0]= -0.5 * aBox->size[0];
	edges[0][1]= -0.5 * aBox->size[1];
	edges[0][2]= -0.5 * aBox->size[2];
	edges[1][0]= 0.5 * aBox->size[0];
	edges[1][1]= -0.5 * aBox->size[1];
	edges[1][2]= -0.5 * aBox->size[2];
	edges[2][0]= 0.5 * aBox->size[0];
	edges[2][1]= 0.5 * aBox->size[1];
	edges[2][2]= -0.5 * aBox->size[2];
	edges[3][0]= -0.5 * aBox->size[0];
	edges[3][1]= 0.5 * aBox->size[1];
	edges[3][2]= -0.5 * aBox->size[2];
	edges[4][0]= -0.5 * aBox->size[0];
	edges[4][1]= -0.5 * aBox->size[1];
	edges[4][2]= 0.5 * aBox->size[2];
	edges[5][0]= 0.5 * aBox->size[0];
	edges[5][1]= -0.5 * aBox->size[1];
	edges[5][2]= 0.5 * aBox->size[2];
	edges[6][0]= 0.5 * aBox->size[0];
	edges[6][1]= 0.5 * aBox->size[1];
	edges[6][2]= 0.5 * aBox->size[2];
	edges[7][0]= -0.5 * aBox->size[0];
	edges[7][1]= 0.5 * aBox->size[1];
	edges[7][2]= 0.5 * aBox->size[2];

    if (appearance != NULL) setAppearance(appearance);

    if (((flags & OglRenderer::drawMode) == OglRenderer::drawSolid)
	|| ((flags & OglRenderer::drawMode) == OglRenderer::drawFacettes)) {
		faceMode= GL_QUADS;
	}
    else if ((flags & OglRenderer::drawMode) == OglRenderer::drawWire) {
		faceMode= GL_LINE_LOOP;
    }

	glBegin(faceMode);
	  glVertex3fv(edges[0]);
	  glVertex3fv(edges[1]);
	  glVertex3fv(edges[2]);
	  glVertex3fv(edges[3]);
	glEnd();
	glBegin(faceMode);
	  glVertex3fv(edges[1]);
	  glVertex3fv(edges[5]);
	  glVertex3fv(edges[6]);
	  glVertex3fv(edges[2]);
	glEnd();
	glBegin(faceMode);
	  glVertex3fv(edges[5]);
	  glVertex3fv(edges[4]);
	  glVertex3fv(edges[7]);
	  glVertex3fv(edges[6]);
	glEnd();
	glBegin(faceMode);
	  glVertex3fv(edges[4]);
	  glVertex3fv(edges[0]);
	  glVertex3fv(edges[3]);
	  glVertex3fv(edges[7]);
	glEnd();
	glBegin(faceMode);
	  glVertex3fv(edges[3]);
	  glVertex3fv(edges[2]);
	  glVertex3fv(edges[6]);
	  glVertex3fv(edges[7]);
	glEnd();
	glBegin(faceMode);
	  glVertex3fv(edges[5]);
	  glVertex3fv(edges[4]);
	  glVertex3fv(edges[0]);
	  glVertex3fv(edges[1]);
	glEnd();
}


void OglRenderer::drawCone(ConeVrml *aCone, AppearanceVrml *appearance)
{
    if (appearance != NULL) setAppearance(appearance);
    if (((flags & OglRenderer::drawMode) == OglRenderer::drawSolid)
	|| ((flags & OglRenderer::drawMode) == OglRenderer::drawFacettes)) {
		glutSolidCone(aCone->bottomRadius, aCone->height, NBR_SLICES, GLUT_STACK);
    }
    else if ((flags & OglRenderer::drawMode) == OglRenderer::drawWire) {
		glutWireCone(aCone->bottomRadius, aCone->height, NBR_SLICES, GLUT_STACK);
    }
}


void OglRenderer::drawCylinder(CylinderVrml *aCylinder, AppearanceVrml *appearance)
{
// ATTN-990928 [HD]: Temporary solution for cylinder drawing.  Loose memory on
// exit.
	static GLUquadricObj *gluQuad= gluNewQuadric();

    if (appearance != NULL) setAppearance(appearance);
    if (((flags & OglRenderer::drawMode) == OglRenderer::drawSolid)
	|| ((flags & OglRenderer::drawMode) == OglRenderer::drawFacettes)) {
		gluQuadricDrawStyle(gluQuad, GLU_FILL);
		glTranslatef(0.0f, aCylinder->height/2.0f, 0.0f);
		gluCylinder(gluQuad, aCylinder->radius, aCylinder->radius, aCylinder->height, NBR_SLICES, GLUT_STACK);
		glTranslatef(0.0f, -aCylinder->height/2.0f, 0.0f);
    }
    else if ((flags & OglRenderer::drawMode) == OglRenderer::drawWire) {
		gluQuadricDrawStyle(gluQuad, GLU_LINE);
		glTranslatef(0.0f, aCylinder->height/2.0f, 0.0f);
		gluCylinder(gluQuad, aCylinder->radius, aCylinder->radius, aCylinder->height, NBR_SLICES, GLUT_STACK);
		glTranslatef(0.0f, aCylinder->height/2.0f, 0.0f);
    }
}


void OglRenderer::drawElevationGrid(ElevationGridVrml *aGrid, AppearanceVrml *appearance)
{
// TODO.
}


void OglRenderer::drawExtrusion(ExtrusionVrml *anExtrusion, AppearanceVrml *appearance)
{
// TODO.
}


void OglRenderer::drawIndexedFaceSet(IndexedFaceSetVrml *aFaceSet, AppearanceVrml *appearance)
{
    unsigned int i;

    if (appearance != NULL) setAppearance(appearance);
    if (((flags & OglRenderer::drawMode) == OglRenderer::drawSolid)
	|| ((flags & OglRenderer::drawMode) == OglRenderer::drawFacettes)) {
	glBegin(GL_TRIANGLES);
    }
    else if ((flags & OglRenderer::drawMode) == OglRenderer::drawWire) {
	glBegin(GL_LINES);
    }
    for (i= 0; i < aFaceSet->nbrIndexes; i++) {
        if (aFaceSet->indexes[i] == -1) continue;
	glVertex3fv(aFaceSet->coordinates[aFaceSet->indexes[i]].points);
    }
    glEnd();
}


void OglRenderer::drawIndexedLineSet(IndexedLineSetVrml *aLineSet, AppearanceVrml *appearance)
{
// TODO.
}


void OglRenderer::drawPointSet(PointSetVrml *aPointSet, AppearanceVrml *appearance)
{
// TODO.
}


void OglRenderer::drawSphere(SphereVrml *aSphere, AppearanceVrml *appearance)
{
    if (appearance != NULL) setAppearance(appearance);
    if (((flags & OglRenderer::drawMode) == OglRenderer::drawSolid)
	|| ((flags & OglRenderer::drawMode) == OglRenderer::drawFacettes)) {
		glutSolidSphere(aSphere->radius, NBR_SLICES, GLUT_STACK);
    }
    else if ((flags & OglRenderer::drawMode) == OglRenderer::drawWire) {
		glutWireSphere(aSphere->radius, NBR_SLICES, GLUT_STACK);
    }
}


void OglRenderer::drawText(TextVrml *aText, AppearanceVrml *appearance)
{
// TODO.
}


void OglRenderer::openTransform(TransformVrml *aTransform)
{
    glPushMatrix();
    glTranslatef(aTransform->translation.points[0], aTransform->translation.points[1], aTransform->translation.points[2]);
    glTranslatef(aTransform->center.points[0], aTransform->center.points[1], aTransform->center.points[2]);
    glRotatef(aTransform->rotation.points[3] * 180.f / 3.1415926f, aTransform->rotation.points[0], aTransform->rotation.points[1], aTransform->rotation.points[2]);
    glRotatef(aTransform->scaleOrient.points[3] * 180.f / 3.1415926f, aTransform->scaleOrient.points[0], aTransform->scaleOrient.points[1], aTransform->scaleOrient.points[2]);
    glScalef(aTransform->scale.points[0], aTransform->scale.points[1], aTransform->scale.points[2]);
    glRotatef(aTransform->scaleOrient.points[3] * -180.f / 3.1415926f, aTransform->scaleOrient.points[0], aTransform->scaleOrient.points[1], aTransform->scaleOrient.points[2]);
    glTranslatef(-aTransform->center.points[0], -aTransform->center.points[1], -aTransform->center.points[2]);
}


void OglRenderer::closeTransform(TransformVrml *aTransform)
{
    glPopMatrix();
}


void OglRenderer::addLighting(LightVrml *aLight)
{
    DirectionalLightVrml *dirLight;
    PointLightVrml *pointLight;
    SpotLightVrml *spotLight;

    switch(aLight->getKind()) {
	case LightVrml::directional:
	    dirLight= (DirectionalLightVrml *)aLight;
	    break;
	case LightVrml::point:
	    pointLight= (PointLightVrml *)aLight;
	    break;
	case LightVrml::spot:
	    spotLight= (SpotLightVrml *)aLight;
	    break;
	default:
		// TODO: Catch the internal error.
	    break;
    }
}


void OglRenderer::setAppearance(AppearanceVrml *appearance)
{
    MaterialVrml *aMaterial;

    if ((aMaterial= appearance->getMaterial()) != NULL) {
	AkColor diffuse, emissive, specular;
	float ambient, shine, transparency;
	float rgbaColor[4];

	aMaterial->getColors(&diffuse, &emissive, &specular);
	aMaterial->getParams(&ambient, &shine, &transparency);
	rgbaColor[0]= diffuse.indexes[0];
	rgbaColor[1]= diffuse.indexes[1];
	rgbaColor[2]= diffuse.indexes[2];
	rgbaColor[3]= 1.0f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rgbaColor);
//	glColor3f(aColor->indexes[0], aColor->indexes[1], aColor->indexes[2]);
    }
}


void OglRenderer::windowDidResize(int newWidth, int newHeight)
{
    GLfloat aspect;

    aspect= (GLfloat)newWidth / newHeight;
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
//    gluPerspective(90.0f, aspect, 1.0f, 2000.0f);
    gluPerspective(viewPoint->fieldOfView * RADTODEG, aspect, 1.0f, 2000.0f);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, newWidth, newHeight);
    guiControler->resize(newWidth, newHeight);
}



