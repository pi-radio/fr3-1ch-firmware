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
### lmx tune <frequency in GHz> 
Tunes the center freqency.  Allowable frequencies are 6-22.6 GHz
### lmx powerup 
Turns on the lmx.
### lmx powerdown 
Turns off the lmx.
### lmx prog
reprograms the lmx.
### lmx drive <power>
Changes the power of the lmx output.  Available powers are the integers 0-7
### lmx write
writes to the lmx register.
### lmx read reg <reg-no>
reads the lmx register 					
## BOOTLOADER
### bootloader
Resets device into Device Firmware Update (DFU) mode.			
## SET
### set lo ext
Sets the lo to external source.
### set lo int
Set the lo to internal source.
### set clock ext
Sets the clock to external source.
### set clock int
Sets the clock to internal source.
### set i_v <voltage in V>
Sets the bias on I.	Allowed values are -0.4V - 0.4V
### set q_v <voltage in V>
Sets the bias on Q. 	Allowed values are -0.4V - 0.4V
### set rxfilter <int>
Sets the filter for RX -- the integer comes from the AM3153 datasheet
### set txfilter <int> 
Sets the filter on TX  -- the integer comes from the AM3153 datasheet
## set iq {swap|thru}						
Sets whether or not to swap IQ lines
## GET 
### get board model
Returns the board model.
### get board serial
Returns the board serial number
  	

  	
  
