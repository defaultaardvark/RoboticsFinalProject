import cv2
import numpy as np
from sklearn import cluster
from imutils.video import VideoStream
import imutils
from time import sleep
import serial

params = cv2.SimpleBlobDetector_Params()
params.filterByInertia
params.minInertiaRatio = 0.6
min_area = 500
detector = cv2.SimpleBlobDetector_create(params)

mean_rolls = 10

def get_blobs(frame):
    frame_blurred = cv2.medianBlur(frame, 7)
    frame_gray = cv2.cvtColor(frame_blurred, cv2.COLOR_BGR2GRAY)
    blobs = detector.detect(frame_gray)

    return blobs


def get_dice_from_blobs(blobs):
    X = []
    for b in blobs:
        pos = b.pt

        if pos != None:
            X.append(pos)

    X = np.asarray(X)

    if len(X) > 0:
        clustering = cluster.DBSCAN(eps=40, min_samples=0).fit(X)

        # Find the largest label assigned + 1 for number of dice
        num_dice = max(clustering.labels_) + 1

        dice = []

        # Calculate centroid of each die
        for i in range(num_dice):
            X_dice = X[clustering.labels_ == i]

            centroid_dice = np.mean(X_dice, axis=0)

            dice.append([len(X_dice), *centroid_dice])

        return dice

    else:
        return []


def overlay_info(frame, dice, blobs):
    # Overlay blobs
    for b in blobs:
        pos = b.pt
        r = b.size / 2
        cv2.circle(frame, (int(pos[0]), int(pos[1])),
                   int(r), (255, 0, 0), 2)

    # Overlay dice number
    for d in dice:
        textsize = cv2.getTextSize(
            str(d[0]), cv2.FONT_HERSHEY_PLAIN, 3, 2)[0]

        cv2.putText(frame, str(d[0]),
                    (int(d[1] - textsize[0] / 2),
                     int(d[2] + textsize[1] / 2)),
                    cv2.FONT_HERSHEY_PLAIN, 3, (0, 255, 0), 2)

# Assumption that there are only 3 dice
def angle_calculation(dice):
    num_dice = []
    for i in range(len(dice)):
        num_dice.append(dice[i][0])

    dice_sum = np.sum(num_dice)
    if not num_dice:
        angle = 0
        dist = 0
    else:
        min_num = np.min(num_dice)
        min_num_index = num_dice.index(min_num)
        min_num_coords = np.array([int(dice[min_num_index][1]), int(dice[min_num_index][2])])
        origin = np.array([0, 0])
        top = np.array([min_num_coords[0], 0])

        # Finds position of die with lowest value by calculating relative angle from origin
        # Sums this value with the midpoint in robot's preplanned motion path to move arm to lowest die.
        cos_angle = np.dot(min_num_coords - origin, top - origin) / (np.linalg.norm(min_num_coords - origin) * np.linalg.norm(top - origin))
        angle = np.arccos(cos_angle)
        angle = int(np.degrees(angle))
        dist = int(np.linalg.norm(min_num_coords - origin)) #Unused
    
    # Continues to reroll lowest value until target sum is reached.
    # In this case, the mean roll summation of 3d6, or 10.
    if dice_sum > mean_rolls:
        done = 1
    else:
        done = 0

    return angle, done

_serial = serial.Serial("COM3", 19200)
_serial.flushInput()
sleep(2)
cap = cv2.VideoCapture(0)
print("Connecting to Arduino....")

while(True):
    ret, frame = cap.read()

    blobs = get_blobs(frame)
    dice = get_dice_from_blobs(blobs)
    overlay_info(frame, dice, blobs)

    angle, done = angle_calculation(dice)
    serialWrite = "A{0:d}D{1:d}Z".format(angle, done)
    print(angle)
    _serial.write(serialWrite.encode())
    # line = _serial.readline()
    # print(int(line))
    # print(angle, dist)

    cv2.imshow("frame", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

_serial.close()
cap.release()
cv2.destroyAllWindows()