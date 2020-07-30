# Main.py

import cv2
import numpy as np
import os

import DetectChars
import DetectPlates
import PossiblePlate
import datetime

# module level variables ##########################################################################
SCALAR_BLACK = (0.0, 0.0, 0.0)
SCALAR_WHITE = (255.0, 255.0, 255.0)
SCALAR_YELLOW = (0.0, 255.0, 255.0)
SCALAR_GREEN = (0.0, 255.0, 0.0)
SCALAR_RED = (0.0, 0.0, 255.0)

showSteps = True

###################################################################################################
def main():

    imgOriginalScene  = cv2.imread("48.jpg")               # open image
    startTime = datetime.datetime.now()
    if imgOriginalScene is None:                            # if image was not read successfully
        print("\nerror: image not read from file \n\n")  # print error message to std out
        os.system("pause")                                  # pause so user can see error message
        return                                              # and exit program
    listOfPossiblePlates = DetectPlates.detectPlatesInScene(imgOriginalScene)           # detect plates
    listOfPossiblePlates = DetectChars.detectCharsInPlates(listOfPossiblePlates)        # detect chars in plates
    cropped_img = listOfPossiblePlates[0:500, 0:500]
    cv2.imshow("cropped",cropped_img)
    cv2.waitKey(0)
   # cv2.imshow("imgOriginalScene", imgOriginalScene)            # show scene image

    if len(listOfPossiblePlates) == 0:                          # if no plates were found
        print("\nno license plates were detected\n")  # inform user no plates were found
    else:                                                       # else
                # if we get in here list of possible plates has at leat one plate
                # sort the list of possible plates in DESCENDING order (most number of chars to least number of chars)
        listOfPossiblePlates.sort(key = lambda possiblePlate: len(possiblePlate.strChars), reverse = True)
                # suppose the plate with the most recognized chars (the first plate in sorted by string length descending order) is the actual plate

        licPlate1 = listOfPossiblePlates[0]
        #licPlate2 = listOfPossiblePlates[1]

        #cv2.imshow("imgPlate", licPlate.imgPlate)           # show crop of plate and threshold of plate
        #cv2.imshow("imgThresh", licPlate.imgThresh)

        drawRedRectangleAroundPlate(imgOriginalScene, licPlate1)             # draw red rectangle around plate
        #drawRedRectangleAroundPlate(imgOriginalScene, licPlate2)  # draw red rectangle around plate

##################################################


##################################################


        endTime = datetime.datetime.now()
        runningTime = endTime - startTime
        #print(runningTime)

        #cv2.imshow("imgOriginalScene", imgOriginalScene)                # re-show scene image
        #cv2.imwrite("imgOriginalScene.png", imgOriginalScene)           # write image out to file
    # end if else

    cv2.waitKey(0)               # hold windows open until user presses a key

    return
# end main

###################################################################################################
def drawRedRectangleAroundPlate(imgOriginalScene, licPlate):

    p2fRectPoints = cv2.boxPoints(licPlate.rrLocationOfPlateInScene)            # get 4 vertices of rotated rect

    cv2.line(imgOriginalScene, tuple(p2fRectPoints[0]), tuple(p2fRectPoints[1]), SCALAR_RED, 2)         # draw 4 red lines
    cv2.line(imgOriginalScene, tuple(p2fRectPoints[1]), tuple(p2fRectPoints[2]), SCALAR_RED, 2)
    cv2.line(imgOriginalScene, tuple(p2fRectPoints[2]), tuple(p2fRectPoints[3]), SCALAR_RED, 2)
    cv2.line(imgOriginalScene, tuple(p2fRectPoints[3]), tuple(p2fRectPoints[0]), SCALAR_RED, 2)
# end function

###################################################################################################
if __name__ == "__main__":
    main()
