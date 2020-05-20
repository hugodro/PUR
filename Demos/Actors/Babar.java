/**************************************************
* File: Babar.java.
* Desc: Robot utilisant la geometrie d'un elephant.
* Rev : 23 septembre 1998 : REV 0 : Hugo DesRosiers : Creation
**************************************************/

package akralog.animation.demos;

import akralog.animation.demos.Elephant;
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



public class Babar extends Actor {
    public Elephant geometry;

    static ToyEvent depart;
    static ToyEvent plancher;
    static ToyEvent marche;

    int frameCounter, totalFrame;

    static {
	depart= new ToyEvent();
	plancher= new ToyEvent();
	marche=  new ToyEvent();
    }


    public Babar() 
    {
	Viewpoint sceneVP;
	Transform babar;
	float position[], orientVP[];

	geometry= new Elephant();
	body= geometry.body;
	babar= geometry.body;
	sceneVP= geometry.camVP;

	position= sceneVP.position;
	orientVP= sceneVP.orientation;
	position[0]= 0.0;
	position[1]= 0.0;
	position[2]= 10.0;
	orientVP[0]= 0.0;
	orientVP[1]= 1.0;
	orientVP[2]= 0.0;
	orientVP[3]= 0.0;

	babar.translation[2]-= 3.0f;
    }


    public void action()
    {
	fire depart;
	wait();
    }


    pstate Babar() {			// FSM initial state.
	totalFrame= 8;

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
	Transform babar;
	RenderingDevice myRenderer;	// TMP: contourne un 'TODO'.
	float position[], orientVP[];
	myRenderer= geometry.renderDevice;

	frameCounter+= 1;
	if (frameCounter > totalFrame) fire plancher;

	babar= geometry.body;

	babar.rotation[3]-= 0.15f;

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
