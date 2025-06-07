# Copyright 2017-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.

from __future__ import print_function
import math, sys, random, argparse, json, os, tempfile
import mathutils
from datetime import datetime as dt
from collections import Counter
#import Blender

"""
Renders random scenes using Blender, each with with a random number of objects;
each object has a random size, position, color, and shape. Objects will be
nonintersecting but may partially occlude each other. Output images will be
written to disk as PNGs, and we will also write a JSON file for each image with
ground-truth scene information.

This file expects to be run from Blender like this:

blender --background --python render_images.py -- [arguments to this script]
"""

INSIDE_BLENDER = True
try:
  import bpy, bpy_extras
  from mathutils import Vector
except ImportError as e:
  INSIDE_BLENDER = False
if INSIDE_BLENDER:
  try:
    import utils
  except ImportError as e:
    print("\nERROR")
    print("Running render_images.py from Blender and cannot import utils.py.") 
    print("You may need to add a .pth file to the site-packages of Blender's")
    print("bundled python with a command like this:\n")
    print("echo $PWD >> $BLENDER/$VERSION/python/lib/python3.5/site-packages/clevr.pth")
    print("\nWhere $BLENDER is the directory where Blender is installed, and")
    print("$VERSION is your Blender version (such as 2.78).")
    sys.exit(1)

parser = argparse.ArgumentParser()

# Input options
parser.add_argument('--base_scene_blendfile', default='data/base_scene.blend',
    help="Base blender file on which all scenes are based; includes " +
          "ground plane, lights, and camera.")
parser.add_argument('--properties_json', default='data/properties.json',
    help="JSON file defining objects, materials, sizes, and colors. " +
         "The \"colors\" field maps from CLEVR color names to RGB values; " +
         "The \"sizes\" field maps from CLEVR size names to scalars used to " +
         "rescale object models; the \"materials\" and \"shapes\" fields map " +
         "from CLEVR material and shape names to .blend files in the " +
         "--object_material_dir and --shape_dir directories respectively.")
parser.add_argument('--shape_dir', default='data/shapes',
    help="Directory where .blend files for object models are stored")
parser.add_argument('--material_dir', default='data/materials',
    help="Directory where .blend files for materials are stored")
parser.add_argument('--shape_color_combos_json', default=None,
    help="Optional path to a JSON file mapping shape names to a list of " +
         "allowed color names for that shape. This allows rendering images " +
         "for CLEVR-CoGenT.")

# Settings for objects
parser.add_argument('--min_objects', default=5, type=int,
    help="The minimum number of objects to place in each scene")
parser.add_argument('--max_objects', default=8, type=int,
    help="The maximum number of objects to place in each scene")
parser.add_argument('--min_dist', default=0.25, type=float,
    help="The minimum allowed distance between object centers")
parser.add_argument('--margin', default=0.4, type=float,
    help="Along all cardinal directions (left, right, front, back), all " +
         "objects will be at least this distance apart. This makes resolving " +
         "spatial relationships slightly less ambiguous.")
parser.add_argument('--min_pixels_per_object', default=200, type=int,
    help="All objects will have at least this many visible pixels in the " +
         "final rendered images; this ensures that no objects are fully " +
         "occluded by other objects.")
parser.add_argument('--max_retries', default=500, type=int,
    help="The number of times to try placing an object before giving up and " +
         "re-placing all objects in the scene.")

# Output settings
parser.add_argument('--start_idx', default=0, type=int,
    help="The index at which to start for numbering rendered images. Setting " +
         "this to non-zero values allows you to distribute rendering across " +
         "multiple machines and recombine the results later.")
parser.add_argument('--num_images', default=5, type=int,
    help="The number of images to render")
parser.add_argument('--filename_prefix', default='CLEVR',
    help="This prefix will be prepended to the rendered images and JSON scenes")
parser.add_argument('--split', default='new',
    help="Name of the split for which we are rendering. This will be added to " +
         "the names of rendered images, and will also be stored in the JSON " +
         "scene structure for each image.")
parser.add_argument('--output_image_dir', default='../test/images/',
    help="The directory where output images will be stored. It will be " +
         "created if it does not exist.")
parser.add_argument('--output_scene_dir', default='../test/scenes/',
    help="The directory where output JSON scene structures will be stored. " +
         "It will be created if it does not exist.")
parser.add_argument('--output_scene_file', default='../test/CLEVR_scenes.json',
    help="Path to write a single JSON file containing all scene information")
parser.add_argument('--output_blend_dir', default='output_5views_cg/blendfiles',
    help="The directory where blender scene files will be stored, if the " +
         "user requested that these files be saved using the " +
         "--save_blendfiles flag; in this case it will be created if it does " +
         "not already exist.")
parser.add_argument('--save_blendfiles', type=int, default=0,
    help="Setting --save_blendfiles 1 will cause the blender scene file for " +
         "each generated image to be stored in the directory specified by " +
         "the --output_blend_dir flag. These files are not saved by default " +
         "because they take up ~5-10MB each.")
parser.add_argument('--version', default='1.0',
    help="String to store in the \"version\" field of the generated JSON file")
parser.add_argument('--license',
    default="Creative Commons Attribution (CC-BY 4.0)",
    help="String to store in the \"license\" field of the generated JSON file")
parser.add_argument('--date', default=dt.today().strftime("%m/%d/%Y"),
    help="String to store in the \"date\" field of the generated JSON file; " +
         "defaults to today's date")

# Rendering options
parser.add_argument('--use_gpu', default=0, type=int,
    help="Setting --use_gpu 1 enables GPU-accelerated rendering using CUDA. " +
         "You must have an NVIDIA GPU with the CUDA toolkit installed for " +
         "to work.")
parser.add_argument('--width', default=256, type=int,
    help="The width (in pixels) for the rendered images")
parser.add_argument('--height', default=256, type=int,
    help="The height (in pixels) for the rendered images")
