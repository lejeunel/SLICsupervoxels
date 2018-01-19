import numpy as np
from libsvx import *
import matplotlib.pyplot as plt
import os
import glob, sys
from skimage import io, segmentation


def make_mean_regions(img, labels):

    img_out = np.zeros(img.shape, dtype=np.uint8)
    for l in np.unique(labels):
        mean_color = np.round(np.mean(img[labels == l, :], axis = 0)).astype(np.uint8)
        img_out[labels==l, :] = mean_color

    return img_out

reqdsupervoxelsize = 1000
compactness = 10.0

# Create supervoxel object
my_svx = svx.create()

img_path = 'bee.jpg'

img = io.imread(img_path)

# Make stack with copies of same image
imgs = np.asarray([img, img, img, img, img]).transpose(1,2,3,0)

dims = imgs.shape
numrequiredsupervoxels = int(dims[0]*dims[1]*dims[3]/reqdsupervoxelsize)

labels, numlabels = my_svx.run(imgs, numrequiredsupervoxels, compactness)

frame = 0
label_cont = segmentation.find_boundaries(labels[...,frame])
mean_img = make_mean_regions(imgs[..., frame], labels[..., frame])
mean_img[label_cont] = (255, 0, 0)


# Plot original image and contours of supervoxels
plt.subplot(121)
plt.imshow(imgs[..., frame])
plt.title('Original image')
plt.subplot(122)
plt.imshow(mean_img)
plt.title('Contours of supervoxels')
plt.show()
