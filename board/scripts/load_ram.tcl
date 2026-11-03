# Matched acquisition-preview candidate only. No reset, Flash, START, retry or live-bit update.
proc acq_candidate_load {root out serial expected_words main_address report_address lcd_report_address acq_report_address {application build_work/app/arm_acq/heart_system.elf}} {
    if {[file pathtype $application] ne "relative" || [lsearch -exact [file split $application] ..]>=0} {error "Application must stay inside project"}
    set connected 0;set held 0;set resumed 0;set bp ""
    set failed [catch {
        connect -url tcp:127.0.0.1:3121;set connected 1
        after 1000
        handoff::select xc7z020 $serial
        foreach n {0 1} {handoff::select "*Cortex-A9*#$n" $serial}
        if {[fpga -state] ne "FPGA is not configured"} {error "Configured/live board: loading refused"}
        set held 1;handoff::halt $serial
        if {[fpga -state] ne "FPGA is not configured"} {error "Configuration changed before load"}
        uplevel #0 [list source [file join $root board firmware ps7_init.tcl]]
        trace add execution ::puts enter handoff::vendor_guard
        ::ps7_init
        handoff::select xc7z020 $serial
        fpga -file [file join $root board firmware system.bit]
        handoff::select "*Cortex-A9*#0" $serial
        ::ps7_post_config
        trace remove execution ::puts enter handoff::vendor_guard
        loadhw -hw [file join $root board firmware hardware.xsa]
        if {[fpga -state] ne "FPGA is configured"} {error "FPGA configuration did not complete"}
        set ctrl [mrd -value 0x43c00000]
        if {[llength $ctrl]!=1 || ($ctrl & 4)==0 || ($ctrl & 0x81)!=0} {error "PW not idle"}
        foreach {address expected} {0x43c20020 0x4c434431 0x43c20004 50 0x43c20014 1024 0x43c2000c 0 0x43c20000 0 0x43c20008 0} {
            set value [mrd -value $address]
            if {[llength $value]!=1 || $value!=$expected} {error "LCD power-on ABI mismatch at $address"}
        }
        foreach {address expected} {0x43c50000 0x41505631 0x43c50004 0 0x43c50018 0 0x43c5001c 0 0x43c5004c 0x00fa01b9} {
            set value [mrd -value $address]
            if {[llength $value]!=1 || $value!=$expected} {error "Acquisition preview power-on ABI mismatch at $address"}
        }
        if {([mrd -value 0x43c60000]&0x85)!=4} {error "Lung accelerator not idle/matched"}
        if {([mrd -value 0x43c70000]&0x85)!=4} {error "Linear accelerator not idle/matched"}
        puts "LINEAR_ACCELERATOR_IDLE_VERIFIED"
        puts "LUNG_ACCELERATOR_IDLE_VERIFIED"
        dow [file join $root $application]
        set observed [mrd -value $main_address 16]
        if {[llength $observed]!=16} {error "Main readback size mismatch"}
        for {set i 0} {$i<16} {incr i} {
            if {[lindex $observed $i]!=[lindex $expected_words $i]} {error "ELF readback mismatch at main"}
        }
        puts "LCD_MAIN_BYTES_VERIFIED"
        set bp [bpadd -addr $main_address -type hw]
        con -block -timeout 25
        scan [dict get [rrd -nvlist pc] pc] %x pc
        if {$pc!=$main_address} {error "LCD ELF main checkpoint mismatch"}
        bpremove $bp;set bp ""
        puts "LCD_MAIN_CHECKPOINT_VERIFIED"
        con;set resumed 1;set healthy 0
        for {set sample 0} {$sample<25} {incr sample} {
            after 1000
            set r [mrd -value $report_address 20]
            set l [mrd -value $lcd_report_address 7]
            set a [mrd -value $acq_report_address 13]
            if {[llength $a]!=13 || [lindex $a 0]!=0x41505231 || [lindex $a 1]!=1} {error "Acquisition report ABI mismatch"}
            foreach i {2 3 4 5 6 7 8 9 10 11 12} {
                if {[lindex $a $i]!=0} {error "Unexpected preview activity/error before manual recording"}
            }
            if {[mrd -value 0x43c50004]!=0 || [mrd -value 0x43c50018]!=0 || [mrd -value 0x43c5001c]!=0} {error "Unexpected acquisition hardware state"}
            puts "LCD_RUNTIME_REPORT $r";puts "LCD_DRIVER_REPORT $l"
            if {[llength $r]!=20 || [lindex $r 0]!=0x32525348 || [lindex $r 1]!=2} {error "Runtime report invalid"}
            if {[lindex $r 4]!=0 || [lindex $r 7]!=0 || [lindex $r 8]!=0 || [lindex $r 10]!=0} {error "Runtime/UART error"}
            if {[llength $l]!=7 || [lindex $l 0]!=0x4c434431 || [lindex $l 1]!=1 || [lindex $l 2]!=0 || [lindex $l 5]!=0} {error "LCD driver error/timeout"}
            if {[mrd -value 0x43c2000c]!=0} {error "LCD hardware error"}
            if {[lindex $r 2]>5000 && [lindex $r 17]>0} {
                if {[lindex $r 3]!=0 || [lindex $r 13]!=0 || [lindex $r 14]!=0} {error "Unexpected acquisition/session state"}
                if {[lindex $l 3]==0 || [lindex $l 4]==0 || [mrd -value 0x43c20000]!=7 || [mrd -value 0x43c20004]!=50} {error "LCD startup incomplete"}
                set healthy 1;break
            }
        }
        if {!$healthy} {error "No healthy menu after startup"}
        set fh [open [file join $out runtime_report.txt] w];puts $fh $r;puts $fh $l;puts $fh $a;close $fh
        puts "ACQ_LOAD_COMPLETE RAM_ONLY_NO_FLASH_NO_RESET_NO_START"
    } problem options]
    catch {trace remove execution ::puts enter handoff::vendor_guard}
    if {$connected} {
        if {$held && !$resumed} {catch {handoff::halt $serial}}
        if {$bp ne ""} {catch {bpremove $bp}}
        catch {disconnect}
    }
    if {$failed} {return -options $options $problem}
}
