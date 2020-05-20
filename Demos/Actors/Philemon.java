/**************************************************
* File: Philemon.java.
* Desc: Insecte volant, base sur le modele geometrique Papillon
* Rev : 17 septembre 1998 : REV 0 : Hugo DesRosiers : Creation
**************************************************/

package pur.actors;

import pur.geometries.Papillon;

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



public class Philemon extends Actor {
    public Papillon geometry;
    public float movCoeff[];

    static ToyEvent depart;
    static ToyEvent finCycle;
    static ToyEvent plancher;
    static ToyEvent marche;

    int frameCounter, seqLength, totalFrame;

    static {
	depart= new ToyEvent();
	finCycle= new ToyEvent();
	plancher= new ToyEvent();
	marche=  new ToyEvent();
    }


    public Philemon() 
    {
	Viewpoint sceneVP;    // Shortcut.
	float position[], orientVP[];

	geometry= new Papillon();
	body= geometry.papillon;
	sceneVP= geometry.camVP;

	position= sceneVP.position;
	orientVP= sceneVP.orientation;
	position[0]= 0.0f;
	position[1]= 0.0f;
	position[2]= 0.15f;
	orientVP[3]= 0.3f;

	movCoeff= new float[4];
    }


    public void action()
    {
	fire depart;
	wait();
    }


    pstate Philemon() {			// FSM initial state.
	totalFrame= 48;
	seqLength= 8;

	blocking transition depart : Allume();
    }


    pstate Allume()
    {
	Transform papillon, aileDroite, aileGauche;
	RenderingDevice myRenderer;	// TMP: contourne un 'TODO'.
	float position[];
	myRenderer= geometry.renderDevice;

	frameCounter= 0;

	papillon= geometry.papillon;
	aileDroite= geometry.aileDroite;
	aileGauche= geometry.aileGauche;

	papillon.rotation[0]= 0.0f;
	papillon.rotation[1]= 0.0f;
	papillon.rotation[2]= 1.0f;
	papillon.rotation[3]= 1.30f;

	aileDroite.rotation[0]= 0.0;
	aileDroite.rotation[1]= 1.0;
	aileDroite.rotation[2]= 0.0;
	aileDroite.rotation[3]= 0.7870f;
	aileGauche.rotation[0]= 0.0;
	aileGauche.rotation[1]= 1.0;
	aileGauche.rotation[2]= 0.0;
	aileGauche.rotation[3]= -0.7870f;

        movCoeff[0]= -0.1f;
	movCoeff[1]= -0.2f;
	movCoeff[2]= 0.2f;
	movCoeff[3]= 0.01;


// TMP: Replace automatic time management.
	myRenderer.renderScene();

	transition default : Oscille();
    }


    pstate Oscille()
    {
	Transform papillon, aileDroite, aileGauche;
	RenderingDevice myRenderer;	// TMP: contourne un 'TODO'.
	Viewpoint sceneVP;    // Shortcut.
	float position[], orientVP[];
	myRenderer= geometry.renderDevice;

	frameCounter+= 1;
	if (frameCounter > totalFrame) fire plancher;
	else
	    if (frameCounter % seqLength == 0) fire finCycle;
	papillon= geometry.papillon;
	aileDroite= geometry.aileDroite;
	aileGauche= geometry.aileGauche;

	papillon.rotation[3]+= movCoeff[0];  // -0.1f;
	aileDroite.rotation[3]+= movCoeff[1];  // -0.2f;
	aileGauche.rotation[3]+= movCoeff[2];   // 0.2f;

       papillon.translation[2]+= movCoeff[3];
        if (frameCounter % 12 == 0) movCoeff[3]*= -1.0f;

// TMP: Replace automatic time management.
	myRenderer.renderScene();

	transition default : Oscille();
	transition finCycle : Renverse();
	transition plancher : Arrete();
    }

    pstate Renverse()
    {
	movCoeff[1]*= -1.0f;
	movCoeff[2]*= -1.0f;

	transition default : Oscille();
    }


    pstate Arrete()
    {
	notifyAll();
	transition default : break;
    }

}
