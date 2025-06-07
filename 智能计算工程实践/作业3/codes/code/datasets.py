import torch
from torch.utils.data import Dataset
import h5py
import json
import os

class CaptionDataset(Dataset):
  def __init__(self, data_folder, data_name, split, captions_per_image, dataset_name):
    self.split = split
    assert self.split in {'TRAIN', 'VAL', 'TEST'}

    self.h1 = h5py.File(os.path.join(data_folder, self.split + '_IMAGE_FEATURES_1_' + data_name + '.h5'), 'r')
    self.imgs1 = self.h1['images_features']

    self.h2 = h5py.File(os.path.join(data_folder, self.split + '_IMAGE_FEATURES_2_' + data_name + '.h5'), 'r')
    self.imgs2 = self.h2['images_features']

    self.cpi = captions_per_image

    self.dataset_name = dataset_name

    with open(os.path.join(data_folder, self.split + '_CAPTIONS_' + data_name + '.json'), 'r') as f:
      self.captions = json.load(f)

    with open(os.path.join(data_folder, self.split + '_CAPLENS_' + data_name + '.json'), 'r') as f:
      self.caplens = json.load(f)

    self.dataset_size = len(self.captions)

  def __getitem__(self, i):

    if self.dataset_name == 'MOSCC':
      img1 = torch.FloatTensor(self.imgs1[i // self.cpi])
      img2 = torch.FloatTensor(self.imgs2[i // self.cpi])

    if self.dataset_name == 'CCHANGE' or self.dataset_name == 'STD':
      img1 = torch.FloatTensor(self.imgs1[i])
      img2 = torch.FloatTensor(self.imgs2[i])

    caption = torch.LongTensor(self.captions[i])
    caplen = torch.LongTensor([self.caplens[i]])

    if self.split is 'TRAIN':
      return img1, img2, caption, caplen
    else:
      if self.dataset_name == 'MOSCC':
        all_captions = torch.LongTensor(
          self.captions[((i // self.cpi) * self.cpi):((i//self.cpi)*self.cpi) + self.cpi])
        return img1, img2, caption, caplen, all_captions
      if self.dataset_name == 'CCHANGE' or self.dataset_name == 'STD':
        return img1, img2, caption, caplen, caption

  def __len__(self):
    return self.dataset_size