parser.add_argument('--key_light_jitter', default=1.0, type=float,
    help="The magnitude of random jitter to add to the key light position.")
parser.add_argument('--fill_light_jitter', default=1.0, type=float,
    help="The magnitude of random jitter to add to the fill light position.")
parser.add_argument('--back_light_jitter', default=1.0, type=float,
    help="The magnitude of random jitter to add to the back light position.")
parser.add_argument('--camera_jitter', default=1.5, type=float,
    help="The magnitude of random jitter to add to the camera position")
parser.add_argument('--render_num_samples', default=512, type=int,
    help="The number of samples to use when rendering. Larger values will " +
         "result in nicer images but will cause rendering to take longer.")
parser.add_argument('--render_min_bounces', default=8, type=int,
    help="The minimum number of bounces to use for rendering.")
parser.add_argument('--render_max_bounces', default=8, type=int,
    help="The maximum number of bounces to use for rendering.")
parser.add_argument('--render_tile_size', default=256, type=int,
    help="The tile size to use for rendering. This should not affect the " +
         "quality of the rendered image but may affect the speed; CPU-based " +
         "rendering may achieve better performance using smaller tile sizes " +
         "while larger tile sizes may be optimal for GPU-based rendering.")

# Test options
parser.add_argument('--camera_radius', default=1.0, type=float,
    help="The magnitude of random jitter to add to the camera position")
parser.add_argument('--largest_size', default=1.2, type=float,
    help="Largest size of default objects")

parser.add_argument('--min_change', default=1, type=int)
parser.add_argument('--max_change', default=4, type=int)
parser.add_argument('--wall_floor_file', default="data/textures/")


def main(args):
  num_digits = 6
  prefix = '%s_%s_' % (args.filename_prefix, args.split)
  img_template = '%s%%0%dd.png' % (prefix, num_digits)
  scene_template = '%s%%0%dd.json' % (prefix, num_digits)
  blend_template = '%s%%0%dd.blend' % (prefix, num_digits)
  img_template = os.path.join(args.output_image_dir, img_template)
  scene_template = os.path.join(args.output_scene_dir, scene_template)
  blend_template = os.path.join(args.output_blend_dir, blend_template)

  if not os.path.isdir(args.output_image_dir):
    os.makedirs(args.output_image_dir)
  if not os.path.isdir(args.output_scene_dir):
    os.makedirs(args.output_scene_dir)
  if args.save_blendfiles == 1 and not os.path.isdir(args.output_blend_dir):
    os.makedirs(args.output_blend_dir)
  
  all_scene_paths = []
  for i in range(args.num_images):
    img_path = img_template % (i + args.start_idx)
    scene_path = scene_template % (i + args.start_idx)
    all_scene_paths.append(scene_path)
    blend_path = None
    if args.save_blendfiles == 1:
      blend_path = blend_template % (i + args.start_idx)
    num_objects = random.randint(args.min_objects, args.max_objects)
    render_scene(args,
      num_objects=num_objects,
      output_index=(i + args.start_idx),
      output_split=args.split,
      output_image=img_path,
      output_scene=scene_path,
      output_blendfile=blend_path,
    )

  # After rendering all images, combine the JSON files for each scene into a
  # single JSON file.
  all_scenes = []
  for scene_path in all_scene_paths:
    with open(scene_path, 'r') as f:
      all_scenes.append(json.load(f))

  with open(args.output_scene_file, 'r') as f:
    #json.dump(output,f)
    temp = json.load(f)
    f.close()
  
  temp['scenes'].append(all_scenes)

  with open(args.output_scene_file, 'w') as f:
    json.dump(temp, f)
  


