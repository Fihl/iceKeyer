// Box for my iceKeyer, https://Fihl.net https://Fihl.net/iceKeyer
// A CW morse keyer, without moving parts

// This file is for OpenSCAD, the best

include <../lasercutter2D.scad>; 
// Modified for 2D only, based on https://github.com/bmsleight/lasercut 
// by OZ1AAB, Christen Fihl, https://github.com/Fihl

thickness = 3.1; // 3mm=3.1, 4mm=4.1

layout = 0;      // 0=My way, best for design, 1=Tall(y), 2=Flat(x).
                 // NB: 1 & 2 includes extra stuff, like key-knob

doLaserCut = 1;  //1: To export DFX file. F5+F6+Export-DXF
                 //0: Otherwise SVG text. F5+F6+Export-SVG

PADDLEx = 37-thickness;             //1' hole for paddle, from front
PADDLEx2 = PADDLEx+10;              //2' hole for paddle
PADDLEw = 12.7;                     //Paddle arm, exact

x = 70;
y = PADDLEw +2*thickness; 
z = 110;
fingerx = 10;     // 10mm..15mm each finger (60mm => 4)
fingery = 3;      // 
fingerz = 15;     // 
fingerBack = 5;   // Less fingers, to enable easy open of bottom

airspace = 1; //Around paddle
PADDLEhole = PADDLEw+2*airspace;
PADDLEleft = x-PADDLEhole-thickness+airspace;
PADDLEbottom = thickness-airspace;
boxY = y; 

$fn=60;

if (layout==0) { //My way, best for design
    doFront(x,y, 0,0);
    doTop(x,z, 0,y+10);
    doBack(x,y, 0,y+z+20);
    doBottom(x,z, 0,y+z+y+30);
    doRight(z,y, x+10,0);
    doLeft(z,y, -z-10,0);
  }
if (layout==1) { //Tall
    doFront(x,y, 0,0);
    doTop(x,z, 0,y+10);
    doBack(x,y, 0,y+z+20);
    doBottom(x,z, 0,y+z+y+30);
    doRight(z,y, 0,-10,-90);
    doLeft(z,y, y+10,-10,-90);
    if (doLaserCut) {
      translate([x+10,200]) rotate(-90) doInnerStuff(); //Export as dxf to lasercut
      translate([x+25,15]) doPaddle();
    }
  }
if (layout==2) { //Flat
    doTop(x,z, 0,0);
    doBottom(x,z, x+10,0);
    doRight(z,y, y+x+x+20, 0, 90);
    doLeft(z,y, y+y+x+x+30,0, 90);
    doFront(x,y, y+y+x+x+30 +y+10, 0, 90);
    doBack (x,y, y+y+x+x+y+40 +y+10, 0, 90);
    if (doLaserCut) {
      translate([x+10+y+y,z+10]) doInnerStuff(); //Export as dxf to lasercut
      translate([20,z+20]) rotate(-90) doPaddle();
    }
  }

module txt(sizex,sizey,posx,posy,s,size=3,direction=0) {
    //Type % in front of following line to see text more clearly. Remove % before export!
    color("red",1)
    translate([posx,posy]) rotate(direction) text(s, size);
    if (!doLaserCut)
    //for (x=[0,1]) 
      //for (y=[0,1]) 
        //translate([sizex*x,sizey*y]) 
          circle(0.1);   //Corners into SVG, for alignment. Do NOT cut
}

module doFront(x,y, dx,dy, direction=0) {
  echo(y=y, boxY=boxY, bot=PADDLEbottom, holeTop=PADDLEbottom+PADDLEhole, PADDLEhole=PADDLEhole, PADDLEw=PADDLEw, testThickness=y-(PADDLEbottom+PADDLEhole)+airspace);
  translate([dx,dy]) {
    rotate(direction) {
      txt(x,y,x/2-7,y-8,"iceKeyer", 4);
      if (doLaserCut) {
            lasercutoutSquare(thickness=thickness, x=x, y=y,
            bumpy_finger_joints=[
                    [UP, 1, fingerx],
                    [RIGHT, 1, fingery],
                    [LEFT, 1, fingery],
                    [DOWN, 1, fingerx],     
                ],
                cutouts = [ 
                    [PADDLEleft, PADDLEbottom, PADDLEhole, PADDLEhole],
                ],
                circles_remove = [
                    [4.8/2, 10, y/2],     //LED
                ]

            );
        }
    }
  }
}

module doBack(x,y, dx,dy, direction=0) {
USBx=22;
USBy=y/2;
USBw=11.2;
USBh=12.2;
USBholes1=30/2;   //USB-B   I do have both 27 & 30 mm models
USBholes2=27/2;   //USB-B
XMITx=x-8;
  translate([dx,dy]) {
    rotate(direction) {
      txt(x,y,XMITx-7,y/2+1,"xmit", 4,180);
      if (doLaserCut)
        lasercutoutSquare(thickness=thickness, x=x, y=y,
            bumpy_finger_joints=[
                [UP, 1, fingerBack],
                [DOWN, 1, fingerx],     
                [RIGHT, 1, fingery],
                [LEFT, 1, fingery]
            ],
            circles_remove = [
                [2, 5.5*x/6, y],             //open the box
                [9/2, x-8, y/2],             //3.5 jack CW OUT to radio

                [2, USBx+USBholes1, USBy],
                [2, USBx-USBholes1, USBy],        //USBb
                [2, USBx+USBholes2, USBy],
                [2, USBx-USBholes2, USBy],        //USBb
            ],
            cutouts = [ 
                [USBx-USBh/2, USBy-USBw/2, USBh, USBw], //USBb
            ]
        );
    }
  }
}

