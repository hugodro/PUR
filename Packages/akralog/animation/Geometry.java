/**************************************************
* File: Geometry.java.
* Desc: Implementation of the Geometry class.
* Module: AkraLog : Animation.
* Rev: 24 septembre 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

package akralog.animation;

import java.util.HashTable;
import vrml.basic.SFNode;
import vrml.nodes.Group;


public class Geometry {

    Transform root;			// This will be the future entry point to the 3D hierarchy.
    HashTable components;	// Pair of <name, Transform> that represent the important points of the
    						// geometry;
    SFNode[] rootNodes;		// TMP: The parsed vrml tree is put there.  To be replaced by 'root'.

	// Generic constructor.
    public Geometry() {
    }

    public Transform getRoot() {
	return root;
    }

	// TMP: Accessor to the root nodes of the vrml parsed tree.
    public SFNode[] getNodes() {
	return rootNodes;
    }


    public Transform getComponent(String aName) {
	return components.get(aName);
    }


    public void archive(String destination) {		// Saves the geometry to a JST.
    }


    public void unarchive(String source) {	// Recreates a Geometry (or subclass) from a JST.
    }

}

