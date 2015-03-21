rod_length = 100; 	// the length of the threaded rod

thread_count = 3;	// number of threads
thread_width = 3;	// the width of each thread
thread_depth = 5;	// the radius of each thread
	// If width is set to exactly twice the depth,
	// you'll end up with a triangular thread profile.
	// However, the resulting radius of the threaded
	// rod will no longer conform to thread_depth.
	
thread_facet_factor = 1024;	// thread facets/slices/resolution. 256 or greater works well.
thread_facets = (rod_length/100)*thread_facet_factor;	

thread_pitch = 0.18;	// turns per mm; this sets twist_degrees.
twist_degrees = (rod_length*thread_pitch)*360;

base_radius = 4;	// outer radius of the central cylinder 
base_facets = 64;	// facets of the central cylinder. Recommended to set this 32 or higher.

inner_radius = 2.5;	// radius of the central hole
inner_facets = 12;	// facets of the central hole; set this to 6 for a hexagonal hole, set it to some high value like 20 for a circular hole.

nut_height = 7;	// the height of the nut
nut_radius = 6;	// the radius of the nut
nut_facets = 30;		// number of nut facets; set this to 6 for a hexagonal nut, 4 for square, 30+ for round.	
nut_scale = 1.2;	// scale the nut by this much in the X and Y axes, in order to fit on the thread cleanly. A value of 1.1 works well.

rotate_increment = 360 / thread_count;  // the angle by which to rotate each thread (don't change this unless you want an asymmetrical result)

//// Generation options ////
shownut = 1;		// generate a nut
showthread = 0;		// generate a threaded rod
showthreadplate = 0;	// generate a plate of multiple threaded rods
plate_number = 2;	// when showthreadplate is set to 1, the number of rods generated will equal this number squared.

module thread()
{
for ( i = [1 : thread_count] ) {
	linear_extrude(height=rod_length, center=true, convexity=10, twist=twist_degrees, slices=thread_facets)
	rotate([0,0,(rotate_increment * i)]) {
		translate([-(thread_width/2),0]) {
		square(size = [thread_width,thread_depth], center=false);
		}
	}
}	

linear_extrude(height=rod_length, center=true, convexity=10, twist=0) {
	circle(r=base_radius,$fn=base_facets);
	}
}

module threadbolt()
{
	difference()
	{
		thread();
		translate(v=[0,0,-rod_length])
			cylinder(h=(rod_length*2),r=inner_radius,$fn=inner_facets);
	}
}

module nut()
{
	scale(v=[nut_scale,nut_scale,1])
	difference()
	{
		cylinder(h=nut_height, r=nut_radius, $fn=nut_facets);
		thread();
	}
}

module main()
{
	if(showthreadplate == 1)
	{
		for(n=[1:plate_number])
			for(m=[1:plate_number])
				translate(v=[n*(thread_depth*2.5),m*(thread_depth*2.5),0])
				threadbolt();
	}
	if(shownut == 1)
	{
		nut();
	}
	if(showthread == 1)
	{
		threadbolt();
	}
}

main();