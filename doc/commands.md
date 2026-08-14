# Serial Commands

## raw

Sets the terminal to 'raw' mode, with no echo or command editing

## cooked

Sets the terminal to cooked mode, with command editing, character echo, and command history.  Useful for debugging.
			
## CONFIG	
### config erase
Erases the config.
### config read
Reads the config. 			
## REDRAW
### redraw
Redo the config.		
## LMX
### lmx tune (6-22.6)
Tunes the center freqency.
### lmx powerup 
Turns on the lmx.
### lmx powerdown 
Turns off the lmx.
### lmx prog
reprograms the lmx.
### lmx drive (1-7)
Changes the power the lmx outputs.
### lmx write
writes to the lmx register.
### lmx read reg ()
reads the lmx register 					
## BOOTLOADER
### bootloader
Resets device.			
## SET
### set lo ext
Sets the lo to external source.
### set lo int
Set the lo to internal source.
### set clock ext
Sets the clock to external source.
### set clock int
Sets the clock to internal source.
### set i_v	()
Sets the bias on I.			
### set q_v ()
setting the bias on Q.
### set rxfilter ()
Sets the filter for RX. 
### set txfilter () 
Sets the filter on TX.
### set swap
Sets swap value
### set thru
Set thru value.
## set iq (V)						
Sets iq voltage.	
## GET 
### get board model
Returns the boards model.
### get board serial

  	

  	
  