def render_scene(args,
    num_objects=5,
    output_index=0,
    output_split='none',
    output_image='render.png',
    output_scene='render_json',
    output_blendfile=None,
  ):

  # Load the main blendfile
  bpy.ops.wm.open_mainfile(filepath=args.base_scene_blendfile)

  # Load materials
  utils.load_materials(args.material_dir)

  # Set render arguments so we can get pixel coordinates later.
  # We use functionality specific to the CYCLES renderer so BLENDER_RENDER
  # cannot be used.
  render_args = bpy.context.scene.render
  render_args.engine = "CYCLES"
  render_args.filepath = output_image
  render_args.resolution_x = args.width
  render_args.resolution_y = args.height
  render_args.resolution_percentage = 100
  render_args.tile_x = args.render_tile_size
  render_args.tile_y = args.render_tile_size
  if args.use_gpu == 1:
    # Blender changed the API for enabling CUDA at some point
    if bpy.app.version < (2, 78, 0):
      bpy.context.user_preferences.system.compute_device_type = 'CUDA'
      bpy.context.user_preferences.system.compute_device = 'CUDA_0'
    else:
      cycles_prefs = bpy.context.user_preferences.addons['cycles'].preferences
      cycles_prefs.compute_device_type = 'CUDA'

  # Some CYCLES-specific stuff
  bpy.data.worlds['World'].cycles.sample_as_light = True
  bpy.context.scene.cycles.blur_glossy = 2.0
  bpy.context.scene.cycles.samples = args.render_num_samples
  bpy.context.scene.cycles.transparent_min_bounces = args.render_min_bounces
  bpy.context.scene.cycles.transparent_max_bounces = args.render_max_bounces
  if args.use_gpu == 1:
    bpy.context.scene.cycles.device = 'GPU'

  # This will give ground-truth information about the scene and its objects
  scene_struct = {
      'split': output_split,
      'image_index': output_index,
      'image_filename': os.path.basename(output_image),
      'objects': [],
      'directions': {},
  }


  ####################### solid color background 
  with open(args.properties_json, 'r') as f:
    background_color = json.load(f)["colors_background"]


  used_color = []
  print(len(background_color))
  color = random.randint(0,len(background_color)-1)

  used_color.append(color)
  ccolor1 = "color" + str(color)
  ccc1 = (float(background_color[ccolor1][0])/255.0,float(background_color[ccolor1][1])/255.0,float(background_color[ccolor1][2])/255.0)


  for obj in bpy.data.objects: ####################################################
    print(obj)
    
    if obj.name.startswith("Ground11") :


      mat = bpy.data.materials.new("PKHG")

      color = random.randint(0,len(background_color)-1)
      while color in used_color:
        color = random.randint(0,len(background_color)-1)
      used_color.append(color)
      ccolor = "color" + str(color)
      ccc = (float(background_color[ccolor][0])/255.0,float(background_color[ccolor][1])/255.0,float(background_color[ccolor][2])/255.0)
      
      mat.diffuse_color = ccc

      obj.active_material = mat

    if obj.name.startswith("wall1") :
      mat = bpy.data.materials.new("PKHG")

      color = random.randint(0,len(background_color)-1)
      while color in used_color:
        color = random.randint(0,len(background_color)-1)
      used_color.append(color)
      ccolor = "color" + str(color)
      ccc = (float(background_color[ccolor][0])/255.0,float(background_color[ccolor][1])/255.0,float(background_color[ccolor][2])/255.0)
      mat.diffuse_color = ccc

      obj.active_material = mat

    if obj.name.startswith("wall2") :
      mat = bpy.data.materials.new("PKHG")

      color = random.randint(0,len(background_color)-1)
      while color in used_color:
        color = random.randint(0,len(background_color)-1)
      used_color.append(color)
      ccolor = "color" + str(color)
      ccc = (float(background_color[ccolor][0])/255.0,float(background_color[ccolor][1])/255.0,float(background_color[ccolor][2])/255.0)
      mat.diffuse_color = ccc

      obj.active_material = mat


    if obj.name.startswith("sky1") :
      mat = bpy.data.materials.new("PKHG")
      mat.diffuse_color = ccc1
      obj.active_material = mat

  ###############################################################################

  # Put a plane on the ground so we can compute cardinal directions
  bpy.ops.mesh.primitive_plane_add(radius=5)
  plane = bpy.context.object


  # Figure out the left, up, and behind directions along the plane and record
  # them in the scene structure
  camera = bpy.data.objects['Camera']
  plane_normal = plane.data.vertices[0].normal
  cam_behind = camera.matrix_world.to_quaternion() * Vector((0, 0, -1))
  cam_left = camera.matrix_world.to_quaternion() * Vector((-1, 0, 0))
  cam_up = camera.matrix_world.to_quaternion() * Vector((0, 1, 0))
  plane_behind = (cam_behind - cam_behind.project(plane_normal)).normalized()
  plane_left = (cam_left - cam_left.project(plane_normal)).normalized()
  plane_up = cam_up.project(plane_normal).normalized()

  ox = camera.location[0]
  oy = camera.location[1]
  oz = camera.location[2]

  # Delete the plane; we only used it for normals anyway. The base scene file
  # contains the actual ground plane.
  utils.delete_object(plane)

  # Save all six axis-aligned directions in the scene struct
  scene_struct['directions']['behind'] = tuple(plane_behind)
  scene_struct['directions']['front'] = tuple(-plane_behind)
  scene_struct['directions']['left'] = tuple(plane_left)
  scene_struct['directions']['right'] = tuple(-plane_left)
  scene_struct['directions']['above'] = tuple(plane_up)
  scene_struct['directions']['below'] = tuple(-plane_up)


  # Add random jitter to lamp positions
  if args.key_light_jitter > 0:
    for i in range(3):
      bpy.data.objects['Lamp_Key'].location[i] += rand(args.key_light_jitter)
  if args.back_light_jitter > 0:
    for i in range(3):
      bpy.data.objects['Lamp_Back'].location[i] += rand(args.back_light_jitter)
  if args.fill_light_jitter > 0:
    for i in range(3):
      bpy.data.objects['Lamp_Fill'].location[i] += rand(args.fill_light_jitter)


  # Now make some random objects
  namelist = []
  original_list = []
  objects, blender_objects, positions, original_list = add_random_objects(scene_struct, num_objects, args, camera, namelist, original_list)

  blender_objects_original = []
  for item in blender_objects:
    blender_objects_original.append(item)

  original_color = []

  for i in range(len(blender_objects_original)):
    original_color.append(i)

  new_color = []


  objectss = []
  for item in objects:
    objectss.append(item)

  # Render the scene and dump the scene data structure
  scene_struct['objects'] = objectss
  scene_struct['relationships'] = compute_all_relationships(scene_struct)

  ### render  images
  filename_0 = render_args.filepath
  render_oneimage(render_args, camera, filename_0,-1, ox, oy, oz, [], [], [], [], 'original', '')
                #(render_args, camera, filename_0, camera_jitter, ox, oy, oz, original_objs, new_objs, original_color, new_color, type_, ttt):
  # save_objectmasks(blender_objects, [0],filename_0.replace('.png','_mask.png'))   


  ## min_change, max_change

  add_num = 0
  move_num = 0
  delete_num = 0
  replace_num = 0

  change_count = 0
  scene_struct['change_record'] = []

  scene_struct['added_object'] = []
  scene_struct['dropped_object'] = []
  scene_struct['moved_object'] = []
  scene_struct['replaced_object'] = []
  scene_struct['new_object'] = []

  attr_list = []
  blender_objects_1 = []

  colors_ = []

  
  for i in range(args.min_change,args.max_change+1):
    i = random.randint(0,3)

    if i == 0:
      new_color, blender_objects_1, obj_,added_object,_,positions,attr_list = add_one_object(new_color, blender_objects_1, blender_objects,scene_struct, args, camera, positions, render_args, namelist,attr_list)

      if _ > 0:
        scene_struct['change_record'].append('add')
        ttt = ""
        for item in scene_struct['change_record']:
          ttt = ttt + '_' + item

        kk = []
        new_color_ = []
        for item in blender_objects:
          kk.append(item)
          new_color_.append(0)
  
        for item in blender_objects_1:
          kk.append(item)

        for item in new_color:
          new_color_.append(item)

        oc_ = []
        for item in original_color:
          oc_.append(item)
        render_oneimage(render_args, camera, filename_0.replace('.png','_change' + str(change_count) + ttt + '.png'), args.camera_jitter, ox, oy, oz, blender_objects_original, kk, oc_, new_color_,'add',ttt)
        colors_.append(oc_)

        change_count = change_count + 1
        scene_struct['added_object'].append(added_object)
      else:
        exit()


    if i == 1:
      original_color, new_color, blender_objects_1, moved_object,_,positions,original_list, attr_list = move_one_object(original_color, new_color, blender_objects_1, scene_struct, blender_objects, objects, args, camera, positions, render_args, namelist,original_list, attr_list)

      if _ > 0:
        scene_struct['change_record'].append('move')
        ttt = ""
        for item in scene_struct['change_record']:
          ttt = ttt + '_' + item

        kk = []
        new_color_ = []
        for item in blender_objects:
          kk.append(item)
          new_color_.append(0)
  
        for item in blender_objects_1:
          kk.append(item)

        for item in new_color:
          new_color_.append(item)

        oc_ = []
        for item in original_color:
          oc_.append(item)

        render_oneimage(render_args, camera, filename_0.replace('.png','_change' + str(change_count) + ttt + '.png'), args.camera_jitter, ox, oy, oz, blender_objects_original, kk, oc_, new_color_,'move',ttt)
        colors_.append(oc_)

        change_count = change_count + 1
        scene_struct['moved_object'].append(moved_object) 
      else:
        exit()


    if i == 2:
      original_color, dropped_object,_, original_list, attr_list = drop_one_object(original_color, blender_objects, objects, args, camera, positions, render_args, namelist, original_list, attr_list)

      if _ > 0:
        scene_struct['change_record'].append('delete')
        ttt = ""
        for item in scene_struct['change_record']:
          ttt = ttt + '_' + item

        kk = []
        new_color_ = []
        for item in blender_objects:
          kk.append(item)
          new_color_.append(0)
  
        for item in blender_objects_1:
          kk.append(item)

        for item in new_color:
          new_color_.append(item)

        oc_ = []
        for item in original_color:
          oc_.append(item)
        render_oneimage(render_args, camera, filename_0.replace('.png','_change' + str(change_count) + ttt + '.png'), args.camera_jitter, ox, oy, oz, blender_objects_original, kk, oc_, new_color_,'delete',ttt)
        colors_.append(oc_)

        change_count = change_count + 1

        scene_struct['dropped_object'].append(dropped_object) 
      else:
        exit()

    if i == 3:
      original_color, new_color, blender_objects_1, replaced_object, new_object,_,original_list,positions, attr_list = replace_one_object(original_color, new_color, blender_objects_1,blender_objects, objects, args, render_args, camera, namelist,original_list,positions, attr_list)

      if _ > 0:
        scene_struct['change_record'].append('replace')
        ttt = ""
        for item in scene_struct['change_record']:
          ttt = ttt + '_' + item

        kk = []
        new_color_ = []
        for item in blender_objects:
          kk.append(item)
          new_color_.append(0)
  
        for item in blender_objects_1:
          kk.append(item)

        for item in new_color:
          new_color_.append(item)

        oc_ = []
        for item in original_color:
          oc_.append(item)
        render_oneimage(render_args, camera, filename_0.replace('.png','_change' + str(change_count) + ttt + '.png'), args.camera_jitter, ox, oy, oz, blender_objects_original, kk, oc_, new_color_,'replace',ttt)
        colors_.append(oc_)

        change_count = change_count + 1
        scene_struct['replaced_object'].append(replaced_object) 
        scene_struct['new_object'].append(new_object) 
      else:
        exit()

  

  if len(scene_struct['change_record']) < args.max_change:
    exit()

  with open(output_scene, 'w') as f:
    json.dump(scene_struct, f, indent=2)

  print("+++++++++++++++++++++++++++++++++++++++++++++++++++++")
  for obj in bpy.data.objects:
    if obj.name.startswith('Smooth') or obj.name.startswith('Sphere') or obj.name.startswith('wall'): 
      utils.delete_object(obj)

  namelist_ = []
  obj_ = []
  for obj_idx in range(len(blender_objects_original)):
    utils.add_object(args.shape_dir, objectss[obj_idx]['obj_name'], objectss[obj_idx]['r'], (objectss[obj_idx]['x'], objectss[obj_idx]['y']), namelist_, theta=objectss[obj_idx]['rotation'])
    obj = bpy.context.object
    obj_.append(obj)
    utils.add_material(objectss[obj_idx]['mat_name'], Color=objectss[obj_idx]['rgba'])

  for i in range(4):
    print(colors_[i])
    save_objectmasks(obj_, colors_[i], filename_0.replace('.png', '_change' + str(i) + '_abefore_mask.png'))
  
  print("+++++++++++++++++++++++++++++++++++++++++++++++++++++")


