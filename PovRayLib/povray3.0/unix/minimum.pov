#include "finish.inc"
#include "colors.inc"

/*
camera {
   location  <0, 0, -20>
   direction <0, 0, 1.2071>
   look_at   <0, 0, 0>
}
*/

#declare RedTexture = texture {
    pigment { color rgb<0.8, 0.2, 0.2> }
    finish { ambient 0.2 diffuse 0.5 }
}

#declare GreenTexture = texture {
    pigment { color rgb<0.2, 0.8, 0.2> }
    finish { ambient 0.2 diffuse 0.5 }
}

#declare BlueTexture = texture {
    pigment { color rgb<0.2, 0.2, 0.8> }
    finish { ambient 0.2 diffuse 0.5 }
}


 camera {
    location <-35, 0, -60>
    right <-1.33, 0, 0>
    look_at <0, 0, 0>
  }

  light_source { <50, 50, -50> color rgb<1, 1, 1> }


light_source { <-10, 3, -20> color red 1 green 1 blue 1 }

sphere { <0, 0, 0.0>, 4
   pigment { color red 0.4 green 0.2 blue 0.6 }
   translate <10.0, 5.0, 0.0>
}

sphere { <0, 0, 0.0>, 4
   pigment { color red 0.4 green 0.2 blue 0.6 }
   translate <0.0, 25.0, 0.0>
}

box {
    <-2.0, -0.2, -2.0>, <2.0, 0.2, 2.0>
   pigment { color red 0 green 0 blue 1 }
//   scale <1, 3, 1>
   translate <0, -1, 5>
//   rotate <90, 0, 0>
}

  cone {
    <0, 5, 0>, 0.0    // Center and radius of one end
    <0, 0, 0>, 5.0    // Center and radius of other end

    texture {
	pigment { Gold }
	finish { Shiny }
    }
    translate x*-8
    rotate <6, 10, 0>
  }

  cylinder {
    <0, 0, 0>,     // Center of one end
    <0, 7, 0>,     // Center of other end
    4            // Radius

    texture {
	checker
	texture {
	    pigment { color red 1 green 0.2 blue 0.5 }
	    finish { Shiny }
	}
	texture { pigment { color red 1 green 1 blue 1 }  }
    }
   translate <10, 10, 0>
   rotate <45, 0, 0>
  }

  mesh {
    /* top side */
    triangle { <-10, 10, -10>, <10, 10, -10>, <10, 10, 10>
      texture { RedTexture }
    }
    triangle { <-10, 10, -10>, <-10, 10, 10>, <10, 10, 10>
      texture { RedTexture }
    }
    /* bottom side */
    triangle { <-10, -10, -10>, <10, -10, -10>, <10, -10, 10> }
    triangle { <-10, -10, -10>, <-10, -10, 10>, <10, -10, 10> }
    /* left side */
    triangle { <-10, -10, -10>, <-10, -10, 10>, <-10, 10, 10> }
    triangle { <-10, -10, -10>, <-10, 10, -10>, <-10, 10, 10> }
    /* right side */
    triangle { <10, -10, -10>, <10, -10, 10>, <10, 10, 10>
      texture { GreenTexture }
    }
    triangle { <10, -10, -10>, <10, 10, -10>, <10, 10, 10>
      texture { GreenTexture }
    }
    /* front side */
    triangle { <-10, -10, -10>, <10, -10, -10>, <-10, 10, -10>
      texture { BlueTexture }
    }
    triangle { <-10, 10, -10>, <10, 10, -10>, <10, -10, -10>
      texture { BlueTexture }
    }
    /* back side */
    triangle { <-10, -10, 10>, <10, -10, 10>, <-10, 10, 10> }
    triangle { <-10, 10, 10>, <10, 10, 10>, <10, -10, 10> }
    texture {
      pigment { color rgb<0.9, 0.9, 0.9> }
      finish { ambient 0.2 diffuse 0.7 }
    }
    rotate <45, 0, 0>
    translate <-25, 0, 0>
  }

  polygon {
    28,
    <0, 0> <1, 0> <0, 1> <0, 0>          // large outer triangle
    <.3, .7> <.4, .7> <.3, .8> <.3, .7>  // small outer triangle #1
    <.5, .5> <.6, .5> <.5, .6> <.5, .5>  // small outer triangle #2
    <.7, .3> <.8, .3> <.7, .4> <.7, .3>  // small outer triangle #3
    <.5, .2> <.6, .2> <.5, .3> <.5, .2>  // inner triangle #1
    <.2, .5> <.3, .5> <.2, .6> <.2, .5>  // inner triangle #2
    <.1, .1> <.2, .1> <.1, .2> <.1, .1>  // inner triangle #3
    translate y*15
	pigment { color red 0.5 green 0.8 blue 0.9 }
    scale < 6, 1, 6 >
  }
