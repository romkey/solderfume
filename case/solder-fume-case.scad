filter_width = 4.3*25.4;
filter_height = 120;
filter_thickness = 1.2*25.4+2;

fan_width = 120;
fan_height = 120;
fan_thickness = 1.2*25.4 - 4;

thickness = 2;

width = fan_width+thickness*2;
depth = thickness*3 + filter_thickness + fan_thickness;
height = fan_height + thickness;
filter_lip = fan_width - filter_width;
fan_lip = 10;

cube([width, depth, thickness]);
translate([0, 0, thickness])
  cube([width, thickness, fan_lip]);
  
translate([0, thickness + filter_thickness, thickness])
  cube([width, thickness, fan_lip]);

translate([0, thickness*2 + filter_thickness + fan_thickness, thickness])
  cube([width, thickness, fan_lip]);

cube([thickness, depth, height]);
translate([width - thickness, 0, 0])
  cube([thickness, depth, height]);
  
module lips(lip) {
  cube([lip, thickness, height]);
  translate([width - lip, 0, 0])
    cube([lip, thickness, height]);
}

lips(filter_lip);

translate([0, filter_thickness+thickness, 0])
  lips(fan_lip);

translate([0, filter_thickness+fan_thickness + thickness*2, 0])
  lips(fan_lip);

//offset = (fan_width - filter_width)/2 - 1.5;
offset = (fan_width - filter_width)/2;

translate([offset, 0, 0])
  cube([thickness, fan_thickness, filter_lip]);
translate([fan_width - offset, 0, 0])
  cube([thickness, fan_thickness, filter_lip]);