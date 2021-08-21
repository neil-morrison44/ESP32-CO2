use <base.scad>
$fa = 0.1;
$fs = 0.1;
$screenRadius = 35.5 / 2;
$usbHeight = 14 + 2.6;

module outerShape(floorOffset = 0, screenHole = false) {
  hull() {
    translate([ 0, 0, -floorOffset ]) baseBoard();
    translate([ 0, 0, 25 ]) rotate([ -80, 0, 0 ]) cylinder(50, $screenRadius - 10, $screenRadius, true);
  }

  if (screenHole) {
    translate([ 0, 0, 25 ]) rotate([ -80, 0, 0 ]) translate([ 0, 0, 2.5 ])
        cylinder(51, $screenRadius - 10, $screenRadius, true);
  }
}

module usbHole() { translate([ 0, -25, $usbHeight ]) cube([ 12, 10, 5 ], true); }

difference() {
  scale([ 1.05, 1.05, 1.05 ]) outerShape();
  outerShape(1, true);
  usbHole();
}

baseScrewHoles(5);

//%wholeBase();
