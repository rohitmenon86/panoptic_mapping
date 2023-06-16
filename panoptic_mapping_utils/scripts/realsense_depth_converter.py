#!/usr/bin/env python
import rospy
import cv_bridge 
from cv_bridge import CvBridge, CvBridgeError
from sensor_msgs.msg import Image
import numpy as np

pub_depth_image = rospy.Publisher("/camera/aligned_depth_to_color/image_float", Image, queue_size=1)
bridge = CvBridge()

def convert_depth_image(ros_image):
	# Use cv_bridge() to convert the ROS image to OpenCV format
	try:
	#Convert the depth image using the default passthrough encoding
		depth_image = bridge.imgmsg_to_cv2(ros_image, desired_encoding ="passthrough")
			#Convert the depth image to a Numpy array
		depth_array = np.array(depth_image, dtype=np.float32)
		depth_array = depth_array/1000.0

		float_img_msg = Image()
		float_img_msg.header = ros_image.header
		float_img_msg.width  = ros_image.width
		float_img_msg.height = ros_image.height
		float_img_msg.encoding = "32FC1"
		float_img_msg.step   = float_img_msg.width*4
		float_img_msg.is_bigendian = False
		float_img_msg.data = depth_array.astype(np.float32).tobytes()

		pub_depth_image.publish(float_img_msg)

	except CvBridgeError as	e:
		print(e)
	

    
      
	#rospy.loginfo(depth_array)

def pixel2depth():
	rospy.init_node('pixel2depth',anonymous=True)
	rospy.Subscriber("/camera/aligned_depth_to_color/image_raw", Image,callback=convert_depth_image, queue_size=10)
	rospy.spin()

if __name__ == '__main__':
	pixel2depth()