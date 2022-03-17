import sys
import cv2
import numpy as np
import math
import logging
import socket
import struct
import time
import requests
import urllib3

# address = ("192.168.1.12", 8080)
address = ("192.168.43.1", 8080)


def lane_detection():

    def detect_lane(frame):

        edges = detect_edges(frame)
        cropped_edges = region_of_interest(edges)
        line_segments = detect_line_segments(cropped_edges)
        lane_lines = average_slope_intercept(frame, line_segments)

        return lane_lines


    def detect_edges(frame):

        # take the image and convert it from BGR to HSV
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        # remove the blue color
        lower_blue = np.array([60, 40, 40])
        upper_blue = np.array([150, 255, 255])
        mask = cv2.inRange(hsv, lower_blue, upper_blue)
        # edge detection using canny function
        edges = cv2.Canny(mask, 200, 400)

        return edges


    def region_of_interest(edges):
        # take the width and height of the image and store them in these variable
        height, width = edges.shape

        # create a mask with the same shape as original image
        mask = np.zeros_like(edges)

        # only focus bottom half of the screen
        polygon = np.array([[
            (0, height * 1 / 2),
            (width, height * 1 / 2),
            (width, height),
            (0, height),
        ]], np.int32)

        # filing the mask with white polygon
        cv2.fillPoly(mask, polygon, 255)
        # performing the bitwise and operation to crop the whole image but the region of interest
        cropped_edges = cv2.bitwise_and(edges, mask)
        return cropped_edges


    def detect_line_segments(cropped_edges):
        """""
        trial and erorr process // we will test and see !

        """""
        # tuning min_threshold, minLineLength, maxLineGap is a trial and error process by hand
        rho = 1  # distance precision in pixel, i.e. 1 pixel
        angle = np.pi / 180  # angular precision in radian, i.e. 1 degree
        min_threshold = 10  # minimal of votes
        line_segments = cv2.HoughLinesP(cropped_edges, rho, angle, min_threshold,
                                        np.array([]), minLineLength=8, maxLineGap=4)

        return line_segments


    def average_slope_intercept(frame, line_segments):
        """
        This function combines line segments into one or two lane lines
        If all line slopes are < 0: then we only have detected left lane
        If all line slopes are > 0: then we only have detected right lane
        """
        lane_lines = []
        if line_segments is None:
            logging.info('No line_segment segments detected')
            return lane_lines

        height, width, _ = frame.shape
        left_fit = []
        right_fit = []

        boundary = 1 / 3
        left_region_boundary = width * (1 - boundary)  # left lane line segment should be on left 2/3 of the screen
        right_region_boundary = width * boundary  # right lane line segment should be on left 2/3 of the screen

        for line_segment in line_segments:
            for x1, y1, x2, y2 in line_segment:
                if x1 == x2:
                    logging.info('skipping vertical line segment (slope=inf): %s' % line_segment)
                    continue
                fit = np.polyfit((x1, x2), (y1, y2), 1)
                slope = fit[0]
                intercept = fit[1]
                if slope < 0:
                    if x1 < left_region_boundary and x2 < left_region_boundary:
                        left_fit.append((slope, intercept))
                else:
                    if x1 > right_region_boundary and x2 > right_region_boundary:
                        right_fit.append((slope, intercept))

        left_fit_average = np.average(left_fit, axis=0)
        if len(left_fit) > 0:
            lane_lines.append(make_points(frame, left_fit_average))

        right_fit_average = np.average(right_fit, axis=0)
        if len(right_fit) > 0:
            lane_lines.append(make_points(frame, right_fit_average))

        logging.debug('lane lines: %s' % lane_lines)  # [[[316, 720, 484, 432]], [[1009, 720, 718, 432]]]

        return lane_lines


    def compute_steering_angle(frame, lane_lines):
        """ Find the steering angle based on lane line coordinate
            We assume that camera is calibrated to point to dead center
        """
        if len(lane_lines) == 0:
            logging.info('No lane lines detected, do nothing')
            return -90

        height, width, _ = frame.shape
        if len(lane_lines) == 1:
            logging.debug('Only detected one lane line, just follow it. %s' % lane_lines[0])
            x1, _, x2, _ = lane_lines[0][0]
            x_offset = x2 - x1
        else:
            _, _, left_x2, _ = lane_lines[0][0]
            _, _, right_x2, _ = lane_lines[1][0]
            camera_mid_offset_percent = 0.02  # 0.0 means car pointing to center, -0.03: car is centered to left, +0.03 means car pointing to right
            mid = int(width / 2 * (1 + camera_mid_offset_percent))
            x_offset = (left_x2 + right_x2) / 2 - mid

        # find the steering angle, which is angle between navigation direction to end of center line
        y_offset = int(height / 2)

        angle_to_mid_radian = math.atan(x_offset / y_offset)  # angle (in radian) to center vertical line
        angle_to_mid_deg = int(angle_to_mid_radian * 180.0 / math.pi)  # angle (in degrees) to center vertical line
        steering_angle = angle_to_mid_deg + 90  # this is the steering angle needed by picar front wheel

        logging.debug('new steering angle: %s' % steering_angle)
        return steering_angle


    def stabilize_steering_angle(curr_steering_angle, new_steering_angle,
                                 num_of_lane_lines,
                                 max_angle_deviation_two_lines=5,
                                 max_angle_deviation_one_lane=1):

        """
        Using last steering angle to stabilize the steering angle
        if new angle is too different from current angle,
        only turn by max_angle_deviation degrees
        """
        if num_of_lane_lines == 2:
            # if both lane lines detected, then we can deviate more
            max_angle_deviation = max_angle_deviation_two_lines
        else:
            # if only one lane detected, don't deviate too much
            max_angle_deviation = max_angle_deviation_one_lane

        angle_deviation = new_steering_angle - curr_steering_angle
        if abs(angle_deviation) > max_angle_deviation:
            stabilized_steering_angle = int(curr_steering_angle
                                            + max_angle_deviation * angle_deviation / abs(angle_deviation))
        else:
            stabilized_steering_angle = new_steering_angle
        return stabilized_steering_angle


    def steer(frame, lane_lines):
        current_angle = 90

        logging.debug('steering...')
        if len(lane_lines) == 0:
            logging.error('No lane lines detected, nothing to do.')
            return frame

        new_steering_angle = compute_steering_angle(frame, lane_lines)
        # current_angle = stabilize_steering_angle(current_angle, new_steering_angle, len(lane_lines))

        curr_lane_image = display_lines(frame, lane_lines)
        curr_heading_image = display_heading_line(curr_lane_image, new_steering_angle)
        # 0-89 degree: turn left
        # 90 degree: going straight
        # 91-180 degree: turn right
        if new_steering_angle>=96 and new_steering_angle<=180:
            print("turn right")
            r=requests.get('http://192.168.43.133/turnright')

        else:
            if new_steering_angle>=0 and new_steering_angle<=84:
                print('turn left')
                r=requests.get('http://192.168.43.133/turnleft')


            else:
                if new_steering_angle <= 95 and new_steering_angle >= 85:
                    print('straight')
                    r=requests.get('http://192.168.43.133/straight')

        print("new angle=", new_steering_angle)
        # print("current angle=", current_angle)
        return curr_heading_image


    def display_lines(frame, lines):
        line_image = np.zeros_like(frame)
        if lines is not None:
            for line in lines:
                for x1, y1, x2, y2 in line:
                    cv2.line(line_image, (x1, y1), (x2, y2), (0, 255, 0), 10)
        line_image = cv2.addWeighted(frame, 0.6, line_image, 1, 1)
        return line_image


    def display_heading_line(frame, steering_angle, line_color=(0, 0, 255), line_width=5):
        # figure out the heading line from steering angle
        # heading line (x1,y1) is always center bottom of the screen
        # (x2, y2) requires a bit of trigonometry
        # Note: the steering angle of:
        # 0-89 degree: turn left
        # 90 degree: going straight
        # 91-180 degree: turn right

        heading_image = np.zeros_like(frame)
        height, width, _ = frame.shape

        steering_angle_radian = steering_angle / 180.0 * math.pi
        x1 = int(width / 2)
        y1 = height
        x2 = int(x1 - height / 2 / math.tan(steering_angle_radian))
        y2 = int(height / 2)

        cv2.line(heading_image, (x1, y1), (x2, y2), line_color, line_width)
        heading_image = cv2.addWeighted(frame, 0.8, heading_image, 1, 1)

        return heading_image


    def make_points(frame, line):

        height, width, _ = frame.shape
        slope, intercept = line
        y1 = height  # bottom of the frame
        y2 = int(y1 * 1 / 2)  # make points from middle of the frame down

        # bound the coordinates within the frame
        x1 = max(-width, min(2 * width, int((y1 - intercept) / slope)))
        x2 = max(-width, min(2 * width, int((y2 - intercept) / slope)))
        return [[x1, y1, x2, y2]]

    img = cv2.imread('tst.PNG')
    scale_percent = 60  # calculate the (scale_percent) percent of original dimensions
    width = int(img.shape[1] * scale_percent / 100)
    height = int(img.shape[0] * scale_percent / 100)
    dsize = (width, height)
    scl_img = cv2.resize(img, dsize)

    lane_lines = detect_lane(scl_img)
    drive = steer(scl_img, lane_lines)
    cv2.imshow("heading", drive)
    cv2.waitKey(1)



while True:

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(address)
    # print("Connected to the server...")
    buf = b''
    while len(buf) < 4:
        buf += s.recv(4 - len(buf))

    size = struct.unpack('!i', buf)
    # print("receiving %s bytes" % size)

    with open('tst.PNG', 'wb') as img:
        while True:
            data = s.recv(100000)
            if not data:
                break
            img.write(data)
    # print('received, yay!')
    lane_detection()
    time.sleep(0.5)
    s.close()