def rand(L):
  return 2.0 * L * (random.random() - 0.5)

def render_oneimage(render_args, camera, filename_0, camera_jitter, ox, oy, oz, original_objs, new_objs, original_color, new_color, type_, ttt):

  if camera_jitter > 0:
    camera.location[0] = ox + rand(camera_jitter)
    camera.location[1] = oy + rand(camera_jitter)
    camera.location[2] = oz + rand(camera_jitter)

  else:
    camera.location[0] = ox 
    camera.location[1] = oy 
    camera.location[2] = oz 

  while True:
    try:
      render_args.filepath = filename_0
      bpy.ops.render.render(write_still=True)

      if type_ != 'original':
        save_objectmasks(new_objs, new_color, filename_0.replace(ttt, '_bafter_mask'))

        camera.location[0] = ox 
        camera.location[1] = oy 
        camera.location[2] = oz 
 
        print(original_color)
        print(len(original_objs))
        #save_objectmasks(original_objs, [], filename_0.replace(ttt, '_before_mask'))

      break
    except Exception as e:
      print(e)




def add_random_objects(scene_struct, num_objects, args, camera, namelist, original_list):
  """
  Add random objects to the current blender scene
  """

  # Load the property file
  with open(args.properties_json, 'r') as f:
    properties = json.load(f)
    color_name_to_rgba = {}
    for name, rgb in properties['colors'].items():
      rgba = [float(c) / 255.0 for c in rgb] + [1.0]
      color_name_to_rgba[name] = rgba
    material_mapping = [(v, k) for k, v in properties['materials'].items()]
    object_mapping = [(v, k) for k, v in properties['shapes'].items()]
    size_mapping = list(properties['sizes'].items())

  shape_color_combos = None
  if args.shape_color_combos_json is not None:
    with open(args.shape_color_combos_json, 'r') as f:
      shape_color_combos = list(json.load(f).items())

  positions = []
  objects = []
  blender_objects = []
  for i in range(num_objects):
    # Choose a random size
    size_name, r = random.choice(size_mapping)

    # Try to place the object, ensuring that we don't intersect any existing
    # objects and that we are more than the desired margin away from all existing
    # objects along all cardinal directions.
    num_tries = 0
    while True:
      # If we try and fail to place an object too many times, then delete all
      # the objects in the scene and start over.
      num_tries += 1
      if num_tries > args.max_retries:
        for obj in blender_objects:
          utils.delete_object(obj)
        return add_random_objects(scene_struct, num_objects, args, camera, namelist, original_list)
      x = random.uniform(-3, 3)
      y = random.uniform(-3, 3)
      # Check to make sure the new object is further than min_dist from all
      # other objects, and further than margin along the four cardinal directions

      dists_good = True
      margins_good = True
      for (xx, yy, rr) in positions:
        dx, dy = x - xx, y - yy
        dist = math.sqrt(dx * dx + dy * dy)
        if dist - args.largest_size - rr < args.min_dist:
          dists_good = False
          break
        for direction_name in ['left', 'right', 'front', 'behind']:
          direction_vec = scene_struct['directions'][direction_name]
          assert direction_vec[2] == 0
          margin = dx * direction_vec[0] + dy * direction_vec[1]
          if 0 < margin < args.margin:
            print(margin, args.margin, direction_name)
            print('BROKEN MARGIN!')
            margins_good = False
            break
        if not margins_good:
          break

      if dists_good and margins_good:
        break

    # Choose random color and shape
    if shape_color_combos is None:
      ### -- ### Check Same Object:
      while(1):
        obj_name, obj_name_out = random.choice(object_mapping)
        color_name, rgba = random.choice(list(color_name_to_rgba.items()))

        flag = 0
        for i in range(len(objects)):
          if ((obj_name_out == objects[i]['shape']) and (color_name == objects[i]['color'])):
            flag = 1
        if (flag == 0):
          break
    else:
      obj_name_out, color_choices = random.choice(shape_color_combos)
      color_name = random.choice(color_choices)
      obj_name = [k for k, v in object_mapping if v == obj_name_out][0]
      rgba = color_name_to_rgba[color_name]

    # For cube, adjust the size a bit
    if obj_name == 'Cube' or obj_name == 'SmoothCube_v2':
      r /= math.sqrt(2)
      print("YOu are here")

    # Choose random orientation for the object.
    theta = 360.0 * random.random()

    # Actually add the object to the scene
    utils.add_object(args.shape_dir, obj_name, r, (x, y), namelist, theta=theta)
    obj = bpy.context.object
    blender_objects.append(obj)
    positions.append((x, y, r))

    # Attach a random material
    mat_name, mat_name_out = random.choice(material_mapping)
    utils.add_material(mat_name, Color=rgba)

    # Record data about the object in the scene data structure
    pixel_coords = utils.get_camera_coords(camera, obj.location)
    objects.append({
      'shape': obj_name_out,
      'size': size_name,
      'material': mat_name_out,
      '3d_coords': tuple(obj.location),
      'rotation': theta,
      'pixel_coords': pixel_coords,
      'color': color_name,
      'pixels':0,
      'obj_name':obj_name,
      'r':r,
      'x':x,
      'y':y,
      'mat_name':mat_name,
      'rgba':rgba
    })

  for i in range(len(blender_objects)):
    original_list.append(i)

  return objects, blender_objects, positions, original_list


