import json
import os
import random

scenes_dir = '../output/scenes'
output_dir = 'change_caption.json'
 
caption_templates = {
'add':['A <s> <c> <t> <z> has been added.',
       'A <s> <c> <t> <z> shows up.',
       'There is a new <s> <c> <t> <z>.',
       'A new <s> <c> <t> <z> is visible.',
       'Someone added a <s> <c> <t> <z>.'],
'delete':['The <s> <c> <t> <z> has disappeared.',
          'The <s> <c> <t> <z> is no longer there.',
          'The <s> <c> <t> <z> is missing.',
          'There is no longer a <s> <c> <t> <z>.',
          'Someone removed the <s> <c> <t> <z>.'],
'move':['The <s> <c> <t> <z> changed its location.',
        'The <s> <c> <t> <z> is in a different location.',
        'The <s> <c> <t> <z> was moved from its original location.',
        'The <s> <c> <t> <z> has been moved.',
        'Someone changed location of the <s> <c> <t> <z>.'],
'replace':['The <s> <c> <t> <z> was replaced by a <s1> <c1> <t1> <z1>.',
           'A <s1> <c1> <t1> <z1> replaced the <s> <c> <t> <z>.',
           'A <s1> <c1> <t1> <z1> is in the original position of <s> <c> <t> <z>.',
           'The <s> <c> <t> <z> gave up its position to a <s1> <c1> <t1> <z1>.',
           'Someone replaced the <s> <c> <t> <z> with a <s1> <c1> <t1> <z1>.']}

def instantiateCap(obj0, obj1, changetype):
  """
  生成描述
  """
  rand_sen_idx = random.randint(0,4)
  change_sentence = caption_templates[changetype][rand_sen_idx]
  
  change_sentence = change_sentence.replace('<s>',obj0['size']).replace('<c>',obj0['color']).replace('<t>',obj0['material']).replace('<z>',obj0['shape'])
  
  if changetype in ['add','delete','move']:
    return change_sentence
    
  change_sentence = change_sentence.replace('<s1>',obj1['size']).replace('<c1>',obj1['color']).replace('<t1>',obj1['material']).replace('<z1>',obj1['shape'])
  return change_sentence

def getObj(change_type, order, current_info): 
  """
  返回变化前后两个物体的信息
  """
  if change_type == 'add':
    return current_info['added_object'][order][0], None
    
  if change_type == 'delete':
    return current_info['dropped_object'][order], None    
  
  if change_type == 'move':
    return current_info['moved_object'][order][0], None    
    
  if change_type == 'replace':
    return current_info['replaced_object'][order], current_info['new_object'][order][0]         

def getOrder(change_record, idx):
  """
  获取同类变化下的序号
  """
  current_change = change_record[idx]
  order = 0 
  
  for i in range(0,idx):
    if change_record[i] == current_change:
      order = order + 1
      
  return order

seq_choices = {
  "2": [[0, 1]],
  "3": [[0, 0, 1], [0, 1, 1], [0, 1, 0]],
  "4": [[0, 0, 1, 1], [0, 1, 1, 0], [0, 1, 1, 1], [0, 0, 0, 1]],
  "5": [[0, 0, 0, 1, 1], [0, 0, 1, 1, 1], [0, 1, 1, 1, 0]],
  "6": [[0, 0, 0, 1, 1, 1], [0, 0, 1, 1, 1, 1], [0, 0, 0, 0, 1, 1], [0, 0, 1, 1, 0, 0], [0, 1, 1, 1, 1, 1], [0, 0, 0, 0, 0, 1], [0, 1, 1, 1, 1, 0]],
}
def generate_seq() -> list:
  """
  生成两项图片出现序列，返回结果是一个数组

  * 0表示原来的图片，1表示变换后的图片
  * 如[0, 0, 1, 1]表示传入的图片序列中，第1张和第2张图片是原来的图片，第3张和第4张图片是变换后的图片
  """
  seq_len= 6
  seq_choice = seq_choices[str(seq_len)]
  seq_order = random.randint(0, len(seq_choice) - 1)
  return seq_choice[seq_order]

ordinal_dict = {
    1: 'first',
    2: 'second',
    3: 'third',
    4: 'fourth',
    5: 'fifth',
    6: 'sixth',
    7: 'seventh',
    8: 'eighth',
    9: 'ninth',
    10: 'tenth',
    11: 'eleventh',
    12: 'twelfth',
    13: 'thirteenth',
    14: 'fourteenth',
    15: 'fifteenth',
    16: 'sixteenth',
    17: 'seventeenth',
    18: 'eighteenth',
    19: 'nineteenth',
    20: 'twentieth',
    21: 'twenty-first',
    22: 'twenty-second',
    23: 'twenty-third',
    24: 'twenty-fourth',
    25: 'twenty-fifth',
    26: 'twenty-sixth',
    27: 'twenty-seventh',
    28: 'twenty-eighth',
    29: 'twenty-ninth',
    30: 'thirtieth'
}

def generate_captions():
  """
  生成描述
  """
  change_captions = []
  for scene in os.listdir(scenes_dir):
    with open(os.path.join(scenes_dir, scene), 'r') as f:
      current_info = json.load(f)
      
    curr_cap_info = {}
    curr_cap_info['image_0'] = current_info['image_filename']
    
    # 变化后图片的文件名
    image_1 = current_info['image_filename'][:-4]
    image_1 += '_change' + str(len(current_info['change_record'])-1)
    for change in current_info['change_record']:
      image_1 += '_' + change
    image_1 += '.png'
    curr_cap_info['image_1'] = image_1
    
    seq = generate_seq()
    # 将序列信息填入当前的信息中
    curr_cap_info['seq'] = seq
    curr_cap_info['change_captions'] = []
    
    
    last_obj = seq[0]
    equal_len = 1 
    for i in range(1, len(seq)):
      " 遍历生成的序列 "
      if last_obj == seq[i]:
        equal_len += 1
      else:
        if equal_len >= 2:
          curr_cap_info['change_captions'].append(f"There is no change from the {ordinal_dict[i-equal_len+1]} image to the {ordinal_dict[i]} image.")
        
        curr_cap_info['change_captions'].append(f"In the {ordinal_dict[i+1]} image, the following changes occur:")
        change_record = current_info['change_record']
        for idx in range(len(change_record)):
          order = getOrder(change_record, idx) # 获取同类变化下的序号
          obj0, obj1 = getObj(change_record[idx], order, current_info)
          if seq[i] == 1:
            curr_cap_info['change_captions'].append(instantiateCap(obj0, obj1, change_record[idx]))
          else:
            if change_record[idx] == "add":
                curr_cap_info['change_captions'].append(instantiateCap(obj0, obj1, "delete"))
            elif change_record[idx] == "delete":
                curr_cap_info['change_captions'].append(instantiateCap(obj0, obj1, "add"))
            elif change_record[idx] == "move":
                curr_cap_info['change_captions'].append(instantiateCap(obj0, obj1, "move"))
            elif change_record[idx] == "replace":
                curr_cap_info['change_captions'].append(instantiateCap(obj1, obj0, "replace"))
        
        equal_len = 1
      last_obj = seq[i]
      
    if equal_len >= 2:
        curr_cap_info['change_captions'].append(f"There is no change from the {ordinal_dict[len(seq)-equal_len+1]} image to the {ordinal_dict[len(seq)]} image.")
      
    change_captions.append(curr_cap_info)
    
  with open(output_dir, 'w') as f:
    json.dump(change_captions, f)
    
    
if __name__ == '__main__':
  generate_captions()
