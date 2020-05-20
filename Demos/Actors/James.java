/**************************************************
* File: James.java.
* Desc: Auto conversion of VRML file.
* Rev : Tue Apr 28 20:36:03 1998.
**************************************************/

package akralog.animation.demos;

import akralog.animation.demos.RobotHumain;
import akralog.animation.RenderingDevice;
import akralog.animation.GenericScene;
import akralog.animation.Actor;
import vrml.basic.*;
import vrml.nodes.*;


class Transition_Demo {
    final int myID= 0;
}

class ToyEvent extends Transition_Demo {
    int gaga;
}



public class James extends Actor {

    public RobotHumain geometry;

    static ToyEvent depart;
    static ToyEvent apex;
    static ToyEvent marche;
    static ToyEvent termine;
    boolean faitUnCycle;

    static {
	depart= new ToyEvent();
	apex= new ToyEvent();
	marche=  new ToyEvent();
	termine= new ToyEvent();
    }


    public James() 
    {
	Viewpoint sceneVP;
	float position[], orientVP[];

	geometry= new RobotHumain();
	body= geometry.rootNodes[0];
	sceneVP= geometry.camVP;

	position= sceneVP.position;
	orientVP= sceneVP.orientation;
	position[0]= 600.0f;
	position[1]= 175.0f;
	position[2]= 200.0f;
//	position[2]= 800.0f;
	orientVP[0]= 0.0f; orientVP[1]= 1.0f; orientVP[2]= 0.0f;
	orientVP[3]= 1.5758f;
    }


    public void action()
    {
	fire depart;
	wait();
    }

    void defJambeServo(Transform transform)
    {
	float rotation[];			//  Idem.
	float center[];			// Idem.
	rotation= transform.rotation;
	center= transform.center;
	rotation[0]= 1.0f; rotation[1]= 0.0f; rotation[2]= 0.0f;
	center[2]= 30.0f;
	center[1]= 124.5f;
    }

    void defBasJambeServo(Transform transform)
    {
	float rotation[];			//  Idem.
	float center[];			// Idem.
	rotation= transform.rotation;
	center= transform.center;
	rotation[0]= 1.0f; rotation[1]= 0.0f; rotation[2]= 0.0f;
	center[2]= 30.0f;
	center[1]= 69.25f;
    }


    void defPiedServo(Transform transform)
    {
	float rotation[];			//  Idem.
	float center[];			// Idem.
	rotation= transform.rotation;
	center= transform.center;
	rotation[0]= 1.0f; rotation[1]= 0.0f; rotation[2]= 0.0f;
	center[2]= 30.0f;
	center[1]= 15.92f;
    }


    pstate James() {			// FSM initial state.
	defJambeServo(geometry.jambeG);
	defJambeServo(geometry.jambeD);
	defBasJambeServo(geometry.basJambeG);
	defBasJambeServo(geometry.basJambeD);
	defPiedServo(geometry.piedG);
	defPiedServo(geometry.piedD);

	faitUnCycle= false;

	blocking transition depart : Allume();
    }


    pstate Allume()
    {
	Transform transform;	// TMP: contourne un 'TODO'.
	RenderingDevice myRenderer;	// TMP: contourne un 'TODO'.

	myRenderer= geometry.renderDevice;

// TMP-Note 1: Replace automatic time management.
	myRenderer.renderScene();

	transition default : DebutMarche();
    }


    pstate DebutMarche()
    {
	Transform tJambeG, tBasJG;	// TMP: contourne un 'TODO'.
	RenderingDevice myRenderer;	//  Idem.
	float rotJambe, rotGenou;		// Idem.

	myRenderer= geometry.renderDevice;

	tJambeG= geometry.jambeG;
	tBasJG= geometry.basJambeG;
	tJambeG.rotation[3]+= -0.07870f;
	tBasJG.rotation[3]+= 0.07870f;

	if (tJambeG.rotation[3] < -0.7870) {
	    fire apex;
	}

	myRenderer.renderScene();		// TMP: cf note 1.

	transition default : DebutMarche();
	transition apex : DebutT2Marche();
    }


    pstate DebutT2Marche()
    {
	Transform transform;	// TMP: contourne un 'TODO'.
	float position[];		//  Idem.
	float rotation[];			//  Idem.
	RenderingDevice myRenderer;	//  Idem.

	myRenderer= geometry.renderDevice;

	transform= geometry.basJambeG;
	position= transform.translation;
	rotation= transform.rotation;
	rotation[3]-= 0.07870f;
	if (rotation[3] < 0.3148) {
	    fire apex;
	}

	transform= geometry.piedD;
	rotation= transform.rotation;
	rotation[3]-= 0.07870f;
	transform= geometry.jambeD;
	rotation= transform.rotation;
	rotation[3]+= 0.07870f;

	transform= geometry.robot;
	position= transform.translation;
	position[1]-= 0.5f;
	position[2]+= 2.0f;

	myRenderer.renderScene();		// TMP: cf note 1.

	transition default : DebutT2Marche();
	transition apex : PasActifGaucheTirant();
    }



