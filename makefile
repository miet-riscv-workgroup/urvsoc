rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
namestrip=$(basename $(notdir $1))

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

PRJ     = $(current_dir)
TCL_DIR = ./scripts
IP_DIR  = ./ip
TMP_DIR = ./tmp
TMP_IP_DIR  = ./tmp/ip
RTL_DIR = ./rtl
TB_DIR  = ./tb
XDC_DIR = ./xdc
GLBL    = "C:/Xilinx/Vivado/2016.4/data/verilog/src/glbl.v"

# define uniq
#   $(eval seen :=)
#   $(eval result :=)
#   $(foreach _,$1,$(if $(filter $(call namestrip,$_),${seen}),,$(eval seen += $(call namestrip,$_)) $(eval result += $_)))
# endef

# $(foreach _,$1,$(if $(filter $(call namestrip,$_),${seen}),,$(eval seen += $(call namestrip,$_)) $(eval result += $_)))

# RTL			 = $(call uniq,$(call rwildcard,./,*.v *.vhd))
RTL			 = $(call with_substr, $(call uniq, $(call rwildcard,./,*.v *.vhd)),rtl)
TB       = $(wildcard $(TB_DIR)/*.v)
DAT      = $(wildcard $(TB_DIR)/*.dat)
# IP       = $(notdir $(wildcard $(IP_DIR)/*.tcl))
IP       = $(call rwildcard,./,*.iptcl)
IP_DONE  = $(foreach script, $(IP), $(TMP_DIR)/$(notdir $(script)).ipdone)
IP_NAMES = $(foreach script, $(IP), $(call namestrip, $(script)))
# XDC      = $(XDC_DIR)/*.xdc

uniq = $(if $1,$(firstword $1) $(call uniq,$(patsubst %$(notdir $(firstword $1)),,$1)), $1)
with_substr = $(foreach _,$1, $(if $(findstring $2,$_),$_,))
# uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out %$(call namestrip,$(firstword $1)),$1)),$1)

.PHONY: all clean ip sim ipsim setup compile

all : setup

test:
	@echo "UNIQ:"
	@echo $(RTL)
	@echo "FILTER:"
	@echo $(call with_substr,$(RTL),rtl)


# add project name to scripts templates
$(TMP_DIR)/setup.tcl: $(TCL_DIR)/setup.tcl
	cmd /c "echo set project $(PRJ) | cat - $(TCL_DIR)/setup.tcl > $(TMP_DIR)/setup.tcl"
$(TMP_DIR)/compile.tcl: $(TCL_DIR)/compile.tcl
	cmd /c "echo set project $(PRJ) | cat - $(TCL_DIR)/compile.tcl > $(TMP_DIR)/compile.tcl"

# run setup script
setup : $(TMP_DIR)/$(PRJ).setup.done
$(TMP_DIR)/$(PRJ).setup.done : $(IP_DONE) $(RTL) $(TMP_DIR)/setup.tcl
	cmd /c "vivado -mode batch -source $(TMP_DIR)/setup.tcl \
	-log $(TMP_DIR)/setup_$(PRJ).log \
	-jou $(TMP_DIR)/setup_$(PRJ).jou"

# genrate IPs with scripts
ip : $(IP_DONE)
	# $(foreach i, $^, $(shell touch $i))
$(IP_DONE) : $(IP)
	$(foreach f, $?, $(if $(filter $(call namestrip, $(basename $@)),$(call namestrip, $f)),(cmd /c "vivado -mode batch -source ./$f \
	-log $(TMP_DIR)/$(call namestrip, $f).log \
	-jou $(TMP_DIR)/$(call namestrip, $f).jou");,)) 

# run compile script
compile : $(TMP_DIR)/$(PRJ).compile.done
$(TMP_DIR)/$(PRJ).compile.done : $(TMP_DIR)/$(PRJ).setup.done $(TMP_DIR)/compile.tcl $(RTL)
	cmd /c "vivado -mode batch -source $(TMP_DIR)/compile.tcl \
	-log $(TMP_DIR)/$(PRJ).compile.log \
	-jou $(TMP_DIR)/$(PRJ).compile.jou"

# process all rtl and tb files through 'xvlog'
# no vhdl here, implement if needed
$(TMP_DIR)/sim_rtl.done : $(TMP_DIR)/sim_ip.done 
	$(foreach f, $(TB), (cd $(TMP_DIR) && xvlog -work work "../$(subst ../,,$(f))");)
	$(foreach f, $(RTL), (cd $(TMP_DIR) && xvlog -work work "../$(subst ./,,$(f))");)
	cmd /c "cd $(TMP_DIR) && xvlog -work work $(GLBL)"
	touch $(TMP_DIR)/sim_rtl.done

# process every project existing for gererated IPs in 'xvlog' and 'xvhdl'
# some IPs have both projects others have only one
ipsim : $(TMP_DIR)/sim_ip.done
$(TMP_DIR)/sim_ip.done: $(IP_DONE)
	$(foreach ipname, $(IP_NAMES),echo $(ipname);\
	 	$(if $(wildcard $(TMP_IP_DIR)/$(ipname)_sim/xsim/vlog.prj),\
		(cd $(TMP_DIR) && xvlog -prj $(subst $(notdir $(TMP_DIR)),.,$(subst ../,,$(wildcard $(TMP_IP_DIR)/$(ipname)_sim/xsim/vlog.prj))));,))
	$(foreach ipname, $(IP_NAMES),\
	 	$(if $(wildcard $(TMP_IP_DIR)/$(ipname)_sim/xsim/vhdl.prj),\
		(cd $(TMP_DIR) && xvhdl -prj $(subst $(notdir $(TMP_DIR)),.,$(subst ../,,$(wildcard $(TMP_IP_DIR)/$(ipname)_sim/xsim/vhdl.prj))));,))
	touch $(TMP_DIR)/sim_ip.done
	
# since IPs use unknown libraries we get full list of generated directories
# and pass them as options for 'xelab'
LIBS= $(call namestrip, $(shell find $(TMP_DIR)/xsim.dir -maxdepth 1 -type d))
LLIBS=$(foreach lib, $(LIBS), -L $(lib))
sim: $(TMP_DIR)/sim_ip.done $(TMP_DIR)/sim_rtl.done
	$(foreach dat, $(DAT), (cp $(dat) $(TMP_DIR));)
	(cd $(TMP_DIR) && xelab $(LLIBS) -L unisims_ver work.$(PRJ)_tb work.glbl -s $(PRJ)_sim)
	(cd $(TMP_DIR) && xsim $(PRJ)_sim -t ../$(TCL_DIR)/xsim.tcl)

# just clean all files in tmp dir
clean :	
	find $(TMP_DIR) -not -name "$(notdir $(TMP_DIR))" -not -name ".gitignore" | xargs rm -rf