def compute_all_relationships(scene_struct, eps=0.2):
  """
  Computes relationships between all pairs of objects in the scene.
  
  Returns a dictionary mapping string relationship names to lists of lists of
  integers, where output[rel][i] gives a list of object indices that have the
  relationship rel with object i. For example if j is in output['left'][i] then
  object j is left of object i.
  """
  all_relationships = {}
  for name, direction_vec in scene_struct['directions'].items():
    if name == 'above' or name == 'below': continue
    all_relationships[name] = []
    for i, obj1 in enumerate(scene_struct['objects']):
      coords1 = obj1['3d_coords']
      related = set()
      for j, obj2 in enumerate(scene_struct['objects']):
        if obj1 == obj2: continue
        coords2 = obj2['3d_coords']
        diff = [coords2[k] - coords1[k] for k in [0, 1, 2]]
        dot = sum(diff[k] * direction_vec[k] for k in [0, 1, 2])
        if dot > eps:
          related.add(j)
      all_relationships[name].append(sorted(list(related)))
  return all_relationships

######################################################################################
def add_one_object(new_color, blender_objects_1, blender_objects, scene_struct, args, camera, positions, render_args, namelist, attr_list):
  # Load the property file
  print("-----------------------------------------------------------------------------------------------------------------------------------------------------")
  with open(args.properties_json, 'r') as f:
    properties = json.load(f)
    color_name_to_rgba = {}
    for name, rgb in properties['colors'].items():
      rgba = [float(c) / 255.0 for c in rgb] + [1.0]
      color_name_to_rgba[name] = rgba
    material_mapping = [(v, k) for k, v in properties['materials'].items()]
    object_mapping = [(v, k) for k, v in properties['shapes'].items()]
    size_mapping = list(properties['sizes'].items())
  
  # Choose a random size
  size_name, r = random.choice(size_mapping)

  # Try to place the object, ensuring that we don't intersect any existing
  # objects and that we are more than the desired margin away from all existing
  # objects along all cardinal directions.
  num_tries = 0
  while True:
    # If we try and fail to place an object too many times, then delete all
    # the objects in the scene and start over.
    num_tries += 1
    if num_tries > args.max_retries:
      return new_color, blender_objects_1,blender_objects,0,0,positions,attr_list

    x = random.uniform(-3, 3)
    y = random.uniform(-3, 3)
    # Check to make sure the new object is further than min_dist from all
    # other objects, and further than margin along the four cardinal directions
    dists_good = True
    margins_good = True
    for (xx, yy, rr) in positions:
      dx, dy = x - xx, y - yy
      dist = math.sqrt(dx * dx + dy * dy)
      if dist - args.largest_size - rr < args.min_dist:
        dists_good = False
        break
      for direction_name in ['left', 'right', 'front', 'behind']:
        direction_vec = scene_struct['directions'][direction_name]
        assert direction_vec[2] == 0
        margin = dx * direction_vec[0] + dy * direction_vec[1]
        if 0 < margin < args.margin:
          print(margin, args.margin, direction_name)
          print('BROKEN MARGIN!')
          margins_good = False
          break
      if not margins_good:
        break

    if dists_good and margins_good:
      break

  while (1):
    obj_name, obj_name_out = random.choice(object_mapping)
    color_name, rgba = random.choice(list(color_name_to_rgba.items()))


    flag = 0
    for i in range(len(attr_list)):
      if ((obj_name_out == attr_list[i]['shape']) and (color_name == attr_list[i]['color'])):
        flag = 1
    if (flag == 0):
      break

  # For cube, adjust the size a bit
  if obj_name == 'Cube' or obj_name == 'SmoothCube_v2':
    r /= math.sqrt(2)
    print("YOu are here")

  # Choose random orientation for the object.
  theta = 360.0 * random.random()

  # Actually add the object to the scene
  utils.add_object(args.shape_dir, obj_name, r, (x, y), namelist, theta=theta)
  obj = bpy.context.object

  new_color.append(100+len(blender_objects_1))
  blender_objects_1.append(obj)

  # Attach a random material
  mat_name, mat_name_out = random.choice(material_mapping)
  utils.add_material(mat_name, Color=rgba)



  positions.append((x, y, r))

  # utils.delete_object(obj)

  objects = []
  objects.append({
    'shape': obj_name_out,
    'size': size_name,
    'material': mat_name_out,
    '3d_coords': tuple(obj.location),
    'color': color_name,
  })

  attr_list.append({
    'shape': obj_name_out,
    'size': size_name,
    'material': mat_name_out,
    'color': color_name,
  })


  obj_ = []
  obj_.append(obj)

  return new_color, blender_objects_1, obj_,objects,1,positions,attr_list


