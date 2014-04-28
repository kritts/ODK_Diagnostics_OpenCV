# Written by Krittika D'Silva (kdsilva@uw.edu)
# Simple script to process all images in the current directory

import glob
import os

images = glob.glob('*.jpg')

# Processes one image at a time. Pauses after each one
# until any key is pressed before processing the next image.
for pictures in images: 
    os.system('ODK_Diagnostics_MRSA.exe ' + pictures) 
   



    
