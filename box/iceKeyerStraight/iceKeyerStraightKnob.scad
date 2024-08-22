
// Do export as dxf to Lightburn import

$fn=32;

D=30;
W=50;

//translate([0,D/2-2,0]) arm();
pad();

KD=15;
for(i = [0,1,2]) translate([25,i*18+D/2-10,0]) {
    knob1(KD);
    translate([KD+1,0,0]) knob2(KD-i,7+i);  // 7..9
}

module pad() {
    difference() {
        circle(d=D);
        for(i = [1 : 360 / 12 : 360])
            rotate([0, 0, i]) translate([D-19, 0, 0]) circle(d=4);
        // for(i = [1 : 360 / 6 : 360]) rotate([0, 0, i]) translate([6, 0, 0]) circle(d=3);
        circle(d=4);
    }
}

H=12.7; //width
module arm() {
    translate([-H/2,0,0]) 
    difference() {
        roundedsquare(H,W,3);
        for (y = [1,2,3,4]) 
            translate([H/2,W+5-10*y, 0]) 
                circle(d=4);
    }
}

module knob1(dia) {
    difference() {
        circle(d=dia);
        for(i = [1 : 360 / 6 : 360])
            rotate([0, 0, i]) translate([dia/2-3, 0, 0]) circle(d=3);
        circle(d=4);
    }
}

module knob2(dia,d2) {
    difference() {
        circle(d=dia);
        circle(d=d2,$fn=6);
    }
}

module roundedsquare(xx, yy, radius) 
{
    deg = [0,90,270,180];
    difference()
    {   
        square([xx,yy]);
        for (x=[0,1])
        for (y=[0,1])
            translate([x*xx,y*yy,0])
            rotate(deg[x+2*y])
            difference(){
                translate([-.5,-.5]) square([radius+.5,radius+.5]);
                translate([radius,radius]) circle(radius, $fn=32);
            }
    }
}
