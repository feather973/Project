import datetime
import cv2
import imutils
import numpy as np
import pytesseract
import math

startTime = datetime.datetime.now()
plateList = []
strChars = ""
pytesseract.pytesseract.tesseract_cmd = r"C:\Program Files\Tesseract-OCR\tesseract.exe"

OLD_PLATE_RATIO = 3

def CropOldPlate(sort_x, sort_y, pt5, pt6, image):
    upper = original[sort_y[0]:int(pt5[1]), sort_x[1]: sort_x[2]]
    lower = original[int(pt5[1]):sort_y[2], sort_x[1]: sort_x[2]]
    return upper, lower


# Read the image file
image = cv2.imread('70.jpg')
cv2.imshow('22', image)

plate_list = []

gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
gray = cv2.bilateralFilter(gray, 11, 17, 17)
edged = cv2.Canny(gray, 170, 200)

# Find contours based on Edges
_, cnts, _ = cv2.findContours(edged.copy(), cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
cnts = sorted(cnts, key=cv2.contourArea, reverse=True)[:30]

NumberPlateCnt = None  # we currently have no Number plate contour
# loop over our contours to find the best possible approximate contour of number plate
count = 0
for c in cnts:
    peri = cv2.arcLength(c, True)
    approx = cv2.approxPolyDP(c, 0.02 * peri, True)
    if len(approx) == 4:  # Select the contour with 4 corners
        NumberPlateCnt = approx  # This is our approx Number Plate Contour
        break

cv2.drawContours(image, [NumberPlateCnt], -1, (0, 255, 0), 3)
cv2.boundingRect(NumberPlateCnt)
print(NumberPlateCnt)

plate_pt1 = NumberPlateCnt[0]
plate_pt2 = NumberPlateCnt[1]
plate_pt3 = NumberPlateCnt[2]
plate_pt4 = NumberPlateCnt[3]
print(plate_pt1, plate_pt2, plate_pt3, plate_pt4)

x_list = []
y_list = []
x1 = plate_pt1[0][0]
x2 = plate_pt2[0][0]
x3 = plate_pt3[0][0]
x4 = plate_pt4[0][0]

x_list.append(x1)
x_list.append(x2)
x_list.append(x3)
x_list.append(x4)

sort_x = sorted(x_list)

y1 = plate_pt1[0][1]
y2 = plate_pt2[0][1]
y3 = plate_pt3[0][1]
y4 = plate_pt4[0][1]

y_list.append(y1)
y_list.append(y2)
y_list.append(y3)
y_list.append(y4)

sort_y = sorted(y_list)

cropping = image[sort_y[1]: sort_y[2], sort_x[1]: sort_x[2]]
print(sort_x, sort_y)
cv2.imshow('233', cropping)

height = sort_y[2] - sort_y[1]

print("height : ", height)
width = sort_x[2] - sort_x[1]
print("width : ", width)

ratio = width / height

if ratio < OLD_PLATE_RATIO:
    pt5 = []
    pt6 = []

    pt5 = [sort_x[0], (sort_y[0] + height / 3)]
    pt6 = [sort_x[2], (sort_y[0] + height / 3)]

    print("pt5 : ", pt5)
    print("pt6 : ", pt6)

    upper = image[sort_y[0]:int(pt5[1]), sort_x[1]: sort_x[2]]
    lower = image[int(pt5[1]):sort_y[2], sort_x[1]: sort_x[2]]

    cv2.imshow('up',upper)
    cv2.imshow('down',lower)

else:
    final = cropping
    plt.imshow(final)
# plt.imshow(upcrop)
# plt.imshow(final)

cv2.imshow('u',upper)

gray = cv2.cvtColor(lower, cv2.COLOR_BGR2GRAY)
img_blurred = cv2.GaussianBlur(gray, ksize=(5, 5), sigmaX=0)
img_thresh = cv2.adaptiveThreshold(
    img_blurred,
    maxValue=255.0,
    adaptiveMethod=cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
    thresholdType=cv2.THRESH_BINARY_INV,
    blockSize=19,
    C=9
)
_, contours, _ = cv2.findContours(img_thresh, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
imgContours = np.zeros((height, width, 3), np.uint8)
cv2.drawContours(imgContours, contours, -1, color=(255, 255, 255))  # 흰색으로 contour 그림
cv2.imshow('s', imgContours)

possible_contours = []
contours_dict = []
cnt = 0
# contour bounding box 그리도록 설정
for contour in contours:
    x, y, w, h = cv2.boundingRect(contour)
    cv2.rectangle(imgContours, pt1=(x, y), pt2=(x + w, y + h), color=(255, 255, 0), thickness=2)
    contours_dict.append(
        {'contour': contour, 'x': x, 'y': y, 'w': w, 'h': h, 'cx': x + (w / 2), 'cy': y + (h / 2)})
    cv2.imshow('222', imgContours)

for d in contours_dict:
    area = d['w'] * d['h']  # 넓이
    ratio = math.sqrt((d['w'] ** 2) + (d['h'] ** 2))  # 대각선
    center = (d['x'] + d['x'] + d['w']) / 2

    y_where = d['y']
    if area > 1000 and area < 7000:
        d['idx'] = cnt
        cnt += 1
        possible_contours.append(d)

    imgContours = np.zeros((height, width, 3), dtype=np.uint8)

for d in possible_contours:
    # cv2.rectangle ( 그릴 이미지 , 시작좌표, 종료좌표, 색깔 , 두께 )
    cv2.rectangle(imgContours, pt1=(d['x'], d['y']), pt2=(d['x'] + d['w'], d['y'] + d['h']),
                  color=(255, 0, 255), thickness=1)
    cv2.imshow('2', imgContours)

dst = cv2.resize(upper, dsize=(355, 47), interpolation=cv2.INTER_AREA)
cv2.imshow('s', dst)

dst2 = cv2.resize(lower, dsize=(123, 47), interpolation=cv2.INTER_AREA)
cv2.imshow('23', dst2)

dncrop2 = dst[5:55, 90:145]
cv2.imshow('23', dncrop2)

config = ('-l re --oem 1 --psm 10')
strCurrentChar = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

dncrop2 = dst[5:55, 145:180]
cv2.imshow('23', dncrop2)

config = ('-l re --oem 1 --psm 10')
strCurrentChar = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

dncrop2 = dst[5:55, 210:280]
cv2.imshow('22', dncrop2)

config = ('-l num10 --oem 1 --psm 10')
strCurrentChar = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

dncrop2 = dst2[1:109, 1:30]
cv2.imshow('3', dncrop2)

config = ('-l re --oem 1 --psm 10')
strCurrentChar = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

dncrop2 = dst2[1:50, 30:50]
cv2.imshow('23', dncrop2)

config = ('-l car --oem 1 --psm 10')
strCurrentChar = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

dncrop2 = dst2[1:50, 50:75]
cv2.imshow('23', dncrop2)

config = ('-l car --oem 1 --psm 10')
strCurrentChar = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

dncrop2 = dst2[1:50, 75:100]
cv2.imshow('2', dncrop2)

config = ('-l re --oem 1 --psm 10')
strCurrentChar = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

dncrop2 = dst2[1:50, 100:114]
cv2.imshow('23', dncrop2)

config = ('-l re --oem 1 --psm 10')
strCurrentChar = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

print(plateList)
cv2.imshow('33', cropping)
cv2.waitKey(0)


# 구번호판 중에 밑에 숫자 4개인 번호판

dst = cv2.resize(upper, dsize=(355,47),interpolation = cv2.INTER_AREA)
cv2.imshow("upper img", dst)
cv2.waitKey(0)

dst2 = cv2.resize(lower, dsize=(123,47),interpolation = cv2.INTER_AREA)
cv2.imshow("lower img", dst2)
cv2.waitKey(0)

dncrop2 = dst[5:55,90:145]
cv2.imshow("crop1", dncrop2)
cv2.waitKey(0)
config = ('-l re --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)


dncrop2 = dst[5:55,145:210]
cv2.imshow("crop2", dncrop2)
cv2.waitKey(0)
config = ('-l re --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

dncrop2 = dst[2:60,210:260]
cv2.imshow("crop3", dncrop2)
cv2.waitKey(0)
config = ('-l re --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

dncrop2 = dst2[1:50,1:30]
cv2.imshow("crop4", dncrop2)
cv2.waitKey(0)
config = ('-l num10 --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

dncrop2 = dst2[1:50,30:50]
cv2.imshow("crop5", dncrop2)
cv2.waitKey(0)
config = ('-l re --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

dncrop2 = dst2[1:50,60:80]
cv2.imshow("crop6", dncrop2)
cv2.waitKey(0)
config = ('-l num10 --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

dncrop2 = dst2[1:50,80:110]
cv2.imshow("crop7", dncrop2)
cv2.waitKey(0)
config = ('-l num10 --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

print(plateList)
cv2.imshow('22', cropping)
cv2.waitKey(0)

# 구 번호판 중에 위에 한글2 숫자 2인 번호판

dst = cv2.resize(upper, dsize=(355,47),interpolation = cv2.INTER_AREA)
cv2.imshow("upper image", dst)
cv2.waitKey(0)


dst2 = cv2.resize(lower, dsize=(123,47),interpolation = cv2.INTER_AREA)
cv2.imshow("upper image", dst2)
cv2.waitKey(0)

dncrop2 = dst[9:50,90:135]
cv2.imshow("crop", dncrop2)
cv2.waitKey(0)
config = ('-l re --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)


dncrop2 = dst[8:50,130:170]
cv2.imshow("crop2", dncrop2)
cv2.waitKey(0)
config = ('-l re --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)


dncrop2 = dst[8:50,180:220]
cv2.imshow("crop3", dncrop2)
cv2.waitKey(0)
config = ('-l num10 --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)


dncrop2 = dst[8:50,220:270]
cv2.imshow("crop4", (dncrop2))
cv2.waitKey(0)
config = ('-l num10 --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)


dncrop2 = dst2[1:50,2:30]
cv2.imshow("crop5", dncrop2)
cv2.waitKey(0)
config = ('-l re --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)


dncrop2 = dst2[1:50,30:45]
cv2.imshow("crop6", dncrop2)
cv2.waitKey(0)
config = ('-l num10 --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)


dncrop2 = dst2[1:50,45:75]
cv2.imshow("crop7", dncrop2)
cv2.waitKey(0)
config = ('-l num10 --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)


dncrop2 = dst2[1:50,75:100]
cv2.imshow("crop8", dncrop2)
cv2.waitKey(0)
config = ('-l num10 --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)


dncrop2 = dst2[1:50,100:120]
cv2.imshow("crop9", dncrop2)
cv2.waitKey(0)
config = ('-l num10 --oem 1 --psm 10')
strCurrentChar  = pytesseract.image_to_string(dncrop2, config=config)
plateList.append(strCurrentChar)
print(strCurrentChar)

print(plateList)
cv2.imshow("original image ", cropping)
cv2.waitKey(0)