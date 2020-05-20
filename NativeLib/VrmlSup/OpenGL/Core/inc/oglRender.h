#ifndef _VRML_OGL_H_
#define _VRML_OGL_H_
/**************************************************
* File: oglRender.h.
* Desc: Definition of the OglRenderer class.
* Module: AkraLog : VrmlSupport.
* Rev: 18 septembre 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/portableDefs.h>
#include <vrcore/rendererVrml.h>


class OglRenderer : public RendererVrml {
  protected:
    enum Constants {
      drawSolid= 1, drawFacettes= 2, drawWire= 3, drawPoint= 4
      , moveTrolley= (1 << 4), moveOrbit= (2 << 4),
	drawMode= 0x0F, moveMode= 0x0F0
    };

  protected:
    float minView, maxView;		// Near/far cropping for the rendering.
    unsigned int flags;			// Drawing flags.
    ContainerStack *containerStack;
    GuiControler *guiControler;

  public:
    OglRenderer(void);
    virtual ~OglRenderer(void);
    virtual void initOGL(int width, int height);
    virtual void pushContainer(ContainerVrml *aContainer);
    virtual void popContainer(void);
    virtual void deleteSceneTree(void);
    virtual void renderTree(void);
    virtual void addToScene(NodeVrml *aNode);
    virtual void drawBox(BoxVrml *aBox, AppearanceVrml *appearance);
    virtual void drawCone(ConeVrml *aCone, AppearanceVrml *appearance);
    virtual void drawCylinder(CylinderVrml *aCylinder, AppearanceVrml *appearance);
    virtual void drawElevationGrid(ElevationGridVrml *aGrid, AppearanceVrml *appearance);
    virtual void drawExtrusion(ExtrusionVrml *anExtrusion, AppearanceVrml *appearance);
    virtual void drawIndexedFaceSet(IndexedFaceSetVrml *aFaceSet, AppearanceVrml *appearance);
    virtual void drawIndexedLineSet(IndexedLineSetVrml *aLineSet, AppearanceVrml *appearance);
    virtual void drawPointSet(PointSetVrml *aPointSet, AppearanceVrml *appearance);
    virtual void drawSphere(SphereVrml *aSphere, AppearanceVrml *appearance);
    virtual void drawText(TextVrml *aText, AppearanceVrml *appearance);
    virtual void openTransform(TransformVrml *aTransform);
    virtual void closeTransform(TransformVrml *aTransform);
    virtual void addLighting(LightVrml *aLight);
    virtual void setAppearance(AppearanceVrml *appearance);
    virtual void useViewPoint(ViewPointVrml *aVP);

    void windowDidResize(int newWidth, int newHeight);
};


#endif		/* _VRML_OGL_H_ */