######################################################################################
def add_one_object_defined_object(new_color, blender_objects_1, scene_struct, args, camera, positions, render_args, objects_info, namelist):
  # Try to place the object, ensuring that we don't intersect any existing
  # objects and that we are more than the desired margin away from all existing
  # objects along all cardinal directions.
  num_tries = 0
  while True:
    # If we try and fail to place an object too many times, then delete all
    # the objects in the scene and start over.
    num_tries += 1
    if num_tries > args.max_retries:
      print("--------------------------------------------------------------------------------------------------------------------------------------")
      return new_color, blender_objects_1, [], -1, positions
    x = random.uniform(-3, 3)
    y = random.uniform(-3, 3)
    # Check to make sure the new object is further than min_dist from all
    # other objects, and further than margin along the four cardinal directions
    dists_good = True
    margins_good = True

    r = objects_info['r']

    for (xx, yy, rr) in positions:
      dx, dy = x - xx, y - yy
      dist = math.sqrt(dx * dx + dy * dy)
      if dist - args.largest_size - rr < args.min_dist:
        dists_good = False
        break
      for direction_name in ['left', 'right', 'front', 'behind']:
        direction_vec = scene_struct['directions'][direction_name]
        assert direction_vec[2] == 0
        margin = dx * direction_vec[0] + dy * direction_vec[1]
        if 0 < margin < args.margin:
          print(margin, args.margin, direction_name)
          print('BROKEN MARGIN!')
          margins_good = False
          break
      if not margins_good:
        break

    if dists_good and margins_good:
      break


  obj_name = objects_info['obj_name']


  # Choose random orientation for the object.
  theta = 360.0 * random.random()

  # Actually add the object to the scene
  utils.add_object(args.shape_dir, obj_name, r, (x, y), namelist, theta=theta)
  obj = bpy.context.object
  positions.append((x,y,r))

  new_color.append(200)
  blender_objects_1.append(obj)

  # Attach a random material
  utils.add_material(objects_info['mat_name'], Color=objects_info['rgba'])

  #render_oneimage(render_args, camera, filename_0)

  #utils.delete_object(obj)

  objects = []
  objects.append({
    'shape': objects_info['shape'],
    'size': objects_info['size'],
    'material': objects_info['material'],
    '3d_coords': tuple(obj.location),
    'color': objects_info['color'],
  })

  return new_color, blender_objects_1, objects,1,positions

