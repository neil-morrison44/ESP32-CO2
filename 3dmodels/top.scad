use <base.scad>
$fa = 0.5;
$fs = 0.5;
//$screenRadius = 35.5 / 2;
$screenRadius = 37.8 / 2;
$usbHeight = 16 + 2.6;

$tubeHeight = 65;

$tubeAngle = -75;

$sensorScrewLength = 45.6;
$sensorScrewWidth = 20;

$actualSensorLength = 36.5;
$actualSensorWidth = 23.5;

$sensorScrewRadius = 2.3 / 2;
$screenScrewRadius = 2.3 / 2;

$screenScrewWidth = 26.5;
$screenDepth = 6.1;

$chinDepth = 4;
$chinWidth = 16.6;
$chinHeight = 6;

module outerShape(floorOffset = 0, screenHole = false) {
  hull() {
    translate([ 0, 0, -floorOffset ]) scale([ 1.005, 1.005, 2 ]) { scale([ 1.01, 1.01, 1.01 ]) baseBoard(); }
    translate([ 0, 0, 25 ]) rotate([ $tubeAngle, 0, 0 ]) innerTube();
  }

  if (screenHole) {
    translate([ 0, 0, 25 ]) rotate([ $tubeAngle, 0, 0 ]) translate([ 0, 0, 2.5 ]) innerTube(1, 1);
  }
}

module innerTube(heightOffset = 0, chinOffset = 0) {
  union() {
    cylinder($tubeHeight + heightOffset, $screenRadius - 10, $screenRadius, true);
    translate([ 0, $screenRadius, ($tubeHeight / 2) - ($chinDepth / 2) - chinOffset ])
        cube([ $chinWidth, $chinHeight, $chinDepth ], true);
  }
}

module usbHole() { translate([ -2.5, -20, $usbHeight ]) cube([ 11, 50, 8 ], true); }

module sensorPosition() { translate([ -16, 0, 30 ]) rotate([ 12, -50, 5 ]) rotate([ 0, 0, -30 ]) children(); }

module sensorScrews() {
  sensorPosition() {
    for (x = [ -($sensorScrewWidth / 2), ($sensorScrewWidth / 2) ]) {
      for (y = [ -($sensorScrewLength / 2), ($sensorScrewLength / 2) ]) {
        translate([ x, y, 0 ]) difference() {
          cylinder(7, $sensorScrewRadius * 3, $sensorScrewRadius * 2, true);
          translate([ 0, 0, 0.1 ]) cylinder(7, $sensorScrewRadius, $sensorScrewRadius, true);
        }
      }
    }
  }
}

module sensorHole() {
  sensorPosition() { cube([ $actualSensorWidth, $actualSensorLength, 10 ], true); }
}

module screenScrews() {
  translate([ 0, 22, 43 ]) {
    for (x = [ -($screenScrewWidth / 2), ($screenScrewWidth / 2) ]) {
      translate([ x, 0, 0 ]) {
        rotate([ 90 + $tubeAngle, 0, 0 ]) {
          difference() {
            union() {
              translate([ 0, 0, 3 / 2 ]) sphere($screenScrewRadius * 2);
              cylinder(3, $screenScrewRadius * 2, $screenScrewRadius * 2, true);
            }

            translate([ 0, 0, -0.1 ]) cylinder(10, $screenScrewRadius, $screenScrewRadius, true);
          }
        }
      }
    }
  }
}

difference() {
  scale([ 1.05, 1.05, 1.05 ]) outerShape();
  outerShape(1, true);
  usbHole();
  sensorHole();
}

scale([ 1.01, 1.01, 1.01 ]) intersection() {
  baseScrewHoles(5);
  outerShape();
}

difference() {
  sensorScrews();
  outerShape();
}

intersection() {
  screenScrews();
  outerShape();
}

% wholeBase();
