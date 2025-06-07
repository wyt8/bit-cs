import torch.backends.cudnn as cudnn
import torch.optim
import torch.utils.data
from datasets import *
from utils import *
from nltk.translate.bleu_score import corpus_bleu
import torch.nn.functional as F
from tqdm import tqdm
import json

import argparse

# Parameters

data_name = '3dcc_5_cap_per_img_0_min_word_freq' # base name shared by data files

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu") # sets device for model and PyTorch tensors
cudnn.benchmark = True # set to true only if inputs to model are fixed size; otherwise lot of computational overhead

captions_per_image = 5
batch_size = 1

# model_name


def get_key(dict_, value):
  return [k for k, v in dict_.items() if v == value]

def evaluate(args, beam_size, n_gram):
  # Load model
  checkpoint = torch.load(args.checkpoint,map_location='cuda:0')

  encoder = checkpoint['encoder']
  encoder = encoder.to(device)
  encoder.eval()

  decoder = checkpoint['decoder']
  decoder = decoder.to(device)
  decoder.eval()

  # Load word map (word2ix)
  with open(args.word_map_file, 'r') as f:
    word_map = json.load(f)

  rev_word_map = {v: k for k, v in word_map.items()}
  vocab_size = len(word_map)

  result_json_file = {}
  reference_json_file = {}


  """
  Evaluation

  :param beam_size: beam size at which to generate captions for evaluation
  :return: BLEU-4 score
  """

  # DataLoader
  loader = torch.utils.data.DataLoader(
      CaptionDataset(args.data_folder, data_name, 'TEST', captions_per_image,'MOSCC'),
      batch_size = batch_size, shuffle=False, num_workers=1, pin_memory=True)

  # TODO: Batched Beam Search
  # Therefore, do not use a batch_size greater than 1 - IMPORTANT!

  # Lists to store references (true captions), and hypothesis (prediction) for each image
  # If for n images, we have n hypotheses, and references a, b, c... for each image, we need -
  # references = [[ref1a, ref1b, ref1c], [ref2a, ref2b], ...], hypotheses = [hyp1, hyp2, ...]
  references = list()
  hypotheses = list()

  # For each image
  ddd = 0
  for i, (image1, image2, caps, caplens, allcaps) in enumerate(
          tqdm(loader, desc="EVALUATING AT BEAM SIZE " + str(beam_size))):

    if (i % 5) != 0:
      continue

    current_index = i
    ddd += 1

    k = beam_size

    # Move to GPU device, if available
    image1 = image1.to(device) # 
    image2 = image2.to(device) #

    memory = encoder(image1, image2)

    tgt = torch.zeros(80,1).to(device).to(torch.int64)
    tgt_length = tgt.size(0)

    #print(tgt_length)

    mask = (torch.triu(torch.ones(tgt_length, tgt_length)) == 1).transpose(0, 1)
    mask = mask.float().masked_fill(mask == 0, float('-inf')).masked_fill(mask == 1, float(0.0))
    mask = mask.to(device)

    tgt[0,0] = word_map['<start>']
    seq = []
    for i in range(tgt_length-1):
      ##
      tgt_embedding = decoder.vocab_embedding(tgt) 
      tgt_embedding = decoder.position_encoding(tgt_embedding) #(length, batch, feature_dim)

      pred = decoder.transformer(tgt_embedding, memory, tgt_mask = mask) #(length, batch, feature_dim)
      pred = decoder.wdc(pred) #(length, batch, vocab_size)

      pred = pred[i,0,:]
      predicted_id = torch.argmax(pred, axis=-1)
   
      ## if word_map['<end>'], end for current sentence
      if predicted_id == word_map['<end>']:
        break

      seq.append(predicted_id)

      ## update mask, tgt
      tgt[i+1,0] = predicted_id
      mask[i+1,0] = 0.0


    # References
    img_caps = allcaps[0].tolist()  ######################
    img_captions = list(
        map(lambda c: [w for w in c if w not in {word_map['<start>'], word_map['<end>'], word_map['<pad>']}],
            img_caps)) # remove <start> and pads
    references.append(img_captions)


    # Hypotheses
    temptemp = [w for w in seq if w not in {word_map['<start>'], word_map['<end>'], word_map['<pad>']}]
    hypotheses.append(temptemp)

    assert len(references) == len(hypotheses)


  #-----------------------------------------------------------------
  kkk = -1
  for item in hypotheses:
    kkk += 1
    line_hypo = ""

    for word_idx in item:
      word = get_key(word_map, word_idx)
        #print(word)
      line_hypo += word[0] + " "

    result_json_file[str(kkk)] = []
    result_json_file[str(kkk)].append(line_hypo)

    line_hypo += "\r\n"


  kkk = -1
  for item in references:
    kkk += 1

    reference_json_file[str(kkk)] = []

    for sentence in item:
      line_repo = ""
      for word_idx in sentence:
        word = get_key(word_map, word_idx)
        line_repo += word[0] + " "
              
      reference_json_file[str(kkk)].append(line_repo)

      line_repo += "\r\n"


  with open('eval_results_fortest/' + args.model_name + '_res.json','w') as f:
    json.dump(result_json_file,f)

  with open('eval_results_fortest/' + args.model_name + '_gts.json','w') as f:
    json.dump(reference_json_file,f)


def r2(bleu):
  result = float(int(bleu*10000.0)/10000.0)
  
  result = str(result)
  while len(result) < 6:
    result += "0"

  return result



if __name__=='__main__':
  parser = argparse.ArgumentParser()
  

  parser.add_argument('--data_folder', default='dataset/for_train/3dcc_v0-2/con-sub_too_r3')
  parser.add_argument('--checkpoint', default='results/v0-2_total_concat_subtractBEST_checkpoint_3dcc_5_cap_per_img_0_min_word_freq.pth.tar')
  parser.add_argument('--word_map_file', default='dataset/for_train/3dcc_v0-2/con-sub_too_r3/WORDMAP_3dcc_5_cap_per_img_0_min_word_freq.json')
  parser.add_argument('--model_name', default='con-sub_too_r3')

  args = parser.parse_args()

  beam_size = 1
  n_gram = 4
  evaluate(args, beam_size, n_gram)













