def add_one_object_defined_position(new_color, blender_objects_1, args, objects_info, render_args, camera, namelist,positions, attr_list):
  # Load the property file
  with open(args.properties_json, 'r') as f:
    properties = json.load(f)
    color_name_to_rgba = {}
    for name, rgb in properties['colors'].items():
      rgba = [float(c) / 255.0 for c in rgb] + [1.0]
      color_name_to_rgba[name] = rgba
    material_mapping = [(v, k) for k, v in properties['materials'].items()]
    object_mapping = [(v, k) for k, v in properties['shapes'].items()]
    size_mapping = list(properties['sizes'].items())

  shape_color_combos = None
  if args.shape_color_combos_json is not None:
    with open(args.shape_color_combos_json, 'r') as f:
      shape_color_combos = list(json.load(f).items())
  
  # Choose a random size
  size_name, r = random.choice(size_mapping)

  while (1):
    obj_name, obj_name_out = random.choice(object_mapping)
    color_name, rgba = random.choice(list(color_name_to_rgba.items()))

    flag = 0
    for i in range(len(attr_list)):
      if ((obj_name_out == attr_list[i]['shape']) and (color_name == attr_list[i]['color'])):
        flag = 1
    if (flag == 0):
      break
  


  # Try to place the object, ensuring that we don't intersect any existing
  # objects and that we are more than the desired margin away from all existing
  # objects along all cardinal directions.
  
  x = objects_info['x']
  y = objects_info['y']

  # For cube, adjust the size a bit
  if obj_name == 'Cube' or obj_name == 'SmoothCube_v2':
    r /= math.sqrt(2)
    print("YOu are here")

  # Choose random orientation for the object.
  theta = 360.0 * random.random()

  # Actually add the object to the scene
  utils.add_object(args.shape_dir, obj_name, r, (x, y), namelist, theta=theta)
  positions.append((x,y,r))
  obj = bpy.context.object

  # Attach a random material
  mat_name, mat_name_out = random.choice(material_mapping)
  utils.add_material(mat_name, Color=rgba)

  new_color.append(300)
  blender_objects_1.append(obj)

  objects = []
  objects.append({
    'shape': obj_name_out,
    'size': size_name,
    'material': mat_name_out,
    '3d_coords': tuple(obj.location),
    'color': color_name,
  })

  attr_list.append({
    'shape': obj_name_out,
    'size': size_name,
    'material': mat_name_out,
    'color': color_name,
  })


  return new_color, blender_objects_1, objects, positions, attr_list


def drop_one_object(original_color, blender_objects, objects, args, camera, positions, render_args, namelist,original_list,attr_list):
  count = 15
  while count > 0:
    picked_obj = random.randint(0,len(original_list)-1)
    picked_obj1 = original_list[picked_obj]    
    obj = blender_objects[picked_obj1]    
    objects_info = objects[picked_obj1]

    flag = 0
    for i in range(len(attr_list)):
      if ((objects_info['shape'] == attr_list[i]['shape']) and (objects_info['color'] == attr_list[i]['color'])):
        flag = 1
    if flag == 0:
      original_list.pop(picked_obj)
      for k in range(picked_obj,len(original_list)):
        original_list[k] = original_list[k] - 1

      obj = blender_objects.pop(picked_obj1)
      objects_info = objects.pop(picked_obj1)

      break

    count = count - 1

  if count < 0:
    return original_color, objects_info,-1,original_list,attr_list

  attr_list.append({
    'shape': objects_info['shape'],
    'size': objects_info['size'],
    'material': objects_info['material'],
    'color': objects_info['color'],
  })

  print('-------------------------')
  print(picked_obj1)
  print(original_color)

  print('-------------------------')

  for i in range(len(original_color)):
    if (((original_color[i])%100)%12) == picked_obj1:
      original_color[i] = 111

      for j in range(i+1, len(original_color)):
        if (((original_color[j])%100)%12) != 11:
          original_color[j] = original_color[j] - 1

      break

  print('+++++++++++++++++++++++++')
  print(picked_obj1)
  print(original_color)

  print('+++++++++++++++++++++++++')

  utils.delete_object(obj)



  return original_color, objects_info, 1, original_list,attr_list


def move_one_object(original_color, new_color, blender_objects_1, scene_struct, blender_objects, objects, args, camera, positions, render_args, namelist,original_list,attr_list):
  ## choose an object
  count = 15
  while count > 0:
    picked_obj = random.randint(0,len(original_list)-1)
    picked_obj1 = original_list[picked_obj]    
    obj = blender_objects[picked_obj1]    
    objects_info = objects[picked_obj1]

    flag = 0
    for i in range(len(attr_list)):
      if ((objects_info['shape'] == attr_list[i]['shape']) and (objects_info['color'] == attr_list[i]['color'])):
        flag = 1

    if flag == 0:
      original_list.pop(picked_obj)
      for k in range(picked_obj,len(original_list)):
        original_list[k] = original_list[k] - 1

      obj = blender_objects.pop(picked_obj1)
      objects_info = objects.pop(picked_obj1)

      break

    count = count - 1
      
  if count < 0:
    return original_color, new_color, blender_objects_1, moved_object,-1,positions,original_list,attr_list


  attr_list.append({
    'shape': objects_info['shape'],
    'size': objects_info['size'],
    'material': objects_info['material'],
    'color': objects_info['color'],
  })




  ## delete the object
  utils.delete_object(obj)
  
  ## add the object to another place, render scene, delete the object
  new_color, blender_objects_1, moved_object, flag1, positions = add_one_object_defined_object(new_color, blender_objects_1, scene_struct, args, camera, positions, render_args, objects_info, namelist)


  if flag1 > 0:
    for i in range(len(original_color)):
      if (((original_color[i])%100)%12) == picked_obj1:
        original_color[i] = 211

        for j in range(i+1, len(original_color)):
          if (((original_color[j])%100)%12) != 11:
            original_color[j] = original_color[j] - 1
        break

  ## return information
  return original_color, new_color, blender_objects_1, moved_object,flag1,positions,original_list,attr_list


