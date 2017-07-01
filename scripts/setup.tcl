set tmpDir       "./tmp"
set rtlDir       "./rtl"
set xdcDir       "./xdc"
set tbDir        "./tb"
set swDir        "./sw"
set ipDir        "$tmpDir/ip"
set scriptsDir   "./scripts"
set ipScriptsDir "./ip"
set done_marker  "$tmpDir/$project.setup.done"

# Create project
create_project -force $project $tmpDir -part xc7a35ticsg324-1L

# Set project properties
set obj [get_projects $project]
set_property "simulator_language" "Mixed" $obj
set_property "target_language" "Verilog"  $obj

proc glob-rdir {{dir .} name} {
  set res {}
  foreach i [lsort [glob -nocomplain -directory $dir *]] {
    if {[string match $name [file tail $i]]} { lappend res $i }\
    else { eval lappend res [glob-rdir $i $name] }}
  return $res
}

proc remove_duplicates {file_list} {
  set res {}
  set unique {}
  foreach f [lsort $file_list] {
    if {[lsearch $unique [file tail $f]] == -1} {\
      eval lappend res $f
      eval lappend unique [file tail $f]
    }}
  return $res
}

# add RTL descriptions
set rtl_dirs [glob-rdir . "rtl"]
set rtl_files {}
foreach d $rtl_dirs {
  eval lappend rtl_files [glob-rdir $d *.vhd]
  eval lappend rtl_files [glob-rdir $d *.v]
}


foreach f [remove_duplicates $rtl_files] {
  add_files -norecurse -fileset sources_1 $f
}

# add testbenches
set sims [glob -directory $tbDir *.v]
foreach f $sims {
  add_files -norecurse -fileset sim_1 $f
}

# add memory initialization files
set data [glob -nocomplain -directory $tbDir *.dat*]
foreach f $data {
  add_files -norecurse -fileset sources_1 $f
}

set sw_data [glob -nocomplain -directory $swDir *.dat*]
foreach f $sw_data {
  add_files -norecurse -fileset sources_1 $f
}

# https://raw.githubusercontent.com/Digilent/Arty/master/Resources/XDC/Arty_Master.xdc
set xdc [glob -nocomplain -directory $xdcDir *.xdc]
foreach f $xdc {
  add_files -fileset constrs_1 $f
}

# Get IP list
set ip_list [glob-rdir . "*.iptcl"]
foreach ip $ip_list {
  set ipname [file rootname [file tail $ip]]
  add_files -norecurse -fileset sources_1 $ipDir/$ipname/$ipname.xci
}

set_property top $project [get_filesets sources_1]
set_property top $project\_tb [get_filesets sim_1]

set marker [open $done_marker w]
close $marker
