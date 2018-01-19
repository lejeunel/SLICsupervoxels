import numpy as np
from libsvx import *
import matplotlib.pyplot as plt
import os
import glob, sys
from skimage import io, segmentation

reqdsupervoxelsize = 1000
compactness = 10.0

#This will hold our data
my_svx = svx.create()

#imgs_paths = glob.glob('images/*.png')
bee_path = 'bee.jpg'

#imgs = np.asarray([io.imread(im) for im in imgs_paths]).transpose(1,2,3,0)
img = io.imread(bee_path)
imgs = np.asarray([img, img, img, img, img]).transpose(1,2,3,0)

dims = imgs.shape
numrequiredsupervoxels = int(dims[0]*dims[1]*dims[3]/reqdsupervoxelsize)

labels, numlabels = my_svx.run(imgs, numrequiredsupervoxels, compactness)
print('numlabels: ')
print(numlabels)
print('numrequiredsupervoxels: ')
print(numrequiredsupervoxels)

frame = 0
label_cont = segmentation.find_boundaries(labels[...,frame])

plt.subplot(121)
plt.imshow(label_cont)
plt.subplot(122)
plt.imshow(labels[..., frame])
plt.show()

#label_mosaic = np.hstack(labels)
#plt.subplot(121); plt.imshow(label_cont);
#plt.subplot(122); plt.imshow(labels[..., frame]);
#plt.show()
