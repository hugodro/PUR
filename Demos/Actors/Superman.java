/**************************************************
* File: Superman.java.
* Desc: Robot base sur le modele geometrique Postava
* Rev : 8 septembre 1998 : REV 0 : Hugo DesRosiers : Creation
**************************************************/

package akralog.animation.demos;

import akralog.animation.demos.Postava;
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



public class Superman extends Actor {
    public Postava geometry;

    static ToyEvent depart;
    static ToyEvent plancher;
    static ToyEvent marche;

    int frameCounter, totalFrame;
    float deltaAngle;
    float dispX, dispY;

    static {
	depart= new ToyEvent();
	plancher= new ToyEvent();
	marche=  new ToyEvent();
    }


    public Superman() 
    {
	Viewpoint sceneVP;
	float position[], orientVP[];

	geometry= new Postava();
	body= geometry.bonhomme;
	sceneVP= geometry.camVP;

	position= sceneVP.position;
	orientVP= sceneVP.orientation;
//	position[0]= 0.0;
//	position[2]= 70.0f;
//	orientVP[3]= 150.0f;
    }


    public void action()
    {
	fire depart;
	wait();
    }


    pstate Superman() {			// FSM initial state.
	totalFrame= 90;
	dispX= -494.7f / totalFrame * 1.45f;
	dispY= dispX * 0.354074453f;
	deltaAngle= 6.28318512 / totalFrame;
//	deltaAngle= 1.57 / totalFrame;
	blocking transition depart : Allume();
    }


    pstate Allume()
    {
	RenderingDevice myRenderer;	// TMP: contourne un 'TODO'.
	myRenderer= geometry.renderDevice;

	frameCounter= 0;

// TMP: Replace automatic time management.
	myRenderer.renderScene();

	transition default : Oscille();
    }


    pstate Oscille()
    {
	Transform transform;	// TMP: contourne un 'TODO'.
	float position[];		//  Idem.
	float rotation[];			//  Idem.
	float center[];			// Idem.
	RenderingDevice myRenderer;	//  Idem.

	transform= body;

	position= transform.translation;
	rotation= transform.rotation;
	center= transform.center;
	myRenderer= geometry.renderDevice;

	if (rotation[3] > 0.1403) {
	    position[0]+= dispX;
	    position[1]+= dispY;
	}
	center[0]=  35.0f;
	center[1]=  - 45.0f;

	if (rotation[3] > 0.56794487096) {
	    deltaAngle*= -1.0f;
	}
	else if (rotation[3] < -0.2) {
	    deltaAngle*= -1.0f;
	}
	rotation[3]+=  deltaAngle;

	frameCounter+= 1;
	if (frameCounter > totalFrame) fire plancher;

// TMP: Replace automatic time management.
	myRenderer.renderScene();

	transition default : Oscille();
	transition plancher : Arrete();
    }


    pstate Arrete()
    {
	notifyAll();
	transition default : break;
    }

}
