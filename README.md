## Atie's ED manual 

# Usage
If a filename isn't specified, you will be asked to fill it in yourself. 
If the file doesn't exist the program will terminate.
After that you will be shown an empty terminal.
        
        
## Commands
        
# p[count][s][n]        
Prints out count lines. If no amount is specified only the current line will be printed.<br>
If an 's' is present in the print command, it will print the surrounding lines (count times in each direction).<br>
If an 'n' is present in the print command, it will add the line numbers to each line printed.

# pp
Toggles autoprint, which will automatically print the current line when navigating through the file.

# n
Prints the current line number.

# nn
Toggles automatic line numbering when printing out lines.

# w
Overwrites the current line of the file with the input from stdin.

# a
Appends a new line after the current line and writes stdin into that line.

# d[number]
Deletes the current line, unless number is specified. When number is specified it will delete the line at that number.

# s[line1]&[line2]
Switches line1 and line2. If no lines are specified it will switch the current line with the next one.<br>
If only line1 is specified it will switch the current line with line1.
The & sign is necesarry for switching with the second line.

# c
Copies the current line.

# v[w]
Pastes the line currently in the clipboard to a new line. If w is present it will overwrite the current line.

# j[+/-][number]
Jump to the specified line. If + or - are specified it will jump to that line relative to the current line.

# [k/l]
Moves to the next line 'l' or the previous line 'k'.


# q
Quits the program and writes the contents to the specified file.
