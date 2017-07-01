set tmpDir "./tmp"
set marker "$tmpDir/$project.compile.done"

open_project $tmpDir/$project.xpr
launch_runs impl_1 
wait_on_run impl_1
open_run impl_1

write_bitstream $tmpDir/$project.bit

set file_marker [open $marker w]
close $file_marker
