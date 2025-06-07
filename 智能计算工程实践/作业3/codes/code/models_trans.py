import torch
from torch import nn
import math
from torch.nn.init import xavier_uniform_

device = torch.device("cuda:2" if torch.cuda.is_available() else "cpu")




class MCCFormers_S(nn.Module):
  """
  MCCFormers-S
  """

  def __init__(self, feature_dim, h, w, d_model = 512, n_head = 4, n_layers = 2, dim_feedforward = 2048, max_image_num = 20):
    """
    :param feature_dim: feature dimension of input dimension
    :param d_model: dimension of input to Transformer
    :param n_head: the number of heads in Transformer
    :param n_layers: the number of layers of Transformer
    :param dim_feedforward: dimension of hidden state
    :param h: height of input image
    :param w: width of input image
    """
    super(MCCFormers_S, self).__init__()

    self.input_proj = nn.Conv2d(feature_dim, d_model, kernel_size = 1)

    self.d_model = d_model

    encoder_layer = nn.TransformerEncoderLayer(d_model, n_head, dim_feedforward = dim_feedforward)
    self.transformer = nn.TransformerEncoder(encoder_layer, n_layers)

    # 修改位置编码，以支持多张图片
    self.max_image_num = max_image_num
    self.idx_embedding = nn.Embedding(max_image_num, d_model)
    self.w_embedding = nn.Embedding(w, int(d_model/2))
    self.h_embedding = nn.Embedding(h, int(d_model/2))

  def forward(self, img_feat_list: list):    
    # 获取输入图像数据的有关特征
    # img_feat_list [(batch_size, feature_dim, h, w)]
    batch = img_feat_list[0].size(0)
    feature_dim = img_feat_list[0].size(1)
    h, w = img_feat_list[0].size(2), img_feat_list[0].size(3)
    img_num = len(img_feat_list)

    d_model = self.d_model
    
    # 对输入的图像数据进行转换
    for i in range(img_num):
      img_feat_list[i] = self.input_proj(img_feat_list[i])
      img_feat_list[i] = img_feat_list[i].view(batch, d_model, -1) # (batch, d_model, h*w)

    # 位置编码
    pos_w = torch.arange(w, device=device).to(device)
    pos_h = torch.arange(h, device=device).to(device)
    embed_w = self.w_embedding(pos_w)
    embed_h = self.h_embedding(pos_h)
    position_embedding = torch.cat([embed_w.unsqueeze(0).repeat(h, 1, 1),
                                    embed_h.unsqueeze(1).repeat(1, w, 1)], 
                                    dim = -1) 

    #(h, w, d_model)
    position_embedding = position_embedding.permute(2, 0, 1).unsqueeze(0).repeat(batch, 1, 1, 1) #(batch, d_model, h, w)
    position_embedding = position_embedding.view(batch, d_model, -1)

    # 对图片特征添加位置编码
    for i in range(img_num):
      img_feat_list[i] = img_feat_list[i] + position_embedding

    img_feat_cat = torch.cat(img_feat_list, dim = 2) #(batch, d_model, n*h*w)
    img_feat_cat = img_feat_cat.permute(2, 0, 1) #(n*h*w, batch, d_model)

    # 序号编码
    idx = []
    for i in range(img_num):
      idx.append(torch.full((batch, h*w), i).long().to(device))
    idx = torch.cat(idx, dim = 1) #(batch, n*h*w)
    idx_embedding = self.idx_embedding(idx) #(batch, n*h*w, d_model)
    idx_embedding = idx_embedding.permute(1, 0, 2) #(n*h*w, batch, d_model)

    feature = img_feat_cat + idx_embedding #(n*h*w, batch, d_model)
    feature = self.transformer(feature) #(n*h*w, batch, d_model)

    for i in range(img_num):
      img_feat_list[i] = feature[i*h*w:(i+1)*h*w].permute(1, 2, 0) # (batch, d_model, h*w)
      img_feat_list[i] = img_feat_list[i].view(batch, d_model, -1).permute(2, 0, 1) #(h*w, batch, d_model)

    img_feat = torch.cat(img_feat_list, dim=2) #(h*w, batch, n * d_model)
    return img_feat



class CrossTransformer(nn.Module):
  """
  Cross Transformer layer
  """
  def __init__(self, dropout, d_model = 512, n_head = 4):
    """
    :param dropout: dropout rate
    :param d_model: dimension of hidden state
    :param n_head: number of heads in multi head attention
    """
    super(CrossTransformer, self).__init__()
    self.attention = nn.MultiheadAttention(d_model, n_head, dropout = dropout)

    self.norm1 = nn.LayerNorm(d_model)
    self.norm2 = nn.LayerNorm(d_model)

    self.dropout1 = nn.Dropout(dropout)
    self.dropout2 = nn.Dropout(dropout)
    self.dropout3 = nn.Dropout(dropout)

    self.activation = nn.ReLU()

    self.linear1 = nn.Linear(d_model, d_model * 4)
    self.linear2 = nn.Linear(d_model * 4, d_model)

  def forward(self, input1, input2):
    attn_output, attn_weight = self.attention(input1, input2, input2)
    output = input1 + self.dropout1(attn_output)
    output = self.norm1(output)
    ff_output = self.linear2(self.dropout2(self.activation(self.linear1(output))))
    output = output + self.dropout3(ff_output)
    output = self.norm2(output)

    return output

