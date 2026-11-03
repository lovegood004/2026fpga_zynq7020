# Tested with mock XSDB commands. Firmware identity is checked by deploy_fpga.py.
# Does not reset PS/PL, write Flash, restore old RAM records or send START.
namespace eval handoff {}
proc handoff::vendor_guard {command operation} {
    if {[regexp -nocase {Timeout Reached|Mask poll failed} $command]} {
        error "Vendor initialization timeout; stop"
    }
}
proc handoff::select {name serial} {
    set rows [targets -target-properties -filter "name =~ \"$name\""]
    if {[llength $rows] != 1} {error "Missing or ambiguous target $name"}
    set row [lindex $rows 0]
    if {![dict exists $row jtag_cable_serial] || [dict get $row jtag_cable_serial] ne $serial} {
        error "Wrong JTAG adapter for target $name"
    }
    targets -set [dict get $row target_id]
    return $row
}
proc handoff::halt {serial} {
    foreach n {0 1} {
        handoff::select "*Cortex-A9*#$n" $serial
        if {[catch {stop} msg] && ![string match -nocase {*already stopped*} $msg]} {error $msg}
        set row [handoff::select "*Cortex-A9*#$n" $serial]
        if {![dict exists $row suspended] || [dict get $row suspended] != 1} {error "Core not halted"}
    }
    handoff::select "*Cortex-A9*#0" $serial
}
proc handoff::run {root mode serial url fresh} {
    if {$mode ni {probe status load}} {error "Unsupported mode"}
    set connected 0; set held 0; set finished 0; set bp ""
    set failed [catch {
        connect -url $url; set connected 1
        # Require a single matching chip and both cores on the explicitly selected adapter.
        handoff::select xc7z020 $serial
        foreach n {1 0} {handoff::select "*Cortex-A9*#$n" $serial}
        set state [fpga -state]
        puts "JTAG_IDENTITY_VERIFIED; $state"
        if {$mode eq "status"} {
            if {$state ne "FPGA is configured"} {error "FPGA not configured"}
            set r [mrd -value 0x001d2744 20]
            if {[llength $r] != 20 || [lindex $r 0] != 0x32525348 || [lindex $r 1] != 2} {
                error "Not a recognized ARM026 report; verify deployed ELF identity"
            }
            puts "ARM026_REPORT $r"
        }
        if {$mode eq "load"} {
            if {$fresh != 1 || $state ne "FPGA is not configured"} {
                error "Fresh physical-reset/unconfigured board required; live update is refused"
            }
            set held 1
            handoff::halt $serial
            set hw [file join $root outputs lcd_control_20261023 artifacts]
            uplevel #0 [list source [file join $hw ps7_init.tcl]]
            trace add execution ::puts enter handoff::vendor_guard
            ::ps7_init
            handoff::select xc7z020 $serial
            fpga -file [file join $hw lcd_control.bit]
            handoff::select "*Cortex-A9*#0" $serial
            ::ps7_post_config
            trace remove execution ::puts enter handoff::vendor_guard
            loadhw -hw [file join $hw lcd_control.xsa]
            set ctrl [mrd -value 0x43c00000]
            if {[llength $ctrl] != 1 || ($ctrl & 4) == 0 || ($ctrl & 0x81) != 0} {
                error "PW not idle; refuse reset/reuse"
            }
            dow [file join $root firmware arm026.elf]
            set bp [bpadd -addr 0x00108c5c -type hw]
            con -block -timeout 25
            scan [dict get [rrd -nvlist pc] pc] %x pc
            if {$pc != 0x00108c5c} {error "ARM026 startup checkpoint mismatch"}
            bpremove $bp; set bp ""
            con; set finished 1
        }
        puts "HANDOFF_COMPLETE $mode; NO_FLASH_NO_RESET_NO_START"
    } problem options]
    catch {trace remove execution ::puts enter handoff::vendor_guard}
    if {$connected} {
        if {$held && !$finished} {catch {handoff::halt $serial}}
        if {$bp ne ""} {catch {bpremove $bp}}
        catch {disconnect}
    }
    if {$failed} {return -options $options $problem}
}