    pstate PasActifDroitTirant()
    {
	Transform transform;	// TMP: contourne un 'TODO'.
	float position[];		//  Idem.
	float rotation[];			//  Idem.
	RenderingDevice myRenderer;	//  Idem.

	myRenderer= geometry.renderDevice;

	transform= geometry.jambeD;
	position= transform.translation;
	rotation= transform.rotation;

	rotation[3]+= 0.07870f;
	if (rotation[3] > -0.0787f) {
	    fire apex;
	}

	transform= geometry.jambeG;
	rotation= transform.rotation;
	rotation[3]-= 0.07870f;
	transform= geometry.basJambeG;
	rotation= transform.rotation;
	rotation[3]+= 0.1f;

	transform= geometry.robot;
	position= transform.translation;
	position[1]+= 0.5f;
	position[2]+= 9.0f;

	myRenderer.renderScene();		// TMP: cf note 1.

	transition default : PasActifDroitTirant();
	transition apex : PasActifDroitPoussant();
    }


    pstate PasActifDroitPoussant()
    {
	Transform transform;	// TMP: contourne un 'TODO'.
	float position[];		//  Idem.
	float rotation[];			//  Idem.
	RenderingDevice myRenderer;	//  Idem.

	myRenderer= geometry.renderDevice;

	transform= geometry.jambeD;
	position= transform.translation;
	rotation= transform.rotation;

	rotation[3]+= 0.0787f;
	if (rotation[3] > 0.6296f) {
//	    fire apex;
	    fire termine;
	}

	if (rotation[3] < -0.1) {
	    transform= geometry.basJambeG;
	    rotation= transform.rotation;
	    rotation[3]+= 0.07870f;
	    transform= geometry.piedG;
	    rotation= transform.rotation;
	    rotation[3]-= 0.1574f;
	    position[1]-= 0.5f;
	}
	else {
	    transform= geometry.basJambeG;
	    rotation= transform.rotation;
	    rotation[3]-= 0.09f;
	    transform= geometry.piedD;
	    rotation= transform.rotation;
	    rotation[3]-= 0.0787f;
	    position[1]-= 0.8f;
	}
	transform= geometry.jambeG;
	rotation= transform.rotation;
	rotation[3]-= 0.07870f;

	transform= geometry.robot;
	position= transform.translation;
	position[2]+= 9.0f;

	myRenderer.renderScene();		// TMP: cf note 1.

	transition default : PasActifDroitPoussant();
	transition apex : PasActifGaucheTirant();
	transition termine : Arrete();
    }


    pstate PasActifGaucheTirant()
    {
	Transform transform;	// TMP: contourne un 'TODO'.
	float position[];		//  Idem.
	float rotation[];			//  Idem.
	RenderingDevice myRenderer;	//  Idem.

	myRenderer= geometry.renderDevice;

	transform= geometry.jambeG;
	position= transform.translation;
	rotation= transform.rotation;

	rotation[3]+= 0.07870f;
	if (rotation[3] > -0.0787f) {
	    fire apex;
	}

	transform= geometry.jambeD;
	rotation= transform.rotation;
	rotation[3]-= 0.07870f;
	transform= geometry.basJambeD;
	rotation= transform.rotation;
	rotation[3]+= 0.07870f;
	transform= geometry.piedD;
	rotation= transform.rotation;
	rotation[3]+= 0.07870f;

	transform= geometry.robot;
	position= transform.translation;
	position[1]+= 0.5f;
	position[2]+= 9.0f;

	myRenderer.renderScene();		// TMP: cf note 1.

	transition default : PasActifGaucheTirant();
	transition apex : PasActifGauchePoussant();
    }


    pstate PasActifGauchePoussant()
    {
	Transform transform;	// TMP: contourne un 'TODO'.
	float position[];		//  Idem.
	float rotation[];			//  Idem.
	RenderingDevice myRenderer;	//  Idem.

	myRenderer= geometry.renderDevice;

	transform= geometry.jambeG;
	position= transform.translation;
	rotation= transform.rotation;

	rotation[3]+= 0.0787f;
	if (rotation[3] > 0.6296f) {
	    fire apex;
	}

	if (rotation[3] < -0.1) {
	    transform= geometry.basJambeD;
	    rotation= transform.rotation;
	    rotation[3]+= 0.07870f;
	    transform= geometry.piedD;
	    rotation= transform.rotation;
	    rotation[3]-= 0.1574f;
	    position[1]-= 0.5f;
	}
	else {
	    transform= geometry.basJambeD;
	    rotation= transform.rotation;
	    rotation[3]-= 0.09f;
	    transform= geometry.piedG;
	    rotation= transform.rotation;
	    rotation[3]-= 0.0787f;
	    position[1]-= 0.8f;
	}
	transform= geometry.jambeD;
	rotation= transform.rotation;
	rotation[3]-= 0.07870f;

	transform= geometry.robot;
	position= transform.translation;
	position[2]+= 9.0f;

	myRenderer.renderScene();		// TMP: cf note 1.

	transition default : PasActifGauchePoussant();
	transition apex : PasActifDroitTirant();
    }


    pstate Arrete()
    {
	notifyAll();
	transition default : break;
    }

}