class MCCFormers_D(nn.Module):
  """
  MCCFormers-D
  """
  def __init__(self, feature_dim, dropout, h, w, d_model = 512, n_head = 4, n_layers = 2):
    """
    :param feature_dim: dimension of input features
    :param dropout: dropout rate
    :param d_model: dimension of hidden state
    :param n_head: number of heads in multi head attention
    :param n_layer: number of layers of transformer layer
    """
    super(MHAFF, self).__init__()
    self.d_model = d_model
    self.n_layers = n_layers
    
    self.w_embedding = nn.Embedding(w, int(d_model/2))
    self.h_embedding = nn.Embedding(h, int(d_model/2))

    self.projection = nn.Conv2d(feature_dim, d_model, kernel_size = 1)
    self.transformer = nn.ModuleList([CrossTransformer(dropout, d_model, n_head) for i in range(n_layers)])

    self._reset_parameters()

  def _reset_parameters(self):
    """Initiate parameters in the transformer model."""        
    for p in self.parameters():
      if p.dim() > 1:
        xavier_uniform_(p)

  def forward(self, img_feat1, img_feat2):
    # img_feat1 (batch_size, feature_dim, h, w)
    batch = img_feat1.size(0)
    feature_dim = img_feat1.size(1)
    w, h = img_feat1.size(2), img_feat1.size(3)

    img_feat1 = self.projection(img_feat1)# + position_embedding # (batch_size, d_model, h, w)
    img_feat2 = self.projection(img_feat2)# + position_embedding # (batch_size, d_model, h, w)

    pos_w = torch.arange(w,device=device).to(device)
    pos_h = torch.arange(h,device=device).to(device)
    embed_w = self.w_embedding(pos_w)
    embed_h = self.h_embedding(pos_h)
    position_embedding = torch.cat([embed_w.unsqueeze(0).repeat(h, 1, 1),
                                   embed_h.unsqueeze(1).repeat(1, w, 1)], 
                                   dim = -1) 
    #(h, w, d_model)
    position_embedding = position_embedding.permute(2, 0, 1).unsqueeze(0).repeat(batch, 1, 1, 1) #(batch, d_model, h, w)

    img_feat1 = img_feat1 + position_embedding # (batch_size, d_model, h, w)
    img_feat2 = img_feat2 + position_embedding # (batch_size, d_model, h, w)

    output1 = img_feat1.view(batch, self.d_model, -1).permute(2, 0, 1) # (h*w, batch_size, d_model)
    output2 = img_feat2.view(batch, self.d_model, -1).permute(2, 0, 1) # (h*w, batch_size, d_model)

    for l in self.transformer:
      output1, output2 = l(output1, output2), l(output2, output1)


    position_embedding = position_embedding.view(batch,self.d_model,-1).permute(2,0,1)
    output1 = output1 #+ position_embedding
    output2 = output2 #+ position_embedding
    
    output = torch.cat([output1,output2],dim=2)
    #output1 = output1.permute(1, 2, 0).view(batch,512,16,16) #(batch_size, d_model, h*w)
    #output2 = output2.permute(1, 2, 0).view(batch,512,16,16) #(batch_size, d_model, h*w)
    return output

class PositionalEncoding(nn.Module):

  def __init__(self, d_model, dropout=0.1, max_len=5000):
      super(PositionalEncoding, self).__init__()
      self.dropout = nn.Dropout(p=dropout)

      pe = torch.zeros(max_len, d_model)
      position = torch.arange(0, max_len, dtype=torch.float).unsqueeze(1)
      div_term = torch.exp(torch.arange(0, d_model, 2).float() * (-math.log(10000.0) / d_model))
      pe[:, 0::2] = torch.sin(position * div_term)
      pe[:, 1::2] = torch.cos(position * div_term)
      pe = pe.unsqueeze(0).transpose(0, 1)
      self.register_buffer('pe', pe)

  def forward(self, x):
      x = x + self.pe[:x.size(0), :]
      return self.dropout(x)