module doRight(x,y, dx,dy, direction=0) {
  translate([dx,dy]) {
    rotate(direction) {
      if (doLaserCut)
        lasercutoutSquare(thickness=thickness, x=x, y=y,
            bumpy_finger_joints=[
                [UP, 1, fingerz],
                [DOWN, 1, fingerz],
                [RIGHT, 1, fingery],
                [LEFT, 1, fingery]
            ],
            circles_remove = [
                [2, PADDLEx,  y/2],             //For fun, side swiper
                [2, PADDLEx2, y/2], 
            ]
        );
    }
  }
}
module doInnerStuff() {
    //inner wall, not for straight key
    difference() { 
        square([z,y]);
        translate([PADDLEx,  y/2]) circle(r=2);
        translate([PADDLEx2, y/2]) circle(r=2);
        for (i=[20:15:z])
            for (j=[0,y])
                translate([i,j]) circle(r=4);     //Pass through holes (space=4mm)
    }
    //small stuff
    W = PADDLEw+2+2*thickness+2;
    for (n=[0:2])
        translate([n*(W+2),y+5,0]) 
        difference() {
            square([W,y-n*10]);                   //Standoff pieces
            for (j=[0,y])
                translate([W/2,j]) circle(r=4);   //Pass through holes (space=4mm)
        }
    //Distance washers
    PL = PADDLEx2-PADDLEx+12;
    for (n=[0:1])
        translate([85+PADDLEw,y-20+(5+PL)*n]) 
        difference() {
            square([PADDLEw,PL]);                 //Spacers
            for (j=[6,6+PADDLEx2-PADDLEx])
                translate([PADDLEw/2,j]) circle(r=2);
        }
}

module doLeft(x,y, dx,dy, direction=0) {
SPEEDx = x-30;
  translate([dx,dy]) {
    rotate(direction) {
      if (doLaserCut)
        lasercutoutSquare(thickness=thickness, x=x, y=y,
            bumpy_finger_joints=[
                [UP, 1, fingerz],
                [DOWN, 1, fingerz],
                [RIGHT, 1, fingery],
                [LEFT, 1, fingery]
            ],
            circles_remove = [
                [7/2, SPEEDx, y/2],               //speed
                [3/2, SPEEDx-6, y/2],             //speed lock, 6mm offset
                [3/2, SPEEDx-7, y/2],             //speed lock, 7mm offset
//                [3/2, SPEEDx, y/2-6],             //speed lock, 6mm offset
                [9/2, SPEEDx-30, y/2+1],          //3.5 spare jack
            ]
        );
    }
  }
}

module doTop(x,y, dx,dy, direction=0) {
  translate([dx,dy]) {
    rotate(direction) {
      txt(x,y,8,21,"iceKeyer",8);
      txt(x,y,8,13,"2022",4);
      txt(x,y,8,5,"OZ1AAB",4);
      if (doLaserCut)
        lasercutoutSquare(thickness=thickness, x=x, y=y,
            bumpy_finger_joints=[
                [UP, 1, fingerx],
                [DOWN, 1, fingerx],
                [RIGHT, 1, fingerz],
                [LEFT, 1, fingerz]
                ],
            circles_remove = [
                [2, x-boxY/2, PADDLEx], 
                [2, x-boxY/2, PADDLEx2], 
            ]
        );
    }
  }
}

module doBottom(x,y, dx,dy, direction=0) {
screwHole = 2.8;
    translate([dx,dy]) {
    rotate(direction) {
      if (doLaserCut)
        lasercutoutSquare(thickness=thickness, x=x, y=y,
            finger_joints=[         //not bumpy_finger_joints => easy open
                [UP, 1, fingerx],
                [DOWN, 1, fingerBack],
                [RIGHT, 1, fingerz],
                [LEFT, 1, fingerz]
            ],
            circles_remove = [
                [ screwHole/2, 10, 10],       //magnets fix
                [ screwHole/2, 10, y-10],     //magnets fix
                [ screwHole/2, x-10, y-10],   //magnets fix
                [ screwHole/2, x-10, 10],     //magnets fix
                [ screwHole/2, x/2, y/2],     //magnets fix
                [2, x-boxY/2, y-PADDLEx], 
                [2, x-boxY/2, y-PADDLEx2], 
            ]
        );
    }
  }
}


//Paddles

// F5+ F6 + Export as dxf to Lightburn import

W=40;
H=12.7; //Load cell width

module doPaddle() {
    translate([25,0]) fixture();
    translate([-2,55]) rotate(180) doOne();
    doOne();
}

module doOne() {
    paddle(0); //1 for base only (test)
    difference() {
        arm();
        paddle(1);
    }
}

//One between both paddel peaces
module fixture() {
    square([10,H],center=true);
    square([3,H+2*3], center=true);
}

module paddle(plateonly) {
$fn=64;
    difference() {
        circle(d=30);
        for(i = [1 : 360 / 12 : 360])
            rotate([0, 0, i]) translate([10, plateonly*100, 0]) circle(d=4);
        for(i = [1 : 360 / 6 : 360])
            rotate([0, 0, i]) translate([5.5, plateonly*100, 0]) circle(d=2.5);
        circle(d=4);
    }
}

module arm() {
    difference() {
        roundedsquare(H,W,3);
        translate([H/2,W-5, 0]) circle(d=4);
        translate([H/2,W-15, 0]) circle(d=4);
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