def replace_one_object(original_color, new_color, blender_objects_1, blender_objects, objects, args, render_args, camera, namelist,original_list,positions, attr_list):
  ## choose an object
  count = 15
  while count > 0:
    picked_obj = random.randint(0,len(original_list)-1)
    picked_obj1 = original_list[picked_obj]    
    obj = blender_objects[picked_obj1]    
    objects_info = objects[picked_obj1]

    flag = 0
    for i in range(len(attr_list)):
      if ((objects_info['shape'] == attr_list[i]['shape']) and (objects_info['color'] == attr_list[i]['color'])):
        flag = 1
    if flag == 0:
      original_list.pop(picked_obj)
      for k in range(picked_obj,len(original_list)):
        original_list[k] = original_list[k] - 1

      obj = blender_objects.pop(picked_obj1)
      objects_info = objects.pop(picked_obj1)

      break

    count = count - 1

  if count < 0:
    return original_color, new_color, blender_objects_1, objects_info, [],-1,original_list,positions,attr_list



  ## delete the object
  utils.delete_object(obj)

  attr_list.append({
    'shape': objects_info['shape'],
    'size': objects_info['size'],
    'material': objects_info['material'],
    'color': objects_info['color'],
  })

  ## add a new object (make sure it is different from the original one), render, delete the new object
  new_color, blender_objects_1, new_object,positions,attr_list = add_one_object_defined_position(new_color, blender_objects_1, args, objects_info, render_args, camera, namelist,positions,attr_list)


  for i in range(len(original_color)):
    if (((original_color[i])%100)%12) == picked_obj1:
      original_color[i] = 311

      for j in range(i+1, len(original_color)):
        if (((original_color[j])%100)%12) != 11:
          original_color[j] = original_color[j] - 1

      break


  return original_color, new_color, blender_objects_1, objects_info, new_object,1,original_list,positions,attr_list

######################################################################################
def save_objectmasks(blender_objects, obj_ind1, file_path):
  # 
  render_shadeless(blender_objects, obj_ind1, path=file_path)
  img = bpy.data.images.load(file_path)


def render_shadeless(blender_objects, obj_ind1, path='flat.png'):
  """
  Render a version of the scene with shading disabled and unique materials
  assigned to all objects, and return a set of all colors that should be in the
  rendered image. The image itself is written to path. This is used to ensure
  that all objects will be visible in the final rendered scene.
  """
  render_args = bpy.context.scene.render

  # Cache the render args we are about to clobber
  old_filepath = render_args.filepath
  old_engine = render_args.engine
  old_use_antialiasing = render_args.use_antialiasing

  # Override some render settings to have flat shading
  render_args.filepath = path
  render_args.engine = 'BLENDER_RENDER'
  render_args.use_antialiasing = False

  # Move the lights and ground to layer 2 so they don't render
  #utils.set_layer(bpy.data.objects['Lamp_Key'], 2)
  #utils.set_layer(bpy.data.objects['Lamp_Fill'], 2)
  #utils.set_layer(bpy.data.objects['Lamp_Back'], 2)
  #utils.set_layer(bpy.data.objects['Ground'], 2)


  pre_color = []
  pre_color.append([0,255,0]) 
  pre_color.append([255,0,0]) 
  pre_color.append([0,0,255]) 
  pre_color.append([0,255,255]) 
  pre_color.append([255,255,0]) 
  pre_color.append([255,0,255]) 
  pre_color.append([0,150,0]) 
  pre_color.append([150,0,0]) 
  pre_color.append([0,0,150]) 
  pre_color.append([0,150,150]) 
  pre_color.append([150,150,0]) 
  pre_color.append([150,0,150]) 

  temp_color = []
  count = 0
  for i in range(20):
    if (i > len(obj_ind1) - 1):
      temp_color.append([0,0,0])
      continue


    if int(obj_ind1[i]/100) == 1:
      temp_color.append(pre_color[0])
      
    elif int(obj_ind1[i]/100) == 2:
      temp_color.append(pre_color[1])
    elif int(obj_ind1[i]/100) == 3:
      temp_color.append(pre_color[2])
    else:
      temp_color.append([0,0,0])

  old_materials = []
  old_materials = []
  for i, obj in enumerate(blender_objects):
    old_materials.append(obj.data.materials[0])
    bpy.ops.material.new()
    mat = bpy.data.materials['Material']
    mat.name = 'Material_%d' % i

    mat.diffuse_color = temp_color[i]
    mat.use_shadeless = True
    obj.data.materials[0] = mat

  print("You are here")
  print(obj_ind1)
  print(len(blender_objects))
  # Render the scene
  bpy.ops.render.render(write_still=True)

  # Undo the above; first restore the materials to objects
  for mat, obj in zip(old_materials, blender_objects):
    obj.data.materials[0] = mat

  # Move the lights and ground back to layer 0
  #utils.set_layer(bpy.data.objects['Lamp_Key'], 0)
  #utils.set_layer(bpy.data.objects['Lamp_Fill'], 0)
  #utils.set_layer(bpy.data.objects['Lamp_Back'], 0)
  #utils.set_layer(bpy.data.objects['Ground'], 0)

  # Set the render settings back to what they were
  render_args.filepath = old_filepath
  render_args.engine = old_engine
  render_args.use_antialiasing = old_use_antialiasing
###########################################################################################


if __name__ == '__main__':
  if INSIDE_BLENDER:
    # Run normally
    argv = utils.extract_args()
    args = parser.parse_args(argv)
    main(args)
  elif '--help' in sys.argv or '-h' in sys.argv:
    parser.print_help()
  else:
    print('This script is intended to be called from blender like this:')
    print()
    print('blender --background --python render_images.py -- [args]')
    print()
    print('You can also run as a standalone python script to view all')
    print('arguments like this:')
    print()
    print('python render_images.py --help')

