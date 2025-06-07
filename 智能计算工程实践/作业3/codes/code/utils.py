import os
import numpy as np
import h5py
import json
import torch

from tqdm import tqdm
from collections import Counter
from random import seed, choice, sample

def init_embedding(embeddings):
  bias = np.sqrt(3.0/embeddings.size(1))
  torch.nn.init.uniform_(embeddings, -bias, bias)

def load_embeddings(emb_file, word_map):
  # Find embedding dimension
  with open(emb_file, 'r') as f:
    emb_dim = len(f.readline().split(' ')) - 1

  vocab = set(word_map.keys())

  embeddings = torch.FloatTensor(len(vocab), emb_dim)
  init_embedding(embeddings)

  # Read embedding file
  print("\nLoading embeddings...")
  for line in open(emb_file, 'r'):
    line = line.split(' ')

    emb_word = line[0]
    embedding = list(map(lambda t: float(t), filter(lambda n: n and not n.isspace(), line[1:])))

    if emb_word not in vocab:
      continue

    embeddings[word_map[emb_word]] = torch.FloatTensor(embedding)

  return embeddings, emb_dim

def save_checkpoint(root_dir, data_name, epoch, encoder, decoder, encoder_optimizer, decoder_optimizer):
  state = {'epoch': epoch,
           'encoder':encoder,
           'decoder':decoder,
           'encoder_optimizer': encoder_optimizer,
           'decoder_optimizer': decoder_optimizer}

  filename = root_dir + 'checkpoint_epoch_' + str(epoch) + '_' + data_name + '.pth.tar'

  if (epoch%10 == 9):
    torch.save(state, filename)

class AverageMeter(object):
  def __init__(self):
    self.reset()

  def reset(self):
    self.val = 0
    self.avg = 0
    self.sum = 0
    self.count = 0

  def update(self, val, n=1):
    self.val = val
    self.sum += val*n
    self.count += n
    self.avg = self.sum / self.count


def adjust_learning_rate(optimizer, shrink_factor):
  print("\nDecaying learning rate.")
  for param_group in optimizer.param_groups:
    param_group['lr'] = param_group['lr'] * shrink_factor
  print("The new learning rate is %f\n" % (optimizer.param_groups[0]['lr'],))


def accuracy(scores, targets, k):
  batch_size = targets.size(0)
  _, ind = scores.topk(k, 1, True, True)
  correct = ind.eq(targets.view(-1,1).expand_as(ind))

  correct_total = torch.sum(correct.view(-1))
  
  return correct_total.item()*(100.0/batch_size)








