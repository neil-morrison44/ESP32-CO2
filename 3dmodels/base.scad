
$fa = 0.1;
$fs = 0.1;
$boardDepth = 50;
$boardWidth = 45;
$boardHeight = 2.6;

$holeRadius = 3.2 / 2;
$screwRadius = 1.92 / 2;

$topScrewRadius = 3 / 2;

$screwDepth = 49;
$screwWidth = 44.2;

$lip = 5;

$baseDepth = $boardDepth + $lip;
$baseWidth = $boardWidth + $lip;
$baseHeight = 3;

$lipHeight = $baseHeight / 4;

$counterSinkHeight = 2;

module baseBoard() {
  cube([ $boardWidth - 5, $boardDepth - 5, $baseHeight ], true);

  translate([ 0, 0, (-($baseHeight / 2)) + ($lipHeight) / 2 ])
      cube([ $baseWidth + $lip, $baseDepth + $lip, $lipHeight ], true);
}

module boardScrewHoles() {
  for (x = [ -($screwWidth / 2), ($screwWidth / 2) ]) {
    for (y = [ -($screwDepth / 2), ($screwDepth / 2) ]) {
      translate([ x, y, -($baseHeight / 2) ]) difference() {
        cylinder($boardHeight + $baseHeight, $holeRadius * 1.5, $holeRadius);
        translate([ 0, 0, 0.1 ]) cylinder($boardHeight + $baseHeight, $screwRadius, $screwRadius);
      }
    }
  }
}

module topScrewHoles() {
  for (x = [ -($baseWidth / 2), 0, ($baseWidth / 2) ]) {
    for (y = [ -($baseDepth / 2), 0, ($baseDepth / 2) ]) {
              if ((x == 0 || y == 0) && (x != y)){
      translate([ x, y, -($baseHeight / 2 + 0.1) ]) cylinder($lipHeight + 0.2, $topScrewRadius + 1, $topScrewRadius);
              }
    }
  }
}

module baseScrewHoles(height = 10) {
  translate([ 0, 0, $lipHeight ]) {
    for (x = [ -($baseWidth / 2), 0, ($baseWidth / 2) ]) {
      for (y = [ -($baseDepth / 2), 0, ($baseDepth / 2) ]) {
        if ((x == 0 || y == 0) && (x != y)){
        translate([ x, y, -($baseHeight / 2 + 0.1) ]) difference() {
          cylinder(height, $topScrewRadius + 1, $topScrewRadius + 1);
          translate([ 0, 0, -0.1 ]) cylinder(height, $topScrewRadius, $topScrewRadius);
        }
      }
      }
    }
  }
}

module wholeBase() {
  difference() {
    baseBoard();
    topScrewHoles();
  }
  boardScrewHoles();
}

wholeBase();
%baseScrewHoles();