class DecoderTransformer(nn.Module):
  """
  Decoder with Transformer.
  """

  def __init__(self, feature_dim, vocab_size, n_head, n_layers, dropout):
    """
    :param n_head: the number of heads in Transformer
    :param n_layers: the number of layers of Transformer
    """
    super(DecoderTransformer, self).__init__()

    self.feature_dim = feature_dim
    self.embed_dim = feature_dim
    self.vocab_size = vocab_size
    self.dropout = dropout

    # embedding layer
    self.vocab_embedding = nn.Embedding(vocab_size, self.embed_dim) #vocaburaly embedding
    
    # Transformer layer
    decoder_layer = nn.TransformerDecoderLayer(feature_dim, n_head, dim_feedforward = feature_dim * 4, dropout=self.dropout)
    self.transformer = nn.TransformerDecoder(decoder_layer, n_layers)
    self.position_encoding = PositionalEncoding(feature_dim)
    
    # Linear layer to find scores over vocabulary
    self.wdc = nn.Linear(feature_dim, vocab_size)
    self.dropout = nn.Dropout(p=self.dropout)
    self.init_weights() # initialize some layers with the uniform distribution

  def init_weights(self):
    """
    Initializes some parameters with values from the uniform distribution, for easier convergence
    """
    self.vocab_embedding.weight.data.uniform_(-0.1,0.1)

    self.wdc.bias.data.fill_(0)
    self.wdc.weight.data.uniform_(-0.1,0.1)    
 

  def forward(self, memory, encoded_captions, caption_lengths):
    """
    :param memory: image feature (S, batch, feature_dim)
    :param tgt: target sequence (length, batch)
    :param sentence_index: sentence index of each token in target sequence (length, batch)
    """
    #memory = torch.cat([memory1,memory2],dim=2)
    # print(memory.shape)
    # print(encoded_captions.shape)
    # print(caption_lengths.shape)
    tgt = encoded_captions.permute(1,0)
    tgt_length = tgt.size(0)

    mask = (torch.triu(torch.ones(tgt_length, tgt_length)) == 1).transpose(0, 1)
    mask = mask.float().masked_fill(mask == 0, float('-inf')).masked_fill(mask == 1, float(0.0))
    mask = mask.to(device)

    tgt_embedding = self.vocab_embedding(tgt) 
    tgt_embedding = self.position_encoding(tgt_embedding) #(length, batch, feature_dim)

    pred = self.transformer(tgt_embedding, memory, tgt_mask = mask) #(length, batch, feature_dim)
    pred = self.wdc(self.dropout(pred)) #(length, batch, vocab_size)

    pred = pred.permute(1,0,2)

    # Sort input data by decreasing lengths
    caption_lengths, sort_ind = caption_lengths.squeeze(1).sort(dim=0, descending=True)
    encoded_captions = encoded_captions[sort_ind]
    pred = pred[sort_ind]
    decode_lengths = (caption_lengths - 1).tolist()

    return pred, encoded_captions, decode_lengths, sort_ind


class PlainDecoder(nn.Module):
  """
  Dynamic speaker network.
  """

  def __init__(self, feature_dim, embed_dim, vocab_size, hidden_dim, dropout):
    """
    """
    super(PlainDecoder, self).__init__()

    self.feature_dim = feature_dim
    self.embed_dim = embed_dim
    self.hidden_dim = hidden_dim
    self.vocab_size = vocab_size
    self.dropout = dropout
    self.softmax = nn.Softmax(dim=1) ##### TODO #####

    # embedding layer
    self.embedding = nn.Embedding(vocab_size, embed_dim)
    self.dropout = nn.Dropout(p=self.dropout)

    
    self.decode_step = nn.LSTMCell(embed_dim + feature_dim, hidden_dim, bias=True)

    self.relu = nn.ReLU()
    # Linear layer to find scores over vocabulary
    self.wdc = nn.Linear(hidden_dim, vocab_size)
    self.init_weights() # initialize some layers with the uniform distribution

  def init_weights(self):
    """
    Initializes some parameters with values from the uniform distribution, for easier convergence
    """
    self.embedding.weight.data.uniform_(-0.1,0.1)

    self.wdc.bias.data.fill_(0)
    self.wdc.weight.data.uniform_(-0.1,0.1)    

  def forward(self, l_total, encoded_captions, caption_lengths):
    # To this point,
    # l_bef, l_aft have dimension
    # (batch_size, feature_dim)

    l_total = l_total.permute(1,2,0).sum(-1)
  
    batch_size = l_total.size(0)
    l_total = l_total.view(batch_size,-1)

    # Sort input data by decreasing lengths
    caption_lengths, sort_ind = caption_lengths.squeeze(1).sort(dim=0, descending=True)
    l_total = l_total[sort_ind]
    encoded_captions = encoded_captions[sort_ind]

    # Embedding
    embeddings = self.embedding(encoded_captions) # (batch_size, max_caption_length, embed_dim)

    h_ds = torch.zeros(batch_size, self.hidden_dim).to(device)
    c_ds = torch.zeros(batch_size, self.hidden_dim).to(device)

    decode_lengths = (caption_lengths - 1).tolist()

    predictions = torch.zeros(batch_size, max(decode_lengths), self.vocab_size).to(device)
    alphas = torch.zeros(batch_size, max(decode_lengths), 3).to(device) 


    for t in range(max(decode_lengths)):
      batch_size_t = sum([l > t for l in decode_lengths])
      c_temp = torch.cat([embeddings[:batch_size_t,t,:],l_total[:batch_size_t]], dim = 1)
      
      h_ds, c_ds = self.decode_step(c_temp, (h_ds[:batch_size_t], c_ds[:batch_size_t]))

      preds = self.wdc(h_ds) 
      predictions[:batch_size_t, t, :] = preds


    return predictions, encoded_captions, decode_lengths, sort_ind
